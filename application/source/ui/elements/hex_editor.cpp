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

#include "ui/elements/hex_editor.hpp"

#include <locale>
#include <codecvt>

#include "ui/fonts.hpp"
#include "helpers/utils.hpp"
#include "cheat/cheat.hpp"

namespace edz::ui::element {

    HexEditor::HexEditor() {
        this->m_clickCallback = [](SelectionType, addr_t, u8) { };

        this->m_historyLabel = new brls::Label(brls::LabelStyle::MEDIUM, "");
        this->m_historyLabel->setHorizontalAlign(NVG_ALIGN_CENTER);
        this->m_historyLabel->setParent(this);

        this->registerAction("OK",   brls::Key::A, [this] { return this->onClick(); });

        this->m_selectX = 0;
        this->m_selectY = 1;
        this->m_selectWidth = 1;
    }

    HexEditor::~HexEditor() {
    }


    void HexEditor::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) {
        // Separator line
        nvgBeginPath(vg);
        nvgFillColor(vg, a(ctx->theme->textColor));
        nvgRect(vg, x, y + 23, width, 1);
        nvgFill(vg);
        
        nvgFontFaceId(vg, Fonts::getMonoSpaceFont());
        nvgFontSize(vg, 25);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

        // Hex-Selector
        {
            float selectionBounds[4];
            float paddingBounds[4];
            nvgTextBounds(vg, x, y, this->m_linesCache[this->m_selectY].c_str() + 31 + this->m_selectX * 3, this->m_linesCache[this->m_selectY].c_str() + 31 + this->m_selectX * 3 + (3 * this->m_selectWidth - 1), selectionBounds);
            nvgTextBounds(vg, x, y, this->m_linesCache[this->m_selectY].c_str(), this->m_linesCache[this->m_selectY].c_str() + 28 + (3 * this->m_selectX), paddingBounds);
            
            nvgFillColor(vg, a(ctx->theme->highlightColor1));
            nvgBeginPath(vg);
            nvgRect(vg, selectionBounds[0] + paddingBounds[2], selectionBounds[1] + this->m_selectY * 26, selectionBounds[2] - selectionBounds[0], selectionBounds[3] - selectionBounds[1]);
            nvgFill(vg);
        }

        nvgFillColor(vg, a(ctx->theme->activeTabColor));

        // Hex editor
        u8 lineNumber = 0;
        for (std::string &line : this->m_linesCache) {
            nvgBeginPath(vg);
            nvgText(vg, x, y + lineNumber * 26, line.c_str(), nullptr);
            
            nvgFillColor(vg, lineNumber % 2 == 0 ? a(ctx->theme->textColor) : a(ctx->theme->descriptionColor));
            lineNumber++;
        }

        this->m_historyLabel->frame(ctx);
    }

    void HexEditor::layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash) {
        this->setBoundaries(50, style->AppletFrame.headerHeightRegular + 20, 1280 - 90, 720 - style->AppletFrame.headerHeightRegular - style->AppletFrame.footerHeight);
    
        this->m_historyLabel->setBoundaries(50, 720 - style->AppletFrame.footerHeight - 50, 1280 - 100, 50);
    }

    brls::View* HexEditor::getNextFocus(brls::FocusDirection direction, void* oldFocus) {    
        switch (direction) {
            case brls::FocusDirection::UP:
                this->m_selectY = std::max(1, this->m_selectY - 1);
                break;
            case brls::FocusDirection::DOWN:
                this->m_selectY = std::min(17, this->m_selectY + 1);
                break;
            case brls::FocusDirection::LEFT:
                this->m_selectX = std::max(0, this->m_selectX - this->m_selectWidth);
                break;
            case brls::FocusDirection::RIGHT:
                this->m_selectX = std::min(16 - this->m_selectWidth, this->m_selectX + this->m_selectWidth);
                break;
        }   
        
        return this->getDefaultFocus();
    }

    brls::View* HexEditor::getDefaultFocus() {
        this->m_selectWidth = static_cast<u8>(this->m_selectionType[(this->m_selectY - 1) * 2 + this->m_selectX / 8]);
        this->m_selectX = (this->m_selectX / this->m_selectWidth) * this->m_selectWidth;

        return this;
    }

    bool HexEditor::onClick() {
        if (this->m_clickCallback == nullptr)
            return true;

        SelectionType type = this->m_selectionType[(this->m_selectY - 1) * 2 + this->m_selectX / 8];
        offset_t offset = (this->m_selectY - 1) * 16 + this->m_selectX;

        if (type == SelectionType::BYTE)
            this->m_clickCallback(type, this->m_address + offset, this->m_buffer[offset]);
        else if (type == SelectionType::ADDRESS)
            this->m_clickCallback(type, this->m_address + offset, reinterpret_cast<u64*>(this->m_buffer)[(this->m_selectY - 1) * 2 + this->m_selectX / 8]);

        return true;
    }



    void HexEditor::setBuffer(u8* buffer, size_t size) {
        this->m_buffer = buffer;
        this->m_size = size;

        reloadMemory(true);
    }

    void HexEditor::setDisplayAddress(addr_t address) {
        // Make sure address is 16 byte aligned 

        this->m_address = address & ~0x0F;

        this->m_selectX = address & 0x0F;
        this->m_selectY = 1;

        reloadMemory(false);

        this->m_selectWidth = static_cast<u8>(this->m_selectionType[(this->m_selectY - 1) * 2 + this->m_selectX / 8]);
        this->m_selectX = (this->m_selectX / this->m_selectWidth) * this->m_selectWidth;
    }

    addr_t HexEditor::getDisplayAddress() {
        return this->m_address;
    }


    void HexEditor::setDisplayType(DisplayType displayType) {
        this->m_displayType = displayType;
    }

    void HexEditor::reloadMemory(bool resetCursor) {
        rebuildCache(this->m_displayType);

        if (resetCursor) {
            this->m_selectX = 0;
            this->m_selectY = 1;

            if (this->m_selectionType.size() != 0)
                this->m_selectWidth = static_cast<u8>(this->m_selectionType[0]);
            else
                this->m_selectWidth = 1;
        }
    }


    void HexEditor::setClickListener(EditCallback callback) {
        if (callback != nullptr)
            this->m_clickCallback = callback;
    }


    void HexEditor::rebuildCache(DisplayType displayType) {
        if (this->m_buffer == nullptr || this->m_size == 0 || this->m_size % 0x10 != 0)
            return;

        this->m_selectionType.clear();
        this->m_linesCache.clear();

        offset_t currOffset = 0x00;
        u32 lineNumber = 0;

        this->m_linesCache.push_back("        == Offset ==           00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F    == ASCII ==");
        do {
            std::string currLine;

            static cheat::types::Region baseRegion = cheat::CheatManager::getBaseRegion();
            static cheat::types::Region heapRegion = cheat::CheatManager::getHeapRegion();
            static cheat::types::Region mainRegion = cheat::CheatManager::getMainRegion();

            // Address list
            if (heapRegion.contains(this->m_address + currOffset))
                currLine += hlp::formatString("[ HEAP + 0x%016lX ]", this->m_address + currOffset - heapRegion.baseAddress);
            else if (mainRegion.contains(this->m_address + currOffset))
                currLine += hlp::formatString("[ MAIN + 0x%016lX ]", this->m_address + currOffset - mainRegion.baseAddress);
            else
                currLine += hlp::formatString("[ BASE + 0x%016lX ]", this->m_address + currOffset - baseRegion.baseAddress);

            // Padding
            currLine += "  ";

            // Hex-View

            if (displayType == DisplayType::POINTER_VIEW) {
                u64 *potentialAddress = reinterpret_cast<u64*>(this->m_buffer + currOffset);

                for (u8 i = 0; i < 2; i++) {
                    if (heapRegion.contains(potentialAddress[i])) {
                        currLine += hlp::formatString("[  HEAP + 0x%08lX  ] ", potentialAddress[i] - heapRegion.baseAddress);
                        this->m_selectionType.push_back(SelectionType::ADDRESS);
                    }
                    else if (mainRegion.contains(potentialAddress[i])) {
                        currLine += hlp::formatString("[  MAIN + 0x%08lX  ] ", potentialAddress[i] - mainRegion.baseAddress);
                        this->m_selectionType.push_back(SelectionType::ADDRESS);
                    }
                    else {
                        for (u8 byte = 0x08 * i; byte < 0x08 + 0x08 * i; byte++)
                            currLine += hlp::formatString("%02lX ", this->m_buffer[currOffset + byte]);
                        
                        this->m_selectionType.push_back(SelectionType::BYTE);
                    }
                }
            }
            else if (displayType == DisplayType::HEX_VIEW) {
                for (u8 byte = 0; byte < 0x10; byte++)
                    currLine += hlp::formatString("%02lX ", this->m_buffer[currOffset + byte]);

                this->m_selectionType.push_back(SelectionType::BYTE);
            }

            // Padding
            currLine += " ";

            // ASCII-View
            std::wstring asciiDisplay;
            for (u8 byte = 0; byte < 0x10; byte++) {
                u8 &currChar = this->m_buffer[currOffset + byte];
                if (currChar <= ' ' || (currChar >= 0x7F && currChar <= 0xA0))
                    asciiDisplay += '.';
                else
                    asciiDisplay += this->m_buffer[currOffset + byte];

            }

            currLine += std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(asciiDisplay);

            this->m_linesCache.push_back(currLine);

            currOffset += 0x10;
            lineNumber++;

        } while (size_t(currOffset) < this->m_size && lineNumber < 17);
    }

    void HexEditor::setAddressHistory(std::vector<addr_t> &returnStack, std::vector<addr_t> &pointerStack) {
        static cheat::types::Region baseRegion = cheat::CheatManager::getBaseRegion();
        static cheat::types::Region heapRegion = cheat::CheatManager::getHeapRegion();
        static cheat::types::Region mainRegion = cheat::CheatManager::getMainRegion();

        std::string historyString;

        cheat::types::Region currRegion;
        if (returnStack.size() > 0 && pointerStack.size() > 0) {

            for (u16 i = std::max(s32(returnStack.size()) - 6, 0); i < returnStack.size(); i++)
                historyString += "[ ";
            
            if (returnStack.size() > 6) {
                historyString += "\uE090";
            } else {
                if (heapRegion.contains(returnStack[0])) {
                    currRegion = heapRegion;
                    historyString += "HEAP";
                }
                else if (mainRegion.contains(returnStack[0])) {
                    currRegion = mainRegion;
                    historyString += "MAIN";
                }
                else {
                    currRegion = baseRegion;
                    historyString += "BASE";
                }
            }


            for (u16 i = std::max(s32(returnStack.size()) - 6, 0); i < returnStack.size(); i++) {
                if (i == 0)
                    historyString += hlp::formatString(" + 0x%08lx ]", offset_t(returnStack[0]) - currRegion.baseAddress);
                else if (returnStack[i] >= pointerStack[i - 1])
                    historyString += hlp::formatString(" + 0x%08lx ]", offset_t(returnStack[i] - currRegion.baseAddress) - (pointerStack[i - 1] - currRegion.baseAddress));
                else
                    historyString += hlp::formatString(" - 0x%08lx ]", offset_t(pointerStack[i - 1] - currRegion.baseAddress) - (returnStack[i] - currRegion.baseAddress));
            }

        }

        this->m_historyLabel->hide([this, historyString]{ this->m_historyLabel->setText(historyString); this->m_historyLabel->show([]{}); });
    }
}