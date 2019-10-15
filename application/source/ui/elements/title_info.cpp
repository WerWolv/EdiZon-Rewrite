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

#include "ui/elements/title_info.hpp"
#include "cheat/cheat.hpp"

#include "helpers/utils.hpp"

namespace edz::ui::element {

    TitleInfo::TitleInfo(u8 *buffer, size_t bufferSize, std::shared_ptr<save::Title> title) {
        this->m_image = new brls::Image(buffer, bufferSize);
        this->m_image->setParent(this);
        this->m_image->setScaleType(brls::ImageScaleType::FIT);      

        this->m_table = new brls::Table();
        this->m_table->setParent(this);
        this->m_table->addRow(brls::TableRowType::HEADER, "Name",      hlp::limitStringLength(title->getName(), 32));
        this->m_table->addRow(brls::TableRowType::BODY,   "Author",    hlp::limitStringLength(title->getAuthor(), 32));
        this->m_table->addRow(brls::TableRowType::BODY,   "Version",   title->getVersionString() + " [ v" + std::to_string(title->getVersion()) + " ]");
        this->m_table->addRow(brls::TableRowType::BODY,   "TitleID",   title->getIDString());
        this->m_table->addRow(brls::TableRowType::BODY,   "ProcessID", std::to_string(save::Title::getRunningProcessID()));
        this->m_table->addRow(brls::TableRowType::BODY,   "BuildID",   hlp::toHexString(cheat::CheatManager::getBuildID()));
    }

    TitleInfo::TitleInfo(u8 *buffer, u16 width, u16 height, std::shared_ptr<save::Title> title) {
        this->m_image = new brls::Image(buffer, width, height);
        this->m_image->setParent(this);
        this->m_image->setScaleType(brls::ImageScaleType::FIT);

        this->m_table = new brls::Table();
        this->m_table->setParent(this);
        this->m_table->addRow(brls::TableRowType::HEADER, "Name",      hlp::limitStringLength(title->getName(), 32));
        this->m_table->addRow(brls::TableRowType::BODY,   "Author",    hlp::limitStringLength(title->getAuthor(), 32));
        this->m_table->addRow(brls::TableRowType::BODY,   "Version",   title->getVersionString() + " [ v" + std::to_string(title->getVersion()) + " ]");
        this->m_table->addRow(brls::TableRowType::BODY,   "TitleID",   title->getIDString());
        this->m_table->addRow(brls::TableRowType::BODY,   "ProcessID", std::to_string(save::Title::getRunningProcessID()));
        this->m_table->addRow(brls::TableRowType::BODY,   "BuildID",   hlp::toHexString(cheat::CheatManager::getBuildID()));
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