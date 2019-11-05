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

#include "overlay/screen.hpp"
#include "overlay/gui.hpp"
#include "overlay/gui_cheats.hpp"
#include "helpers/results.hpp"
#include "helpers/hidsys_shim.hpp"
#include "helpers/utils.hpp"
#include "cheat/cheat.hpp"

#include <lvgl.h>

#include <stdio.h>

using namespace edz;
using namespace ams;

constexpr ams::ncm::ProgramId EdiZonSysProgramId = { 0x0100000000ED1204 };

namespace ams::result {

    bool CallFatalOnResultAssertion = false;

}

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

        res = pmdmntInitialize();
        if (res.failed())
            fatalThrow(res);

        res = hidInitialize();
        if (res.failed())
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        res = fsInitialize();
        if (res.failed())
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));


        edz::dmntcht::initialize();
        edz::cheat::CheatManager::initialize();

        fsdevMountSdmc();

        if (edz::ovl::Screen::initialize().failed())
            fatalThrow(edz::ResultEdzScreenInitFailed);

    }

    void __attribute__((weak)) userAppExit(void);

    void __attribute__((weak)) __appExit(void) {
        fsdevUnmountAll();
        fsExit();
        timeExit();
        hidExit();
        smExit();

        pmdmntExit();
        
        edz::dmntcht::exit();
        edz::ovl::Screen::exit();
    }

}

EResult focusOverlay(bool focus) {
    aruid_t qlaunchAruid = 0, overlayAruid = 0, edzAruid = 0, applicationAruid = 0;

    pmdmntGetProcessId(&qlaunchAruid, (u64)ams::ncm::ProgramId::AppletQlaunch);
    pmdmntGetProcessId(&overlayAruid, (u64)ams::ncm::ProgramId::AppletOverlayDisp);
    pmdmntGetApplicationProcessId(&applicationAruid);
    appletGetAppletResourceUserIdOfCallerApplet(&edzAruid);

    ER_TRY(edz::hidsys::enableAppletToGetInput(!focus, qlaunchAruid));
    ER_TRY(edz::hidsys::enableAppletToGetInput(!focus, overlayAruid));
    ER_TRY(edz::hidsys::enableAppletToGetInput(!focus, applicationAruid));
    ER_TRY(edz::hidsys::enableAppletToGetInput(true,  edzAruid));

    return edz::ResultSuccess;
}

static Event overlayComboEvent;

static void hidLoop(void *args) {
    HidControllerID controllerID = HidControllerID::CONTROLLER_UNKNOWN;
    u64 kDown = 0;
    u64 kHeld = 0;
    JoystickPosition joyStickPos[2] = { 0 };
    touchPosition touchPos = { 0 };
    
    while (true) {
        hidScanInput();

        controllerID = hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1;
        hidJoystickRead(&joyStickPos[0], controllerID, HidControllerJoystick::JOYSTICK_LEFT);
        hidJoystickRead(&joyStickPos[1], controllerID, HidControllerJoystick::JOYSTICK_RIGHT);
        hidTouchRead(&touchPos, 0);
        kDown = hidKeysDown(controllerID);
        kHeld = hidKeysHeld(controllerID);

        svcSleepThread(20E6); // Sleep 20ms
        //edz::vc::VirtualControllerManager::getInstance().process(kDown);

        if ((kHeld & (KEY_L | KEY_DDOWN)) == (KEY_L | KEY_DDOWN) && kDown & KEY_RSTICK)
            eventFire(&overlayComboEvent);

    }
} 

static void ovlLoop(void *args) {
    lv_init();

    edz::ovl::Screen *screen = new edz::ovl::Screen();

    while (true) {
        eventWait(&overlayComboEvent, U64_MAX);
        eventClear(&overlayComboEvent);
        focusOverlay(true);

        edz::ovl::Gui *gui = new edz::ovl::GuiCheats(screen);

        gui->createUI();

        while (true) {
            lv_tick_inc(1);
            lv_task_handler();

            gui->tick();
            
            if (gui->shouldClose())
                break;

            svcSleepThread(1E6);
        }

        screen->clear();
        screen->flush();

        delete gui;

        focusOverlay(false);
    }
    
}


int main(int argc, char* argv[]) {
    Thread hidThread, ovlThread;

    eventCreate(&overlayComboEvent, true);

    threadCreate(&hidThread, hidLoop, nullptr, nullptr, 0x500, 0x2C, -2);
    threadCreate(&ovlThread, ovlLoop, nullptr, nullptr, 0x30000, 0x2C, -2);

    threadStart(&hidThread);
    threadStart(&ovlThread);


    while (true)
        svcSleepThread(1E9);
    
    return 0;
}
