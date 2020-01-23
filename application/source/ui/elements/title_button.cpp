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

#include "ui/elements/title_button.hpp"

namespace edz::ui::element {

    TitleButton::TitleButton(std::unique_ptr<save::Title> &title, u8 column) : m_title(title), m_column(column) {
        this->m_image = new brls::Image(title->getIcon());
        this->m_image->setParent(this);
        this->m_image->setScaleType(brls::ImageScaleType::SCALE);
        this->m_image->invalidate();

        this->addHint("OK", brls::Key::A, [this] { return this->onClick(); });
    }

    TitleButton::~TitleButton() {
        delete this->m_image;
    }


    void TitleButton::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) {
        View *baseView = this->getParent()->getParent();

        nvgSave(vg);
        nvgScissor(vg, baseView->getX(), baseView->getY(), baseView->getWidth(), baseView->getHeight());
        this->m_image->frame(ctx);
        nvgRestore(vg);
    }

    void TitleButton::layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash) {
        this->width = 150;
        this->height = 150;

        this->m_image->setBoundaries(this->getX(), this->getY(), 150, 150);
        this->m_image->invalidate();
    }

    brls::View* TitleButton::requestFocus(brls::FocusDirection direction, brls::View *oldFocus, bool fromUp) {   
        return this;
    }

    bool TitleButton::onClick()
    {
        if (this->m_clickListener)
            this->m_clickListener(this);

        return this->m_clickListener != nullptr;
    }

    void TitleButton::setClickListener(brls::EventListener listener) {
        this->m_clickListener = listener;
    }

    u8 TitleButton::getColumn() {
        return this->m_column;
    }

    void TitleButton::setColumn(u8 column) {
        this->m_column = column;
    }

    std::unique_ptr<save::Title>& TitleButton::getTitle() {
        return this->m_title;
    }

    HorizontalTitleList::HorizontalTitleList() : BoxLayout(brls::BoxLayoutOrientation::HORIZONTAL, 0) {
        this->setHeight(150);
        this->setSpacing(30);
        this->setMargins(0, 0, 0, 0);
    }

    HorizontalTitleList::~HorizontalTitleList() {

    }


    std::vector<brls::BoxLayoutChild*> HorizontalTitleList::getChildren() {
        return this->children;
    }

    void HorizontalTitleList::addTitle(std::unique_ptr<save::Title> &title) {
        TitleButton *titleButton = new TitleButton(title, this->children.size());

        this->addView(titleButton);
    }


    brls::View* HorizontalTitleList::defaultFocus(brls::View *oldFocus) {
        TitleButton *oldTitleButton = dynamic_cast<TitleButton*>(oldFocus);
        
        if (oldTitleButton != nullptr) {

            if (oldTitleButton->getColumn() < this->children.size()) {

                TitleButton *newTitleButton = dynamic_cast<TitleButton*>(this->children[oldTitleButton->getColumn()]->view);
            
                if (newTitleButton != nullptr) {
                    if (oldTitleButton->getParent() != newTitleButton->getParent()) {
                        this->focusedIndex = oldTitleButton->getColumn();
                        return newTitleButton;
                    } else return oldFocus;
                } else return oldFocus;
            } else return oldFocus;
        }

        return BoxLayout::defaultFocus(oldFocus);
    }

    brls::View* HorizontalTitleList::requestFocus(brls::FocusDirection direction, View *oldFocus, bool fromUp) {
        if (direction == brls::FocusDirection::LEFT)
            if (oldFocus == this->getChildren()[0]->view)
                return this->getParent()->getParent()->requestFocus(direction, oldFocus, fromUp);

        return BoxLayout::requestFocus(direction, oldFocus, fromUp);
    }

}