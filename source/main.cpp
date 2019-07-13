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
#include "save/edit/config.hpp"
#include "helpers/lang_entry.hpp"

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

    // RomFS for guide and localization
    romfsInit();

    // Language code setting querying
    setInitialize();

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
    romfsExit();
    setExit();
}

u8 *buffer = nullptr; 
size_t titleIconSize;
void initInterface() {
    Application::init(StyleEnum::ACCURATE);

    TabFrame *rootFrame = new TabFrame();
    rootFrame->setTitle("EdiZon");

    /*std::stringstream ss;
    for (auto [titleID, title] : edz::save::SaveFileSystem::getAllTitles()) {
        ss.str("");
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(sizeof(u64) * 2) << titleID;
        ListItem *titleItem = new ListItem(title->getName());
        titleItem->setValue(ss.str(), true, true);
        titleList->addView(titleItem);
    }*/

    edz::save::Title *title1;
    edz::save::Account *acc1;

    for (auto &[tid, title] : edz::save::SaveFileSystem::getAllTitles()) {
        title1 = title;
        break;
    }

    for (auto &[tid, acc] : edz::save::SaveFileSystem::getAllAccounts()) {
        acc1 = acc;
        break;
    }
    
    edz::save::edit::Config config(title1, acc1);

    config.createUI(rootFrame);

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
    nxlinkStdio();

    while(Application::mainLoop());
    
    exitServices();

    svcSetHeapSize(&haddr, ((u8*) envGetHeapOverrideAddr() + envGetHeapOverrideSize()) - (u8*) haddr); // clean up the heap
}