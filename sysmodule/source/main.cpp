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

#include <switch.h>

#include <stratosphere.hpp>

#include "vc/edz_vc_service.hpp"
#include "vc/vc_manager.hpp"
#include "overlay/font.hpp"
#include "overlay/screen.hpp"
#include "helpers/results.hpp"

#include "comms/tcp.hpp"
#include "comms/usb.hpp"

#include <stdio.h>

extern "C" {

    u32 __nx_applet_type = AppletType_None;

    #define INNER_HEAP_SIZE 0x200000
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];

    u32 __nx_nv_transfermem_size = 0x15000;

    void __libnx_initheap(void) {
        void*  addr = nx_inner_heap;
        size_t size = nx_inner_heap_size;

        // Newlib
        extern char* fake_heap_start;
        extern char* fake_heap_end;

        fake_heap_start = (char*)addr;
        fake_heap_end   = (char*)addr + size;
    }

    void __attribute__((weak)) __appInit(void) {
        edz::EResult res;
        smInitialize();

        if (hosversionGet() == 0) {
            res = setsysInitialize();
            if (res.succeeded()) {
                SetSysFirmwareVersion fw;
                res = setsysGetFirmwareVersion(&fw);
                if (res.succeeded())
                    hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
                setsysExit();
            }
        }

        SocketInitConfig sockConf = {
            .bsdsockets_version = 1,

            .tcp_tx_buf_size = 0x800,
            .tcp_rx_buf_size = 0x1000,
            .tcp_tx_buf_max_size = 0x4000,
            .tcp_rx_buf_max_size = 0x4000,

            .udp_tx_buf_size = 0x2400,
            .udp_rx_buf_size = 0xA500,

            .sb_efficiency = 4,

            .serialized_out_addrinfos_max_size = 0x1000,
            .serialized_out_hostent_max_size = 0x200,
            .bypass_nsd = false,
            .dns_timeout = 0,
        };

        res = socketInitialize(&sockConf);
        if (res.failed())
            fatalSimple(res);

        res = edz::comms::tcp::TCPManager::initialize();
        if (res.failed())
            fatalSimple(edz::ResultEdzTCPInitFailed);

        res = edz::comms::usb::USBManager::initialize();
        if (res.failed())
            fatalSimple(edz::ResultEdzUSBInitFailed);

        res = hidInitialize();
        if (res.failed())
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        res = hiddbgInitialize();
        if (res.failed())
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        res = fsInitialize();
        if (res.failed())
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

        res = plInitialize();
        if (res.failed())
            fatalSimple(edz::ResultEdzFontInitFailed);

        fsdevMountSdmc();

        if (edz::ovl::Screen::initialize().failed())
            fatalSimple(edz::ResultEdzScreenInitFailed);

    }

    void __attribute__((weak)) userAppExit(void);

    void __attribute__((weak)) __appExit(void) {
        fsdevUnmountAll();
        fsExit();
        timeExit();
        hidExit();
        hiddbgExit();
        smExit();
        plExit();
        
        edz::comms::tcp::TCPManager::exit();
        edz::comms::usb::USBManager::exit();
        edz::ovl::Screen::exit();
    }

}


static void netLoop(void *args) {
    while (true) {
        edz::comms::tcp::TCPManager::process();
        svcSleepThread(1E6); // Sleep 1ms
    }
} 

static void usbLoop(void *args) {
    while (true) {
        edz::comms::usb::USBManager::process();
        svcSleepThread(1E6); // Sleep 1ms
    }
} 

static bool cheatOverlayVisible = true;

static void hidLoop(void *args) {
    HidControllerID controllerID = HidControllerID::CONTROLLER_UNKNOWN;
    u64 kDown = 0;
    u64 kHeld = 0;
    JoystickPosition joyStickPos[2] = { 0 };
    touchPosition touchPos = { 0 };

    while (true) {
        hidScanInput();

        controllerID = hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1;
        hidJoystickRead(&joyStickPos[0], CONTROLLER_HANDHELD, HidControllerJoystick::JOYSTICK_LEFT);
        hidJoystickRead(&joyStickPos[1], CONTROLLER_HANDHELD, HidControllerJoystick::JOYSTICK_RIGHT);
        hidTouchRead(&touchPos, 0);
        kDown = hidKeysDown(CONTROLLER_HANDHELD);
        kHeld = hidKeysHeld(CONTROLLER_HANDHELD);

        svcSleepThread(20E6); // Sleep 20ms
        //edz::vc::VirtualControllerManager::getInstance().process(kDown);

        if (kDown & KEY_MINUS)
            cheatOverlayVisible = !cheatOverlayVisible;

    }
} 

static void ovlLoop(void *args) {
    //svcSleepThread(5E9); // Wait 5 seconds to make sure pl is ready
    edz::ovl::StbFont::initialize();
    edz::ovl::StbFont *font = new edz::ovl::StbFont();
    font->setFontSize(0, 32);
    edz::ovl::Screen *screen = new edz::ovl::Screen();

    edz::ovl::rgba4444_t color(0, 0, 0, 0xF);

    while (true) {
        screen->fill(color);
        screen->drawText(font, 10, 50, edz::ovl::rgba4444_t(0xF, 0xF, 0xF, 0xF), "Hello World");
        screen->flush();
    }

    delete screen;
    delete font;
    edz::ovl::StbFont::exit();
    
}

int main(int argc, char* argv[]) {
    Thread netThread, usbThread, hidThread, ovlThread;

    //edz::vc::VirtualControllerManager::getInstance().connectVC(0xFFFF00FF, 0xFFFFFFFF);

    threadCreate(&netThread, netLoop, nullptr, 0x20500, 0x2C, -2);
    threadCreate(&usbThread, usbLoop, nullptr, 0x30000, 0x2C, -2);
    threadCreate(&hidThread, hidLoop, nullptr, 0x500, 0x2C, -2);
    threadCreate(&ovlThread, ovlLoop, nullptr, 0x2000, 0x2C, -2);

    threadStart(&netThread);
    threadStart(&usbThread);
    //threadStart(&hidThread);
    //threadStart(&ovlThread);

    consoleDebugInit(debugDevice_SVC);

    while(true) {
        svcSleepThread(10E9);
    }
    static auto serverManager = WaitableManager(1);
    serverManager.AddWaitable(new ServiceServer<edz::vc::EdzVCService>("edz:vc", 1));
    
    serverManager.Process();
    
    return 0;
}
