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

#include "ui/gui_game_image.hpp"

#include "ui/pages/page_fullscreen_image.hpp"

#include "save/title.hpp"

namespace edz::ui {
    
    brls::View* GuiGameImage::setupUI() {
        
        std::vector<u8> thumbnailBuffer(1280 * 720 * 4);
        if (save::Title::getLastTitleForgroundImage(&thumbnailBuffer[0]).failed())
            std::fill(thumbnailBuffer.begin(), thumbnailBuffer.end(), 0x00);

        auto gameImage = new brls::Image();
        gameImage->setImageRGBA(thumbnailBuffer.data(), 1280, 720);

        return new page::PageFullscreenImage(gameImage);
    }

    void GuiGameImage::update() {

    }

}