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

#include "save/edit/widgets/widget.hpp"
#include <vector>

namespace edz::save::edit::widget {

    class WidgetList : public Widget {
    public:
        WidgetList(std::string name, std::vector<std::string> displayStrings, std::vector<std::shared_ptr<widget::Arg>> arguments);
        ~WidgetList();

        WidgetType getWidgetType() override;
        View* getView() override;

    private:
        std::vector<std::string> m_displayStrings;
        std::vector<std::shared_ptr<widget::Arg>> m_arguments;

        s16 m_currValue;
    };

}