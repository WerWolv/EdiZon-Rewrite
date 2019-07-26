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
#include "save/edit/script/script.hpp"
#include "helpers/lang_entry.hpp"


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
    //pcvInitialize();
    //clkrstInitialize();

    // RomFS for guide and localization
    romfsInit();

    // Language code setting querying
    setInitialize();

    // Controller LED
    edz::helper::controllerLEDInitialize();

    // UI (Borealis)
    Application::init(StyleEnum::ACCURATE);
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
    Application::quit();
}


u8 *buffer = nullptr; 
size_t titleIconSize;
void initInterface() {

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
    
    //edz::save::edit::Config config(nullptr, nullptr);

    //config.createUI(rootFrame);
    List *list = new List();

    ListItem *item = new ListItem("TX service running", std::to_string(edz::helper::isServiceRunning("tx")));
    ListItem *item2 = new ListItem("Run TX service");
    item2->setClickListener([](View* view){ 
        Handle handle;
        smRegisterService(&handle, "tx", false, 1);
     });

     list->addView(item);
     list->addView(item2);
     rootFrame->addTab("Test", list);

    Application::pushView(rootFrame);
}



// Access Denied 0x0100A9900CB5C000

int main(int argc, char* argv[]) {
    void *haddr;
    extern char *fake_heap_end;
    
    initServices();

    // Setup Heap for swkbd on applets
    // If this fails, something's messed up with the hb environment. Applets and probably other things will not work, abort.
    if (edz::EResult(svcSetHeapSize(&haddr, 0x10000000)).failed()) {
        Application::crash(edz::LangEntry("edz.error.heap").get());
        while(Application::mainLoop());
        exitServices();

        return 0;
    }

    initInterface();

    fake_heap_end = (char*) haddr + 0x10000000;

    while(Application::mainLoop());
    
    exitServices();

    svcSetHeapSize(&haddr, ((u8*) envGetHeapOverrideAddr() + envGetHeapOverrideSize()) - (u8*) haddr); // clean up the heap
}