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
#include "ui/pages/page_memory_editor.hpp"
#include "helpers/config_manager.hpp"

#include "save/title.hpp"
#include "helpers/hidsys_shim.hpp"


namespace edz::ui {

    void GuiCheatEngine::createAddressList(brls::List *list, std::vector<addr_t> addresses) {
        bool addressesFound = addresses.size() > 0;
        brls::Application::blockInputs();

        if (list->isChildFocused())
            brls::Application::removeFocus();

        list->clear();

        brls::ListItem *goBackItem = new brls::ListItem("Go back ingame");
        goBackItem->setClickListener([](brls::View *view) {
            save::Title::getRunningTitle()->launch();
        });

        list->addView(goBackItem);
        list->addView(new brls::Header("Addresses", !addressesFound));

        if (addressesFound) {
            for (addr_t &address : addresses)
                list->addView(new brls::IntegerInputListItem(hlp::formatString("[ HEAP + 0x%016lX ]", address), 0, "Enter a value"));
        } else {
            list->addView(new brls::Label(brls::LabelStyle::MEDIUM, "No addresses found yet", true));
        }

        setSearchCountText(GET_CONFIG(CheatEngine.searchCount));

        brls::Application::unblockInputs();
    }

    void GuiCheatEngine::setSearchCountText(u16 searchCount) {
        switch (searchCount) {
            case 0:
                this->rootFrame->getSidebar()->setSubtitle("edz.gui.cheatengine.subtitle.nosearches"_lang);
                break;
            case 1:
                this->rootFrame->getSidebar()->setSubtitle("edz.gui.cheatengine.subtitle.onesearch"_lang);
                break;
            default:
                this->rootFrame->getSidebar()->setSubtitle(hlp::formatString("edz.gui.cheatengine.subtitle.moresearches"_lang, searchCount));
                break;
        }
    }

    brls::View* GuiCheatEngine::setupUI() {
        this->rootFrame = new brls::ThumbnailFrame("edz.gui.cheatengine.button.search"_lang);

        this->rootFrame->setTitle("edz.gui.cheatengine.title"_lang);

        this->rootFrame->setCancelListener([](brls::View *view) {
            Gui::goBack();
            return true;
        });

        std::vector<u8> thumbnailBuffer(1280 * 720 * 4);

        if (save::Title::getLastTitleForgroundImage(&thumbnailBuffer[0]).succeeded())
            this->rootFrame->getSidebar()->setThumbnail(&thumbnailBuffer[0], 1280, 720);
        
        this->rootFrame->getSidebar()->getButton()->setClickListener([](brls::View *view) {
            brls::StagedAppletFrame *memoryEditor = new brls::StagedAppletFrame();
            memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, ui::page::PageMemoryEditor::SettingType::SEARCH_TYPE));
            memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, ui::page::PageMemoryEditor::SettingType::DATA_TYPE));
            memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, ui::page::PageMemoryEditor::SettingType::MEMORY_REGION));

            brls::Application::pushView(memoryEditor);
        });

        this->addressList = new brls::List();
        createAddressList(this->addressList, { 1234, 0x1337, 0x555, 0xFFFFF });

        this->rootFrame->setContentView(this->addressList);

        return rootFrame;
    }

    void GuiCheatEngine::update() {
        
    }

}