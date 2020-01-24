/**
 * Copyright (C) 2020 WerWolv
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
        brls::ThumbnailFrame    *m_rootFrame;
        brls::LayerView         *m_contentLayers;

        brls::SelectListItem    *m_knownPrimarySearchType;
        brls::SelectListItem    *m_knownPrimarySearchRegion;
        brls::SelectListItem    *m_knownPrimaryDataType;
        brls::ListItem          *m_knownPrimaryValue;
        brls::ListItem          *m_knownPrimarySize;
        brls::ToggleListItem    *m_knownPrimaryAligned;

        brls::TableRow          *m_knownSecondaryFixedTableRowSearchType;
        brls::TableRow          *m_knownSecondaryFixedTableRowSearchRegion;
        brls::TableRow          *m_knownSecondaryFixedTableRowDataType;
        brls::TableRow          *m_knownSecondaryFixedTableRowSearchSize;
        brls::TableRow          *m_knownSecondaryFixedTableRowSearchAligned;
        brls::ListItem          *m_knownSecondaryFoundAddresses;
        brls::SelectListItem    *m_knownSecondarySearchType;
        brls::ListItem          *m_knownSecondaryValue;

        brls::SelectListItem    *m_unknownPrimarySearchType;
        brls::SelectListItem    *m_unknownPrimarySearchRegion;
        brls::SelectListItem    *m_unknownPrimaryDataType;
        brls::ListItem          *m_unknownPrimarySize;
        brls::ToggleListItem    *m_unknownPrimaryAligned;

        brls::TableRow          *m_unknownSecondaryFixedTableRowSearchType;
        brls::TableRow          *m_unknownSecondaryFixedTableRowSearchRegion;
        brls::TableRow          *m_unknownSecondaryFixedTableRowDataType;
        brls::TableRow          *m_unknownSecondaryFixedTableRowSearchSize;
        brls::TableRow          *m_unknownSecondaryFixedTableRowSearchAligned;
        brls::ListItem          *m_unknownSecondaryFoundAddresses;
        brls::SelectListItem    *m_unknownSecondaryUnknownSearchType;

        enum class SearchLayer : s8 {
            None                = -1,
            KnownPrimary        = 0,
            KnownSecondary      = 1,
            UnknownPrimary      = 2,
            UnknownSecondary    = 3
        };

        SearchLayer m_selectedSearchLayer;
        SearchLayer m_nextSearchLayer;
    };

}