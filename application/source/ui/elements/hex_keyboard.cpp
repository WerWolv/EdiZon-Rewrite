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

#include "ui/elements/hex_keyboard.hpp"

namespace edz::ui::element {

    HexKeyboard::HexKeyboard(std::string title, KeyboardListener listener, size_t maxInputSize) {
        this->m_topOffset = 400;
        
        static std::string buttonNames[] = { "HEAP", "MAIN", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "A", "B", "C", "D", "E", "F", "+", "-", ".", "0x", "0d", "\uE056" };
        
        u8 i = 0;
        for (auto &button : this->m_buttons) {
            button = new brls::Button(brls::ButtonStyle::PLAIN, buttonNames[i++]);
            button->setParent(this);
        }

        this->m_hint = new brls::Hint();

        this->addHint("Back", brls::Key::B, [this] { return this->onCancel(); });
    }

    HexKeyboard::~HexKeyboard() {

    }

    void HexKeyboard::willAppear() {

    }

    void HexKeyboard::show(std::function<void(void)> cb, bool animate, brls::ViewAnimation animation) {
        View::show(cb);

        brls::menu_animation_ctx_entry_t entry;

        entry.duration     = this->getShowAnimationDuration(animation);
        entry.easing_enum  = brls::EASING_OUT_QUAD;
        entry.subject      = &this->m_topOffset;
        entry.tag          = (uintptr_t) nullptr;
        entry.target_value = 0.0f;
        entry.tick         = [this](void* userdata) { this->invalidate(); };
        entry.userdata     = nullptr;

        menu_animation_push(&entry);
    }

    void HexKeyboard::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
        unsigned top = 720 - (400 - this->m_topOffset);

        // Backdrop
        nvgFillColor(vg, a(ctx->theme->dropdownBackgroundColor));
        nvgBeginPath(vg);
        nvgRect(vg, x, y, width, top);
        nvgFill(vg);

        // TODO: Shadow

        // Background
        nvgFillColor(vg, a(ctx->theme->sidebarColor));
        nvgBeginPath(vg);
        nvgRect(vg, x, top, width, height - top);
        nvgFill(vg);

        // Buttons
        for (auto &button : this->m_buttons)
            button->frame(ctx);

        // Footer
        this->m_hint->frame(ctx);

        nvgFillColor(vg, ctx->theme->separatorColor); // we purposely don't apply opacity

        nvgBeginPath(vg);
        nvgRect(vg, x + style->AppletFrame.separatorSpacing, y + height - style->AppletFrame.footerHeight, width - style->AppletFrame.separatorSpacing * 2, 1);
        nvgFill(vg);

        nvgFillColor(vg, ctx->theme->textColor); // we purposely don't apply opacity
        nvgFontSize(vg, style->AppletFrame.footerTextSize);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgBeginPath(vg);
        nvgText(vg, x + style->AppletFrame.separatorSpacing + style->AppletFrame.footerTextSpacing, y + height - style->AppletFrame.footerHeight / 2, brls::Application::getCommonFooter()->c_str(), nullptr);
    }

    bool HexKeyboard::onCancel() {
        if (this->m_listener)
            this->m_listener(nullptr, 0);

        brls::Application::popView();
        return true;
    }

    unsigned HexKeyboard::getShowAnimationDuration(brls::ViewAnimation animation) {
        return View::getShowAnimationDuration(animation) / 2;
    }

    void HexKeyboard::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
        unsigned top = 720 - (400 - this->m_topOffset);

        this->m_buttons[0]->setBoundaries(100, top + 90, 150, 70);  // HEAP
        this->m_buttons[1]->setBoundaries(100, top + 170, 150, 70); // MAIN

        this->m_buttons[2]->setBoundaries(500, top + 50, 70, 70);   // 1
        this->m_buttons[3]->setBoundaries(580, top + 50, 70, 70);   // 2
        this->m_buttons[4]->setBoundaries(660, top + 50, 70, 70);   // 3
        this->m_buttons[5]->setBoundaries(500, top + 130, 70, 70);  // 4
        this->m_buttons[6]->setBoundaries(580, top + 130, 70, 70);  // 5
        this->m_buttons[7]->setBoundaries(660, top + 130, 70, 70);  // 6
        this->m_buttons[8]->setBoundaries(500, top + 210, 70, 70);  // 7
        this->m_buttons[9]->setBoundaries(580, top + 210, 70, 70);  // 8
        this->m_buttons[10]->setBoundaries(660, top + 210, 70, 70); // 9

        for (auto &button : this->m_buttons)
            button->invalidate();

        this->m_hint->setBoundaries(this->x, this->y, this->width, this->height);
    }

    brls::View* HexKeyboard::requestFocus(brls::FocusDirection direction, brls::View* oldFocus, bool fromUp) {
        if (direction == brls::FocusDirection::NONE)
            return this->m_buttons[0]->requestFocus(direction, oldFocus, fromUp);
        return nullptr;
    }


    void HexKeyboard::open(std::string title, KeyboardListener listener, size_t maxInputSize) {
        HexKeyboard* keyboard = new HexKeyboard(title, listener, maxInputSize);
        brls::Application::pushView(keyboard);
    }

}
