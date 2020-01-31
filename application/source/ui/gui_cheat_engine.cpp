/**
 * Copyright (C) 2019 - 2020 WerWolv
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
#include "ui/gui_game_image.hpp"

#include "ui/elements/popup_list.hpp"
#include "helpers/config_manager.hpp"

#include "save/title.hpp"

#include "cheat/cheat.hpp"
#include "cheat/cheat_engine_types.hpp"
#include "cheat/cheat_engine.hpp"

#include <cmath>

namespace edz::ui {

    std::vector<cheat::types::Region> GuiCheatEngine::getHeapRegions() {
        std::vector<cheat::types::Region> out;

        auto heapRegion = cheat::CheatManager::getHeapRegion();
        for (auto memInfo : cheat::CheatManager::getMemoryRegions()) {
            if (heapRegion.contains(memInfo.addr))
                out.push_back({ memInfo.addr, memInfo.size });
        }
        
        return out;
    }

    std::vector<cheat::types::Region> GuiCheatEngine::getMainRegions() {
        std::vector<cheat::types::Region> out;

        auto mainRegion = cheat::CheatManager::getMainRegion();
        for (auto memInfo : cheat::CheatManager::getMemoryRegions()) {
            if (mainRegion.contains(memInfo.addr))
                out.push_back({ memInfo.addr, memInfo.size });
        }
        
        return out;
    }


    void GuiCheatEngine::createKnownPrimarySearchLayer(brls::LayerView *layerView) {
        auto *list = new brls::List();

        this->m_knownPrimarySearchType   = new brls::SelectListItem("Search Type", { "Exactly...", "Greater than...", "Less than...", "Between...", "Unknown value..." }, 0);
        this->m_knownPrimarySearchRegion = new brls::SelectListItem("Region", { "HEAP", "MAIN", "HEAP and MAIN" }, 0);
        this->m_knownPrimaryDataType     = new brls::SelectListItem("Data Type", { "Unsigned", "Signed", "Floating Point" }, 0);
        this->m_knownPrimaryValue        = new brls::IntegerInputListItem("Value", 0, "Set pattern to search for");
        this->m_knownPrimarySize         = new brls::IntegerInputListItem("Size", 1, "Set search Size");
        this->m_knownPrimaryAligned      = new brls::ToggleListItem("Aligned Search", true);


        this->m_knownPrimarySearchType->setListener([this](s32 selection) {
            this->m_unknownPrimarySearchType->setSelectedValue(selection);

            switch (selection) {
                case 0: // Exact match
                    this->m_knownOperation = STRATEGY(==);
                    break;
                case 1: // Greater than
                    this->m_knownOperation = STRATEGY(>);
                    break;
                case 2: // Less than
                    this->m_knownOperation = STRATEGY(<);
                case 3: // Between
                    // TODO: Add this somehow
                    break;
                case 4: // Unknown
                    this->m_nextSearchLayer = SearchLayer::UnknownPrimary;
                    break;
                default: break;
            }
        });

        this->m_knownPrimarySearchRegion->setListener([this](s32 selection) {
            this->m_unknownPrimarySearchRegion->setSelectedValue(selection);

            this->m_regions.clear();
            switch (selection) {
                case 0: // HEAP
                    for (auto &region : this->getHeapRegions())
                        this->m_regions.push_back(region);
                    break;
                case 1: // MAIN
                    for (auto &region : this->getMainRegions())
                        this->m_regions.push_back(region);
                    break;
                case 2: // HEAP and MAIN
                    for (auto &region : this->getHeapRegions())
                        this->m_regions.push_back(region);

                    for (auto &region : this->getHeapRegions())
                        this->m_regions.push_back(region);
                    break;
            }
        });

        this->m_knownPrimaryDataType->setListener([this](s32 selection) {
            this->m_unknownPrimaryDataType->setSelectedValue(selection);

            switch (selection) {
                case 0: // Unsigned
                    this->m_signedness = cheat::types::Signedness::Unsigned;
                    break;
                case 1: // Signed
                    this->m_signedness = cheat::types::Signedness::Signed;
                    break;
                case 2: // Floating point
                    this->m_signedness = cheat::types::Signedness::Signed;
                    break;
            }
        });

        this->m_knownPrimaryValue->setValue("0");
        this->m_knownPrimarySize->setValue("1");
        this->m_knownPrimarySize->setClickListener([this](brls::View *view) {
            u32 value = std::strtoul(this->m_knownPrimarySize->getValue().c_str(), nullptr, 10);

            if (value < 1) {
                this->m_knownPrimarySize->setValue("1");
                return;
            }

            this->m_unknownPrimarySize->setValue(this->m_knownPrimarySize->getValue());

            this->m_patternSize = value;
            
            if (this->m_pattern != nullptr)
                delete[] this->m_pattern;

            this->m_pattern = new u8[value];
            std::memset(this->m_pattern, 0x00, this->m_patternSize);
        });

        this->m_knownPrimaryAligned->setClickListener([this](brls::View *view) {
            this->m_unknownPrimaryAligned->setToggleState(this->m_knownPrimaryAligned->getToggleState());

            this->m_alignedSearch = this->m_knownPrimaryAligned->getToggleState();
        });


        list->addView(this->m_knownPrimarySearchType);  
        list->addView(this->m_knownPrimarySearchRegion);
        list->addView(this->m_knownPrimaryDataType);
        list->addView(this->m_knownPrimaryValue);       
        list->addView(this->m_knownPrimarySize);        
        list->addView(this->m_knownPrimaryAligned);

        layerView->addLayer(list);
    }

    void GuiCheatEngine::createUnknownPrimarySearchLayer(brls::LayerView *layerView) {
        auto *list = new brls::List();

        this->m_unknownPrimarySearchType   = new brls::SelectListItem("Search Type", { "Exactly...", "Greater than...", "Less than...", "Between...", "Unknown value..." }, 0);
        this->m_unknownPrimarySearchRegion = new brls::SelectListItem("Region", { "HEAP", "MAIN", "HEAP and MAIN" }, 0);
        this->m_unknownPrimaryDataType     = new brls::SelectListItem("Data Type", { "Unsigned", "Signed", "Floating Point" }, 0);
        this->m_unknownPrimarySize         = new brls::IntegerInputListItem("Size", 1, "Set search Size");
        this->m_unknownPrimaryAligned      = new brls::ToggleListItem("Aligned Search", true);


        this->m_unknownPrimarySearchType->setListener([this](s32 selection) {
            this->m_knownPrimarySearchType->setSelectedValue(selection);
            switch (selection) {
                case 0: // Exact match
                    this->m_knownOperation = STRATEGY(==);
                    break;
                case 1: // Greater than
                    this->m_knownOperation = STRATEGY(>);
                    break;
                case 2: // Less than
                    this->m_knownOperation = STRATEGY(<);
                case 3: // Between
                    // TODO: Add this somehow
                    break;
                case 4: // Unknown
                    break;
                default: break;
            }

            if (selection != 4)
                this->m_nextSearchLayer = SearchLayer::KnownPrimary;
        });

        this->m_unknownPrimarySearchRegion->setListener([this](s32 selection) {
            this->m_unknownPrimarySearchRegion->setSelectedValue(selection);

            this->m_regions.clear();
            switch (selection) {
                case 0: // HEAP
                    for (auto &region : this->getHeapRegions())
                        this->m_regions.push_back(region);
                    break;
                case 1: // MAIN
                    for (auto &region : this->getMainRegions())
                        this->m_regions.push_back(region);
                    break;
                case 2: // HEAP and MAIN
                    for (auto &region : this->getHeapRegions())
                        this->m_regions.push_back(region);

                    for (auto &region : this->getHeapRegions())
                        this->m_regions.push_back(region);
                    break;
            }
        });

        this->m_unknownPrimaryDataType->setListener([this](s32 selection) {
            this->m_knownPrimaryDataType->setSelectedValue(selection);

            switch (selection) {
                case 0: // Unsigned
                    this->m_signedness = cheat::types::Signedness::Unsigned;
                    break;
                case 1: // Signed
                    this->m_signedness = cheat::types::Signedness::Signed;
                    break;
                case 2: // Floating point
                    this->m_signedness = cheat::types::Signedness::Signed;
                    break;
            }
        });

        this->m_unknownPrimarySize->setValue("1");
        this->m_unknownPrimarySize->setClickListener([this](brls::View *view) {
            u32 value = std::strtoul(this->m_unknownPrimarySize->getValue().c_str(), nullptr, 10);

            if (value < 1) {
                this->m_unknownPrimarySize->setValue("1");
                return;
            }

            this->m_knownPrimarySize->setValue(this->m_unknownPrimarySize->getValue());

            this->m_patternSize = value;
            
            if (this->m_pattern != nullptr)
                delete[] this->m_pattern;

            this->m_pattern = new u8[value];
            std::memset(this->m_pattern, 0x00, this->m_patternSize);
        });

        this->m_unknownPrimaryAligned->setClickListener([this](brls::View *view) {
            this->m_knownPrimaryAligned->setToggleState(this->m_unknownPrimaryAligned->getToggleState());

            this->m_alignedSearch = this->m_unknownPrimaryAligned->getToggleState();
        });


        list->addView(this->m_unknownPrimarySearchType);  
        list->addView(this->m_unknownPrimarySearchRegion);
        list->addView(this->m_unknownPrimaryDataType);
        list->addView(this->m_unknownPrimarySize);        
        list->addView(this->m_unknownPrimaryAligned);

        layerView->addLayer(list);
    }
    
    void GuiCheatEngine::createKnownSecondarySearchLayer(brls::LayerView *layerView) {
        auto *list = new brls::List();

        auto *table = new brls::Table();


        table->addRow(brls::TableRowType::HEADER, "Known Value Search");
        this->m_knownSecondaryFixedTableRowSearchType    = table->addRow(brls::TableRowType::BODY, "Search Type");
        this->m_knownSecondaryFixedTableRowSearchRegion  = table->addRow(brls::TableRowType::BODY, "Search Region");
        this->m_knownSecondaryFixedTableRowDataType      = table->addRow(brls::TableRowType::BODY, "Data Type");
        this->m_knownSecondaryFixedTableRowSearchSize    = table->addRow(brls::TableRowType::BODY, "Size");
        this->m_knownSecondaryFixedTableRowSearchAligned = table->addRow(brls::TableRowType::BODY, "Aligned");

        this->m_knownSecondaryFoundAddresses    = new brls::ListItem("Found addresses");
        this->m_knownSecondarySearchType        = new brls::SelectListItem("Search Type", { "Exactly...", "Greater than...", "Less than...", "Between..." }, 0);
        this->m_knownSecondaryValue             = new brls::IntegerInputListItem("Value", 1, "Set pattern to search for");

        this->m_knownSecondaryFoundAddresses->setValue("0");

        this->m_knownSecondarySearchType->setListener([this](s32 selection) {
            switch (selection) {
                case 0: // Exact match
                    this->m_knownOperation = STRATEGY(==);
                    break;
                case 1: // Greater than
                    this->m_knownOperation = STRATEGY(>);
                    break;
                case 2: // Less than
                    this->m_knownOperation = STRATEGY(<);
                case 3: // Between
                    // TODO: Add this somehow
                    break;
                default: break;
            }
        });

        this->m_knownSecondaryValue->setValue("0");

        list->addView(this->m_knownSecondaryFoundAddresses);
        list->addView(table);
        list->addView(this->m_knownSecondarySearchType);
        list->addView(this->m_knownSecondaryValue);

        layerView->addLayer(list);
    }
    
    void GuiCheatEngine::createUnknownSecondarySearchLayer(brls::LayerView *layerView) {
        auto *list = new brls::List();

        auto *table = new brls::Table();


        table->addRow(brls::TableRowType::HEADER, "Unknown Value Search");
        this->m_unknownSecondaryFixedTableRowSearchType    = table->addRow(brls::TableRowType::BODY, "Search Type");
        this->m_unknownSecondaryFixedTableRowSearchRegion  = table->addRow(brls::TableRowType::BODY, "Search Region");
        this->m_unknownSecondaryFixedTableRowDataType      = table->addRow(brls::TableRowType::BODY, "Data Type");
        this->m_unknownSecondaryFixedTableRowSearchSize    = table->addRow(brls::TableRowType::BODY, "Size");
        this->m_unknownSecondaryFixedTableRowSearchAligned = table->addRow(brls::TableRowType::BODY, "Aligned");
                
        this->m_unknownSecondaryFoundAddresses      = new brls::ListItem("Found addresses");
        this->m_unknownSecondaryUnknownSearchType   = new brls::SelectListItem("Strategy", { "Value stayed the same", "Value changed", "Value Increased", "Value Decreased" }, 0);

        this->m_unknownSecondaryFoundAddresses->setValue("0");

        this->m_unknownSecondaryUnknownSearchType->setListener([this](s32 selection) {
            switch (selection) {
                case 0: // Stayed the same
                    this->m_unknownOperation = STRATEGY(==);
                    break;
                case 1: // Changed
                    this->m_unknownOperation = STRATEGY(!=);
                    break;
                case 2: // Increased
                    this->m_unknownOperation = STRATEGY(>);
                    break;
                case 3: // Decreased
                    this->m_unknownOperation = STRATEGY(<);
                default: break;
            }
        });

        list->addView(this->m_unknownSecondaryFoundAddresses);
        list->addView(table);
        list->addView(this->m_unknownSecondaryUnknownSearchType);

        layerView->addLayer(list);
    }
    

    EResult GuiCheatEngine::startKnownPrimarySearch() {
        Gui::runAsyncWithDialog([this]{
            /*for (auto region : this->m_regions)
                cheat::CheatEngine::findPattern(this->m_pattern, this->m_patternSize, this->m_signedness, this->m_knownOperation, region, this->m_alignedSearch);
*/
            this->m_nextSearchLayer = SearchLayer::KnownSecondary;
        }, "Searching title memory. This may take a while...");

        return ResultSuccess;
    }

    EResult GuiCheatEngine::startKnownSecondarySearch() {
        return ResultSuccess;
    }
    
    EResult GuiCheatEngine::startUnKnownPrimarySearch() {
        this->m_nextSearchLayer = SearchLayer::UnknownSecondary;
        return ResultSuccess;
    }
    
    EResult GuiCheatEngine::startUnKnownSecondarySearch() {
        return ResultSuccess;
    }


    brls::View* GuiCheatEngine::setupUI() {
        this->m_rootFrame = new brls::ThumbnailFrame("edz.gui.cheatengine.sidebar.search"_lang);
        this->m_rootFrame->setTitle("edz.gui.cheatengine.title"_lang);

        this->m_rootFrame->setCancelListener([this](brls::View *view) {
            Gui::goBack();

            return true;
        });

        this->m_rootFrame->registerAction("Memory View", brls::Key::X, [this]() {
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

        this->m_rootFrame->registerAction("Reset Search", brls::Key::MINUS, [this]() {
            if (this->m_selectedSearchLayer == SearchLayer::KnownSecondary) {
                this->m_nextSearchLayer = SearchLayer::KnownPrimary;
                Gui::runLater([this]{ brls::Application::requestFocus(this->m_rootFrame, brls::FocusDirection::NONE); }, 10);
            }
            else if (this->m_selectedSearchLayer == SearchLayer::UnknownSecondary) {
                this->m_nextSearchLayer = SearchLayer::UnknownPrimary;
                Gui::runLater([this]{ brls::Application::requestFocus(this->m_rootFrame, brls::FocusDirection::NONE); }, 10);
            }
                if (this->m_pattern != nullptr)
                    delete[] this->m_pattern;

                this->m_pattern = new u8[1];
                this->m_pattern[0] = 0x00;
                this->m_patternSize = 0;
                this->m_signedness = cheat::types::Signedness::Signed;
                this->m_knownOperation = STRATEGY(==);
                this->m_unknownOperation = STRATEGY(==);
                this->m_regions.clear();
                this->m_alignedSearch = true;

                this->m_knownPrimarySearchType->setSelectedValue(0);
                this->m_knownPrimarySearchRegion->setSelectedValue(0);
                this->m_knownPrimaryDataType->setSelectedValue(0);
                this->m_knownPrimaryValue->setValue("0");
                this->m_knownPrimarySize->setValue("1");
                this->m_knownPrimaryAligned->setToggleState(true);

                this->m_knownSecondaryFoundAddresses->setValue("0");
                this->m_knownSecondarySearchType->setSelectedValue(0);
                this->m_knownSecondaryValue->setValue("0");

                this->m_unknownPrimarySearchType->setSelectedValue(0);
                this->m_unknownPrimarySearchRegion->setSelectedValue(0);
                this->m_unknownPrimaryDataType->setSelectedValue(0);
                this->m_unknownPrimarySize->setValue("0");
                this->m_unknownPrimaryAligned->setToggleState(true);

                this->m_unknownSecondaryFoundAddresses->setValue("0");
                this->m_unknownSecondaryUnknownSearchType->setSelectedValue(0);

            return true;
        });

        this->m_rootFrame->registerAction("Ingame View", brls::Key::Y, []() {
            Gui::changeTo<GuiGameImage>();

            return true;
        });

        // Sidebar 
        std::vector<u8> thumbnailBuffer(1280 * 720 * 4);

        if (save::Title::getLastTitleForgroundImage(&thumbnailBuffer[0]).failed())
            std::fill(thumbnailBuffer.begin(), thumbnailBuffer.end(), 0x80);

        this->m_rootFrame->getSidebar()->setThumbnail(&thumbnailBuffer[0], 1280, 720);
        

        this->m_contentLayers = new brls::LayerView();
        
        this->createKnownPrimarySearchLayer(this->m_contentLayers);
        this->createKnownSecondarySearchLayer(this->m_contentLayers);
        this->createUnknownPrimarySearchLayer(this->m_contentLayers);
        this->createUnknownSecondarySearchLayer(this->m_contentLayers);

        this->m_rootFrame->setContentView(this->m_contentLayers);

        this->m_rootFrame->getSidebar()->getButton()->setClickListener([this](brls::View *view) {
            Gui::runAsyncWithDialog([this] {

                switch (this->m_selectedSearchLayer) {
                    case SearchLayer::None: break;
                    case SearchLayer::KnownPrimary:
                        this->startKnownPrimarySearch();
                        break;
                    case SearchLayer::KnownSecondary:
                        this->startKnownSecondarySearch();
                        break;
                    case SearchLayer::UnknownPrimary:
                        this->startUnKnownPrimarySearch();
                        break;
                    case SearchLayer::UnknownSecondary:
                        this->startUnKnownSecondarySearch();
                        break;
                }

            }, "Searching memory. This might take a while...");
        });

        this->m_pattern = new u8[1];
        this->m_pattern[0] = 0x00;

        this->m_selectedSearchLayer = this->m_nextSearchLayer = SearchLayer::KnownPrimary;

        return this->m_rootFrame;
    }

    void GuiCheatEngine::update() {

        if (this->m_nextSearchLayer != this->m_selectedSearchLayer) {
            this->m_contentLayers->changeLayer(u8(this->m_nextSearchLayer));
            brls::Application::requestFocus(this->m_contentLayers, brls::FocusDirection::NONE);

            this->m_selectedSearchLayer = this->m_nextSearchLayer;

            this->m_knownSecondaryFixedTableRowSearchType->setValue(this->m_knownPrimarySearchType->getValue()); 
            this->m_knownSecondaryFixedTableRowSearchRegion->setValue(this->m_knownPrimarySearchRegion->getValue());
            this->m_knownSecondaryFixedTableRowDataType->setValue(this->m_knownPrimaryDataType->getValue());
            this->m_knownSecondaryFixedTableRowSearchSize->setValue(this->m_knownPrimarySize->getValue());
            this->m_knownSecondaryFixedTableRowSearchAligned->setValue(this->m_knownPrimaryAligned->getValue());

            this->m_knownSecondarySearchType->setSelectedValue(0);

            this->m_unknownSecondaryFixedTableRowSearchType->setValue(this->m_unknownPrimarySearchType->getValue());
            this->m_unknownSecondaryFixedTableRowSearchRegion->setValue(this->m_unknownPrimarySearchRegion->getValue());
            this->m_unknownSecondaryFixedTableRowDataType->setValue(this->m_unknownPrimaryDataType->getValue());
            this->m_unknownSecondaryFixedTableRowSearchSize->setValue(this->m_unknownPrimarySize->getValue());
            this->m_unknownSecondaryFixedTableRowSearchAligned->setValue(this->m_unknownPrimaryAligned->getValue());
        }

    }

}