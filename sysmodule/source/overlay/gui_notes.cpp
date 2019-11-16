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

#include "overlay/gui_notes.hpp"
#include "overlay/gui_main.hpp"
#include "overlay/styles.hpp"
#include "cheat/cheat.hpp"
#include "helpers/utils.hpp"

namespace edz::ovl {
    
    GuiNotes::GuiNotes() {

    }

    GuiNotes::~GuiNotes() {
        lv_obj_del(this->m_titleLabel);
        lv_obj_del(this->m_textPage);
        lv_obj_del(this->m_btnLeft);
        lv_obj_del(this->m_btnRight);
        lv_obj_del(this->m_btnBack);
    }

    void GuiNotes::createUI() {
        setBackgroundStyle();

        this->m_titleLabel = lv_label_create(lv_scr_act(), nullptr);
        lv_obj_set_pos(this->m_titleLabel, 10, 10);
        lv_label_set_text(this->m_titleLabel, "Notes");
        setLabelTitleStyle(this->m_titleLabel);

        this->m_textPage = lv_page_create(lv_scr_act(), nullptr);
        setPageStyle(this->m_textPage);
        lv_obj_set_pos(this->m_textPage, 20, 50);
        lv_obj_set_size(this->m_textPage, 220, 335);

        this->m_textLabel = lv_label_create(this->m_textPage, nullptr);
        setLabelStyle(this->m_textLabel);
        lv_label_set_text(this->m_textLabel, "dsadf asdhf sdf kjasdhfk ashdf  kashdfasdf asdfkashdfkha sdkfdf as dfa sdf as dfa sdf  asdf  asdf  asd f asd f asd fasdfasdf  asdfa sd fas dfa sdfa sdfas df asdf asdf asdf asdfa sdghgsdeg sdfa sddsadf asdhf sdf kjasdhfk ashdf  kashdfasdf asdfkashdfkha sdkfdf as dfa sdf as dfa sdf  asdf  asdf  asd f asd f asd fasdfasdf  asdfa sd fas dfa sdfa sdfas df asdf asdf asdf asdfa sdghgsdeg sdfa sddsadf asdhf sdf kjasdhfk ashdf  kashdfasdf asdfkashdfkha sdkfdf as dfa sdf as dfa sdf  asdf  asdf  asd f asd f asd fasdfasdf  asdfa sd fas dfa sdfa sdfas df asdf asdf asdf asdfa sdghgsdeg sdfa sddsadf asdhf sdf kjasdhfk ashdf  kashdfasdf asdfkashdfkha sdkfdf as dfa sdf as dfa sdf  asdf  asdf  asd f asd f asd fasdfasdf  asdfa sd fas dfa sdfa sdfas df asdf asdf asdf asdfa sdghgsdeg sdfa sddsadf asdhf sdf kjasdhfk ashdf  kashdfasdf asdfkashdfkha sdkfdf as dfa sdf as dfa sdf  asdf  asdf  asd f asd f asd fasdfasdf  asdfa sd fas dfa sdfa sdfas df asdf asdf asdf asdfa sdghgsdeg sdfa sd");
        lv_label_set_long_mode(this->m_textLabel, LV_LABEL_LONG_BREAK);
        lv_obj_set_width(this->m_textLabel, 200);

        this->m_btnLeft = lv_btn_create(lv_scr_act(), nullptr);
        this->m_btnRight = lv_btn_create(lv_scr_act(), nullptr);

        lv_obj_t *lblLeft  = lv_label_create(this->m_btnLeft, nullptr);
        lv_obj_t *lblRight = lv_label_create(this->m_btnRight, nullptr);
        setLabelStyle(lblLeft);
        setLabelStyle(lblRight);
        lv_label_set_text(lblLeft, LV_SYMBOL_LEFT);
        lv_label_set_text(lblRight, LV_SYMBOL_RIGHT);
        lv_obj_set_pos(this->m_btnLeft, 20, 400);
        lv_obj_set_pos(this->m_btnRight, 150, 400);
        lv_obj_set_size(this->m_btnLeft, 80, 40);
        lv_obj_set_size(this->m_btnRight, 80, 40);
        setButtonStyle(this->m_btnLeft);
        setButtonStyle(this->m_btnRight);

        this->m_btnBack = lv_btn_create(lv_scr_act(), nullptr);

        lv_obj_t *lblBack = lv_label_create(this->m_btnBack, nullptr);
        setLabelStyle(lblBack);
        lv_label_set_text(lblBack, "Back");
        lv_obj_set_pos(this->m_btnBack, 50, 470);
        lv_obj_set_size(this->m_btnBack, 156, 40);
        setButtonStyle(this->m_btnBack);
        lv_obj_set_event_cb(this->m_btnBack, [](lv_obj_t * obj, lv_event_t event) {
            if (event != LV_EVENT_CLICKED)
                return;

            Gui::changeTo<GuiMain>();
        });
    }

    void GuiNotes::update() {
        
    }

}