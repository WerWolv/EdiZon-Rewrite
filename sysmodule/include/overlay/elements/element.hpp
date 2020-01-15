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

#pragma once

#include <edizon.hpp>

#include "overlay/screen.hpp"
#include <memory>
#include <utility>

namespace edz::ovl::element {
    
    class Element {
    public:
        enum class FocusDirection { NONE, UP, DOWN, LEFT, RIGHT };

        Element() { }
        virtual ~Element() { }

        virtual Element* requestFocus(Element *oldFocus, FocusDirection direction) = 0;
        
        virtual bool onClick(u64 key) {
            return false;
        }

        virtual void draw(ovl::Screen *screen) = 0;
        virtual void layout() = 0;

        void setParent(Element *parent) { this->m_parent = parent; }
        Element* getParent() { return this->m_parent; }

        constexpr void setPosition(u16 x, u16 y) { this->m_x = x; this->m_y = y; }
        constexpr std::pair<u16, u16> getPosition() { return { this->m_x, this->m_y }; }

        constexpr void setSize(u16 width, u16 height) { this->m_width = width; this->m_height = height; }
        constexpr std::pair<u16, u16> getSize() { return { this->m_width, this->m_height }; }


    private:
        Element *m_parent = nullptr;
        bool m_focused = false;

        u16 m_x = 0, m_y = 0, m_width = 0, m_height = 0;
    };

}