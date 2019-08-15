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

namespace edz::ui::elements {

    TitleButton::TitleButton(unsigned char *buffer, size_t bufferSize) {
        this->m_image = new Image(buffer, bufferSize);
        this->m_image->setParent(this);
        this->m_image->setImageScaleType(ImageScaleType::SCALE);
        this->m_image->invalidate();
    }

    TitleButton::~TitleButton() {
        delete this->m_image;
    }


    void TitleButton::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) {
        this->m_image->frame(ctx);
    }

    void TitleButton::layout(NVGcontext* vg, Style *style, FontStash *stash) {
        this->setBoundaries(this->getX(), this->getY(), 256, 256);
        this->m_image->setBoundaries(this->getX(), this->getY(), this->getWidth(), this->getHeight());
        this->m_image->invalidate();
    }

    View* TitleButton::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp) {
        this->m_image->invalidate();
        
        return this;
    }

}