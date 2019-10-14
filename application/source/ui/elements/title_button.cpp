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

#include "ui/elements/title_button.hpp"

namespace edz::ui::element {

    using namespace brls;

    TitleButton::TitleButton(save::Title *title, u8 column) : m_title(title), m_column(column) {
        size_t iconSize = title->getIconSize();
        u8 *iconBuffer = new u8[iconSize];
        title->getIcon(iconBuffer, iconSize);

        this->m_image = new Image(iconBuffer, iconSize);
        this->m_image->setParent(this);
        this->m_image->setScaleType(ImageScaleType::SCALE);
        this->m_image->invalidate();

        delete[] iconBuffer;
    }

    TitleButton::~TitleButton() {
        delete this->m_image;
    }


    void TitleButton::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) {
        View *baseView = this->getParent()->getParent();

        nvgSave(vg);
        nvgScissor(vg, baseView->getX(), baseView->getY(), baseView->getWidth(), baseView->getHeight());
        this->m_image->frame(ctx);
        nvgRestore(vg);
    }

    void TitleButton::layout(NVGcontext* vg, Style *style, FontStash *stash) {
        this->width = 150;
        this->height = 150;

        this->m_image->setBoundaries(this->getX(), this->getY(), 150, 150);
        this->m_image->invalidate();
    }

    View* TitleButton::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp) {   
        return this;
    }

    bool TitleButton::onClick()
    {
        if (this->m_clickListener)
            this->m_clickListener(this);

        return this->m_clickListener != nullptr;
    }

    void TitleButton::setClickListener(EventListener listener) {
        this->m_clickListener = listener;
    }

    u8 TitleButton::getColumn() {
        return this->m_column;
    }

    void TitleButton::setColumn(u8 column) {
        this->m_column = column;
    }

    save::Title* TitleButton::getTitle() {
        return this->m_title;
    }

    HorizontalTitleList::HorizontalTitleList() : BoxLayout(BoxLayoutOrientation::HORIZONTAL, 0) {
        this->setHeight(150);
        this->setSpacing(30);
        this->setMargins(0, 0, 0, 0);
    }

    HorizontalTitleList::~HorizontalTitleList() {

    }


    std::vector<BoxLayoutChild*> HorizontalTitleList::getChildren() {
        return this->children;
    }

    void HorizontalTitleList::addTitle(save::Title *title) {
        TitleButton *titleButton = new TitleButton(title, this->children.size());

        this->addView(titleButton);
    }


    // TODO: Fix focus not being able to exit the TitleList sideways
    View* HorizontalTitleList::defaultFocus(View *oldFocus) {
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

}