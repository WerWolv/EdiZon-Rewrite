#pragma once

#include <switch.h>
#include <result.hpp>

#include <functional>

namespace edz::comms {

    class USBManager {
    public:
        USBManager() = delete;

        static EResult initialize();
        static void exit();

        static void process();

        static void read(std::function<void(u8 *buffer, size_t size)> callback, size_t bufferSize, u32 timeout);
        static void write(u8 *buffer, size_t size, u32 timeout);
    };

}