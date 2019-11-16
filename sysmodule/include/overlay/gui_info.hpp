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

#pragma once

#include "overlay/gui.hpp"
#include <thread>
#include <vector>

namespace edz::ovl {

    class GuiInfo : public Gui {
    public:
        GuiInfo();
        ~GuiInfo();

        void createUI() override;
        void update() override;

    private:
        ClkrstSession m_cpuClkSession, m_gpuClkSession, m_memClkSession;

        lv_obj_t *m_titleLabel = nullptr;


        lv_obj_t *m_infoLabels[6];

        lv_obj_t *m_btnBack = nullptr;
    };

}