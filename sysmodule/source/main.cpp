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

#include "overlay/gui/gui.hpp"
#include "overlay/gui/gui_main.hpp"

#include "overlay/screen.hpp"
#include "helpers/results.hpp"
#include "helpers/hidsys_shim.hpp"
#include "helpers/utils.hpp"
#include "cheat/cheat.hpp"
#include "services/edz_service.hpp"

#include <stdio.h>
#include <array>
#include <mutex>
#include <thread>
#include <atomic>

using namespace edz;

namespace ams::result {
    bool CallFatalOnResultAssertion = false;
}

// Libnx initialization
extern "C" {

    u32 __nx_applet_type = AppletType_None;

    #define INNER_HEAP_SIZE 0x300000
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

}

void __appInit(void) {
    ams::hos::SetVersionForLibnx();

    // Use ams::sm::DoWithSession instead of smInitialize so we're not unnecessarily using up a sm session
    ams::sm::DoWithSession([] {
        EResult res;

        // Overlay focus requesting and main loop
        ER_ASSERT_RESULT(appletInitialize(), MAKERESULT(Module_Libnx, LibnxError_InitFail_AM));
        ER_ASSERT_RESULT(hidsysInitialize(), MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        // Filesystem
        ER_ASSERT_RESULT(fsInitialize(),     MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

        // Key presses
        ER_ASSERT_RESULT(hidInitialize(),    MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        // Running title information
        ER_ASSERT(pmdmntInitialize());

        // Temparature display
        ER_ASSERT(tsInitialize());

        // Nintendo Font
        ER_ASSERT(plInitialize());

        // Cheat toggling
        ER_ASSERT(dmntcht::initialize());
        ER_ASSERT(cheat::CheatManager::initialize());

        // Overlay drawing
        ER_ASSERT_RESULT(ovl::Screen::initialize(), edz::ResultEdzScreenInitFailed);

        // Clock speed display
        if (ams::hos::GetVersion() >= ams::hos::Version_800) {
            ER_ASSERT(clkrstInitialize());
        } else {
            ER_ASSERT(pcvInitialize());
        }

    });

    // SD card access
    ER_ASSERT(fsdevMountSdmc());
}

void __appExit(void) {
    appletExit();
    hidsysExit();
    fsExit();
    hidExit();
    pmdmntExit();
    tsExit();
    plExit();
    dmntcht::exit();
    cheat::CheatManager::exit();
    ovl::Screen::exit();
    clkrstExit();
    pcvExit();
}


static Event s_overlayComboEvent;
static std::mutex s_hidMutex;

static u64 s_keysDown, s_keysHeld;
static JoystickPosition s_joyStickPosition[2];
static touchPosition s_touchPosition;

// Joycon and touch inputs reader loop
static void hidLoop(void *args) {
    JoystickPosition tmpJoyStickPosition[2];
    touchPosition tmpTouchPosition;

    while (appletMainLoop()) {
        // Scan for button presses
        hidScanInput();

        // Read in touch positions
        hidTouchRead(&tmpTouchPosition, 0);
        
        // Read in joystick values
        hidJoystickRead(&tmpJoyStickPosition[HidControllerJoystick::JOYSTICK_LEFT], CONTROLLER_HANDHELD, HidControllerJoystick::JOYSTICK_LEFT);
        hidJoystickRead(&tmpJoyStickPosition[HidControllerJoystick::JOYSTICK_RIGHT], CONTROLLER_HANDHELD, HidControllerJoystick::JOYSTICK_RIGHT);

        {
            std::scoped_lock lock(::s_hidMutex);

            ::s_keysDown        = hidKeysDown(CONTROLLER_HANDHELD);
            ::s_keysHeld        = hidKeysHeld(CONTROLLER_HANDHELD);
            ::s_touchPosition   = tmpTouchPosition;

            ::s_joyStickPosition[HidControllerJoystick::JOYSTICK_LEFT]  = tmpJoyStickPosition[HidControllerJoystick::JOYSTICK_LEFT];
            ::s_joyStickPosition[HidControllerJoystick::JOYSTICK_RIGHT] = tmpJoyStickPosition[HidControllerJoystick::JOYSTICK_RIGHT];
        }

        // Detect overlay key-combo
        if ((::s_keysHeld & (KEY_L | KEY_DDOWN)) == (KEY_L | KEY_DDOWN) && ::s_keysDown & KEY_RSTICK)// && hlp::isTitleRunning())
            eventFire(&::s_overlayComboEvent);
        
        // Sleep 20ms
        svcSleepThread(20E6); 
    }
} 

// Overlay drawing loop
static void ovlLoop(void *args) {
    ovl::Screen *screen = new ovl::Screen();
    ovl::gui::Gui::init(screen);

    while (appletMainLoop()) {
        // Wait for the overlay key combo event to trigger
        eventWait(&::s_overlayComboEvent, U64_MAX);
        eventClear(&::s_overlayComboEvent);

        //focusOverlay(true);

        // By default, open GuiMain
        ovl::gui::Gui::changeTo<ovl::gui::GuiMain>();
        ovl::gui::Gui *gui = ovl::gui::Gui::getCurrGui();

        ovl::gui::Gui::playIntroAnimation();

        // Draw the overlay till the user closes the overlay
        while (true) {

            {
                std::scoped_lock lock(::s_hidMutex);

                ovl::gui::Gui::tick(::s_keysDown, ::s_keysHeld, ::s_joyStickPosition, ::s_touchPosition);
            }
            
            if (gui->shouldClose())
                break;

            ovl::Screen::waitForVsync();
        }

        // Clear the screen
        screen->clear();
        screen->flush();

       //focusOverlay(false);

       // Clear the key-combo event again in case the combination was pressed again while the overlay was open already
        eventClear(&::s_overlayComboEvent);
    }
    
    ovl::gui::Gui::exit();
    delete screen;
}


namespace {

    using ServerOptions = ams::sf::hipc::DefaultServerManagerOptions;

    constexpr ams::sm::ServiceName  SettingsServiceName = ams::sm::ServiceName::Encode("edz:set");
    constexpr size_t                SettingsMaxSessions = 3;

    constexpr size_t NumServers  = 1;
    constexpr size_t MaxSessions = SettingsMaxSessions + 1;

    ams::sf::hipc::ServerManager<NumServers, ServerOptions, MaxSessions> g_serverManager;

}

using namespace ams;


int main(int argc, char* argv[]) {
    ams::os::Thread hidThread, ovlThread;
    
    g_serverManager.RegisterServer<edz::serv::EdzService>(SettingsServiceName, SettingsMaxSessions);


    // Create an event 
    eventCreate(&::s_overlayComboEvent, true);

    // Setup and start the key-combo scanner and the overlay render thread
    hidThread.Initialize(hidLoop, nullptr, 0x500, 0x2C);
    ovlThread.Initialize(ovlLoop, nullptr, 0x3000, 0x2C);

    hidThread.Start();
    ovlThread.Start();

    // We're done with all our work on the main thread. Go to sleep
    while (true)
        svcSleepThread(10'000'000);

    return 0;
}
