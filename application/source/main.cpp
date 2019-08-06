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
    TRY(!Application::init(StyleEnum::ACCURATE));

    // Curl
    TRY(curl_global_init(CURL_GLOBAL_ALL));

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

    // Controller LED
    TRY(hidsysInitialize());
    TRY(helper::controllerLEDInitialize());

    return ResultSuccess;
}

void exitServices() {
    socketExit();
    curl_global_cleanup();
    ncmExit();
    nsExit();
    accountExit();
    pctlExit();
    pcvExit();
    clkrstExit();
    romfsExit();
    setExit();
    Application::quit();
}

int main(int argc, char* argv[]) {  
    EResult res = initServices();
    if (res.failed()) {
        Gui::fatal(edz::LangEntry("edz.fatal.service.init").get() + res.getString());

        exitServices();
        return 1;
    }

    Gui::changeTo<GuiMain>();

    while (Application::mainLoop()) {
        Gui::tick();
    }
    
    exitServices();
}