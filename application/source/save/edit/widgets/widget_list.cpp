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

#include "save/edit/widgets/widget_list.hpp"
#include "helpers/utils.hpp"
#include <Borealis.hpp>

namespace edz::save::edit::widget {

    WidgetList::WidgetList(std::string name, std::vector<std::string> displayStrings, std::vector<std::shared_ptr<widget::Arg>> arguments) : Widget(name), m_displayStrings(displayStrings), m_arguments(arguments) {
        this->m_currValue = 0;
    }

    WidgetList::~WidgetList() {

    }


    WidgetType WidgetList::getWidgetType() {
        return WidgetType::LIST;
    }

    View* WidgetList::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ListItem(this->m_name, this->m_description);
            ListItem *listItem = reinterpret_cast<ListItem*>(this->m_widgetView);

            listItem->setValue(this->m_displayStrings[this->m_currValue]);

            listItem->setClickListener([&](View *view){
                Dropdown::open(this->m_name, this->m_displayStrings, [&](s32 selectedItem){
                    if (selectedItem == -1) return;

                    this->m_currValue = selectedItem;
                    listItem->setValue(this->m_displayStrings[selectedItem]);
                }, this->m_currValue);
            });
        }

        return this->m_widgetView;
    }

}
