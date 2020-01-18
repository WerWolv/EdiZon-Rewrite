/**
 * Copyright (C) 2020 WerWolv
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

#include "overlay/gui/gui_cheats.hpp"

#include "overlay/elements/frame.hpp"
#include "overlay/elements/custom_drawer.hpp"
#include "overlay/elements/list.hpp"
#include "overlay/elements/list_item.hpp"

#include "cheat/cheat.hpp"

#include "logo_bin.h"

namespace edz::ovl::gui {

    GuiCheats::GuiCheats() {
        cheat::CheatManager::reload();
    }

    GuiCheats::~GuiCheats() {
        this->m_cheatToggleItems.clear();
    }


    Element* GuiCheats::createUI() {
        auto rootFrame = new element::Frame();

        auto header = new element::CustomDrawer(0, 0, 100, FB_WIDTH, [](u16 x, u16 y, Screen *screen){
            screen->drawString("Cheats", false, 20, 50, 30, a(0xFFFF));

            if (cheat::CheatManager::getCheats().size() == 0) {
                screen->drawString("\uE150", false, 180, 250, 90, a(0xFFFF));
                screen->drawString("No Cheats loaded!", false, 110, 340, 25, a(0xFFFF));
            }
        });

        auto list = new element::List();
        
        for (auto &cheat : cheat::CheatManager::getCheats()) {
            auto cheatToggleItem = new element::ToggleListItem(hlp::limitStringLength(cheat->getName(), 20), cheat->isEnabled());
            cheatToggleItem->setStateChangeListener([&cheat](bool state) { cheat->setState(state); });

            this->m_cheatToggleItems.push_back(cheatToggleItem);
            list->addItem(cheatToggleItem);
        }
        
        rootFrame->addElement(header);
        rootFrame->addElement(list);

        return rootFrame;
    }

    void GuiCheats::update() {
        for (u16 i = 0; i < this->m_cheatToggleItems.size(); i++)
            this->m_cheatToggleItems[i]->setState(cheat::CheatManager::getCheats()[i]->isEnabled());
    }

}