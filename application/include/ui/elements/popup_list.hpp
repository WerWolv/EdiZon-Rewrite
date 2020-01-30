/**
 * Copyright (C) 2019 - 2020 WerWolv
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

namespace edz::ui::element {

    class PopupList : public brls::View {
    public:
        PopupList(std::string lButton, std::string rButton, brls::EventListener lButtonEvent, brls::EventListener rButtonEvent);
        ~PopupList();

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) override;
        void layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash) override;
        brls::View* requestFocus(brls::FocusDirection direction, View *oldFocus, bool fromUp = false) override;

        bool onCancel();

        void addItem(brls::ListItem *listItem);

        void open();
        void close();
    private:
        u16 m_frameX, m_frameY, m_frameWidth, m_frameHeight;
        brls::List *m_list;

        brls::Button *m_lButton, *m_rButton;

        bool isTranslucent() override {
            return true;
        }
    };

}
