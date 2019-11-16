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
#include "overlay/styles.hpp"

namespace edz::ovl {

    class Gui {
    public:
        Gui();
        virtual ~Gui();

        static void initialize(Screen *screen);
        static void deinitialize();

        bool shouldClose();

        virtual void createUI() = 0;
        virtual void update() = 0;

        template<typename T>
        static void changeTo() {
            Gui::s_nextGui = new T();
        }

        static Gui* getCurrGui() {
            return Gui::s_currGui;
        }

        static void tick() {
            static lv_area_t area = { 0, 0, 256, 512 };
            Gui::s_frameCount++;

            if (Gui::s_nextGui != nullptr) {
                if (Gui::s_currGui != nullptr) {
                    lv_group_del(Gui::s_currGui->m_group);
                    delete Gui::s_currGui;
                }

                Gui::s_currGui = Gui::s_nextGui;
                Gui::s_nextGui = nullptr;

                Gui::s_currGui->m_group = lv_group_create();
                setGroupStyle(Gui::s_currGui->m_group);
                lv_indev_set_group(Gui::s_inputDeviceButtons, Gui::s_currGui->m_group);
                Gui::s_currGui->createUI();
            }

            if (Gui::s_currGui == nullptr)
                return;

            Gui::s_currGui->update();

            if (Gui::s_guiOpacity < 1.0 && Gui::s_introAnimationPlaying) {
                Gui::s_guiOpacity += 0.03F;
                lv_inv_area(lv_disp_get_default(), &area);
            }

            if (Gui::s_guiOpacity > 0.0 && Gui::s_outroAnimationPlaying) {
                Gui::s_guiOpacity -= 0.03F;
                lv_inv_area(lv_disp_get_default(), &area);
            }

            if (Gui::s_guiOpacity >= 1.0)
                Gui::s_introAnimationPlaying = false;
            if (Gui::s_guiOpacity <= 0.0)
                Gui::s_outroAnimationPlaying = false;
        }

        static u64 getFrameCount() {
            return Gui::s_frameCount;
        }
        
        static void setGuiOpacity(float opacity) {
            Gui::s_guiOpacity = opacity;
        }

        static void playIntroAnimation() {
            Gui::s_introAnimationPlaying = true;
            Gui::s_guiOpacity = 0.0F;
        }

    protected:
        void registerForButtonInput(lv_obj_t *obj);
    private:
        static inline Screen *s_screen = nullptr;
        static inline Gui *s_currGui = nullptr;
        static inline Gui *s_nextGui = nullptr;

        static inline lv_disp_drv_t s_displayDriver = { 0 };
        static inline lv_disp_buf_t s_displayBuffer = { 0 };
        static inline lv_indev_drv_t s_inputDeviceDriverTouch = { 0 };
        static inline lv_indev_drv_t s_inputDeviceDriverButtons = { 0 };
        static inline lv_indev_t *s_inputDeviceTouch = nullptr, *s_inputDeviceButtons = nullptr;

        static inline bool s_initialized = false;
        static inline u64 s_frameCount = 0;
        static inline float s_guiOpacity = 0.0;

        static inline bool s_introAnimationPlaying = true;
        static inline bool s_outroAnimationPlaying = true;

        static void lvglDisplayFlush(lv_disp_drv_t *displayDriver, const lv_area_t *area, lv_color_t *color);
        static bool lvglTouchRead(_lv_indev_drv_t *indev, lv_indev_data_t *data);
        static bool lvglButtonRead(_lv_indev_drv_t *indev, lv_indev_data_t *data);

        lv_group_t *m_group = nullptr;
    };

}