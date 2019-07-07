#include "edizon.hpp"

#include "helpers/file.hpp"
#include "helpers/folder.hpp"
#include "helpers/title.hpp"
#include "helpers/account.hpp"
#include "helpers/save_data.hpp"

void initServices() {
    socketInitializeDefault();
    ncmInitialize();
    nsInitialize();
    accountInitialize();
}

void exitServices() {
    socketExit();
    ncmExit();
    nsExit();
    accountExit();
}

int main(int argc, char* argv[]) {
    initServices();

    nxlinkStdio();

    //consoleInit(nullptr);

    while (appletMainLoop()) {
        hidScanInput();
        u64 buttons = hidKeysDown(CONTROLLER_P1_AUTO);

        if (buttons & KEY_A) {
            for (auto [titleID, title] : edz::helper::SaveFileSystem::getAllTitles()) {
                printf("%lx - %s\n", titleID, title->getTitleName().c_str());
            }
        }

        if (buttons & KEY_PLUS) break;
    }

    //consoleExit(nullptr);

    exitServices();
}