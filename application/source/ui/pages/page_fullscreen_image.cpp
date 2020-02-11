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

#include "ui/pages/page_fullscreen_image.hpp"

#include "helpers/utils.hpp"

namespace edz::ui::page {

    PageFullscreenImage::PageFullscreenImage(brls::Image *image) {
        this->m_image = image;
        this->m_image->setScaleType(brls::ImageScaleType::FIT);
        this->m_image->setParent(this);
    }

    PageFullscreenImage::~PageFullscreenImage() {
        delete this->m_image;
    }

    void PageFullscreenImage::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
        this->m_image->frame(ctx);
    }

    brls::View* PageFullscreenImage::requestFocus(brls::FocusDirection direction, brls::View* oldFocus, bool fromUp) {
        return nullptr;
    }

    void PageFullscreenImage::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
        this->m_image->setBoundaries(0, 0, 1280, 720);
    }

}