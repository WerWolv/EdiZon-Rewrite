#pragma once

#include "edizon.hpp"

#include <string>
#include <functional>

namespace edz::helper {

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