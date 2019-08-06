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

#include "save/edit/widgets/widget_comment.hpp"
#include "helpers/utils.hpp"

namespace edz::save::edit::widget {

    WidgetComment::WidgetComment(std::string name, std::string comment) : Widget(name), m_comment(comment) {

    }

    WidgetComment::~WidgetComment() {

    }


    WidgetType WidgetComment::getWidgetType() {
        return WidgetType::COMMENT;
    }

    View* WidgetComment::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new Label(LabelStyle::REGULAR, this->m_comment, true);
        }

        return this->m_widgetView;
    }

}
