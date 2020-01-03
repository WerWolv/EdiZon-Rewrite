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

#include <any>

#include "ui/gui.hpp"
#include "cheat/cheat_engine.hpp"
#include "ui/elements/hex_editor.hpp"

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
        brls::ListItem *m_foundAddresses = nullptr;

        cheat::types::DataType m_dataType = cheat::types::DataType::U8;
        cheat::types::SearchOperation m_operation = cheat::types::SearchOperation::EQUALS;
        std::vector<cheat::types::Region> m_regions;

        cheat::types::Value m_value[2];

        std::string m_valueString;

        void setSearchCountText(u16 searchCount);

        void createSearchSettings(brls::LayerView *layerView);
        brls::List* createValueSearchSettings(bool knownValue);
        brls::List* createPointerSearchSettings();

        void openInputMenu(cheat::types::DataType inputType, bool range, size_t valueSize, brls::ListItem *searchValueItem);

        static inline cheat::types::Value NOVALUE;
        void handleSearchOperation(std::vector<cheat::types::Region>& regions, cheat::types::SearchOperation operation, cheat::types::Value &value1, cheat::types::Value &value2 = NOVALUE) {
            switch (operation.getOperation()) {
                case cheat::types::SearchOperation::EQUALS:
                    cheat::CheatEngine::findIn(regions, STRATEGY(==), value1, value2);
                    break;
                case cheat::types::SearchOperation::GREATER_THAN:
                    cheat::CheatEngine::findIn(regions, STRATEGY(>), value1, value2);
                    break;
                case cheat::types::SearchOperation::LESS_THAN:
                    cheat::CheatEngine::findIn(regions, STRATEGY(<), value1, value2);
                    break;
                case cheat::types::SearchOperation::BETWEEN:
                case cheat::types::SearchOperation::SAME:
                case cheat::types::SearchOperation::DIFFERENT:
                case cheat::types::SearchOperation::INCREASED:
                case cheat::types::SearchOperation::DECREASED:
                    break;
            }
            
        }

    };

}