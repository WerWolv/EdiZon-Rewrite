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

#include "overlay/gui_cheats.hpp"
#include "overlay/styles.hpp"
#include "cheat/cheat.hpp"
#include "helpers/utils.hpp"

namespace edz::ovl {
    
    GuiCheats::GuiCheats(Screen *screen) : Gui(screen) {
        if (hosversionAtLeast(8,0,0))
            clkrstOpenSession(&this->m_clkrstSession, PcvModuleId_CpuBus, 3);
    }

    GuiCheats::~GuiCheats() {
        this->m_cheatToggleButtons.clear();
        lv_obj_del(this->m_titleLabel);
        lv_list_clean(this->m_cheatsList);
        lv_obj_del(this->m_cheatsList);
        lv_obj_del(this->m_titleInfoLabel);
        lv_obj_del(this->m_cpuInfoLabel);
        lv_obj_del(this->m_footerLine);

        if (this->m_noCheatsLabel != nullptr)
            lv_obj_del(this->m_noCheatsLabel);

        if (hosversionAtLeast(8,0,0))
            clkrstCloseSession(&this->m_clkrstSession);
    }

    void GuiCheats::createUI() {
        this->m_titleLabel = lv_label_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_titleLabel, 10, 10);
        lv_label_set_text(this->m_titleLabel, "EdiZon Quick menu");
        setLabelTitleStyle(this->m_titleLabel);

        this->m_cheatsList = lv_list_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_cheatsList, 20, 100);
        lv_obj_set_size(this->m_cheatsList, 216, 512 - 155);
        setListStyle(this->m_cheatsList);

        {
            static lv_point_t p[] = {{5, 512 - 52}, {256 - 5, 512 - 52}};
            this->m_footerLine = lv_line_create(lv_scr_act(), nullptr);
            lv_obj_set_pos(this->m_footerLine, 0, 0);
            lv_line_set_points(this->m_footerLine, p, 2);
            setLineStyle(this->m_footerLine);
        }

        this->m_cpuInfoLabel = lv_label_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_cpuInfoLabel, 100, 45);
        setLabelStyle(this->m_cpuInfoLabel);

        this->m_titleInfoLabel = lv_label_create(lv_scr_act(), nullptr);
        lv_label_set_text(this->m_titleInfoLabel, ("TID: " + hlp::toHexString(cheat::CheatManager::getTitleID()) + " | BID: " + hlp::toHexString(cheat::CheatManager::getBuildID()) + " | PID: " + std::to_string(cheat::CheatManager::getProcessID()) + " |").c_str());
        lv_obj_set_pos(this->m_titleInfoLabel, 0, 512 - 35);
        lv_label_set_anim_speed(this->m_titleInfoLabel, 20);
        setLabelStyle(this->m_titleInfoLabel);
        lv_label_set_long_mode(this->m_titleInfoLabel, LV_LABEL_LONG_SROLL_CIRC);
        lv_obj_set_width(this->m_titleInfoLabel, 256);
        lv_obj_set_height(this->m_titleInfoLabel, 30);

        edz::cheat::CheatManager::forceAttach();
        edz::cheat::CheatManager::reload();

        if (edz::cheat::CheatManager::getCheats().size() == 0) {
            this->m_noCheatsLabel = lv_label_create(lv_scr_act(), nullptr);

            lv_label_set_text(this->m_noCheatsLabel, "No cheats loaded");
            lv_obj_set_pos(this->m_noCheatsLabel, 50, 256);
            setLabelStyle(this->m_noCheatsLabel);
        } else {
            for (auto cheat : edz::cheat::CheatManager::getCheats()) {
                lv_obj_t *btn = lv_list_add_btn(this->m_cheatsList, nullptr, cheat->getName().c_str());
                this->m_cheatToggleButtons.push_back(btn);

                lv_btn_set_toggle(btn, true);
                lv_btn_set_fit(btn, LV_FIT_NONE);
                lv_obj_set_height(btn, 50);
                lv_label_set_long_mode(lv_obj_get_child(btn, nullptr), LV_LABEL_LONG_DOT);

                if (cheat->isEnabled() != lv_btn_get_state(btn))
                    lv_btn_toggle(btn);

                lv_obj_set_user_data(btn, cheat);
                lv_obj_set_event_cb(btn, [](lv_obj_t * obj, lv_event_t event) {
                    if (event != LV_EVENT_CLICKED)
                        return;

                    reinterpret_cast<edz::cheat::Cheat*>(obj->user_data)->toggle();
                    lv_obj_invalidate(obj);
                });
            }
        }
    }

    void GuiCheats::update() {
        if (Gui::getFrameCount() % 20) {
            u16 i = 0;
            for (auto &cheatToggle : this->m_cheatToggleButtons) {
                bool isEnabled = cheat::CheatManager::getCheats()[i++]->isEnabled();
                lv_btn_state_t btnState = lv_btn_get_state(cheatToggle);
                if (btnState == LV_BTN_STATE_REL || btnState == LV_BTN_STATE_TGL_REL)
                    lv_btn_set_state(cheatToggle, isEnabled ? LV_BTN_STATE_TGL_REL : LV_BTN_STATE_REL);
            }

            s32 temparature = 0;
            u32 cpuClk = 0;
            tsGetTemperatureMilliC(TsLocation_Internal, &temparature);

            if (hosversionBefore(8,0,0))
                pcvGetClockRate(PcvModule_CpuBus, &cpuClk);
            else
                clkrstGetClockRate(&this->m_clkrstSession, &cpuClk);

            lv_label_set_text(this->m_cpuInfoLabel, hlp::formatString("%.1f'C | %dMHz", static_cast<float>(temparature) / 1'000.0F, cpuClk / 1'000'000).c_str());
        }
    }

}