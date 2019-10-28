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

#include "ui/pages/page_memory_editor.hpp"

#include "helpers/utils.hpp"
#include "api/switchcheatsdb_api.hpp"
#include "helpers/config_manager.hpp"

#include <math.h>

namespace edz::ui::page {

    PageMemoryEditor::PageMemoryEditor(brls::StagedAppletFrame *stageFrame, SettingType settingType) : m_settingType(settingType) {
        this->m_confirmButton = new brls::Button(brls::ButtonStyle::PLAIN, "Confirm");
        this->m_confirmButton->setState(brls::ButtonState::DISABLED);
        this->m_confirmButton->setClickListener([this, stageFrame](brls::View *view) {
            if (stageFrame->getCurrentStage() == stageFrame->getStagesCount() - 1)
                brls::Application::popView();
            else {
                this->hide([this, stageFrame] {
                    stageFrame->nextStage();
                    this->show([]{});
                });
            }
        });

        this->m_settingList = new brls::List();

        switch (settingType) {
            case SettingType::SEARCH_TYPE:
                stageFrame->setTitle("Cheat Engine - Search Type");

                this->m_description = new brls::Label(brls::LabelStyle::DESCRIPTION, "How would you like to search for your value? If you can see the exact value you want to search for, choose a known type. If not, choose unknown.", true);
                this->m_description->setHorizontalAlign(NVG_ALIGN_CENTER);    

                for (u8 i = 0; i < 9; i++) {
                   auto typeOption = new brls::ListItem(LangEntry("edz.page.memoryeditor.searchtype." + std::to_string(i))); 
                   typeOption->setClickListener([this](brls::View *view) {
                        auto list = static_cast<brls::List*>(view->getParent());

                        for (auto &child : list->getChildren()) {
                            brls::ListItem *listItem = static_cast<brls::ListItem*>(child->view);
                            listItem->setChecked(listItem == view);
                            this->m_confirmButton->setState(brls::ButtonState::ENABLED);
                        }
                   });

                   m_settingList->addView(typeOption);
                }
                break;
            case SettingType::DATA_TYPE:
                stageFrame->setTitle("Cheat Engine - Data Type");

                this->m_description = new brls::Label(brls::LabelStyle::DESCRIPTION, "What type of value do you want to search for? The three different types of values are Signed, Unsigned and Floating point. Unsigned should be chosen on positive integers, signed on negative integers. Floating point point numbers on the other hand are positive or negative fractional numbers. You will need to provide a upper- and lower limit because of the nature of floats.", true);
                this->m_description->setHorizontalAlign(NVG_ALIGN_CENTER);    

                for (u8 i = 0; i < 10; i++) {
                   auto typeOption = new brls::ListItem(LangEntry("edz.page.memoryeditor.datatype." + std::to_string(i))); 
                   typeOption->setClickListener([this](brls::View *view) {
                        auto list = static_cast<brls::List*>(view->getParent());

                        for (auto &child : list->getChildren()) {
                            brls::ListItem *listItem = static_cast<brls::ListItem*>(child->view);
                            listItem->setChecked(listItem == view);
                            this->m_confirmButton->setState(brls::ButtonState::ENABLED);
                        }
                   });

                   m_settingList->addView(typeOption);
                }
                break;
            case SettingType::MEMORY_REGION:
                stageFrame->setTitle("Cheat Engine - Memory Region");

                this->m_description = new brls::Label(brls::LabelStyle::DESCRIPTION, "Where do you want to search for your values? The heap is where the game puts data that was loaded or calculated during runtime. E.g coin count, health and player coordinates. The main section is where the game code lives. Search here if you can't find anything in the heap.", true);
                this->m_description->setHorizontalAlign(NVG_ALIGN_CENTER);    

                for (u8 i = 0; i < 4; i++) {
                   auto typeOption = new brls::ListItem(LangEntry("edz.page.memoryeditor.region." + std::to_string(i))); 
                   typeOption->setClickListener([this](brls::View *view) {
                        auto list = static_cast<brls::List*>(view->getParent());

                        for (auto &child : list->getChildren()) {
                            brls::ListItem *listItem = static_cast<brls::ListItem*>(child->view);
                            listItem->setChecked(listItem == view);
                            this->m_confirmButton->setState(brls::ButtonState::ENABLED);
                        }
                   });

                   m_settingList->addView(typeOption);
                }
                break;
            
        }

        this->m_confirmButton->setParent(this);
        this->m_description->setParent(this);
        this->m_settingList->setParent(this);
    }

    PageMemoryEditor::~PageMemoryEditor() {

    }

    void PageMemoryEditor::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
        this->m_description->frame(ctx);
        this->m_settingList->frame(ctx);
        this->m_confirmButton->frame(ctx);
    }

    brls::View* PageMemoryEditor::requestFocus(brls::FocusDirection direction, brls::View* oldFocus, bool fromUp) {
        if (direction == brls::FocusDirection::LEFT || direction == brls::FocusDirection::NONE)
            return this->m_settingList->defaultFocus(oldFocus);
        else if (direction == brls::FocusDirection::RIGHT)
            return this->m_confirmButton;
        else return nullptr;
    }

    void PageMemoryEditor::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
        if (this->m_settingType == SettingType::DATA_TYPE || this->m_settingType == SettingType::SEARCH_TYPE || this->m_settingType == SettingType::MEMORY_REGION) {
            this->m_description->setBoundaries(750, style->AppletFrame.headerHeightRegular + 50, 1280 - 825 - 20, 400);
            this->m_settingList->setBoundaries(20, style->AppletFrame.headerHeightRegular, 750, 720 - style->AppletFrame.footerHeight - style->AppletFrame.headerHeightRegular);
            this->m_confirmButton->setBoundaries(825, 500, 300, style->List.Item.height);

            this->m_description->invalidate();
            this->m_settingList->invalidate();
            this->m_confirmButton->invalidate();
        } else if (this->m_settingType == SettingType::VALUE_INPUT) {

        }
    }

    bool PageMemoryEditor::onCancel() {
        brls::Application::popView();
        return true;
    }

}