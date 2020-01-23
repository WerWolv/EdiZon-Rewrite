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

        void createPrimaryKnownSearchLayer(brls::LayerView *layerView);
        void createPrimaryUnknownSearchLayer(brls::LayerView *layerView);
        void createSecondaryKnownSearchLayer(brls::LayerView *layerView);
        void createSecondaryUnknownSearchLayer(brls::LayerView *layerView);

        brls::View* setupUI() override;
        void update() override;

    private:
        brls::ThumbnailFrame *m_rootFrame;

        brls::Table *m_fixedValues;
    };

}