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

#include "cheat/cheat.hpp"

namespace edz::ui {

    class GuiCheats : public Gui {
    public:
        GuiCheats() : Gui() { }
        ~GuiCheats() { }

        View* setupUI() override {
            TabFrame *rootFrame = new TabFrame();
            rootFrame->setTitle("EdiZon");
            rootFrame->setSubtitle("");

            List *infoList = new List();
            infoList->addView(new Header("System"));
            // dmnt:cht running, edz running, Running game, TID, PID, BID
            infoList->addView(new Header("Memory Layout"));
            // HEAP base, MAIN base, ADDR base, (memory layout map?)

            Table *table = new Table();
            table->addRow(TableRowType::HEADER, "Region", "Address");
            table->addRow(TableRowType::BODY, "subsdk1", "0x0000000 - 0x0000123");
            table->addRow(TableRowType::BODY, "subsdk2", "0x0000124 - 0x0001243");

            List *cheatList = new List(0);
            cheatList->addView(new Header("Global Category"));

            for (auto cheat : edz::cheat::CheatManager::get().getCheats())
                cheatList->addView(new ToggleListItem(cheat->getName(), cheat->isEnabled()));

            if (edz::cheat::CheatManager::get().getCheats().size() == 0) {
                cheatList->setSpacing(10);
                cheatList->addView(new Label(LabelStyle::DESCRIPTION, "No cheats for this game loaded"));
            }
            
            rootFrame->addTab("Cheats", cheatList);
            rootFrame->addSeparator();
            rootFrame->addTab("System information", infoList);
            rootFrame->addTab("Memory Regions", table);

            return rootFrame;
        }

        void update() override {
            
        }
    };

}