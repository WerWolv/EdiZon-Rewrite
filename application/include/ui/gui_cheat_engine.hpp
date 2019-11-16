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
#include "cheat/cheat_engine.hpp"

namespace edz::ui {

    class GuiCheatEngine : public Gui {
    public:
        GuiCheatEngine() : Gui() { }
        ~GuiCheatEngine() { }

        brls::View* setupUI() override;
        void update() override;

    private:
        brls::ThumbnailFrame *m_rootFrame;
        brls::LayerView *m_searchSettings;

        enum class ScanType {
            EXACT_VALUE,
            GREATER_THAN,
            LESS_THAN,
            BETWEEN,
            UNKNOWN_VALUE,
            POINTER
        };

        enum class ScanRegion {
            HEAP,
            MAIN,
            HEAP_AND_MAIN,
            EVERYTHING
        };

        cheat::DataType m_dataType = cheat::DataType::UNSIGNED;
        cheat::CompareFunction m_scanType = cheat::cmp::equals;
        cheat::ReadFunction m_scanRegion = cheat::region::readHeap;
        size_t m_valueSize = 1;
        u8 *m_value1 = nullptr, *m_value2 = nullptr;
        std::string m_valueString;

        void setSearchCountText(u16 searchCount);

        void createSearchSettings(brls::LayerView *layerView);
        brls::List* createValueSearchSettings(bool knownValue);
        brls::List* createPointerSearchSettings();


        void openInputMenu(cheat::DataType inputType, bool range, size_t valueSize, brls::ListItem *searchValueItem);

    };

}