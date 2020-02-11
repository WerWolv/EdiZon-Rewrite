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

#include "ui/fonts.hpp"
#include "ui/elements/title_info.hpp"
#include "cheat/cheat.hpp"

#include "helpers/utils.hpp"

namespace edz::ui::element {

    TitleInfo::TitleInfo(std::vector<u8> &buffer, std::unique_ptr<save::Title> &title) {
        this->m_image = new brls::Image(buffer);
        this->m_image->setParent(this);
        this->m_image->setScaleType(brls::ImageScaleType::FIT);      

        this->m_table = new brls::Table();
        this->m_table->setParent(this);
        this->m_table->addRow(brls::TableRowType::HEADER, "edz.element.titleinfo.name"_lang,      hlp::limitStringLength(title->getName(), 32));
        this->m_table->addRow(brls::TableRowType::BODY,   "edz.element.titleinfo.author"_lang,    hlp::limitStringLength(title->getAuthor(), 32));
        this->m_table->addRow(brls::TableRowType::BODY,   "edz.element.titleinfo.version"_lang,   title->getVersionString() + " [ v" + std::to_string(title->getVersion()) + " ]");
        this->m_table->addRow(brls::TableRowType::BODY,   "edz.element.titleinfo.programid"_lang, title->getIDString());
        this->m_table->addRow(brls::TableRowType::BODY,   "edz.element.titleinfo.buildid"_lang,   hlp::toHexString(cheat::CheatManager::getBuildID()));
        this->m_table->addRow(brls::TableRowType::BODY,   "edz.element.titleinfo.processid"_lang, std::to_string(save::Title::getRunningProcessID()));
    }

    TitleInfo::~TitleInfo() {
        delete this->m_image;
        delete this->m_table;
    }


    void TitleInfo::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) {
        this->m_image->frame(ctx);
        this->m_table->frame(ctx);
    }

    void TitleInfo::layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash) {
        this->height = 320;
        this->m_image->setBoundaries(this->getX() + 32, this->getY() + 32, 150, 150);
        this->m_image->invalidate();
        this->m_table->setBoundaries(this->getX() + 182, this->getY() + 32, 500, this->m_table->getHeight());
        this->m_table->invalidate();
    }

    brls::View* TitleInfo::requestFocus(brls::FocusDirection direction, brls::View *oldFocus, bool fromUp) {        
        return this;
    }

    void TitleInfo::drawHighlight(NVGcontext* vg, brls::ThemeValues* theme, float alpha, brls::Style* style, bool background) {
        // The view has to be focusable for scrolling to work but we don't want the highlight to be drawn
    }

}