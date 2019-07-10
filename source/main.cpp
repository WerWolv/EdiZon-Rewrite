#include "edizon.hpp"

#include "helpers/file.hpp"
#include "helpers/folder.hpp"
#include "save/title.hpp"
#include "save/account.hpp"
#include "save/save_data.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>
#include "cheat/cheat.hpp"
#include "helpers/utils.hpp"

Service *fsService;

void initServices() {
    // Network sockets
    socketInitializeDefault();

    // Title querying
    ncmInitialize();
    nsInitialize();

    // Account querying
    accountInitialize();

    // Parential control lockdown
    pctlInitialize();

    // Overclock
    pcvInitialize();
    clkrstInitialize();

    // Controller LED
    edz::helper::controllerLEDInitialize();
}

void exitServices() {
    socketExit();
    ncmExit();
    nsExit();
    accountExit();
    pctlExit();
    pcvExit();
    clkrstExit();
}

u8 *buffer = nullptr; 
size_t titleIconSize;
void initInterface() {
    Application::init(StyleEnum::ACCURATE);

    AppletFrame *rootFrame = new AppletFrame(true, false);
    rootFrame->setTitle("EdiZon");

    List *titleList = new List();

    std::stringstream ss;
    for (auto [titleID, title] : edz::save::SaveFileSystem::getAllTitles()) {
        ss.str("");
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(sizeof(u64) * 2) << titleID;
        ListItem *titleItem = new ListItem(title->getName());
        titleItem->setValue(ss.str(), true, true);
        titleList->addView(titleItem);
    }

    rootFrame->setContentView(titleList);

    Application::pushView(rootFrame);
}



// Access Denied 0x0100A9900CB5C000

int main(int argc, char* argv[]) {
    void *haddr;
    extern char *fake_heap_end;
    
    // Setup Heap for swkbd on applets
    Result rc = svcSetHeapSize(&haddr, 0x10000000);
    if (R_FAILED(rc))
        fatalSimple(0xDEAD);
    fake_heap_end = (char*) haddr + 0x10000000;

    initServices();

    initInterface();

    while(Application::mainLoop());
    
    auto &titles = edz::save::SaveFileSystem::getAllTitles();

    exitServices();

    svcSetHeapSize(&haddr, ((u8*) envGetHeapOverrideAddr() + envGetHeapOverrideSize()) - (u8*) haddr); // clean up the heap
}