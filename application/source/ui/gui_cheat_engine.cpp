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
        
        auto hexViewItem    = new brls::ListItem("Memory View");
        auto foundAddresses = new brls::ListItem("Found Addresses");
        auto clearSearchItem    = new brls::ListItem("Clear Search");

        auto scanTypeItem   = new brls::SelectListItem("Scan Type", { "Exact Value", "Greater than...", "Less than...", "Value between...", "Unknown initial value"});
        auto valueTypeItem  = new brls::SelectListItem("Value Type", { "1 Byte (Unsigned)", "1 Byte (Signed)", "2 Bytes (Unsigned)", "2 Bytes (Signed)", "4 Bytes (Unsigned)", "4 Bytes (Signed)", "8 Bytes (Unsigned)", "8 Bytes (Signed)", "Float", "Double", "String", "Array" });
        auto scanRegionItem = new brls::SelectListItem("Scan Region", { "HEAP", "MAIN", "HEAP + MAIN" });
        auto valueItem      = new brls::ListItem("Search Value");
        auto fastScanItem   = new brls::ToggleListItem("Fast Scanning Mode", true, "If enabled, only aligned values will be searched");

        hexViewItem->setValue(Fonts::MaterialIcons::SUBMENU);
        hexViewItem->setClickListener([=](brls::View *view) {
            auto regionSelection = new brls::Dialog("Look at the HEAP or MAIN section?");

            regionSelection->addButton("HEAP", [=](brls::View *view) {
                Gui::changeTo<GuiHexEditor>(cheat::CheatManager::getHeapRegion().getBase());
            });

            regionSelection->addButton("MAIN", [=](brls::View *view) {
                Gui::changeTo<GuiHexEditor>(cheat::CheatManager::getMainRegion().getBase());
            });

            regionSelection->open();
        });

        foundAddresses->setValue(Fonts::MaterialIcons::SUBMENU);
        foundAddresses->setClickListener([this](brls::View *view) {
            if (cheat::CheatEngine::getFoundAddresses().size() > 0 && cheat::CheatEngine::getFoundAddresses().size() < 50) {
                auto rootFrame = new brls::AppletFrame(true, true);
                auto list = new brls::List();
                

                auto heapRegion = cheat::CheatManager::getHeapRegion();
                auto mainRegion = cheat::CheatManager::getMainRegion();
                auto baseRegion = cheat::CheatManager::getBaseRegion();

                for (auto &address : cheat::CheatEngine::getFoundAddresses()) {
                    brls::ListItem *item = nullptr;
                    u64 buffer = 0;
                    cheat::CheatManager::readMemory(address, &buffer, this->m_dataType.getSize());

                    if (heapRegion.contains(address))
                        item = new brls::ListItem(hlp::formatString("[ HEAP + %08lx ]  ( 0x%lx )", address - heapRegion.getBase(), buffer));
                    else if (mainRegion.contains(address))
                        item = new brls::ListItem(hlp::formatString("[ MAIN + %08lx ]  ( 0x%lx )", address - mainRegion.getBase(), buffer));
                    else
                        item = new brls::ListItem(hlp::formatString("[ BASE + %08lx ]  ( 0x%lx )", address - baseRegion.getBase(), buffer));

                    item->setClickListener([this, address](brls::View *view) {
                        hlp::openSwkbdForNumber([&](std::string str) {
                            s64 valueToWrite = strtoll(str.c_str(), nullptr, 10);
                            cheat::CheatManager::writeMemory(address, &valueToWrite, this->m_dataType.getSize());
                        }, "Enter a new value", "", "", "", 10);
                    });

                    list->addView(item);

                }

                rootFrame->setContentView(list);
                
                brls::PopupFrame::open("Found Addresses", rootFrame);
            }

        });

        clearSearchItem->setClickListener([](brls::View *view) {
            cheat::CheatEngine::clearFoundAddresses();

            hlp::File foundAddressesFile(EDIZON_BASE_DIR "/found_addrs.bin");
            foundAddressesFile.remove();
        });

        scanTypeItems.push_back(scanTypeItem);
        scanTypeItem->setListener([this](s32 selection) {
            printf("Set operation: %d\n", selection);
            switch (selection) {
                case 0: this->m_operation = cheat::types::SearchOperation(cheat::types::SearchOperation::EQUALS);       break;
                case 1: this->m_operation = cheat::types::SearchOperation(cheat::types::SearchOperation::GREATER_THAN); break;
                case 2: this->m_operation = cheat::types::SearchOperation(cheat::types::SearchOperation::LESS_THAN);    break;
                case 3: this->m_operation = cheat::types::SearchOperation(cheat::types::SearchOperation::BETWEEN);      break;
            }

            switch (selection) {
                case 0 ... 3: this->m_searchSettings->changeLayer(0, true);  break;
                case 4:       this->m_searchSettings->changeLayer(1, true);  break;
            }

            for (auto& item : scanTypeItems)
                item->setSelectedValue(selection);
        });

        valueTypeItem->setListener([=](s32 selection) {
            using DataType = cheat::types::DataType;

            const DataType dataTypes[] = { 
                DataType::U8,       DataType::S8,
                DataType::U16,      DataType::S16, 
                DataType::U32,      DataType::S32, 
                DataType::U64,      DataType::S64, 
                DataType::FLOAT,    DataType::DOUBLE, 
                DataType::ARRAY,    DataType::STRING
            };

            if (selection < 0)
                return;
            this->m_dataType = dataTypes[selection];
            valueItem->setValue("0");
            this->m_value[0] = cheat::types::Value(nullptr, this->m_dataType);
        });

        this->m_regions.clear();
        for (const auto& memoryInfo : cheat::CheatManager::getMemoryRegions()) {
            if (memoryInfo.type == MemType_Heap)
                this->m_regions.push_back(cheat::types::Region(memoryInfo.addr, memoryInfo.size));
        }

        scanRegionItem->setListener([this](s32 selection) {
            this->m_regions.clear();
            for (const auto& memoryInfo : cheat::CheatManager::getMemoryRegions()) {
                if ((selection == 0 || selection == 2) && memoryInfo.type == MemType_Heap)
                    this->m_regions.push_back(cheat::types::Region(memoryInfo.addr, memoryInfo.size));
                if ((selection == 1 || selection == 2) && memoryInfo.type == MemType_CodeMutable && (memoryInfo.perm & Perm_Rw) == Perm_Rw)
                    this->m_regions.push_back(cheat::types::Region(memoryInfo.addr, memoryInfo.size));
            }
        });


        list->addView(hexViewItem);
        list->addView(foundAddresses);
        list->addView(clearSearchItem);

        list->addView(new brls::Header("Search Settings", false));
        list->addView(scanTypeItem);
        list->addView(valueTypeItem);
        list->addView(scanRegionItem);

        if (knownValue) {
            valueItem->setValue("0");
            valueItem->setClickListener([=](brls::View *view) { 
                hlp::openSwkbdForNumber([=](std::string numString) {
                    valueItem->setValue(numString);
                    if (this->m_dataType.isSigned()) {
                        s64 val = strtoll(numString.c_str(), nullptr, 10);
                        this->m_value[0] = cheat::types::Value(&val, this->m_dataType, true);
                    }
                    else if (this->m_dataType.isFloatingPoint()) {
                        double val = strtod(numString.c_str(), nullptr);
                        this->m_value[0] = cheat::types::Value(&val, this->m_dataType, true);
                    }
                    else {
                        u64 val = strtoull(numString.c_str(), nullptr, 10);
                        this->m_value[0] = cheat::types::Value(&val, this->m_dataType, true);
                    }


                }, "Enter Search Value", "Enter the value you want to search for", this->m_dataType.isSigned() ? "-" : "", this->m_dataType.isFloatingPoint() ? "." : "", std::ceil(std::log10(std::pow(2, this->m_dataType.getSize() * 8))));
            });
            list->addView(valueItem);
        }

        list->addView(fastScanItem);

        return list;
    }

    brls::List* GuiCheatEngine::createPointerSearchSettings() {
        //this->m_foundAddresses = new brls::ListItem("Found Addresses");

        //auto scanTypeItem   = new brls::SelectListItem("Scan Type", { "Exact Value", "Greater than...", "Less than...", "Value between...", "Unknown initial value"});
        //auto scanRegionItem = new brls::SelectListItem("Scan Region", { "HEAP", "MAIN", "HEAP + MAIN", "Everything" });
        return nullptr;
    }

    /*void GuiCheatEngine::openInputMenu(cheat::types::DataType inputType, bool range, size_t valueSize, brls::ListItem *searchValueItem) {
        brls::ListItem *upperLimitItem = nullptr, *lowerLimitItem = nullptr, *valueItem = nullptr;

        ui::element::PopupList *dialog = new ui::element::PopupList("Cancel", "Apply", [=](brls::View*) {
            if (range)
                searchValueItem->setValue(lowerLimitItem->getValue() + " - " + upperLimitItem->getValue());
            else
                searchValueItem->setValue(valueItem->getValue());
            brls::Application::popView();
        }, [=](brls::View*) {
            brls::Application::popView();
        });

        if(range) {
            upperLimitItem = new brls::ListItem("Upper Search Limit");
            lowerLimitItem = new brls::ListItem("Lower Search Limit");

            upperLimitItem->setValue("0");
            lowerLimitItem->setValue("0");

            upperLimitItem->setClickListener([=](brls::View *view) {
                hlp::openSwkbdForNumber([=](std::string numString) {
                // TODO: Fix
                }, "Enter Search Value", "Enter the value you want to search for", this->m_dataType.isSigned() ? "-" : "", this->m_dataType.isFloatingPoint() ? "." : "", std::ceil(std::log10(std::pow(2, valueSize))));
            });
            lowerLimitItem->setClickListener([=](brls::View *view) {
                hlp::openSwkbdForNumber([=](std::string numString) {
                // TODO: Fix
                }, "Enter Search Value", "Enter the value you want to search for", this->m_dataType.isSigned() ? "-" : "", this->m_dataType.isFloatingPoint() ? "." : "", std::ceil(std::log10(std::pow(2, valueSize * 8))));
            });

            dialog->addItem(upperLimitItem);
            dialog->addItem(lowerLimitItem);

        }
        else {
            valueItem = new brls::ListItem("Search Value");
            valueItem->setValue("0");
            valueItem->setClickListener([=](brls::View *view) {
                hlp::openSwkbdForNumber([=](std::string numString) {
                // TODO: Fix
                }, "Enter Search Value", "Enter the value you want to search for", this->m_dataType.isSigned() ? "-" : "", this->m_dataType.isFloatingPoint() ? "." : "", std::ceil(std::log10(std::pow(2, valueSize * 8))));
            });

            dialog->addItem(valueItem);
        }

        dialog->open();
    }*/


    brls::View* GuiCheatEngine::setupUI() {
        this->m_rootFrame = new brls::ThumbnailFrame("edz.gui.cheatengine.button.search"_lang);

        this->m_rootFrame->setTitle("edz.gui.cheatengine.title"_lang);

        this->m_rootFrame->setCancelListener([this](brls::View *view) {
            this->m_rootFrame = nullptr;
            Gui::goBack();
            return true;
        });

        std::vector<u8> thumbnailBuffer(1280 * 720 * 4);

        if (save::Title::getLastTitleForgroundImage(&thumbnailBuffer[0]).succeeded())
            this->m_rootFrame->getSidebar()->setThumbnail(&thumbnailBuffer[0], 1280, 720);
        

        hlp::File foundAddressesFile(EDIZON_BASE_DIR "/found_addrs.bin");

        if (foundAddressesFile.exists()) {
            addr_t buffer[foundAddressesFile.size() / sizeof(addr_t)];

            cheat::CheatEngine::getFoundAddresses().reserve(foundAddressesFile.size() / sizeof(addr_t));
            foundAddressesFile.read(reinterpret_cast<u8*>(buffer), foundAddressesFile.size());
            std::copy(buffer, buffer + (foundAddressesFile.size() / sizeof(addr_t)), std::back_inserter(cheat::CheatEngine::getFoundAddresses()));
        }


        this->m_searchSettings = new brls::LayerView();
        createSearchSettings(this->m_searchSettings);

        this->m_rootFrame->setContentView(this->m_searchSettings);
        this->m_rootFrame->getSidebar()->getButton()->setClickListener([this](brls::View*) {
            Gui::runAsyncWithDialog([this] { 
                appletSetMediaPlaybackState(true);

                this->handleSearchOperation(this->m_regions, this->m_operation, this->m_value[0]);

                hlp::File foundAddressesFile(EDIZON_BASE_DIR "/found_addrs.bin");
                foundAddressesFile.write(reinterpret_cast<u8*>(&cheat::CheatEngine::getFoundAddresses()[0]), cheat::CheatEngine::getFoundAddresses().size() * sizeof(addr_t));

                appletSetMediaPlaybackState(false);
            }, "Searching memory. This might take a while...");
            
        });

        return m_rootFrame;
    }

    void GuiCheatEngine::update() {
        if (this->m_rootFrame != nullptr)
            this->m_rootFrame->getSidebar()->setSubtitle(hlp::formatString("Found Addresses: %d", cheat::CheatEngine::getFoundAddresses().size()));
    }

}