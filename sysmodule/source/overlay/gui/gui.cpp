#include "overlay/gui/gui.hpp"

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
        return Gui::s_guiOpacity <= 0.0 && !Gui::s_introAnimationPlaying;
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

        Gui::s_screen->fillScreen({ 0x0, 0x0, 0x0, static_cast<u8>(0xD * Gui::s_guiOpacity) });
        Gui::s_screen->drawRect(15, 720 - 73, FB_WIDTH - 30, 1, 0xFFFF);

        Gui::s_screen->drawString("\uE0E1  Back     \uE0E0  OK", false, 30, 693, 23, 0xFFFF);

        if (Gui::s_topElement != nullptr)
            Gui::s_topElement->frame(Gui::s_screen);

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

        // Make sure we run at a maximum of 60FPS
        std::this_thread::sleep_for(16.66ms - (frameStartTime - std::chrono::system_clock::now()));

        Gui::s_lastFrameDuration = frameStartTime - std::chrono::system_clock::now();
    }

    void Gui::hidUpdate(s64 keysDown, s64 keysHeld, JoystickPosition joyStickPosLeft, JoystickPosition joyStickPosRight, u32 touchX, u32 touchY) {
        if (keysDown & KEY_UP)
            Gui::requestFocus(Gui::s_focusedElement, FocusDirection::UP);
        else if (keysDown & KEY_DOWN)
            Gui::requestFocus(Gui::s_focusedElement, FocusDirection::DOWN);
        else if (keysDown & KEY_LEFT)
            Gui::requestFocus(Gui::s_focusedElement, FocusDirection::LEFT);
        else if (keysDown & KEY_RIGHT)
            Gui::requestFocus(Gui::s_focusedElement, FocusDirection::RIGHT);
    }

    void Gui::setGuiOpacity(float opacity) {
        Gui::s_guiOpacity = opacity;
    }

    void Gui::playIntroAnimation() {
        Gui::s_introAnimationPlaying = true;
        Gui::s_guiOpacity = 0.0F;
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