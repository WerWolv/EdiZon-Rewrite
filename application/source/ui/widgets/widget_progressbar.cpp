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

#include "ui/widgets/widget_progressbar.hpp"
#include "helpers/utils.hpp"
#include <Borealis.hpp>

namespace edz::ui::widget {

    WidgetProgressbar::WidgetProgressbar(std::string name) : Widget(name) {
        this->m_progress = 0;
    }

    WidgetProgressbar::~WidgetProgressbar() {

    }


    WidgetType WidgetProgressbar::getWidgetType() {
        return WidgetType::PROGRESS_BAR;
    }

    View* WidgetProgressbar::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ProgressDisplay(PROGRESS_DISPLAY_STYLE_PERCENTAGE);
            ProgressDisplay *progressBar = reinterpret_cast<ProgressDisplay*>(this->m_widgetView);
            
            progressBar->setProgress(this->m_progress, 100);
        }

        return this->m_widgetView;
    }

}
