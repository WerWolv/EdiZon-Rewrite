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

#include "ui/gui.hpp"
#include "save/save_data.hpp"

#include "ui/elements/title_button.hpp"

namespace edz::ui {

    class GuiMain : public Gui {
    public:
        GuiMain() : Gui() { }
        ~GuiMain() { }

        View* setupUI() override {
            AppletFrame *rootFrame = new AppletFrame(false, false);
            BoxLayout *list = new BoxLayout(BoxLayoutOrientation::VERTICAL);
            int i = 0;
            BoxLayout *boxLayout = nullptr;
            for (auto& [titleID, title] : edz::save::SaveFileSystem::getAllTitles()) {
                u8 buffer[title->getIconSize()];
                title->getIcon(buffer, sizeof(buffer));
                
                elements::TitleButton *titleButton = new elements::TitleButton(buffer, sizeof(buffer));
                
                if (i++ % 3 == 0) {
                    if (boxLayout != nullptr) {
                        list->addView(boxLayout);
                        boxLayout->expand(true);
                    }
                    
                    boxLayout = new BoxLayout(BoxLayoutOrientation::HORIZONTAL);
                }

                boxLayout->addView(titleButton, false);
            }

            rootFrame->setContentView(list);

            return rootFrame;
        }

        void update() override {
            
        }
    };

}