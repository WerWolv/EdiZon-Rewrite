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

#include "overlay/elements/element.hpp"
#include "overlay/gui/gui.hpp"

namespace edz::ovl::element {
    
    void Element::frame(ovl::Screen *screen) {
        if (gui::Gui::isFocused(this))
            this->drawFocus(screen);

        this->draw(screen, this->m_x, this->m_y);
    }

    void Element::drawFocus(ovl::Screen *screen) {
        static float counter = 0;
        const float progress = (std::sin(counter) + 1) / 2;
        rgba4444_t highlightColor = {   static_cast<u8>((0x2 - 0x8) * progress + 0x8),
                                        static_cast<u8>((0x8 - 0xF) * progress + 0xF), 
                                        static_cast<u8>((0xC - 0xF) * progress + 0xF), 
                                        0xF };

        counter += 0.2F;

        screen->drawRect(this->m_x - 4, this->m_y - 4, this->m_width + 8, 4, a(highlightColor));
        screen->drawRect(this->m_x - 4, this->m_y + this->m_height, this->m_width + 8, 4, a(highlightColor));
        screen->drawRect(this->m_x - 4, this->m_y, 4, this->m_height, a(highlightColor));
        screen->drawRect(this->m_x + this->m_width, this->m_y, 4, this->m_height, a(highlightColor));

        screen->drawRect(this->m_x, this->m_y, this->m_width, this->m_height, a(0xF000));
    }

}