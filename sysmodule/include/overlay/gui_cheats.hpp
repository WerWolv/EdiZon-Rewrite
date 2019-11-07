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

    class GuiCheats : public Gui {
    public:
        GuiCheats(Screen *screen);
        ~GuiCheats();

        void createUI() override;
        void update() override;

    private:
        ClkrstSession m_clkrstSession;

        lv_obj_t *m_titleLabel = nullptr;
        lv_obj_t *m_noCheatsLabel = nullptr;
        lv_obj_t *m_titleInfoLabel = nullptr;
        lv_obj_t *m_cpuInfoLabel = nullptr;
        lv_obj_t *m_cheatsList = nullptr;
        lv_obj_t *m_footerLine = nullptr;

        std::vector<lv_obj_t*> m_cheatToggleButtons;
    };

}