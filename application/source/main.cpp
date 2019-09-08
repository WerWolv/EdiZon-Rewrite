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
#include <Borealis.hpp>
#include <curl/curl.h>

#include <stdlib.h>

#include "helpers/utils.hpp"
#include "helpers/config_manager.hpp"

#include "ui/gui.hpp"
#include "ui/gui_main.hpp"


using namespace edz;
using namespace edz::ui;


void exitServices();

extern "C" {
    u32 __nx_exception_ignoredebug = 1;
    
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
            "A fatal exception occured!",
            "Reason",
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
    if (!brls::Application::init(brls::StyleEnum::ACCURATE))
        return ResultEdzBorealisInitFailed;

    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
    brls::Application::setCommonFooter(VERSION_STRING);

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
    if (hosversionBefore(8,0,0)) {
        TRY(pcvInitialize());
    }
    else {
        TRY(clkrstInitialize());
    }

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
        EDIZON_BACKUP_DIR,
        EDIZON_BATCH_BACKUP_DIR,
        EDIZON_COMMON_RESTORE_DIR,
        EDIZON_CONFIGS_DIR,
        EDIZON_SCRIPTS_DIR,
        EDIZON_LIBS_DIR,
        EDIZON_CHEATS_DIR,
        EDIZON_TMP_DIR
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

    //brls::Application::quit();
}

int main(int argc, char* argv[]) {  

    // Try to initialize all services
    if (EResult res = initServices(); res.failed()) {
        Gui::fatal("edz.fatal.service.init"_lang + res.getString());

        exitServices();
        return -1;
    }

    // Create folder structure
    if (EResult res = createFolderStructure(); res.failed()) {
        Gui::fatal("edz.fatal.folder_structure.init"_lang);

        exitServices();
        return -2;
    }

    // Clear the tmp folder
    hlp::clearTmpFolder(); 

    // Load config file
    //hlp::ConfigManager::get().load();

    // Set the startup Gui
    Gui::changeTo<GuiMain>();

    // Main loop for UI
    while (brls::Application::mainLoop())
        Gui::tick();
    
    // Exit services
    exitServices();
}