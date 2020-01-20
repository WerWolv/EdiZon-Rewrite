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

#include <edizon.hpp>
#include <borealis.hpp>
#include <curl/curl.h>

#include <thread>

#include "helpers/debug_helpers.hpp"
#include "helpers/utils.hpp"
#include "helpers/config_manager.hpp"
#include "helpers/background_tasks.hpp"

#include "cheat/cheat.hpp"

#include "ui/fonts.hpp"
#include "ui/gui.hpp"
#include "ui/gui_splash.hpp"
#include "ui/gui_main.hpp"

using namespace edz;

EResult initServices() {
    // Already initialized by Borealis but also used in EdiZon: romfs, sockets, pl and set:sys

    // Initialize Borealis (UI library)
    if (!brls::Application::init())
        return ResultEdzBorealisInitFailed;

    // Extra fonts
    ui::Fonts::initFonts();

    // Curl
    if (EResult(curl_global_init(CURL_GLOBAL_ALL)).failed())
        return ResultEdzCurlInitFailed;

    // Title querying
    ER_TRY(ncmInitialize());
    ER_TRY(nsInitialize());

    // Account querying
    ER_TRY(accountInitialize(AccountServiceType_Administrator));

    // Overclock
    if (hosversionBefore(8, 0, 0)) {
        ER_TRY(pcvInitialize());
    } else {
        ER_TRY(clkrstInitialize());
    }

    // Language code setting querying
    ER_TRY(setInitialize());

    // Companion sysmodule launching
    ER_TRY(pmdmntInitialize());
    ER_TRY(pmshellInitialize());
    ER_TRY(pminfoInitialize());

    // Controller LED
    ER_TRY(hidsysInitialize());
    ER_TRY(hlp::controllerLEDInitialize());

    // Play time querying
    ER_TRY(pdmqryInitialize());

    // Atmosphere version querying
    ER_TRY(splInitialize());

    // Atmosphere cheat service (Cheat toggling and memory editing)
    if (cheat::CheatManager::isCheatServiceAvailable()) {
        dmntcht::initialize();
        cheat::CheatManager::initialize();
    }

    return ResultSuccess;
}

void exitServices() {
    // Cleaned up by Borealis: romfs, sockets, pl and set:sys

    curl_global_cleanup();
    ncmExit();
    nsExit();
    accountExit();
    pcvExit();
    clkrstExit();
    setExit();
    pmdmntExit();
    pmshellExit();
    pminfoExit();
    hidsysExit();
    pdmqryExit();
    splExit();
    dmntcht::exit();
    cheat::CheatManager::exit();
    brls::Application::quit();
}

EResult createFolderStructure() {
    std::string paths[] = {
        EDIZON_BACKUP_DIR,
        EDIZON_BATCH_BACKUP_DIR,
        EDIZON_COMMON_RESTORE_DIR,
        EDIZON_CONFIGS_DIR,
        EDIZON_SCRIPTS_DIR,
        EDIZON_LIBS_DIR,
        EDIZON_CHEATS_DIR,
        EDIZON_TMP_DIR};

    for (auto path : paths) {
        hlp::Folder folder(path);
        ER_TRY(folder.createDirectories());
    }

    return ResultSuccess;
}

int main(int argc, char *argv[]) {
    brls::Logger::setLogLevel(VERBOSE_LOG_OUTPUT ? brls::LogLevel::DEBUG : brls::LogLevel::ERROR);

    // Try to initialize all services
    if (EResult res = initServices(); res.failed()) {
        ui::Gui::fatal("edz.fatal.service.init"_lang + res.getString());

        exitServices();
        return -1;
    }

    // Create folder structure
    if (EResult res = createFolderStructure(); res.failed()) {
        ui::Gui::fatal("edz.fatal.folder_structure.init"_lang);

        exitServices();
        return -2;
    }

    // Make sure EdiZon cannot be forcefully closed (Homebutton / Close Application) without cleanup code being ran first
    appletLockExit();

    // Redirects stdout and stderr to a log file if the compile time flag was set
    stdioRedirectToFile();

    // Clear the tmp folder
    hlp::clearTmpFolder();

    // Load config file
    hlp::ConfigManager::load();

    printf("\033[0;33mWelcome to EdiZon\033[0m\n");

// Set the startup Gui
#if SPLASH_ENABLED
    ui::Gui::changeTo<ui::GuiSplash>();
#else
    ui::Gui::changeTo<ui::GuiMain>();
#endif

    // Start background tasks
    hlp::BackgroundTasks backgroundTasks;

    // Main loop for UI
    while (brls::Application::mainLoop() && appletMainLoop()) {
        ui::Gui::tick();
    }

    // Cleanup after ourselves
    exitServices();
    stdioRedirectCleanup();

    // Cleanup has ran, EdiZon can now be exited normally
    appletUnlockExit();
}