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

#include "ui/pages/page_login.hpp"

#include "helpers/utils.hpp"
#include "api/switchcheatsdb_api.hpp"
#include "helpers/config_manager.hpp"

#include <math.h>

namespace edz::ui::page {

    PageLogin::PageLogin(brls::ListItem *loginItem) {
        this->m_label = new brls::Label(brls::LabelStyle::MEDIUM, "edz.page.login.hint"_lang, true);
        this->m_emailItem = new brls::ListItem("edz.page.login.email"_lang);
        this->m_passwordItem = new brls::ListItem("edz.page.login.password"_lang);
        this->m_loginBtn = new brls::Button(brls::ButtonStyle::PLAIN, "edz.page.login.login"_lang);

        this->m_label->setParent(this);
        this->m_emailItem->setParent(this);
        this->m_passwordItem->setParent(this);
        this->m_loginBtn->setParent(this);

        this->m_label->setHorizontalAlign(NVG_ALIGN_CENTER);

        this->m_emailItem->setClickListener([&](View *view) {
            hlp::openSwkbdForText([&](std::string text) {
                this->m_email = text;
                static_cast<brls::ListItem*>(view)->setValue(text);
            }, "edz.page.login.email"_lang, "edz.page.login.email.help"_lang);
        });
        
        this->m_passwordItem->setClickListener([&](View *view) {
            hlp::openSwkbdForPassword([&](std::string text) {
                this->m_password = text;

                std::string itemText = "";
                for (u8 i = 0; i < text.length(); i++)
                    itemText += "●";

                static_cast<brls::ListItem*>(view)->setValue(itemText);
            }, "edz.page.login.password"_lang, "edz.page.login.password.help"_lang);
        });

        this->m_loginBtn->setClickListener([=](View *view) {
            brls::Dialog *dialog = new brls::Dialog("Error");
            dialog->addButton("edz.dialog.okay"_lang, [=](View *view) { dialog->close(); });

            api::SwitchCheatsDBAPI scdbApi;

            auto [result, token] = scdbApi.getToken(this->m_email, this->m_password);

            this->m_password = "";

            if (result != ResultSuccess) {
                dialog->open();
                return;
            }

            SET_CONFIG(Update.loggedIn, true);
            SET_CONFIG(Update.switchcheatsdbEmail, this->m_email);
            SET_CONFIG(Update.switchcheatsdbApiToken, token);

            loginItem->setValue(this->m_email);
            brls::Application::popView();
        });

    }

    PageLogin::~PageLogin() {
        delete this->m_label;
        delete this->m_emailItem;
        delete this->m_passwordItem;
        delete this->m_loginBtn;
    }

    void PageLogin::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {
        this->m_label->frame(ctx);
        this->m_emailItem->frame(ctx);
        this->m_passwordItem->frame(ctx);
        this->m_loginBtn->frame(ctx);
    }

    brls::View* PageLogin::requestFocus(brls::FocusDirection direction, brls::View* oldFocus, bool fromUp) {
        if (direction == brls::FocusDirection::NONE)
            return this->m_emailItem;

        if (direction == brls::FocusDirection::DOWN) {
            if (oldFocus == this->m_emailItem)
                return this->m_passwordItem;
            if (oldFocus == this->m_passwordItem)
                return this->m_loginBtn;
        } else if (direction == brls::FocusDirection::UP) {
            if (oldFocus == this->m_passwordItem)
                return this->m_emailItem;
            if (oldFocus == this->m_loginBtn)
                return this->m_passwordItem;
        }

        return nullptr;
    }

    void PageLogin::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
        this->m_label->setBoundaries((1280 / 2) - (1080 / 2), 175, 1080, this->m_label->getHeight());
        this->m_emailItem->setBoundaries((1280 / 2) - (700 / 2), 300, 700, style->List.Item.height);
        this->m_passwordItem->setBoundaries((1280 / 2) - (700 / 2), 400, 700, style->List.Item.height);
        this->m_loginBtn->setBoundaries((1280 / 2) - (300 / 2), 520, 300, style->List.Item.height);

        this->m_label->invalidate();
        this->m_emailItem->invalidate();
        this->m_passwordItem->invalidate();
        this->m_loginBtn->invalidate();
    }

    bool PageLogin::onCancel() {
        brls::Application::popView();
        return true;
    }

}