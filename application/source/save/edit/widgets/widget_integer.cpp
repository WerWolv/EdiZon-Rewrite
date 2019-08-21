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

#include "save/edit/widgets/widget_integer.hpp"
#include "helpers/utils.hpp"
#include <Borealis.hpp>
#include <math.h>

namespace edz::save::edit::widget {

    WidgetInteger::WidgetInteger(std::string name, s64 minValue, s64 maxValue) : Widget(name), m_minValue(minValue), m_maxValue(maxValue) {
        this->m_currValue = minValue;
    }

    WidgetInteger::~WidgetInteger() {

    }


    WidgetType WidgetInteger::getWidgetType() {
        return WidgetType::INTEGER;
    }

    View* WidgetInteger::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ListItem(this->m_name, this->m_description);
            ListItem *listItem = reinterpret_cast<ListItem*>(this->m_widgetView);

            listItem->setValue(std::to_string(this->m_currValue));

            listItem->setClickListener([&](View *view){
                edz::hlp::askSwkbdNumber([&](std::string str){

                    s64 newValue = std::stoll(str);
                    this->m_currValue = std::max(this->m_minValue, std::min(this->m_maxValue, newValue));
                    listItem->setValue(std::to_string(this->m_currValue));

                }, edz::LangEntry("edz.widget.integer.title").get(), edz::LangEntry("edz.widget.integer.subtitle").get(), "-", "", std::floor(std::log10(this->m_maxValue)) + 1, std::to_string(this->m_currValue));
            });
            }

        return this->m_widgetView;
    }

}
