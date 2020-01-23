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
#include "ui/gui_hex_editor.hpp"

#include "ui/elements/popup_list.hpp"
#include "helpers/config_manager.hpp"

#include "save/title.hpp"

#include "cheat/cheat.hpp"

#include <cmath>

namespace edz::ui {

    brls::View* GuiCheatEngine::setupUI() {
        this->m_rootFrame = new brls::ThumbnailFrame("edz.gui.cheatengine.sidebar.search"_lang);
        this->m_rootFrame->setTitle("edz.gui.cheatengine.title"_lang);

        this->m_rootFrame->setCancelListener([this](brls::View *view) {
            this->m_rootFrame = nullptr;
            Gui::goBack();

            return true;
        });

        this->m_rootFrame->addHint("Memory View", brls::Key::X, [this]() {
            auto regionSelection = new brls::Dialog("edz.gui.hexeditor.dialog"_lang);

            regionSelection->addButton("edz.gui.cheatengine.scanregion.heap"_lang, [this, regionSelection](brls::View *view) {
                Gui::runLater([]{
                    Gui::changeTo<GuiHexEditor>(cheat::CheatManager::getHeapRegion().baseAddress);
                }, 15);

                regionSelection->close();
            });

            regionSelection->addButton("edz.gui.cheatengine.scanregion.main"_lang, [this, regionSelection](brls::View *view) {
                Gui::runLater([]{
                    Gui::changeTo<GuiHexEditor>(cheat::CheatManager::getMainRegion().baseAddress);
                }, 15);

                regionSelection->close();
            });

            regionSelection->open();

            return true;
        });

        this->m_rootFrame->addHint("Reset Search", brls::Key::MINUS, []() {
            brls::Logger::debug("Reset Search");
            return true;
        });

        // Sidebar 
        std::vector<u8> thumbnailBuffer(1280 * 720 * 4);

        if (save::Title::getLastTitleForgroundImage(&thumbnailBuffer[0]).failed())
            std::fill(thumbnailBuffer.begin(), thumbnailBuffer.end(), 0x80);

        this->m_rootFrame->getSidebar()->setThumbnail(&thumbnailBuffer[0], 1280, 720);
        

        /*brls::LayerView *contentLayers = new brls::LayerView();
        
        this->createPrimaryKnownSearchLayer(contentLayers);
        this->createPrimaryUnknownSearchLayer(contentLayers);
        this->createSecondaryKnownSearchLayer(contentLayers);
        this->createSecondaryUnknownSearchLayer(contentLayers);*/
        auto list = new brls::List();
        list->addView(new brls::ListItem("Hello"));
        this->m_rootFrame->setContentView(list);

        return this->m_rootFrame;
    }

    void GuiCheatEngine::update() {
        
    }

}