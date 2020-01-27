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

#include "ui/gui_hex_editor.hpp"
#include "ui/elements/hex_editor.hpp"

#include "cheat/cheat.hpp"

namespace edz::ui {


    addr_t parseAddressString(std::string str) {
        enum class Operation {
            None = 0,
            Plus = 1,
            Minus = -1
        };

        static addr_t HEAP = cheat::CheatManager::getHeapRegion().baseAddress;
        static addr_t MAIN = cheat::CheatManager::getMainRegion().baseAddress;

        Operation currentOperation = Operation::Plus;
        addr_t result = 0;
        char *endPtr = nullptr;
        u32 previousIndex = 0;

        for (u32 i = 0; i < str.length();) {
            if (isspace(str[i]))
                i++;
            if (str[i] == '+') {
                currentOperation = Operation::Plus;
                i++;
            } else if (str[i] == '-') {
                currentOperation = Operation::Minus;
                i++;
            }
            else if (strncasecmp(&str[i], "HEAP", 4) == 0) {
                result += (HEAP * std::uint64_t(currentOperation));
                i += 4;
                currentOperation = Operation::None;
            }
            else if (strncasecmp(&str[i], "MAIN", 4) == 0) {
                result += (MAIN * std::uint64_t(currentOperation));
                i += 4;
                currentOperation = Operation::None;
            }
            else {
                std::uint64_t res = strtol(&str[i], &endPtr, 0);
                result += (res * std::uint64_t(currentOperation));
                i += (endPtr - &str[i]);
                currentOperation = Operation::None;
            }

            if (previousIndex == i)
                i++;
            
            previousIndex = i;
        }

        return result;
    }


    brls::View* GuiHexEditor::setupUI() {
        brls::AppletFrame *rootFrame = new brls::AppletFrame(false, false);

        rootFrame->setTitle("edz.gui.hexeditor.title"_lang);

        cheat::CheatManager::readMemory(this->m_address, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);

        auto editor = new ui::element::HexEditor();
        editor->setBuffer(this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
        editor->setDisplayAddress(this->m_address);

        editor->setClickListener([this, editor](ui::element::HexEditor::SelectionType type, addr_t address, u64 value) {
            if (type == ui::element::HexEditor::SelectionType::BYTE) {
                hlp::openSwkbdForNumber([&, editor](std::string str) {
                    u8 valueToWrite = strtoul(str.c_str(), nullptr, 10);
                    cheat::CheatManager::writeMemory(address, &valueToWrite, 1);
                    cheat::CheatManager::readMemory(this->m_address, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
                    editor->reloadMemory(false);
                }, "edz.widget.integer.title"_lang, "", "", "", 3);
            } else {
                if (value == 0)
                    return;

                this->m_returnStack.push_back(address);
                this->m_pointerStack.push_back(value);
                editor->setAddressHistory(this->m_returnStack, this->m_pointerStack);
                this->m_address = value;
                cheat::CheatManager::readMemory(this->m_address, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
                editor->setDisplayAddress(this->m_address);
            }
        });

        editor->addHint("Back",   brls::Key::B, [this, editor] { 
            if (this->m_returnStack.size() > 0) {
                addr_t lastAddress = this->m_returnStack.back();

                this->m_returnStack.pop_back();
                this->m_pointerStack.pop_back();
                editor->setAddressHistory(this->m_returnStack, this->m_pointerStack);
                this->m_address = lastAddress;
                cheat::CheatManager::readMemory(lastAddress, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
                editor->setDisplayAddress(lastAddress); 
            } else 
                brls::Application::popView();

            return true;
        });
        editor->addHint("Down", brls::Key::R, [this, editor] {
            static cheat::types::Region heapRegion = cheat::CheatManager::getHeapRegion();
            static cheat::types::Region mainRegion = cheat::CheatManager::getMainRegion();

            addr_t newAddress = 0x00;
            if (heapRegion.contains(editor->getDisplayAddress()))
                newAddress = std::min(heapRegion.baseAddress + heapRegion.size, editor->getDisplayAddress() + 0x110);
            else if (mainRegion.contains(editor->getDisplayAddress()))
                newAddress = std::min(mainRegion.baseAddress + heapRegion.size, editor->getDisplayAddress() + 0x110);
            else
                newAddress = editor->getDisplayAddress() + 0x110;

            cheat::CheatManager::readMemory(newAddress, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
            editor->setDisplayAddress(newAddress);
            this->m_address = newAddress;
            return true;
        });
        editor->addHint("Up",   brls::Key::L, [this, editor] { 
            static cheat::types::Region heapRegion = cheat::CheatManager::getHeapRegion();
            static cheat::types::Region mainRegion = cheat::CheatManager::getMainRegion();

            addr_t newAddress = 0x00;
            if (heapRegion.contains(editor->getDisplayAddress()))
                newAddress = std::max(heapRegion.baseAddress, editor->getDisplayAddress() - 0x110);
            else if (mainRegion.contains(editor->getDisplayAddress()))
                newAddress = std::max(mainRegion.baseAddress, editor->getDisplayAddress() - 0x110);
            else
                newAddress = editor->getDisplayAddress() - 0x110;

            cheat::CheatManager::readMemory(newAddress, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
            editor->setDisplayAddress(newAddress); 
            this->m_address = newAddress;
            return true;
        });
        editor->addHint("Goto",   brls::Key::Y, [this, editor] { 
            hlp::openSwkbdForText([this, editor](std::string str) {
                addr_t newAddress = parseAddressString(str) & ~0x0F;
                cheat::CheatManager::readMemory(newAddress, this->m_buffer, GuiHexEditor::HEX_EDITOR_SIZE);
                editor->setDisplayAddress(newAddress); 
            }, "Enter a expression", "Valid tokens are HEAP, MAIN, +, -, decimals and hexadecimals ( prefixed with 0x )");

            return true;
        });


        rootFrame->setContentView(editor);

        return rootFrame;
    }

    void GuiHexEditor::update() {

    }

}