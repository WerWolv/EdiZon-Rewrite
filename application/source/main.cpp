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

#include "edizon.hpp"
#include <Borealis.hpp>

#include <vector>
#include <functional>

#include "helpers/curl.hpp"
#include "helpers/utils.hpp"

#include "ui/gui.hpp"
#include "ui/gui_main.hpp"


using namespace edz;
using namespace edz::ui;

EResult initServices() {
    // UI (Borealis)
    if (!Application::init(StyleEnum::ACCURATE))
        return ResultEdzBorealisInitFailed;

    // Curl
    if (EResult(curl_global_init(CURL_GLOBAL_ALL)).failed())
        return ResultEdzCurlInitFailed;

    // Title querying
    TRY(ncmInitialize());
    TRY(nsInitialize());

    // Account querying
    TRY(accountInitialize());

    // Parential control lockdown
    TRY(pctlInitialize());

    // Overclock
    TRY(pcvInitialize());
    TRY(clkrstInitialize());

    // Language code setting querying
    TRY(setInitialize());

    // Companion sysmodule launching
    TRY(pmdmntInitialize());
    TRY(pmshellInitialize());
    TRY(pminfoInitialize());

    // Controller LED
    TRY(hidsysInitialize());
    TRY(helper::controllerLEDInitialize());

    return ResultSuccess;
}

void exitServices() {
    curl_global_cleanup();
    ncmExit();
    nsExit();
    accountExit();
    pctlExit();
    pcvExit();
    clkrstExit();
    setExit();
    pmdmntExit();
    pmshellExit();
    pminfoExit();
    hidsysExit();

    Application::quit();
}

int main(int argc, char* argv[]) {  
    EResult res = initServices();
    if (res.failed()) {
        Gui::fatal(edz::LangEntry("edz.fatal.service.init").get() + res.getString());

        exitServices();
        return -1;
    }

    Gui::changeTo<GuiMain>();

    while (Application::mainLoop())
        Gui::tick();
    
    exitServices();
}