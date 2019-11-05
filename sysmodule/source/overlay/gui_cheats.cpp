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
    }

    GuiCheats::~GuiCheats() {
        lv_list_clean(this->m_list);
        lv_obj_del(this->m_list);
    }

    void GuiCheats::createUI() {
        this->m_list = lv_list_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_list, 0, 20);
        lv_obj_set_size(this->m_list, lv_obj_get_width(lv_scr_act()), lv_obj_get_height(lv_scr_act()));
        lv_obj_set_size(this->m_list, 256, 512);
        setListStyle(this->m_list);

        edz::cheat::CheatManager::forceAttach();
        edz::cheat::CheatManager::reload();


        for (auto cheat : edz::cheat::CheatManager::getCheats()) {
            lv_obj_t *btn = lv_list_add_btn(this->m_list, nullptr, (hlp::limitStringLength(cheat->getName(), 25)).c_str());
            lv_btn_set_toggle(btn, true);
            lv_obj_set_height(btn, 70);

            if (cheat->isEnabled() == lv_btn_get_toggle(btn))
                lv_btn_toggle(btn);

            lv_obj_set_user_data(btn, cheat);
            lv_obj_set_event_cb(btn, [](lv_obj_t * obj, lv_event_t event){
                if (event != LV_EVENT_CLICKED)
                    return;

                reinterpret_cast<edz::cheat::Cheat*>(obj->user_data)->toggle();
                lv_obj_invalidate(obj);
            });
        }
    }

    void GuiCheats::update() {

    }

}