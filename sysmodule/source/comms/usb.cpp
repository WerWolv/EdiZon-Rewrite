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

#include "comms/usb.hpp"

#include "helpers/results.hpp"

#include <cstring>
#include <malloc.h>

extern "C" {
    #include "comms/usb_comms.h"
}

namespace edz::comms::usb {

    EResult USBManager::initialize() {
        USBManager::s_handleCallback = [](edz::comms::common_packet_t* recv, edz::comms::common_packet_t* send) {
            memcpy(send, recv, 0xFFFF);
        };

        return usbCommsModInitialize();
    }

    void USBManager::exit() {
        usbCommsModExit();
    }


    void USBManager::process() {
        u8 recvPacket[0xFF];
        usbCommsModRead(&recvPacket, sizeof(recvPacket), [](){ return (s32)USBManager::s_shouldAbort;});
        //USBManager::s_handleCallback(&recvPacket, &sendPacket);
        usbCommsModWrite(&recvPacket, sizeof(recvPacket), [](){ return (s32)USBManager::s_shouldAbort;});
    }

    

    void USBManager::setHandleCallback(std::function<void(edz::comms::common_packet_t*, edz::comms::common_packet_t*)> handleCallback) {
        USBManager::s_handleCallback = handleCallback;
    }

    bool USBManager::isBusy() {
        return USBManager::s_busy;
    }

    EResult USBManager::abort() {
        if (USBManager::isBusy()) {
            USBManager::s_shouldAbort = true;
            return ResultSuccess;
        }

        return ResultEdzAbortFailed;
    }

    bool isConnected() {
        u32 state = 0;
        usbDsGetState(&state);

        return state == 5;
    }

}