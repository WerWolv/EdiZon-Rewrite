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

}