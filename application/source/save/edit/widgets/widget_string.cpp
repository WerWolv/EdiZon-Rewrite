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

#include "save/edit/widgets/widget_string.hpp"
#include "helpers/utils.hpp"
#include <Borealis.hpp>

namespace edz::save::edit::widget {

    WidgetString::WidgetString(std::string name, u32 minLength, u32 maxLength) : Widget(name), m_minLength(minLength), m_maxLength(maxLength) {
        this->m_currValue = "";
    }

    WidgetString::~WidgetString() {

    }


    WidgetType WidgetString::getWidgetType() {
        return WidgetType::STRING;
    }

    View* WidgetString::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ListItem(this->m_name, this->m_description);
            ListItem *listItem = reinterpret_cast<ListItem*>(this->m_widgetView);
            
            listItem->setValue(this->m_currValue);

            listItem->setClickListener([&](View *view){
                edz::hlp::askSwkbdText([&](std::string str){
                    if (str.length() < this->m_minLength) return;

                    this->m_currValue = str;
                    listItem->setValue(this->m_currValue);

                }, edz::LangEntry("edz.widget.string.title").get(), edz::LangEntry("edz.widget.string.subtitle").get(), this->m_maxLength, this->m_currValue);
            });
        }

        return this->m_widgetView;
    }

}
