#pragma once

#include "edizon.hpp"

#include <string>
#include <functional>

namespace edz::helper {

    bool askPctl(std::function<void()> f);

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