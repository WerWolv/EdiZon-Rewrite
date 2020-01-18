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

#include "overlay/gui/gui.hpp"

namespace edz::ovl::gui {

    class GuiStats : public Gui {
    public:
        GuiStats();
        ~GuiStats();

        Element* createUI() override;
        void update() override;

    private:
        ClkrstSession m_clkrstSessionCpu, m_clkrstSessionGpu, m_clkrstSessionMem;
        int m_ipAddress;

        titleid_t m_runningTitleID;
        processid_t m_runningProcessID;
        buildid_t m_runningBuildID;
    };

}