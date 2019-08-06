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