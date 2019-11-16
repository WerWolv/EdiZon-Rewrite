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
#include "ui/elements/popup_list.hpp"
#include "helpers/config_manager.hpp"

#include "save/title.hpp"

#include <cmath>

namespace edz::ui {

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

    void GuiCheatEngine::createSearchSettings(brls::LayerView *layerView) {
        layerView->addLayer(createValueSearchSettings(true));
        layerView->addLayer(createValueSearchSettings(false));

        layerView->changeLayer(0, true);
    }

    brls::List* GuiCheatEngine::createValueSearchSettings(bool knownValue) {
        static std::vector<brls::SelectListItem*> scanTypeItems;
        brls::List *list = new brls::List();
        
        auto foundAddresses = new brls::ListItem("Found Addresses");

        auto scanTypeItem   = new brls::SelectListItem("Scan Type", { "Exact Value", "Greater than...", "Less than...", "Value between...", "Unknown initial value"});
        auto valueTypeItem  = new brls::SelectListItem("Value Type", { "1 Byte", "2 Bytes", "4 Bytes", "8 Bytes", "Float", "Double", "String" });
        auto scanRegionItem = new brls::SelectListItem("Scan Region", { "HEAP", "MAIN", "HEAP + MAIN", "Everything" });
        auto fastScan       = new brls::ToggleListItem("Fast Scanning Mode", true, "If enabled, only aligned values will be searched");

        foundAddresses->setValue("0");

        scanTypeItems.push_back(scanTypeItem);
        scanTypeItem->setListener([this](s32 selection) {
            //this->m_scanType = static_cast<ScanType>(selection);

            switch (selection) {
                case 0 ... 3: this->m_searchSettings->changeLayer(0, true);  break;
                case 4:       this->m_searchSettings->changeLayer(1, true);  break;
            }

            for (auto& item : scanTypeItems)
                item->setSelectedValue(selection);
        });

        valueTypeItem->setListener([this](s32 selection) {
            constexpr s8 valueSizes[] = { 1, 2, 4, 8, 4, 8, -1 };

            /*switch (selection) {
                case 0 ... 3: this->m_inputMenuType = InputMenuType::INTEGER; break;
                case 4 ... 5: this->m_inputMenuType = InputMenuType::FLOAT;   break;
                case 6:       this->m_inputMenuType = InputMenuType::STRING;  break;
            }*/

            this->m_valueSize = valueSizes[selection];
        });

        scanRegionItem->setListener([this](s32 selection) {
            //this->m_scanRegion = static_cast<ScanRegion>(selection);
        });

        list->addView(foundAddresses);
        list->addView(new brls::Header("Search Settings", false));
        list->addView(scanTypeItem);
        list->addView(valueTypeItem);
        list->addView(scanRegionItem);

        if (knownValue) {
            auto valueItem = new brls::ListItem("Search Value");
            valueItem->setClickListener([=](brls::View *view) { 
                //this->openInputMenu(this->m_dataType, this->m_scanType == ScanType::BETWEEN, this->m_valueSize, valueItem);
            });
            list->addView(valueItem);
        }

        list->addView(fastScan);

        return list;
    }

    brls::List* GuiCheatEngine::createPointerSearchSettings() {
        auto foundAddresses = new brls::ListItem("Found Addresses");

        auto scanTypeItem   = new brls::SelectListItem("Scan Type", { "Exact Value", "Greater than...", "Less than...", "Value between...", "Unknown initial value"});
        auto scanRegionItem = new brls::SelectListItem("Scan Region", { "HEAP", "MAIN", "HEAP + MAIN", "Everything" });
    }

    void GuiCheatEngine::openInputMenu(cheat::DataType inputType, bool range, size_t valueSize, brls::ListItem *searchValueItem) {
        ui::element::PopupList *dialog = new ui::element::PopupList("Cancel", "Apply", [](brls::View*) {
            brls::Application::popView();
        }, [=](brls::View*) {
            brls::Application::popView();
        });

        if(range) {
            auto upperLimit = new brls::ListItem("Upper Search Limit");
            auto lowerLimit = new brls::ListItem("Lower Search Limit");

            upperLimit->setValue("0");
            lowerLimit->setValue("0");

            upperLimit->setClickListener([=](brls::View *view) {
                hlp::openSwkbdForNumber([=](std::string numString) {

                    if (this->m_value1 != nullptr)
                        delete[] this->m_value1;
                    this->m_value1 = new u8[valueSize];
                    
                    if (inputType == cheat::DataType::UNSIGNED || inputType == cheat::DataType::SIGNED) {
                        s128 value = std::strtoll(numString.c_str(), nullptr, 10);
                        std::memcpy(this->m_value1, &value, valueSize);

                        if (value != 0)
                            upperLimit->setValue(numString);
                        else 
                            upperLimit->setValue("0");
                    } else if (inputType == cheat::DataType::FLOATING_POINT) {
                        double value = std::strtod(numString.c_str(), nullptr);
                        std::memcpy(this->m_value1, &value, valueSize);

                        if (value != 0)
                            upperLimit->setValue(std::to_string(value));
                        else 
                            upperLimit->setValue("0.0");
                    }

                }, "Enter Search Value", "Enter the value you want to search for", "-", inputType == cheat::DataType::FLOATING_POINT ? "." : "", std::ceil(std::log10(std::pow(2, valueSize))));
            });
            lowerLimit->setClickListener([=](brls::View *view) {
                hlp::openSwkbdForNumber([=](std::string numString) {

                    if (this->m_value2 != nullptr)
                        delete[] this->m_value2;
                    this->m_value2 = new u8[valueSize];
                    
                    if (inputType == cheat::DataType::UNSIGNED || inputType == cheat::DataType::SIGNED) {
                        s128 value = std::strtoll(numString.c_str(), nullptr, 10);
                        std::memcpy(this->m_value2, &value, valueSize);

                        if (value != 0)
                            lowerLimit->setValue(numString);
                        else 
                            lowerLimit->setValue("0");
                    } else if (inputType == cheat::DataType::FLOATING_POINT) {
                        double value = std::strtod(numString.c_str(), nullptr);
                        std::memcpy(this->m_value2, &value, valueSize);

                        if (value != 0)
                            lowerLimit->setValue(std::to_string(value));
                        else 
                            lowerLimit->setValue("0.0");
                    }

                }, "Enter Search Value", "Enter the value you want to search for", "-", inputType == cheat::DataType::FLOATING_POINT ? "." : "", std::ceil(std::log10(std::pow(2, valueSize * 8))));
            });

            dialog->addItem(upperLimit);
            dialog->addItem(lowerLimit);

        }
        else {
            auto searchValue = new brls::ListItem("Search Value");
            searchValue->setValue("0");
            searchValue->setClickListener([=](brls::View *view) {
                hlp::openSwkbdForNumber([=](std::string numString) {

                    if (this->m_value1 != nullptr)
                        delete[] this->m_value1;
                    this->m_value1 = new u8[valueSize];
                    
                    if (inputType == cheat::DataType::UNSIGNED || inputType == cheat::DataType::SIGNED) {
                        s128 value = std::strtoll(numString.c_str(), nullptr, 10);
                        std::memcpy(this->m_value1, &value, valueSize);

                        if (value != 0)
                            searchValue->setValue(numString);
                        else 
                            searchValue->setValue("0");
                    } else if (inputType == cheat::DataType::FLOATING_POINT) {
                        double value = std::strtod(numString.c_str(), nullptr);
                        std::memcpy(this->m_value1, &value, valueSize);

                        if (value != 0)
                            searchValue->setValue(std::to_string(value));
                        else 
                            searchValue->setValue("0.0");
                    }

                }, "Enter Search Value", "Enter the value you want to search for", "-", inputType == cheat::DataType::FLOATING_POINT ? "." : "", std::ceil(std::log10(std::pow(2, valueSize * 8))));
            });

            dialog->addItem(searchValue);
        }

        dialog->open();
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
        

        this->m_searchSettings = new brls::LayerView();
        createSearchSettings(this->m_searchSettings);

        this->m_rootFrame->setContentView(this->m_searchSettings);
        this->m_rootFrame->getSidebar()->getButton()->setClickListener([=](brls::View*) {
            //cheat::CheatEngine::findPattern(this->m_value1, this->m_valueSize, )
        });

        return m_rootFrame;
    }

    void GuiCheatEngine::update() {

    }

}