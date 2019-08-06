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

#include <switch.h>
#include <result.hpp>

#include <functional>

namespace edz::comms {

    typedef struct {
        u64 magic;  // EDZN
        enum class PacketType : u16 { HEART_BEAT = 0, GET = 1, SET = 2, CONTINUE = 3 } type;
        enum class Origin : u8 { TCP = 'T', USB = 'U' } origin;
        u16 payloadSize;
        u8 payload[0xFFFF - sizeof(magic) - sizeof(PacketType) - sizeof(Origin) - sizeof(payloadSize)];
    } PACKED bridge_packet_t;
    static_assert(sizeof(bridge_packet_t) == 0xFFFF, "Incorrect bridge packet definition!");

    class USBManager {
    public:
        USBManager() = delete;

        static EResult initialize();
        static void exit();

        static void process();

        static EResult read(std::function<void(u8 *buffer, size_t size)> callback, size_t bufferSize, u32 timeout);
        static EResult write(u8 *buffer, size_t size, u32 timeout);

        static bool isBusy();
        static EResult abort();

    private:
        static inline bool s_initialized = false;
        static inline bool s_busy = false;
        static inline bool s_listening = false;

        static inline RwLock s_usbLock;
        static inline RwLock s_interfaceLock, s_inLock, s_outLock;

        static inline UsbDsInterface *s_interface;
        static inline UsbDsEndpoint *s_endpointIn, *s_endpointOut;
        static inline u8 *s_endpointInBuffer, *s_endpointOutBuffer;

        static EResult usbCommsInterfaceInit5x();
        static EResult usbCommsInterfaceInit1x();
    };

}