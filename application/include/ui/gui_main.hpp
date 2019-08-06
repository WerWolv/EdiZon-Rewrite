#pragma once

#include "ui/gui.hpp"
#include "save/save_data.hpp"

namespace edz::ui {

    class GuiMain : public Gui {
    public:
        GuiMain() : Gui() { }
        ~GuiMain() { }

        View* setupUI() {
            AppletFrame *rootFrame = new AppletFrame(false, false);

            List *titleList = new List();

            for (auto& [titleID, title] : edz::save::SaveFileSystem::getAllTitles()) {
                ListItem *item = new ListItem(title->getName());
                u8 buffer[title->getIconSize()];
                title->getIcon(buffer, sizeof(buffer));

                item->setThumbnail(buffer, sizeof(buffer));
                titleList->addView(item);
            }

            rootFrame->setTitle("EdiZon");
            rootFrame->setContentView(titleList);

            return rootFrame;
        }

        void update() {
            
        }
    };

}