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

#include "ui/gui_hex_editor.hpp"
#include "ui/elements/hex_editor.hpp"

#include "cheat/cheat.hpp"

namespace edz::ui {

    brls::View* GuiHexEditor::setupUI() {
        brls::AppletFrame *rootFrame = new brls::AppletFrame(false, false);

        rootFrame->setTitle("Hex Memory View");

        cheat::CheatManager::readMemory(this->m_address, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);

        auto editor = new ui::element::HexEditor();
        editor->setBuffer(this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
        editor->setDisplayAddress(this->m_address);
        editor->setClickListener([this, editor](ui::element::HexEditor::SelectionType type, addr_t address, u8 value) {
            if (type == ui::element::HexEditor::SelectionType::BYTE) {
                hlp::openSwkbdForNumber([&, editor](std::string str) {
                    u8 valueToWrite = strtoul(str.c_str(), nullptr, 10);
                    cheat::CheatManager::writeMemory(address, &valueToWrite, 1);
                    cheat::CheatManager::readMemory(this->m_address, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
                    editor->reloadMemory();
                }, "Enter a new value", "", "", "", 3);
            } else {
                brls::Logger::debug("%lx %lx", address, value);
                this->m_address = address;
                cheat::CheatManager::readMemory(this->m_address, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
                editor->setDisplayAddress(this->m_address);
            }
        });

        rootFrame->setContentView(editor);

        return rootFrame;
    }

    void GuiHexEditor::update() {

    }

}