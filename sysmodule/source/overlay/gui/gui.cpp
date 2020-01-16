#include "overlay/gui/gui.hpp"

#include "logo_bin.h"

#include <atomic>

namespace edz::ovl::gui {

    Gui::Gui() {
        Gui::s_screen->clear();

        Gui::s_outroAnimationPlaying = false;
    }

    Gui::~Gui() {

    }

    void Gui::init(Screen *screen) {
        Gui::s_screen = screen;
    }

    void Gui::exit() {
        if (Gui::s_currGui != nullptr)
            delete Gui::s_currGui;
    }

    bool Gui::shouldClose() {
        return Gui::s_screen->getOpacity() <= 0.0 && !Gui::s_introAnimationPlaying;
    }

    void Gui::tick() {
        auto frameStartTime = std::chrono::system_clock::now();

        if (Gui::s_nextGui != nullptr) {
            if (Gui::s_currGui != nullptr)
                delete Gui::s_currGui;

            if (Gui::s_topElement != nullptr)
                delete Gui::s_topElement;

            Gui::s_currGui = Gui::s_nextGui;
            Gui::s_nextGui = nullptr;

            Gui::s_topElement = Gui::s_currGui->createUI();
            Gui::s_topElement->layout();
            Gui::requestFocus(Gui::s_topElement, FocusDirection::NONE);
        }

        if (Gui::s_currGui == nullptr)
            return;

        Gui::s_currGui->update();

        Gui::s_screen->fillScreen(a({ 0x0, 0x0, 0x0, 0xD }));
        Gui::s_screen->drawRect(15, 720 - 73, FB_WIDTH - 30, 1, a(0xFFFF));

        Gui::s_screen->drawString("\uE0E1  Back     \uE0E0  OK", false, 30, 693, 23, a(0xFFFF));
        Gui::s_screen->drawRGBA8Image(20, 20, 84, 31, logo_bin);
        Gui::s_screen->drawString("v" VERSION_STRING, false, 108, 51, 15, a(0xFFFF));

        if (Gui::s_topElement != nullptr)
            Gui::s_topElement->frame(Gui::s_screen);

        Gui::s_screen->flush();

        const float opacity = Gui::s_screen->getOpacity();

        if (opacity < 1.0 && Gui::s_introAnimationPlaying)
            Gui::s_screen->setOpacity(opacity + 0.2F);

        if (opacity > 0.0 && Gui::s_outroAnimationPlaying)
            Gui::s_screen->setOpacity(opacity - 0.2F);

        if (opacity >= 1.0)
            Gui::s_introAnimationPlaying = false;
        if (opacity <= 0.0)
            Gui::s_outroAnimationPlaying = false;

        // Make sure we run at a maximum of 60FPS
        std::this_thread::sleep_for(16.66ms - (frameStartTime - std::chrono::system_clock::now()));

        Gui::s_lastFrameDuration = frameStartTime - std::chrono::system_clock::now();
    }

    void Gui::hidUpdate(s64 keysDown, s64 keysHeld, JoystickPosition joyStickPosLeft, JoystickPosition joyStickPosRight, u32 touchX, u32 touchY) {
        if (keysDown & KEY_UP)
            Gui::requestFocus(Gui::s_focusedElement->getParent(), FocusDirection::UP);
        else if (keysDown & KEY_DOWN)
            Gui::requestFocus(Gui::s_focusedElement->getParent(), FocusDirection::DOWN);
        else if (keysDown & KEY_LEFT)
            Gui::requestFocus(Gui::s_focusedElement->getParent(), FocusDirection::LEFT);
        else if (keysDown & KEY_RIGHT)
            Gui::requestFocus(Gui::s_focusedElement->getParent(), FocusDirection::RIGHT);
        else
            Gui::s_focusedElement->onClick(keysDown);

        
        
        if (touchX > FB_WIDTH)
            Gui::playOutroAnimation();
    }

    void Gui::playIntroAnimation() {
        if (Gui::s_introAnimationPlaying)
            return;

        Gui::s_introAnimationPlaying = true;
        Gui::s_screen->setOpacity(0.0F);
    }

    void Gui::playOutroAnimation() {
        if (Gui::s_outroAnimationPlaying)
            return;

        Gui::s_outroAnimationPlaying = true;
        Gui::s_screen->setOpacity(1.0F);
    }

    
    void Gui::requestFocus(Element *element, FocusDirection direction) {
        Element *oldFocus = Gui::s_focusedElement;
        Gui::removeFocus();

        if (element != nullptr)
            Gui::s_focusedElement = element->requestFocus(oldFocus, direction);
    }

    void Gui::removeFocus(Element *element) {
        if (element == Gui::s_focusedElement || element == nullptr)
            Gui::s_focusedElement = nullptr;
    }

}