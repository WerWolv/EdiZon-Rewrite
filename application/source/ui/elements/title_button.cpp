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

    TitleButton::TitleButton(unsigned char *buffer, size_t bufferSize, u8 column) : m_column(column) {
        this->m_image = new Image(buffer, bufferSize);
        this->m_image->setParent(this);
        this->m_image->setImageScaleType(ImageScaleType::SCALE);
        this->m_image->invalidate();
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

    u8 TitleButton::getColumn() {
        return this->m_column;
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
        size_t iconSize = title->getIconSize();
        u8 *iconBuffer = new u8[iconSize];
        title->getIcon(iconBuffer, iconSize);

        TitleButton *titleButton = new TitleButton(iconBuffer, iconSize, this->children.size());

        this->addView(titleButton);

        delete[] iconBuffer;
    }

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