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

#include "ui/widgets/widget_string.hpp"
#include "helpers/utils.hpp"
#include <borealis.hpp>

namespace edz::ui::widget {

    WidgetString::WidgetString(std::string name, u32 minLength, u32 maxLength) : Widget(name), m_minLength(minLength), m_maxLength(maxLength) {
        this->m_currValue = "";
    }

    WidgetString::~WidgetString() {

    }


    WidgetType WidgetString::getWidgetType() {
        return WidgetType::STRING;
    }

    brls::View* WidgetString::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new brls::ListItem(this->m_name, this->m_description);
            brls::ListItem *listItem = reinterpret_cast<brls::ListItem*>(this->m_widgetView);
            
            listItem->setValue(this->m_currValue);

            listItem->getClickEvent()->subscribe([&](brls::View *view){
                edz::hlp::openSwkbdForText([&](std::string str){
                    if (str.length() < this->m_minLength) return;

                    this->m_currValue = str;
                    listItem->setValue(this->m_currValue);

                }, "edz.widget.string.title"_lang, "edz.widget.string.subtitle"_lang, this->m_maxLength, this->m_currValue);
            });
        }

        return this->m_widgetView;
    }

}
