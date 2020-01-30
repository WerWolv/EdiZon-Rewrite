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

#pragma once

#include <edizon.hpp>

#include "ui/gui.hpp"
#include "cheat/cheat_engine.hpp"
#include "cheat/dump_file.hpp"
#include "ui/elements/hex_editor.hpp"

namespace edz::ui {

    class GuiCheatEngine : public Gui {
    public:
        GuiCheatEngine() : Gui() { }
        ~GuiCheatEngine() { }

        std::vector<cheat::types::Region> getHeapRegions();
        std::vector<cheat::types::Region> getMainRegions();

        void createKnownPrimarySearchLayer(brls::LayerView *layerView);
        void createKnownSecondarySearchLayer(brls::LayerView *layerView);
        void createUnknownPrimarySearchLayer(brls::LayerView *layerView);
        void createUnknownSecondarySearchLayer(brls::LayerView *layerView);

        EResult startKnownPrimarySearch();
        EResult startKnownSecondarySearch();
        EResult startUnKnownPrimarySearch();
        EResult startUnKnownSecondarySearch();

        brls::View* setupUI() override;
        void update() override;

    private:
        brls::ThumbnailFrame        *m_rootFrame;
        brls::LayerView             *m_contentLayers;

        brls::SelectListItem        *m_knownPrimarySearchType;
        brls::SelectListItem        *m_knownPrimarySearchRegion;
        brls::SelectListItem        *m_knownPrimaryDataType;
        brls::IntegerInputListItem  *m_knownPrimaryValue;
        brls::IntegerInputListItem  *m_knownPrimarySize;
        brls::ToggleListItem        *m_knownPrimaryAligned;

        brls::TableRow              *m_knownSecondaryFixedTableRowSearchType;
        brls::TableRow              *m_knownSecondaryFixedTableRowSearchRegion;
        brls::TableRow              *m_knownSecondaryFixedTableRowDataType;
        brls::TableRow              *m_knownSecondaryFixedTableRowSearchSize;
        brls::TableRow              *m_knownSecondaryFixedTableRowSearchAligned;
        brls::ListItem              *m_knownSecondaryFoundAddresses;
        brls::SelectListItem        *m_knownSecondarySearchType;
        brls::IntegerInputListItem  *m_knownSecondaryValue;

        brls::SelectListItem        *m_unknownPrimarySearchType;
        brls::SelectListItem        *m_unknownPrimarySearchRegion;
        brls::SelectListItem        *m_unknownPrimaryDataType;
        brls::IntegerInputListItem  *m_unknownPrimarySize;
        brls::ToggleListItem        *m_unknownPrimaryAligned;

        brls::TableRow              *m_unknownSecondaryFixedTableRowSearchType;
        brls::TableRow              *m_unknownSecondaryFixedTableRowSearchRegion;
        brls::TableRow              *m_unknownSecondaryFixedTableRowDataType;
        brls::TableRow              *m_unknownSecondaryFixedTableRowSearchSize;
        brls::TableRow              *m_unknownSecondaryFixedTableRowSearchAligned;
        brls::ListItem              *m_unknownSecondaryFoundAddresses;
        brls::SelectListItem        *m_unknownSecondaryUnknownSearchType;

        enum class SearchLayer : s8 {
            None                = -1,
            KnownPrimary        = 0,
            KnownSecondary      = 1,
            UnknownPrimary      = 2,
            UnknownSecondary    = 3
        };

        SearchLayer m_selectedSearchLayer;
        SearchLayer m_nextSearchLayer;

        u8* m_pattern = nullptr;
        std::size_t m_patternSize = 0;
        cheat::types::Signedness m_signedness = cheat::types::Signedness::Signed;
        cheat::types::Operation m_knownOperation = STRATEGY(==);
        cheat::types::Operation m_unknownOperation = STRATEGY(==);
        std::vector<cheat::types::Region> m_regions;
        bool m_alignedSearch = true;
    };

}