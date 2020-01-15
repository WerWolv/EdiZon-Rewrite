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

#include <memory>
#include <vector>

#include "overlay/screen.hpp"
#include "overlay/elements/element.hpp"

#include "helpers/utils.hpp"

namespace edz::ovl::gui {

    using Element = element::Element;

    class Gui {
    public:
        Gui();
        virtual ~Gui();

        static void init(Screen *screen);
        static void exit();

        bool shouldClose();

        virtual Element* createUI() = 0;
        virtual void update() = 0;

        template<typename T>
        static void changeTo() {
            Gui::s_nextGui = new T();
        }

        static Gui* getCurrGui() {
            return Gui::s_currGui;
        }

        static void tick(s64 keysDown, s64 keysHeld, JoystickPosition joyStickPosLeft, JoystickPosition joyStickPosRight, touchPosition touchPos) {
            Gui::s_frameCount++;

            if (Gui::s_nextGui != nullptr) {
                if (Gui::s_currGui != nullptr)
                    delete Gui::s_currGui;

                if (Gui::s_topElement != nullptr)
                    delete Gui::s_topElement;

                Gui::s_currGui = Gui::s_nextGui;
                Gui::s_nextGui = nullptr;

                Gui::s_topElement = Gui::s_currGui->createUI();
                Gui::s_topElement->layout();
            }

            if (Gui::s_currGui == nullptr)
                return;

            Gui::s_currGui->update();

            Gui::s_screen->fillScreen({ 0x0, 0x0, 0x0, static_cast<u8>(0xD * Gui::s_guiOpacity) });
            Gui::s_screen->drawRect(15, 720 - 73, FB_WIDTH - 30, 1, 0xFFFF);

            Gui::s_screen->drawString("\uE0E1  Back     \uE0E0  OK", false, 30, 693, 0.023, 0xFFFF);

            if (Gui::s_topElement != nullptr)
                Gui::s_topElement->draw(Gui::s_screen);

            Gui::s_screen->flush();

            if (Gui::s_guiOpacity < 1.0 && Gui::s_introAnimationPlaying) {
                Gui::s_guiOpacity += 0.2F;
            }

            if (Gui::s_guiOpacity > 0.0 && Gui::s_outroAnimationPlaying) {
                Gui::s_guiOpacity -= 0.1F;
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

    private:
        static inline Screen *s_screen = nullptr;
        static inline Gui *s_currGui = nullptr;
        static inline Gui *s_nextGui = nullptr;
        static inline Element *s_topElement = nullptr;

        static inline bool s_initialized = false;
        static inline u64 s_frameCount = 0;
        static inline float s_guiOpacity = 0.0;

        static inline bool s_introAnimationPlaying = true;
        static inline bool s_outroAnimationPlaying = true;

    };

}