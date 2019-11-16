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
#include <edizon.hpp>

#include <stratosphere.hpp>

#include "overlay/screen.hpp"
#include "overlay/gui.hpp"
#include "overlay/gui_main.hpp"
#include "overlay/gui_cheats.hpp"
#include "overlay/gui_notes.hpp"
#include "helpers/results.hpp"
#include "helpers/hidsys_shim.hpp"
#include "helpers/utils.hpp"
#include "cheat/cheat.hpp"
#include "services/edz_service.hpp"

#include <lvgl.h>

#include <stdio.h>
#include <array>
#include <thread>
#include <atomic>

using namespace edz;

std::atomic<u64> g_keysDown, g_keysHeld;
constexpr ams::ncm::ProgramId EdiZonSysProgramId = { EDIZON_SYSMODULE_TITLEID };

static ams::os::Event overlayComboEvent;

namespace ams::result {
    bool CallFatalOnResultAssertion = false;
}

extern "C" {

    u32 __nx_applet_type = AppletType_OverlayApplet;

    #define INNER_HEAP_SIZE 0x100000
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
        EResult res;
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

        res = appletInitialize();
        if (res.failed())
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_AM));

        res = pmdmntInitialize();
        if (res.failed())
            fatalThrow(res);

        res = hidInitialize();
        if (res.failed())
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        res = hidsysInitialize();
        if (res.failed())
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        res = fsInitialize();
        if (res.failed())
            fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

        res = tsInitialize();
        if (res.failed())
            fatalThrow(res);

        res = hiddbgInitialize();
        if (res.failed())
            fatalThrow(res);

        if (hosversionBefore(8,0,0)) {
            res = pcvInitialize();
            if (res.failed())
                fatalThrow(res);
        }
        else {
            res = clkrstInitialize();
            if (res.failed())
                fatalThrow(res);
        }

        res = dmntcht::initialize();
        if (res.failed())
            fatalThrow(res);

        res = cheat::CheatManager::initialize();
        if (res.failed())
            fatalThrow(res);

        res = fsdevMountSdmc();
        if (res.failed())
            fatalThrow(res);

        res = ovl::Screen::initialize();
        if (res.failed())
            fatalThrow(edz::ResultEdzScreenInitFailed);

    }

    void __attribute__((weak)) userAppExit(void);

    void __attribute__((weak)) __appExit(void) {
        fsdevUnmountAll();
        fsExit();
        timeExit();
        hidExit();
        smExit();
        tsExit();
        hiddbgExit();
        pcvExit();
        clkrstExit();
        pmdmntExit();
        dmntcht::exit();
        ovl::Screen::exit();
    }

}

EResult focusOverlay(bool focus) {
    aruid_t edzAruid = 0, applicationAruid = 0, appletAruid = 0;

    for (u64 programId = (u64)ams::ncm::ProgramId::AppletStart; programId < (u64)ams::ncm::ProgramId::AppletEnd; programId++) {
        pmdmntGetProcessId(&appletAruid, programId);
        
        if (appletAruid != 0)
            hidsys::enableAppletToGetInput(!focus, appletAruid);
    }

    pmdmntGetApplicationProcessId(&applicationAruid);
    appletGetAppletResourceUserIdOfCallerApplet(&edzAruid);

    hidsys::enableAppletToGetInput(!focus, applicationAruid);
    hidsys::enableAppletToGetInput(true,  edzAruid);

    return edz::ResultSuccess;
}


static void hidLoop(void *args) {
    JoystickPosition joyStickPos[2] = { 0 };
    touchPosition touchPos = { 0 };

    while (appletMainLoop()) {
        hidScanInput();

        hidJoystickRead(&joyStickPos[0], CONTROLLER_HANDHELD, HidControllerJoystick::JOYSTICK_LEFT);
        hidJoystickRead(&joyStickPos[1], CONTROLLER_HANDHELD, HidControllerJoystick::JOYSTICK_RIGHT);
        hidTouchRead(&touchPos, 0);
        g_keysDown = hidKeysDown(CONTROLLER_HANDHELD);
        g_keysHeld = hidKeysHeld(CONTROLLER_HANDHELD);

        svcSleepThread(20E6); // Sleep 20ms

        if ((g_keysHeld & (KEY_L | KEY_DDOWN)) == (KEY_L | KEY_DDOWN) && g_keysDown & KEY_RSTICK)// && hlp::isTitleRunning())
            overlayComboEvent.Signal();
    }
} 

static void ovlLoop(void *args) {
    lv_init();

    ovl::Screen *screen = new ovl::Screen();
    ovl::Gui::initialize(screen);

    while (appletMainLoop()) {
        overlayComboEvent.TimedWait(U64_MAX);
        overlayComboEvent.Reset();

        focusOverlay(true);

        ovl::Gui::changeTo<ovl::GuiMain>();
        ovl::Gui *gui = ovl::Gui::getCurrGui();

        ovl::Gui::playIntroAnimation();

        while (true) {
            lv_tick_inc(1);
            lv_task_handler();

            ovl::Gui::tick();
            
            if (gui->shouldClose())
                break;

            svcSleepThread(1E6);
        }

        screen->clear();
        screen->flush();

        focusOverlay(false);
        overlayComboEvent.Reset();
    }

    ovl::Gui::deinitialize();
}


namespace {

    using ServerOptions = ams::sf::hipc::DefaultServerManagerOptions;

    constexpr ams::sm::ServiceName EdiZonServiceName = ams::sm::ServiceName::Encode("edz:-");
    constexpr size_t               EdiZonMaxSessions = 4;

    /* dmnt:-, dmnt:cht. */
    constexpr size_t NumServers  = 1;
    constexpr size_t NumSessions = EdiZonMaxSessions;

    ams::sf::hipc::ServerManager<NumServers, ServerOptions, NumSessions> g_server_manager;

    void LoopServerThread(void *arg) {
        g_server_manager.LoopProcess();
    }

    constexpr size_t TotalThreads = EdiZonMaxSessions + 1;
    static_assert(TotalThreads >= 1, "TotalThreads");
    constexpr size_t NumExtraThreads = TotalThreads - 1;
    constexpr size_t ThreadStackSize = 0x4000;
    alignas(0x1000) u8 g_extra_thread_stacks[NumExtraThreads][ThreadStackSize];

    ams::os::Thread g_extra_threads[NumExtraThreads];

}

int main(int argc, char* argv[]) {
    ams::os::Thread hidThread, ovlThread;
    
    g_server_manager.RegisterServer<edz::serv::EdzService>(EdiZonServiceName, EdiZonMaxSessions);


    hidThread.Initialize(hidLoop, nullptr, 0x500, 0x2C);
    ovlThread.Initialize(ovlLoop, nullptr, 0x30000, 0x2C);
    
    if constexpr (NumExtraThreads > 0) {
        const u32 priority = ams::os::GetCurrentThreadPriority();
        for (size_t i = 0; i < NumExtraThreads; i++) {
            R_ASSERT(g_extra_threads[i].Initialize(LoopServerThread, nullptr, g_extra_thread_stacks[i], ThreadStackSize, priority));
        }
    }


    hidThread.Start();
    ovlThread.Start();

    if constexpr (NumExtraThreads > 0) {
        for (size_t i = 0; i < NumExtraThreads; i++) {
            R_ASSERT(g_extra_threads[i].Start());
        }
    }


    LoopServerThread(nullptr);
    

    if constexpr (NumExtraThreads > 0) {
        for (size_t i = 0; i < NumExtraThreads; i++) {
            R_ASSERT(g_extra_threads[i].Join());
        }
    }

    return 0;
}
