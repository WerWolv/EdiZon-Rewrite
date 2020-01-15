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

#include "overlay/gui/gui_main.hpp"

#include "overlay/elements/list.hpp"
#include "overlay/elements/list_item.hpp"

namespace edz::ovl::gui {

    GuiMain::GuiMain() {

    }

    GuiMain::~GuiMain() {

    }


    Element* GuiMain::createUI() {
        auto list = new element::List();
        list->addItem(new element::ListItem("\uE225 Cheats"));
        list->addItem(new element::ListItem("\uE22B Notes"));
        list->addItem(new element::ListItem("\uE202 Stats"));
        list->addItem(new element::ListItem("\uE227 Settings"));

        return list;
    }

    void GuiMain::update() {
        
    }

}