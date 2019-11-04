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
#include "overlay/styles.h"
#include "cheat/cheat.hpp"
#include <thread>

namespace edz::ovl {
    
    GuiCheats::GuiCheats(Screen *screen) : Gui(screen) {
    }

    GuiCheats::~GuiCheats() {
        lv_list_clean(this->m_list);
        lv_obj_del(this->m_list);
    }

    void GuiCheats::createUI() {
        this->m_title = lv_label_create(lv_scr_act(), nullptr);
        //set_background_style(lv_scr_act());
        lv_label_set_text(this->m_title, "Cheats");
        lv_label_set_align(this->m_title, LV_ALIGN_CENTER);
        lv_obj_set_size(this->m_title, 256, 20);

        this->m_list = lv_list_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_list, 0, 20);
        lv_obj_set_size(this->m_list, lv_obj_get_width(lv_scr_act()), lv_obj_get_height(lv_scr_act()));
        lv_obj_set_size(this->m_list, 256, 512);
        set_list_styles(this->m_list);

        edz::cheat::CheatManager::forceAttach();
        edz::cheat::CheatManager::reload();

        for (auto cheat : edz::cheat::CheatManager::getCheats()) {
            lv_obj_t *btn = lv_list_add_btn(this->m_list, nullptr, (cheat->isEnabled() ? "\xef\x80\x8c  " + cheat->getName() : "\xef\x80\x8d  " + cheat->getName()).c_str());
            lv_btn_set_style(btn, LV_BTN_STYLE_REL, get_button_released_style_enabled());
            lv_obj_set_user_data(btn, cheat);
            lv_obj_set_event_cb(btn, [](lv_obj_t * obj, lv_event_t event){
                static int i = 0;
                if (event != LV_EVENT_CLICKED) return;

                edz::cheat::Cheat *cheat = reinterpret_cast<edz::cheat::Cheat*>(obj->user_data);

                lv_obj_t *label = lv_obj_get_child(obj, nullptr);
                lv_label_set_text(label, (cheat->toggle() ? "\xef\x80\x8c  " + cheat->getName() : "\xef\x80\x8d  " + cheat->getName()).c_str());

                lv_obj_invalidate(obj);
            });
        }
    }

    void GuiCheats::update() {

    }

}