#pragma once

#include "edizon.hpp"

#include <string>
#include <functional>

namespace edz::helper {

    bool askPctl(std::function<void()> f);
    bool askSwkbdText(std::function<void(std::string)> f, std::string headerText = "", std::string guideText = "", u8 maxStringLength = 32, std::string initialText = "");
    bool askSwkbdNumber(std::function<void(std::string)> f, std::string headerText = "", std::string guideText = "", std::string leftButton = "", std::string rightButton = "", u8 maxStringLength = 32, std::string initialText = "");
    bool isServiceRunning(const char *serviceName);

    bool isOnAMS();
    bool isOnRNX();
    bool isOnSXOS();

    std::string getCurrentTimeString();
    u8 getCurrentBatteryPercentage();

    void overclockSystem(bool enable);

    void controllerLEDInitialize();
    void setLedState(bool state);
    
}