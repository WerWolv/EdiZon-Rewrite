#include "edizon.hpp"

#include <Borealis.hpp>
#include <ImageView.hpp>

#include "helpers/file.hpp"
#include "helpers/folder.hpp"
#include "save/title.hpp"
#include "save/account.hpp"
#include "save/save_data.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>
#include "cheat/cheat.hpp"

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

u8 *buffer = nullptr; 
size_t titleIconSize;
void initInterface() {
    Application::init(StyleEnum::ACCURATE);

    TabFrame *rootFrame = new TabFrame();
    rootFrame->setTitle("EdiZon");

    List *titleList = new List();

    std::stringstream ss;
    for (auto [titleID, title] : edz::save::SaveFileSystem::getAllTitles()) {
        ss.str("");
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(sizeof(u64) * 2) << titleID;
        ListItem *titleItem = new ListItem(title->getTitleName());
        titleItem->setValue(ss.str(), true, true);
        titleList->addView(titleItem);

        if (buffer == nullptr) {
            titleIconSize = title->getTitleIconSize();
            buffer = new u8[titleIconSize];
            title->getTitleIcon(buffer, titleIconSize);
        }
    }


    ImageView *image = new ImageView(buffer, titleIconSize, 256, 256);
    rootFrame->addTab("Titles", titleList);
    rootFrame->addTab("Image", image);

    Application::pushView(rootFrame);
}

int main(int argc, char* argv[]) {
    initServices();

    //nxlinkStdio();

    //initInterface();

    //edz::cheat::CheatManager &cheatManager = edz::cheat::CheatManager::getInstance();

    //consoleInit(nullptr);

    while (appletMainLoop()) {
        hidScanInput();
        u64 btn = hidKeysDown(CONTROLLER_P1_AUTO);

        if (btn & KEY_PLUS) break;

        /*if (btn & KEY_A) {
            printf("Hello\n");
            cheatManager.forceAttach();

            cheatManager.getCheats()[0]->toggle();

            for (auto &cheat : cheatManager.getCheats())
                printf("%d : %s %c\n", cheat->getID(), cheat->getName().c_str(), cheat->isEnabled() ? 'X' : 'O');
        }*/

        if (btn & KEY_B) {
            edz::save::Title *title1 = 0;
            edz::save::Account *account1 = 0;
            std::map<u64, edz::save::Title*> titles = edz::save::SaveFileSystem::getAllTitles();
            std::map<u128, edz::save::Account*> accounts =  edz::save::SaveFileSystem::getAllAccounts();

            for (auto &[titleID, title] : titles) {
                title1 = title;
                break;
            }

            for (auto &[userID, account] : accounts) {
                account1 = account;
                break;
            }
            edz::save::SaveFileSystem saveFileSystem(title1, account1);
            saveFileSystem.getSaveFolder().copyTo("/test");
        }

        //consoleUpdate(nullptr);
    }  

    //consoleExit(nullptr);

    //while(Application::mainLoop());

    exitServices();
}