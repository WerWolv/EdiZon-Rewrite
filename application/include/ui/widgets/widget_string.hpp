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

#pragma once

#include <edizon.hpp>

#include "ui/widgets/widget.hpp"

namespace edz::ui::widget {

    class WidgetString : public Widget {
    public:
        WidgetString(std::string name, u32 minLength, u32 maxLength);
        ~WidgetString();

        WidgetType getWidgetType() override;
        View* getView() override;

    private:
        std::string m_currValue;
        u32 m_minLength;
        u32 m_maxLength;
    };

}