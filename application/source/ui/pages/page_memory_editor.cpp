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
#include "ui/gui_cheat_engine.hpp"

#include "cheat/cheat_engine.hpp"

#include <math.h>

namespace edz::ui::page {

    PageMemoryEditor::PageMemoryEditor(brls::StagedAppletFrame *stageFrame, GuiCheatEngine *cheatEngineGui, SettingType settingType) : m_cheatEngineGui(cheatEngineGui), m_settingType(settingType) {
        this->m_confirmButton = new brls::Button(brls::ButtonStyle::PLAIN, "edz.dialog.confirm"_lang);
        this->m_confirmButton->setState(brls::ButtonState::DISABLED);
        this->m_confirmButton->setClickListener([this, stageFrame, settingType](brls::View *view) {
            if (stageFrame->getCurrentStage() == stageFrame->getStagesCount() - 1)
                brls::Application::popView();
            else {
                this->hide([this, stageFrame, settingType] {
                    stageFrame->nextStage();

                    switch (settingType) {
                        case SettingType::SEARCH_TYPE:   stageFrame->setTitle("edz.page.memoryeditor.datatype.title"_lang); break;
                        case SettingType::DATA_TYPE:     stageFrame->setTitle("edz.page.memoryeditor.region.title"_lang);   break;
                        case SettingType::MEMORY_REGION: stageFrame->setTitle("edz.page.memoryeditor.value.title"_lang);    break;
                    }

                    if (settingType == SettingType::VALUE_INPUT) {
                        cheat::CheatEngine::setProgressCallback([this](u8 progress) { this->m_progress = progress; });
                        //std::async(std::launch::async, cheat::CheatEngine::findPattern, &this->m_searchValue.u, this->m_valueSize, this->m_dataType, this->m_compareFunc, this->m_regionFunc);
                    }

                    this->show([]{});
                });
            }
        });

        std::vector<std::string> dataTypeNames = {
            "edz.page.memoryeditor.datatype.u8"_lang,
            "edz.page.memoryeditor.datatype.s8"_lang,
            "edz.page.memoryeditor.datatype.u16"_lang,
            "edz.page.memoryeditor.datatype.s16"_lang,
            "edz.page.memoryeditor.datatype.u32"_lang,
            "edz.page.memoryeditor.datatype.s32"_lang,
            "edz.page.memoryeditor.datatype.u64"_lang,
            "edz.page.memoryeditor.datatype.s64"_lang,
            "edz.page.memoryeditor.datatype.f32"_lang,
            "edz.page.memoryeditor.datatype.f64"_lang
        };

        std::vector<std::string> searchTypeNames = {
            "edz.page.memoryeditor.searchtype.eq"_lang,
            "edz.page.memoryeditor.searchtype.neq"_lang,
            "edz.page.memoryeditor.searchtype.gt"_lang,
            "edz.page.memoryeditor.searchtype.lt"_lang,
            "edz.page.memoryeditor.searchtype.range"_lang,
            "edz.page.memoryeditor.searchtype.same"_lang,
            "edz.page.memoryeditor.searchtype.diff"_lang,
            "edz.page.memoryeditor.searchtype.inc"_lang,
            "edz.page.memoryeditor.searchtype.dec"_lang
        };

        std::vector<std::string> regionNames = {
            "edz.page.memoryeditor.region.heap"_lang,
            "edz.page.memoryeditor.region.main"_lang,
            "edz.page.memoryeditor.region.heap_main"_lang,
            "edz.page.memoryeditor.region.ram"_lang
        };

        switch (settingType) {
            case SettingType::SEARCH_TYPE:
                this->m_settingList = new brls::List();
                this->createSelectionList(this->m_settingList, searchTypeNames, "edz.page.memoryeditor.searchtype.desc"_lang, [this](u8 selectedIndex) {
                    switch (selectedIndex) {
                        case 0: this->m_compareFunc = cheat::cmp::equals; break;
                        case 1: this->m_compareFunc = cheat::cmp::notEquals; break;
                        default: this->m_compareFunc = [](cheat::DataType, size_t, void*, void*){ return false; };
                    }
                });

                break;
            case SettingType::DATA_TYPE:
                this->m_settingList = new brls::List();
                this->createSelectionList(this->m_settingList, dataTypeNames, "edz.page.memoryeditor.datatype.desc"_lang, [this](u8 selectedIndex) {
                    if (selectedIndex >= 8) {
                        this->m_dataType = cheat::DataType::FLOATING_POINT;
                        this->m_valueSize = selectedIndex % 2 * 4 + 4;
                    }
                    else if (selectedIndex % 2 == 0) {
                        this->m_dataType = cheat::DataType::UNSIGNED;
                        this->m_valueSize = selectedIndex / 2;
                    }
                    else {
                        this->m_dataType = cheat::DataType::SIGNED;
                        this->m_valueSize = selectedIndex / 2;
                    }
                });
                break;
            case SettingType::MEMORY_REGION:
                this->m_settingList = new brls::List();
                this->createSelectionList(this->m_settingList, regionNames, "edz.page.memoryeditor.region.desc"_lang, [this](u8 selectedIndex) {
                    switch (selectedIndex) {
                        case 0: this->m_regionFunc = cheat::region::readHeap; break;
                        case 1: this->m_regionFunc = cheat::region::readMain; break;
                    }
                });
                break;
            case SettingType::VALUE_INPUT:
                this->m_settingList = new brls::List();
                this->m_description = new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.page.memoryeditor.value.desc"_lang, true);
                this->m_description->setHorizontalAlign(NVG_ALIGN_CENTER);    

                {
                    brls::InputListItem *searchValueItem = new brls::IntegerInputListItem("edz.page.memoryeditor.value"_lang, 0, "");
                    searchValueItem->setClickListener([this](brls::View *view) {
                        switch (this->m_dataType) {
                            case cheat::DataType::UNSIGNED:
                                this->m_searchValue.u = std::stoull(static_cast<brls::InputListItem*>(view)->getValue(), nullptr, 0);
                                break;
                            case cheat::DataType::SIGNED:
                                this->m_searchValue.u = std::stoll(static_cast<brls::InputListItem*>(view)->getValue(), nullptr, 0);
                                break;
                            case cheat::DataType::FLOATING_POINT:
                                this->m_searchValue.u = std::stold(static_cast<brls::InputListItem*>(view)->getValue());
                                break;    
                        }
                        this->m_confirmButton->setState(brls::ButtonState::ENABLED);
                    });

                    this->m_settingList->addView(searchValueItem);
                }
                break;
            case SettingType::LOADING:
                this->m_progressBar = new brls::ProgressDisplay(brls::ProgressDisplayFlags::PERCENTAGE);
                this->m_progressBar->setProgress(0, 100);
                this->m_progressBar->setParent(this);
                break;
            
        }

        if (this->m_confirmButton)
            this->m_confirmButton->setParent(this);
        if (this->m_description)
            this->m_description->setParent(this);
        if (this->m_progressBar)
            this->m_progressBar->setParent(this);
        if (this->m_settingList)
            this->m_settingList->setParent(this);
    }

    PageMemoryEditor::~PageMemoryEditor() {
        if (this->m_settingList)
            delete this->m_confirmButton;
        if (this->m_settingList)
            delete this->m_settingList;
        if (this->m_description)
            delete this->m_description;
        if (this->m_progressBar)
            delete this->m_progressBar;
    }

    void PageMemoryEditor::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
        if (this->m_settingList)
            this->m_description->frame(ctx);
        if (this->m_settingList)
            this->m_confirmButton->frame(ctx);
        if (this->m_settingList)
            this->m_settingList->frame(ctx);
        if (this->m_progressBar) {
            this->m_progressBar->setProgress(this->m_progress, 100);
            this->m_progressBar->frame(ctx);
        }
    }

    brls::View* PageMemoryEditor::requestFocus(brls::FocusDirection direction, brls::View* oldFocus, bool fromUp) {
        if (this->m_settingType == SettingType::SEARCH_TYPE || this->m_settingType == SettingType::DATA_TYPE || this->m_settingType == SettingType::MEMORY_REGION) {
            if (direction == brls::FocusDirection::LEFT || direction == brls::FocusDirection::NONE)
                return this->m_settingList->defaultFocus(oldFocus);
            else if (direction == brls::FocusDirection::RIGHT)
                return this->m_confirmButton;
            else return nullptr;
        } else if (this->m_settingType == SettingType::VALUE_INPUT) {
            if (direction == brls::FocusDirection::UP || direction == brls::FocusDirection::NONE)
                return this->m_settingList->defaultFocus(oldFocus);
            else if (direction == brls::FocusDirection::DOWN)
                return this->m_confirmButton;
            else return nullptr;
        } else if (this->m_settingType == SettingType::LOADING)
            return nullptr;
        else return nullptr;
    }

    void PageMemoryEditor::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
        if (this->m_settingType == SettingType::DATA_TYPE || this->m_settingType == SettingType::SEARCH_TYPE || this->m_settingType == SettingType::MEMORY_REGION) {
            this->m_description->setBoundaries(750, style->AppletFrame.headerHeightRegular + 50, this->width - 825 - 20, 400);
            this->m_settingList->setBoundaries(20, style->AppletFrame.headerHeightRegular, 750, this->height);
            this->m_confirmButton->setBoundaries(825, 500, 300, style->List.Item.height);

            this->m_description->invalidate();
            this->m_settingList->invalidate();
            this->m_confirmButton->invalidate();
        } else if (this->m_settingType == SettingType::VALUE_INPUT) {
            this->m_description->setBoundaries(50, style->AppletFrame.headerHeightRegular + 50, this->width - 100, 300);
            this->m_settingList->setBoundaries(200, style->AppletFrame.headerHeightRegular + 200, this->width - 400, style->List.Item.height * 2);
            this->m_confirmButton->setBoundaries((this->width - 300) / 2, 500, 300, style->List.Item.height);

            this->m_description->invalidate();
            this->m_settingList->invalidate();
            this->m_confirmButton->invalidate();
        } else if (this->m_settingType == SettingType::LOADING) {
            this->m_progressBar->setBoundaries(
            this->x + this->width / 2 - style->CrashFrame.buttonWidth,
            this->y + this->height / 2,
            style->CrashFrame.buttonWidth * 2,
            style->CrashFrame.buttonHeight);
        }
    }

    bool PageMemoryEditor::onCancel() {
        brls::Application::popView();
        return true;
    }


    void PageMemoryEditor::createSelectionList(brls::List *list, std::vector<std::string> options, std::string description, std::function<void(u8)> selectCallback) {
        this->m_description = new brls::Label(brls::LabelStyle::DESCRIPTION, description, true);
        this->m_description->setHorizontalAlign(NVG_ALIGN_CENTER);    

        for (auto &option : options) {
            auto typeOption = new brls::ListItem(option); 
            typeOption->setClickListener([this, selectCallback](brls::View *view) {
                auto list = static_cast<brls::List*>(view->getParent());

                u8 selectedIndex = 0;
                for (auto &child : list->getChildren()) {
                    brls::ListItem *listItem = static_cast<brls::ListItem*>(child->view);
                    listItem->setChecked(listItem == view);
                    this->m_confirmButton->setState(brls::ButtonState::ENABLED);
                    
                    selectCallback(selectedIndex);
                }
            });

            this->m_settingList->addView(typeOption);
        }
    }

}