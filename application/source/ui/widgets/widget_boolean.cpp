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

#include "ui/widgets/widget_boolean.hpp"
#include "helpers/utils.hpp"
#include <borealis.hpp>

namespace edz::ui::widget {

    WidgetBoolean::WidgetBoolean(std::string name, Argument onValue, Argument offValue) : Widget(name), m_onValue(onValue), m_offValue(offValue) {
        this->m_state = false;
    }

    WidgetBoolean::~WidgetBoolean() {

    }


    WidgetType WidgetBoolean::getWidgetType() {
        return WidgetType::BOOLEAN;
    }

    brls::View* WidgetBoolean::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new brls::ListItem(this->m_name, this->m_description);
            brls::ListItem *listItem = reinterpret_cast<brls::ListItem*>(this->m_widgetView);

            listItem->setValue(this->m_state ? "edz.widget.boolean.on"_lang : "edz.widget.boolean.off"_lang, !this->m_state);

            listItem->getClickEvent()->subscribe([&](brls::View *view){
                this->m_state = !this->m_state;
                listItem->setValue(this->m_state ? "edz.widget.boolean.on"_lang : "edz.widget.boolean.off"_lang, !this->m_state);
            });
        }

        return this->m_widgetView;
    }

}
