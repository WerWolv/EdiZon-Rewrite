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

#include <lvgl.h>

#include "overlay/screen.hpp"
#include "overlay/font.hpp"

namespace edz::ovl {

    class Gui {
    public:
        Gui(Screen *screen);
        virtual ~Gui();

        bool shouldClose();

        virtual void createUI() = 0;
        virtual void update() = 0;

    private:
        static inline Screen *s_screen = nullptr;

        lv_disp_drv_t m_displayDriver = { 0 };
        lv_disp_buf_t m_displayBuffer = { 0 };
        lv_indev_drv_t m_inputDevice = { 0 };

        static void lvglDisplayFlush(lv_disp_drv_t *displayDriver, const lv_area_t *area, lv_color_t *color);
        static bool lvglTouchRead(_lv_indev_drv_t *indev, lv_indev_data_t *data);
    };

}