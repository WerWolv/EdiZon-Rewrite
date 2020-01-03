/**
 * Copyright (C) 2020 werwolv
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
#include <borealis.hpp>

#include <borealis/theme.hpp>
#include <nanovg.h>

#include <vector>
#include <string>
#include <functional>

#include "cheat/cheat_engine_types.hpp"

namespace edz::ui::element {

    class HexEditor : public brls::View {
    public:
        enum class SelectionType    { BYTE = 1, ADDRESS = 8 };
        enum class DisplayType      { HEX_VIEW, POINTER_VIEW };

        using EditCallback = std::function<void(SelectionType, addr_t, u8)>;

        HexEditor();
        ~HexEditor();
        
        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) override;
        void layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash) override;
        brls::View* requestFocus(brls::FocusDirection direction, View *oldFocus, bool fromUp = false) override;
        void drawHighlight(NVGcontext* vg, brls::ThemeValues* theme, float alpha, brls::Style* style, bool background) override;
        bool onClick() override;

        void setBuffer(u8* buffer, size_t size);
        void setDisplayAddress(addr_t address);
        addr_t getDisplayAddress();
        void setDisplayType(DisplayType displayType);

        void reloadMemory();

        void setClickListener(EditCallback callback);
    
    private:
        u8 *m_buffer = nullptr;
        size_t m_size = 0;
        addr_t m_address = 0x00;

        std::vector<SelectionType> m_selectionType;
        std::vector<std::string> m_linesCache;

        s8 m_selectX = 0, m_selectY = 1, m_selectWidth = 1;

        DisplayType m_displayType = DisplayType::POINTER_VIEW;

        EditCallback m_clickCallback;


        void rebuildCache(DisplayType displayType);
    };

}