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
                if (asyncTask.wait_for(10us) == std::future_status::ready && timeout == -1)
                    timeout = 60;
                else if (timeout == 0) {
                    if (dialog != nullptr)
                        dialog->close();
                    brls::Application::unblockInputs();
                    Gui::s_asyncTasks.erase(Gui::s_asyncTasks.begin() + taskIndex);
                }
                else if (timeout > 0)
                    timeout--;

                taskIndex++;
            }

            if (Gui *currGui = Gui::s_guiStack.back(); currGui != nullptr) {
                taskIndex = 0;
                for (auto& [task, frameDelay] : currGui->m_syncTasks) {
                    if (frameDelay-- == 0) {
                        task();
                        currGui->m_syncTasks.erase(currGui->m_syncTasks.begin() + taskIndex);
                    }
                    taskIndex++;
                }

                currGui->update();

                for (auto& [task, period] : currGui->m_syncRepetitiveTasks) {
                    if (Gui::s_frames % period)
                        task();
                }
            }

            
            Gui::s_frames++;
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

        void runLater(std::function<void()> task, u32 frameDelay) {
            this->m_syncTasks.push_back({ task, frameDelay });
        }

        static void runAsync(std::function<void()> task) {
            Gui::s_asyncTasks.push_back({ std::async(std::launch::async, task), nullptr, -1 });
        }

        static void runAsyncWithDialog(std::function<void()> task, std::string dialogText) {
            brls::Dialog *dialog = new brls::Dialog(dialogText);
            Gui::s_asyncTasks.push_back({ std::async(std::launch::async, task), dialog, -1 });

            dialog->open();
        }

        static void runAsyncWithDialog(std::function<void()> task, brls::View *dialogContent) {
            brls::Dialog *dialog = new brls::Dialog(dialogContent);
            Gui::s_asyncTasks.push_back({ std::async(std::launch::async, task), dialog, -1 });

            dialog->open();
        }

        void runRepetiviely(std::function<void()> task, u32 period) {
            this->m_syncRepetitiveTasks.push_back({ task, period });
        }

        static u64 getFrameCount() {
            return Gui::s_frames;
        }

    private:
        typedef struct {
            std::future<void> task;
            brls::Dialog *dialog;
            s32 timeout;
        } asyncTask_t;

        typedef struct {
            std::function<void()> task;
            u32 frameDelay;
        } syncTask_t;

        static inline std::vector<asyncTask_t> s_asyncTasks;
        std::vector<syncTask_t> m_syncTasks;
        std::vector<syncTask_t> m_syncRepetitiveTasks;
        static inline std::vector<Gui*> s_guiStack;

        static inline u64 s_frames = 0;
    };

}