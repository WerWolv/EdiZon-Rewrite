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

namespace edz::ui {

    class GuiHexEditor : public Gui {
    public:
        GuiHexEditor(addr_t address) : Gui(), m_address(address) { }
        ~GuiHexEditor() { }

        brls::View* setupUI() override;
        void update() override;

    private:
        static inline const u16 HEX_EDITOR_SIZE = 0x10 * 19;

        addr_t m_address;
        u8 m_buffer[GuiHexEditor::HEX_EDITOR_SIZE];
        std::vector<addr_t> m_returnStack;
        std::vector<addr_t> m_pointerStack;
    };

}