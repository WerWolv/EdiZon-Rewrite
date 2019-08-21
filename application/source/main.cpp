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

#include <stdlib.h>

#include "helpers/curl.hpp"
#include "helpers/utils.hpp"

#include "ui/gui.hpp"
#include "ui/gui_main.hpp"

#include "cheat/cheat.hpp"

#include "api/switchcheatsdb_api.hpp"


using namespace edz;
using namespace edz::ui;


void exitServices();

extern "C" {

    alignas(16) u8 __nx_exception_stack[0x1000];
    u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);

    void __libnx_exception_handler(ThreadExceptionDump *ctx) {
        std::string errorDesc;

        switch (ctx->error_desc) {
            case ThreadExceptionDesc_BadSVC:
                errorDesc = "Bad SVC";
                break;
            case ThreadExceptionDesc_InstructionAbort:
                errorDesc = "Instruction Abort";
                break;
            case ThreadExceptionDesc_MisalignedPC:
                errorDesc = "Misaligned Program Counter";
                break;
            case ThreadExceptionDesc_MisalignedSP:
                errorDesc = "Misaligned Stack Pointer";
                break;
            case ThreadExceptionDesc_SError:
                errorDesc = "SError";
                break;
            case ThreadExceptionDesc_Trap:
                errorDesc = "SIGTRAP";
                break;
            case ThreadExceptionDesc_Other:
                errorDesc = "Segmentation Fault";
                break;
            default:
                errorDesc = "Unknown Exception [ 0x" + hlp::toHexString<u16>(ctx->error_desc) + " ]";
                break;
        }

        Gui::fatal("%s\n\n%s: %s\nPC: 0x%016lx",
            edz::LangEntry("edz.fatal.exception").c_str(),
            edz::LangEntry("edz.fatal.exception.reason").c_str(),
            errorDesc.c_str(),
            ctx->pc);
        
        exitServices();

        #if DEBUG_MODE_ENABLED
            exit(-3);
        #endif
    }


}

EResult initServices() {
    // UI (Borealis)
    if (!Application::init(StyleEnum::ACCURATE))
        return ResultEdzBorealisInitFailed;

    setLogLevel(LogLevel::DEBUG);
    Application::setCommonFooter(VERSION_STRING);

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
    clkrstInitialize();     // Don't check here because this service is only available on 8.0.0+

    // Language code setting querying
    TRY(setInitialize());

    // Companion sysmodule launching
    TRY(pmdmntInitialize());
    TRY(pmshellInitialize());
    TRY(pminfoInitialize());

    // Controller LED
    TRY(hidsysInitialize());
    TRY(hlp::controllerLEDInitialize());

    return ResultSuccess;
}

EResult createFolderStructure() {
    std::string paths[] = {
        EDIZON_BASE_DIR "/scripts/libs",
        EDIZON_BASE_DIR "/configs",
        EDIZON_BASE_DIR "/backups",
        EDIZON_BASE_DIR "/batch_backups",
        EDIZON_BASE_DIR "/restore",
        EDIZON_BASE_DIR "/cheats",
        EDIZON_BASE_DIR "/tmp"
    };

    for (auto path : paths) {
        hlp::Folder folder(path);
        TRY(folder.createDirectories());
    }

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

    // Try to initialize all services
    if (EResult res = initServices(); res.failed()) {
        Gui::fatal(edz::LangEntry("edz.fatal.service.init") + res.getString());

        exitServices();
        return -1;
    }

    // Create folder structure
    if (EResult res = createFolderStructure(); res.failed()) {
        Gui::fatal(edz::LangEntry("edz.fatal.folder_structure.init"));

        exitServices();
        return -2;
    }

    // Clear the tmp folder
    hlp::clearTmpFolder(); 

    // Set the startup Gui
    Gui::changeTo<GuiMain>();

    // Main loop for UI
    while (Application::mainLoop())
        Gui::tick();
    
    // Exit services
    exitServices();
}