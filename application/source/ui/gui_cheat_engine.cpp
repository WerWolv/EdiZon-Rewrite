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


namespace edz::ui {

    void GuiCheatEngine::createAddressList(std::vector<addr_t> addresses) {
        bool addressesFound = addresses.size() > 0;
        brls::Application::blockInputs();

        if (this->m_addressList->isChildFocused())
            brls::Application::removeFocus();

        this->m_addressList->clear();

        brls::ListItem *goBackItem = new brls::ListItem("Go back ingame");
        goBackItem->setClickListener([](brls::View *view) {
            save::Title::getRunningTitle()->launch();
        });

        this->m_addressList->addView(goBackItem);
        this->m_addressList->addView(new brls::Header("Addresses", !addressesFound));

        if (addressesFound) {
            for (addr_t &address : addresses)
                this->m_addressList->addView(new brls::IntegerInputListItem(hlp::formatString("[ HEAP + 0x%016lX ]", address), 0, "Enter a value"));
        } else {
            this->m_addressList->addView(new brls::Label(brls::LabelStyle::MEDIUM, "No addresses found yet", true));
        }

        setSearchCountText(GET_CONFIG(CheatEngine.searchCount));

        brls::Application::unblockInputs();
    }

    void GuiCheatEngine::setSearchCountText(u16 searchCount) {
        switch (searchCount) {
            case 0:
                this->m_rootFrame->getSidebar()->setSubtitle("edz.gui.cheatengine.subtitle.nosearches"_lang);
                break;
            case 1:
                this->m_rootFrame->getSidebar()->setSubtitle("edz.gui.cheatengine.subtitle.onesearch"_lang);
                break;
            default:
                this->m_rootFrame->getSidebar()->setSubtitle(hlp::formatString("edz.gui.cheatengine.subtitle.moresearches"_lang, searchCount));
                break;
        }
    }

    brls::View* GuiCheatEngine::setupUI() {
        this->m_rootFrame = new brls::ThumbnailFrame("edz.gui.cheatengine.button.search"_lang);

        this->m_rootFrame->setTitle("edz.gui.cheatengine.title"_lang);

        this->m_rootFrame->setCancelListener([](brls::View *view) {
            Gui::goBack();
            return true;
        });

        std::vector<u8> thumbnailBuffer(1280 * 720 * 4);

        if (save::Title::getLastTitleForgroundImage(&thumbnailBuffer[0]).succeeded())
            this->m_rootFrame->getSidebar()->setThumbnail(&thumbnailBuffer[0], 1280, 720);
        
        this->m_rootFrame->getSidebar()->getButton()->setClickListener([this](brls::View *view) {
            brls::Dialog *searchTypeDialog = new brls::Dialog("Do you want to search for a known or unknown value?");

            searchTypeDialog->addButton("Known Value", [this](brls::View *view) {
                brls::StagedAppletFrame *memoryEditor = new brls::StagedAppletFrame();
                memoryEditor->setTitle("edz.page.memoryeditor.searchtype.title"_lang);

                memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, this, ui::page::PageMemoryEditor::SettingType::SEARCH_TYPE));
                memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, this, ui::page::PageMemoryEditor::SettingType::DATA_TYPE));
                memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, this, ui::page::PageMemoryEditor::SettingType::MEMORY_REGION));
                memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, this, ui::page::PageMemoryEditor::SettingType::VALUE_INPUT));
                memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, this, ui::page::PageMemoryEditor::SettingType::LOADING));
                
                brls::Application::popView();
                Gui::runLater([memoryEditor] { brls::Application::pushView(memoryEditor); }, 20);
            });

            searchTypeDialog->addButton("Unknown Value", [this](brls::View *view) {
                brls::StagedAppletFrame *memoryEditor = new brls::StagedAppletFrame();
                memoryEditor->setTitle("edz.page.memoryeditor.searchtype.title"_lang);

                memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, this, ui::page::PageMemoryEditor::SettingType::SEARCH_TYPE));
                memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, this, ui::page::PageMemoryEditor::SettingType::DATA_TYPE));
                memoryEditor->addStage(new ui::page::PageMemoryEditor(memoryEditor, this, ui::page::PageMemoryEditor::SettingType::MEMORY_REGION));

                brls::Application::popView();
                Gui::runLater([memoryEditor] { brls::Application::pushView(memoryEditor); }, 20);
            });

            searchTypeDialog->open();

        });

        this->m_addressList = new brls::List();
        createAddressList({ });

        this->m_rootFrame->setContentView(this->m_addressList);

        return m_rootFrame;
    }

    void GuiCheatEngine::update() {
        
    }

}