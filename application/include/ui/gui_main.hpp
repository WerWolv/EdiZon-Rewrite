#pragma once

#include "ui/gui.hpp"
#include "save/save_data.hpp"

namespace edz::ui {

    class GuiMain : public Gui {
    public:
        GuiMain() : Gui() { }
        ~GuiMain() { }

        View* setupUI() {
            BoxLayout *rootFrame = new BoxLayout(BoxLayoutOrientation::VERTICAL);

            for (auto& [titleID, title] : edz::save::SaveFileSystem::getAllTitles()) {
                u8 buffer[title->getIconSize()];
                title->getIcon(buffer, sizeof(buffer));

                Image *image = new Image(buffer, sizeof(buffer));
                rootFrame->addView(image, false);
            }

            return rootFrame;
        }

        void update() {
            
        }
    };

}