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

#include "overlay/gui_main.hpp"
#include "overlay/gui_cheats.hpp"
#include "overlay/gui_notes.hpp"
#include "overlay/gui_info.hpp"

#include "overlay/styles.hpp"
#include "helpers/utils.hpp"
#include <atomic>

extern std::atomic<u64> g_keysDown, g_keysHeld;

namespace edz::ovl {

    GuiMain::GuiMain() {

    }

    GuiMain::~GuiMain() {
        lv_obj_del(this->m_titleLabel);
        lv_obj_del(this->m_cpuInfoLabel);
        lv_list_clean(this->m_menuList);
        lv_obj_del(this->m_menuList);
    }

    void GuiMain::createUI() {
        setBackgroundStyle();
        
        this->m_titleLabel = lv_label_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_titleLabel, 10, 10);
        lv_label_set_text(this->m_titleLabel, "EdiZon Quick Menu");
        setLabelTitleStyle(this->m_titleLabel);

        {
            static lv_point_t p[] = {{5, 512 - 52}, {256 - 5, 512 - 52}};
            this->m_footerLine = lv_line_create(lv_scr_act(), nullptr);
            lv_obj_set_pos(this->m_footerLine, 0, 0);
            lv_line_set_points(this->m_footerLine, p, 2);
            setLineStyle(this->m_footerLine);
        }

        this->m_cpuInfoLabel = lv_label_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_cpuInfoLabel, 90, 45);
        setLabelStyle(this->m_cpuInfoLabel);

        this->m_menuList = lv_list_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_menuList, 20, 100);
        lv_obj_set_size(this->m_menuList, 216, 512 - 155);
        setListStyle(this->m_menuList);

        lv_obj_t *cheatsBtn = lv_list_add_btn(this->m_menuList, nullptr, LV_SYMBOL_EDIT      " Cheats");
        lv_obj_t *notesBtn = lv_list_add_btn(this->m_menuList, nullptr,  LV_SYMBOL_FILE      " Notes");
        lv_obj_t *overclockBtn = lv_list_add_btn(this->m_menuList, nullptr, LV_SYMBOL_CHARGE " Overclocking");
        lv_obj_t *infosBtn = lv_list_add_btn(this->m_menuList, nullptr,  LV_SYMBOL_SETTINGS  " System Information");

        lv_btn_set_fit(cheatsBtn, LV_FIT_NONE);
        lv_obj_set_height(cheatsBtn, 50);
        lv_btn_set_fit(notesBtn, LV_FIT_NONE);
        lv_obj_set_height(notesBtn, 50);
        lv_btn_set_fit(infosBtn, LV_FIT_NONE);
        lv_obj_set_height(infosBtn, 50);

        lv_obj_set_event_cb(cheatsBtn, [](lv_obj_t * obj, lv_event_t event) {
            if (event != LV_EVENT_CLICKED)
                return;

            Gui::changeTo<GuiCheats>();
        });

        lv_obj_set_event_cb(notesBtn, [](lv_obj_t * obj, lv_event_t event) {
            if (event != LV_EVENT_CLICKED)
                return;
            
            Gui::changeTo<GuiNotes>();
        });

        lv_obj_set_event_cb(infosBtn, [](lv_obj_t * obj, lv_event_t event) {
            if (event != LV_EVENT_CLICKED)
                return;
            
            Gui::changeTo<GuiInfo>();
        });

        Gui::registerForButtonInput(this->m_menuList);
    }

    void GuiMain::update() {
        lv_label_set_text(m_cpuInfoLabel, std::to_string(g_keysHeld).c_str());
    }

}