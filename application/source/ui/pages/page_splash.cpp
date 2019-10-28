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

#include "ui/pages/page_splash.hpp"


namespace edz::ui::page {

    PageSplash::PageSplash() {
        this->m_logo = new brls::Image("romfs:/assets/icon_edz_color.jpg");
        this->m_logo->setParent(this);
    }

    PageSplash::~PageSplash() {
        delete this->m_logo;
    }

    void PageSplash::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
        NVGcolor color = a(nvgRGB(0x5D, 0x4F, 0x4E));

        if (color.a == 0.0f)
            return;

        nvgFillColor(vg, color);

        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, width, (height - this->m_logo->getWidth()) / 2);
        nvgRect(vg, 0, (height + this->m_logo->getWidth()) / 2, width, (height - this->m_logo->getWidth()) / 2);
        nvgRect(vg, 0, (height - this->m_logo->getWidth()) / 2, (width - this->m_logo->getWidth()) / 2, this->m_logo->getWidth());
        nvgRect(vg, (width + this->m_logo->getWidth()) / 2, (height - this->m_logo->getWidth()) / 2, (width - this->m_logo->getWidth()) / 2, this->m_logo->getWidth());
        nvgFill(vg);

        this->m_logo->frame(ctx);
    }

    brls::View* PageSplash::requestFocus(brls::FocusDirection direction, brls::View* oldFocus, bool fromUp) {
        return nullptr;
    }

    void PageSplash::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
        this->m_logo->setBoundaries((1280 - 256) / 2, (720 - 256) / 2, 256, 256);
    }

}