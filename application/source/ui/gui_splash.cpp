/**
 * Copyright (C) 2020 WerWolv
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

#include "ui/gui_splash.hpp"
#include "ui/pages/page_splash.hpp"
#include "ui/gui_main.hpp"

namespace edz::ui {
    
    bool isEnoughMemoryReserved() {
        bool enoughMemoryReserved = false;

        if (hlp::isInApplicationMode())
            return true;

        {
            auto [result, value] = hlp::readSetting<u64>("hbloader", "applet_heap_size");

            if (result.succeeded())
                enoughMemoryReserved = (value == 0 || value > 0x1910'0000);
            
            else return false;
        }

        {
            auto [result, value] = hlp::readSetting<u64>("hbloader", "applet_heap_reservation_size");

            if (result.succeeded())
                enoughMemoryReserved = enoughMemoryReserved && (value >= 0x0860'0000);
            
            else return false;
        }

        return enoughMemoryReserved;
    }

    brls::View* GuiSplash::setupUI() {
        bool isAMSVersionSupported = hlp::getAtmosphereVersion() >= MINIMUM_AMS_VERSION_REQUIRED;

        bool enoughMemoryReserved = isEnoughMemoryReserved();

        if (isAMSVersionSupported && enoughMemoryReserved)
            Gui::runLater([this] { Gui::changeTo<GuiMain>(); }, 20);

        if (!isAMSVersionSupported)
            return new ui::page::PageSplash(ui::page::PageSplash::WarningType::TooLowAtmosphereVersion);
        else if (!enoughMemoryReserved)
            return new ui::page::PageSplash(ui::page::PageSplash::WarningType::NotEnoughMemoryReserved);
        else
            return new ui::page::PageSplash(ui::page::PageSplash::WarningType::None);
    }

    void GuiSplash::update() {

    }

}