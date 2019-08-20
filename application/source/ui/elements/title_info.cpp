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

#include "helpers/utils.hpp"

namespace edz::ui::element {

    TitleInfo::TitleInfo(u8 *buffer, size_t bufferSize, std::shared_ptr<save::Title> title, processid_t processID, buildid_t buildID) {
        this->m_image = new Image(buffer, bufferSize);
        this->m_image->setParent(this);
        this->m_image->setImageScaleType(ImageScaleType::SCALE);

        this->m_table = new Table();
        this->m_table->setParent(this);
        this->m_table->addRow(TableRowType::HEADER, "Name",      hlp::limitStringLength(title->getName(), 32));
        this->m_table->addRow(TableRowType::BODY,   "Author",    hlp::limitStringLength(title->getAuthor(), 32));
        this->m_table->addRow(TableRowType::BODY,   "Version",   title->getVersionString() + " [ v" + std::to_string(title->getVersion()) + " ]");
        this->m_table->addRow(TableRowType::BODY,   "TitleID",   title->getIDString());
        this->m_table->addRow(TableRowType::BODY,   "ProcessID", std::to_string(processID));
        this->m_table->addRow(TableRowType::BODY,   "BuildID",   hlp::toHexString(buildID));
    }

    TitleInfo::~TitleInfo() {
        delete this->m_image;
        delete this->m_table;
    }


    void TitleInfo::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) {
        this->m_image->frame(ctx);
        this->m_table->frame(ctx);
    }

    void TitleInfo::layout(NVGcontext* vg, Style *style, FontStash *stash) {
        this->height = 320;
        this->m_image->setBoundaries(this->getX() + 32, this->getY() + 32, 150, 150);
        this->m_image->invalidate();
        this->m_table->setBoundaries(this->getX() + 182, this->getY() + 32, 500, this->m_table->getHeight());
        this->m_table->invalidate();
    }

    View* TitleInfo::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp) {        
        return this;
    }

}