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

namespace edz::ui {

    class Gui {
    public:
        Gui() { }
        virtual ~Gui() { }

        virtual View* setupUI() = 0;

        virtual void update() = 0;


        static void tick() {
            if (Gui::s_currentGui != nullptr)
                Gui::s_currentGui->update();
        }

        template<typename T, typename... Args>
        static void replaceWith(Args... args) {
            Application::popView();
            
            if (Gui::s_currentGui != nullptr)
                delete Gui::s_currentGui;
            
            Gui::s_currentGui = new T(args...);

            Application::pushView(Gui::s_currentGui->setupUI());
            Gui::s_guiStackSize++;
        }

        template<typename T, typename... Args>
        static void changeTo(Args... args) {
            if (Gui::s_currentGui != nullptr)
                delete Gui::s_currentGui;
            
            Gui::s_currentGui = new T(args...);

            Application::pushView(Gui::s_currentGui->setupUI());
            Gui::s_guiStackSize++;
        }

        static void goBack() {
            if (s_guiStackSize <= 1)
                return;

            Application::popView();
            Gui::s_guiStackSize--;
        }

        static void fatal(std::string error) {
            Application::crash(error);
            while (Application::mainLoop());
        }

    private:
        static inline Gui *s_currentGui = nullptr;
        static inline u8 s_guiStackSize = 0;
    };

}