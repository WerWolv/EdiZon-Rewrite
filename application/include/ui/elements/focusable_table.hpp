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
#include <Borealis.hpp>

#include <string>
#include <vector>

#include "save/title.hpp"

#include <memory>

#include <borealis/theme.hpp>
#include <nanovg.h>

namespace edz::ui::element {

    class FocusableTable : public brls::Table {
    public:
        FocusableTable();
        virtual ~FocusableTable();
        View* requestFocus(brls::FocusDirection direction, View *oldFocus, bool fromUp = false) override;

        void drawHighlight(NVGcontext* vg, brls::ThemeValues* theme, float alpha, brls::Style* style, bool background) override;
    };

}
