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


#include "ui/elements/popup_list.hpp"

namespace edz::ui::element {

    PopupList::PopupList(std::string lButton, std::string rButton, brls::EventListener lButtonEvent, brls::EventListener rButtonEvent) {
        this->m_list = new brls::List();

        this->m_lButton = new brls::Button(brls::ButtonStyle::DIALOG);
        this->m_lButton->setLabel(lButton);
        this->m_lButton->setClickListener(lButtonEvent);

        this->m_rButton = new brls::Button(brls::ButtonStyle::DIALOG);
        this->m_rButton->setLabel(rButton);
        this->m_rButton->setClickListener(rButtonEvent);

        this->m_list->setParent(this);
        this->m_lButton->setParent(this);
        this->m_rButton->setParent(this);

        this->registerAction("Back", brls::Key::B, [this] { return this->onCancel(); });
    }

    PopupList::~PopupList() {
        delete this->m_list;
        delete this->m_lButton;
        delete this->m_rButton;
    }

    void PopupList::open() {
        brls::Application::pushView(this);
    }

    void PopupList::close() {
        brls::Application::popView();
    }

    void PopupList::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
        // Backdrop
        nvgFillColor(vg, a(ctx->theme->dialogBackdrop));
        nvgBeginPath(vg);
        nvgRect(vg, x, y, width, height);
        nvgFill(vg);

        // Shadow
        float shadowWidth   = style->Dialog.shadowWidth;
        float shadowFeather = style->Dialog.shadowFeather;
        float shadowOpacity = style->Dialog.shadowOpacity;
        float shadowOffset  = style->Dialog.shadowOffset;

        NVGpaint shadowPaint = nvgBoxGradient(vg,
            this->m_frameX, this->m_frameY + shadowWidth,
            this->m_frameWidth, this->m_frameHeight,
            style->Dialog.cornerRadius * 2, shadowFeather,
            RGBA(0, 0, 0, shadowOpacity * alpha), brls::transparent);

        nvgBeginPath(vg);
        nvgRect(vg, this->m_frameX - shadowOffset, this->m_frameY - shadowOffset,
            this->m_frameWidth + shadowOffset * 2, this->m_frameHeight + shadowOffset * 3);
        nvgRoundedRect(vg, this->m_frameX, this->m_frameY, this->m_frameWidth, this->m_frameHeight, style->Dialog.cornerRadius);
        nvgPathWinding(vg, NVG_HOLE);
        nvgFillPaint(vg, shadowPaint);
        nvgFill(vg);

        // Frame
        nvgFillColor(vg, a(ctx->theme->dialogColor));
        nvgBeginPath(vg);
        nvgRoundedRect(vg, this->m_frameX, this->m_frameY, this->m_frameWidth, this->m_frameHeight, style->Dialog.cornerRadius);
        nvgFill(vg);
        
        nvgFillColor(vg, a(ctx->theme->dialogButtonSeparatorColor));

        nvgBeginPath(vg);
        nvgRect(vg, this->m_frameX, this->m_frameY + this->m_frameHeight - style->Dialog.buttonHeight, this->m_frameWidth, style->Dialog.buttonSeparatorHeight);
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(
            vg,
            this->m_frameX + this->m_frameWidth / 2 + style->Dialog.buttonSeparatorHeight / 2,
            this->m_frameY + this->m_frameHeight - style->Dialog.buttonHeight + 1, // offset by 1 to fix aliasing artifact
            style->Dialog.buttonSeparatorHeight,
            style->Dialog.buttonHeight - 1);
        nvgFill(vg);

        // Content view
        if (this->m_list)
            this->m_list->frame(ctx);

        this->m_lButton->frame(ctx);
        this->m_rButton->frame(ctx);
    }

    brls::View* PopupList::requestFocus(brls::FocusDirection direction, View* oldFocus, bool fromUp) {
        if ((direction == brls::FocusDirection::DOWN && oldFocus != this->m_rButton) || (direction == brls::FocusDirection::LEFT && oldFocus != this->m_list) || direction == brls::FocusDirection::NONE)
            return this->m_lButton;
        else if (direction == brls::FocusDirection::RIGHT && oldFocus != this->m_list)
            return this->m_rButton;
        else return nullptr;
    }

    bool PopupList::onCancel() {
        this->close();

        return true;
    }

    void PopupList::addItem(brls::ListItem *listItem) {
        this->m_list->addView(listItem);
    }

    void PopupList::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
        this->m_frameWidth  = style->Dialog.width;
        this->m_frameHeight = style->Dialog.height * 1.5;

        this->m_frameX = getWidth() / 2 - this->m_frameWidth / 2;
        this->m_frameY = getHeight() / 2 - this->m_frameHeight / 2;

        this->m_list->setBoundaries(this->m_frameX, this->m_frameY, this->m_frameWidth, this->m_frameHeight - style->Dialog.buttonHeight);

        this->m_lButton->setBoundaries(this->m_frameX, this->m_frameY + this->m_frameHeight - style->Dialog.buttonHeight, this->m_frameWidth / 2, style->Dialog.buttonHeight);
        this->m_rButton->setBoundaries(this->m_frameX + this->m_frameWidth / 2 + 1, this->m_frameY + this->m_frameHeight - style->Dialog.buttonHeight, this->m_frameWidth / 2, style->Dialog.buttonHeight);
    }

}
