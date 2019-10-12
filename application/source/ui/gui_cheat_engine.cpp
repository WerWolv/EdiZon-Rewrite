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

#include "ui/gui_cheat_engine.hpp"

#include "save/title.hpp"
#include "helpers/hidsys_shim.hpp"

extern "C" {
    u32 __nx_applet_exit_mode = 0;
}

namespace edz::ui {

    brls::View* GuiCheatEngine::setupUI() {
        brls::ThumbnailFrame *rootFrame = new brls::ThumbnailFrame("edz.gui.cheatengine.button.search"_lang);

        rootFrame->setTitle("edz.gui.cheatengine.title"_lang);
        rootFrame->getSidebar()->setSubtitle("Test");
        rootFrame->setCancelListener([](brls::View *view) {
            Gui::goBack();
            return true;
        });

        u8 *thumbnailBuffer = new u8[1280 * 720 * 4];

        if (save::Title::getLastTitleForgroundImage(thumbnailBuffer).succeeded())
            rootFrame->getSidebar()->setThumbnail(thumbnailBuffer, 1280, 720);

        delete[] thumbnailBuffer;
        

        brls::List *list = new brls::List();
        

        rootFrame->setContentView(new brls::Label(brls::LabelStyle::MEDIUM, "Hello", false));

        return rootFrame;
    }

    void GuiCheatEngine::update() {
        
    }

}