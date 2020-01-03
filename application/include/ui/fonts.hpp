/**
 * Copyright (C) 2020 werwolv
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

#include <nanovg.h>
#include <borealis.hpp>

namespace edz::ui {

    class Fonts {
    public:

        static void initFonts() {
            if (Fonts::s_monoSpaceFont == 0)
                Fonts::s_monoSpaceFont = nvgCreateFont(brls::Application::getNVGContext(), "monospace", "romfs:/assets/fonts/FiraCode-Regular.ttf");
        }

        static s32 getMonoSpaceFont() {
            return Fonts::s_monoSpaceFont;
        }

    private:
        static inline s32 s_monoSpaceFont = 0;
    };

}