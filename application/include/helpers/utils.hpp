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

#pragma once

#include <edizon.hpp>

#include <string>
#include <functional>

namespace edz::hlp {

    namespace edz::save {
        class Title;
    }

    bool askPctl(std::function<void()> f);
    bool askSwkbdText(std::function<void(std::string)> f, std::string headerText = "", std::string subText = "", u8 maxStringLength = 32, std::string initialText = "");
    bool askSwkbdNumber(std::function<void(std::string)> f, std::string headerText = "", std::string subText = "", std::string leftButton = "", std::string rightButton = "", u8 maxStringLength = 32, std::string initialText = "");
    bool isServiceRunning(const char *serviceName);

    bool isOnAMS();
    bool isOnRNX();
    bool isOnSXOS();

    std::string getLFSTitlesPath();
    std::string getLFSCheatsPath(edz::save::Title *title);

    template <typename ...Args>
    std::string formatString(const std::string& format, Args && ...args) {
        auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
        std::string output(size + 1, '\0');

        std::sprintf(&output[0], format.c_str(), std::forward<Args>(args)...);

        return output;
    }

    std::string getCurrentTimeString();
    u8 getCurrentBatteryPercentage();

    void overclockSystem(bool enable);

    EResult controllerLEDInitialize();
    void setLedState(bool state);
    
    EResult startBackgroundService(bool startOnBoot);
    EResult stopBackgroundService(bool startOnBoot);

}