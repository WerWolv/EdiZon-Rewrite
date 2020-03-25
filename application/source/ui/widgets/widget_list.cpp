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

#include "ui/widgets/widget_list.hpp"
#include "helpers/utils.hpp"
#include <borealis.hpp>

namespace edz::ui::widget {

    WidgetList::WidgetList(std::string name, std::vector<NamedArgument> entries) : Widget(name), m_entries(entries) {
        this->m_currValue = 0;
    }

    WidgetList::~WidgetList() {

    }


    WidgetType WidgetList::getWidgetType() {
        return WidgetType::LIST;
    }

    brls::View* WidgetList::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new brls::ListItem(this->m_name, this->m_description);
            brls::ListItem *listItem = reinterpret_cast<brls::ListItem*>(this->m_widgetView);

            listItem->setValue(this->m_entries[this->m_currValue].first);

            std::vector<std::string> displayStrings;
            for (auto &[displayString, argument] : this->m_entries)
                displayStrings.push_back(displayString);

            listItem->getClickEvent()->subscribe([&](brls::View *view){
                brls::Dropdown::open(this->m_name, displayStrings, [&](s32 selectedItem){
                    if (selectedItem == -1) return;

                    this->m_currValue = selectedItem;
                    listItem->setValue(this->m_entries[this->m_currValue].first);
                }, this->m_currValue);
            });
        }

        return this->m_widgetView;
    }

}
