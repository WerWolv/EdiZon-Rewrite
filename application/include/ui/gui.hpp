/**
 * Copyright (C) 2019 WerWolv
 * 
 * This file is part of EdiZon.
 * 
 * EdiZon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * EdiZon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EdiZon.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <edizon.hpp>
#include <borealis.hpp>

#include <future>
#include <functional>
#include <utility>

#include "helpers/utils.hpp"

namespace edz::ui {

    class Gui {
    public:
        Gui() { }
        virtual ~Gui() { }

        virtual brls::View* setupUI() = 0;

        virtual void update() = 0;


        static void tick() {
            u16 taskIndex = 0;
            for (auto& [asyncTask, dialog, timeout] : Gui::s_asyncTasks) {
                if (asyncTask.wait_for(std::chrono::nanoseconds(1)) == std::future_status::ready && timeout == -1)
                    timeout = 60;
                else if (timeout == 0) {
                    dialog->close();
                    brls::Application::unblockInputs();
                    Gui::s_asyncTasks.erase(Gui::s_asyncTasks.begin() + taskIndex);
                }
                else if (timeout > 0)
                    timeout--;

                taskIndex++;
            }

            taskIndex = 0;
            for (auto& [task, frameDelay] : Gui::s_syncTasks) {
                if (frameDelay-- == 0) {
                    task();
                    Gui::s_syncTasks.erase(Gui::s_syncTasks.begin() + taskIndex);
                }
                taskIndex++;
            }

            if (Gui::s_guiStack.back() != nullptr)
                Gui::s_guiStack.back()->update();
        }

        template<typename T, typename... Args>
        static void changeTo(Args... args) {
            Gui *newGui = new T(args...);

            Gui::s_guiStack.push_back(newGui);
            brls::Application::pushView(newGui->setupUI());
        }

        template<typename T, typename... Args>
        static void replaceWith(Args... args) {
            brls::Application::blockInputs();
            brls::Application::removeFocus();

            brls::Application::popView();

            Gui *newGui = new T(args...);

            Gui::s_guiStack.push_back(newGui);
            brls::Application::pushView(newGui->setupUI());

            brls::Application::unblockInputs();
        }

        static void goBack() {
            if (Gui::s_guiStack.size() <= 1)
                return;

            delete Gui::s_guiStack.back();

            brls::Application::popView();
            Gui::s_guiStack.pop_back();
        }

        template<typename... Args>
        static void fatal(std::string format, Args... args) {
            brls::Application::crash(hlp::formatString(format, args...));
            while (brls::Application::mainLoop());
        }

        static void runLater(std::function<void()> task, u32 frameDelay) {
            Gui::s_syncTasks.push_back({ task, frameDelay });
        }

        static void runAsyncWithDialog(std::future<EResult> task, std::string dialogText) {
            brls::Dialog *dialog = new brls::Dialog(dialogText);
            Gui::s_asyncTasks.push_back({ std::move(task), dialog, -1 });

            dialog->open();
        }

        static void runAsyncWithDialog(std::future<EResult> task, brls::View *dialogContent) {
            brls::Dialog *dialog = new brls::Dialog(dialogContent);
            Gui::s_asyncTasks.push_back({ std::move(task), dialog, -1 });

            dialog->open();
        }

    private:
        typedef struct {
            std::future<EResult> task;
            brls::Dialog *dialog;
            s32 timeout;
        } asyncTask_t;

        typedef struct {
            std::function<void()> task;
            u32 frameDelay;
        } syncTask_t;

        static inline std::vector<asyncTask_t> s_asyncTasks;
        static inline std::vector<syncTask_t> s_syncTasks;
        static inline std::vector<Gui*> s_guiStack;
    };

}