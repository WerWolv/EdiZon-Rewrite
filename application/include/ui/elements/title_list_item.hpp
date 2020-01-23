/**
 * Copyright (C) 2020 WerWolv
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
#include <borealis.hpp>

#include <memory>

#include "save/title.hpp"

namespace edz::ui::element {

    class TitleListItem : public brls::ListItem {
    public:
        TitleListItem(std::unique_ptr<save::Title>& title, std::string label, std::string description = "", std::string subLabel = "")
            : brls::ListItem(label, description, subLabel), m_title(title) {}

        std::unique_ptr<save::Title>& getTitle() {
            return this->m_title;
        }

    private:
        std::unique_ptr<save::Title>& m_title;
    };

}
