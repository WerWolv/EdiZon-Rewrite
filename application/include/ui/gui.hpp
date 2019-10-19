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
            for (auto& [asyncTask, dialog] : Gui::s_asyncTasks) {
                if (asyncTask.wait_for(std::chrono::nanoseconds(1)) == std::future_status::ready) {
                    dialog->close();

                    brls::Application::unblockInputs();
                    Gui::s_asyncTasks.erase(Gui::s_asyncTasks.begin() + taskIndex);
                }
                taskIndex++;
            }

            if (Gui::s_guiStack.back() != nullptr)
                Gui::s_guiStack.back()->update();
        }

        template<typename T, typename... Args>
        static void replaceWith(Args... args) {
            Gui::goBack();

            Gui *newGui = new T(args...);
            
            Gui::s_guiStack.push_back(newGui);

            brls::Application::pushView(newGui->setupUI());
        }

        template<typename T, typename... Args>
        static void changeTo(Args... args) {
            Gui *newGui = new T(args...);

            Gui::s_guiStack.push_back(newGui);
            brls::Application::pushView(newGui->setupUI());
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

        static void runAsyncWithDialog(std::future<EResult> task, std::string dialogText) {
            brls::Dialog *dialog = new brls::Dialog(dialogText);
            Gui::s_asyncTasks.push_back({ std::move(task), dialog });

            dialog->open();
        }

        static void runAsyncWithDialog(std::future<EResult> task, brls::View *dialogContent) {
            brls::Dialog *dialog = new brls::Dialog(dialogContent);
            Gui::s_asyncTasks.push_back({ std::move(task), dialog });

            dialog->open();
        }

    private:
        typedef struct {
            std::future<EResult> task;
            brls::Dialog *dialog;
        } asyncTask_t;

        static inline std::vector<asyncTask_t> s_asyncTasks;

        static inline std::vector<Gui*> s_guiStack;
    };

}