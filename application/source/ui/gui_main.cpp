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

#include "ui/gui_main.hpp"

#include "helpers/config_manager.hpp"
#include "api/switchcheatsdb_api.hpp"

namespace edz::ui {

    using namespace brls;

    EResult openWebpage(std::string url) {
        EResult res;
        WebCommonConfig config;

        res = webPageCreate(&config, url.c_str());

        if (res.succeeded()) {
            res = webConfigSetWhitelist(&config, "^.*");
            if (res.succeeded())
                res = webConfigShow(&config, nullptr);
        }

        return res;
    }

    void GuiMain::createTitlePopup(save::Title *title) {
        size_t iconSize = title->getIconSize();
        u8 *iconBuffer = new u8[iconSize];
        title->getIcon(iconBuffer, iconSize);

        TabFrame *rootFrame = new TabFrame();

        List *softwareInfoList = new List();

        softwareInfoList->addView(new Header("Save Data", false));
        
        if (title->hasSaveFile()) {
            for (userid_t userid : title->getUserIDs()) {
                save::Account *account = save::SaveFileSystem::getAllAccounts()[userid].get();

                if (title->hasSaveFile(account)) {
                    ListItem *userItem = new ListItem(account->getNickname(), "", hlp::formatString("Played for %d hours and %d minutes",  title->getPlayTime(account) / 3600, (title->getPlayTime(account) % 3600) / 60));
                    
                    size_t iconBufferSize = account->getIconSize();
                    u8 *iconBuffer = new u8[iconBufferSize];
                    account->getIcon(iconBuffer, iconBufferSize);
                    userItem->setThumbnail(iconBuffer, iconBufferSize);

                    delete[] iconBuffer;

                    softwareInfoList->addView(userItem);
                }
            }
        }
        else {
            ListItem *createSaveFSItem = new ListItem("Create Save Filesystem");
            createSaveFSItem->setClickListener([&](View *view) {
                hlp::openPlayerSelect([&](save::Account *account) {
                    title->createSaveDataFileSystem(account);
                });
            });

            softwareInfoList->addView(new Label(LabelStyle::DESCRIPTION, "No save files found. To create a save file system for any account, use the option bellow."));
            softwareInfoList->addView(createSaveFSItem);
        }

        List *saveManagement = new List();
        
        ListItem *backupItem = new ListItem("Create backup", "", "Creates a save file backup");
        backupItem->setClickListener([=](View *view) {
            save::Account *currUser = nullptr;
            std::string backupName;
            
            if (hlp::openPlayerSelect([&](save::Account *account) { currUser = account; }))
                if (hlp::openSwkbdForText([&](std::string str) { backupName = str; }, "Enter a backup name"))
                    save::SaveManager::backup(title, currUser, backupName);
        });

        ListItem *restoreItem = new ListItem("Restore backup", "", "Restores a save file backup");
        restoreItem->setClickListener([=](View *view) {
            auto [result, list] = save::SaveManager::getLocalBackupList(title);

            Dropdown::open("Select a backup to restore", list, [&](int selection) {
                save::Account *currUser = nullptr;
                if (selection != -1)
                    if (hlp::openPlayerSelect([&](save::Account *account) { currUser = account; }))
                        save::SaveManager::restore(title, currUser, list[selection]);
            });
        });

        saveManagement->addView(new Header("Basic operations", false));
        saveManagement->addView(backupItem);
        saveManagement->addView(restoreItem);

        rootFrame->addTab("Software Information", softwareInfoList);
        rootFrame->addSeparator();
        rootFrame->addTab("Save Management", saveManagement);
        rootFrame->addTab("Save Editing", new Rectangle(nvgRGB(0x00, 0x00, 0xFF)));

        PopupFrame::open(title->getName(), iconBuffer, iconSize, rootFrame, "Ver. " + title->getVersionString(), title->getAuthor());

        delete[] iconBuffer;
    }

    void GuiMain::createTitleListView(List *list) {
        for (auto &[titleID, title] : save::SaveFileSystem::getAllTitles()) {
            ListItem *titleItem = new ListItem(hlp::limitStringLength(title->getName(), 45), "", title->getIDString());
            
            size_t iconSize = title->getIconSize();
            u8 *iconBuffer = new u8[iconSize];
            title->getIcon(iconBuffer, iconSize);
            
            titleItem->setThumbnail(iconBuffer, iconSize);

            delete[] iconBuffer;
            
            titleItem->setClickListener([&](View *view) {
                createTitlePopup(title.get());
            });


            list->addView(titleItem);
        }
        
        list->setSpacing(30);
    }

    void GuiMain::createTitleGridView(List *list) {
        std::vector<element::HorizontalTitleList*> hLists;
        u16 column = 0;
        
        for (auto &[titleID, title] : save::SaveFileSystem::getAllTitles()) {                    
            if (column % 4 == 0)
                hLists.push_back(new element::HorizontalTitleList());
            
            size_t iconSize = title->getIconSize();
            u8 *iconBuffer = new u8[iconSize];
            title->getIcon(iconBuffer, iconSize);

            element::TitleButton *titleButton = new element::TitleButton(iconBuffer, iconSize, column % 4);

            titleButton->setClickListener([&](View *view) {
                createTitlePopup(title.get());
            });

            hLists[hLists.size() - 1]->addView(titleButton);

            delete[] iconBuffer;


            column++;
        }
        
        for (element::HorizontalTitleList*& hList : hLists) {
            list->addView(hList);
        }

        list->setSpacing(30);

        list->invalidate();
        
    }

    void GuiMain::createTitlesListTab(LayerView *layerView) {
        List *listDisplayList = new List();
        List *gridDisplayList = new List();

        SelectListItem *sortingOptionList = new SelectListItem("edz.gui.main.titles.style"_lang, { "edz.gui.main.titles.style.list"_lang, "edz.gui.main.titles.style.grid"_lang });
        SelectListItem *sortingOptionGrid = new SelectListItem("edz.gui.main.titles.style"_lang, { "edz.gui.main.titles.style.list"_lang, "edz.gui.main.titles.style.grid"_lang });
        
        sortingOptionList->setListener([=](size_t selection) {
            this->m_titleList->changeLayer(selection, true);
            SET_CONFIG(Save.titlesSortingStyle, selection);
            sortingOptionGrid->setSelectedValue(selection);
        });

        sortingOptionGrid->setListener([=](size_t selection) {
            this->m_titleList->changeLayer(selection, true);
            SET_CONFIG(Save.titlesSortingStyle, selection);
            sortingOptionList->setSelectedValue(selection);
        });
        

        listDisplayList->addView(new Header("edz.gui.main.titles.options"_lang, false));
        listDisplayList->addView(sortingOptionList);
        listDisplayList->addView(new Header("edz.gui.main.titles.tab"_lang, false));
        createTitleListView(listDisplayList);

        gridDisplayList->addView(new Header("edz.gui.main.titles.options"_lang, false));
        gridDisplayList->addView(sortingOptionGrid);
        gridDisplayList->addView(new Header("edz.gui.main.titles.tab"_lang, false));
        createTitleGridView(gridDisplayList);

        layerView->addLayer(listDisplayList);
        layerView->addLayer(gridDisplayList);
        layerView->changeLayer(GET_CONFIG(Save.titlesSortingStyle), false);
    }

    void GuiMain::createRunningTitleInfoTab(List *list) {
        std::shared_ptr<save::Title> runningTitle = save::SaveFileSystem::getAllTitles()[save::Title::getRunningTitleID()];
                
        size_t iconSize = runningTitle->getIconSize();
        u8 *iconBuffer = new u8[iconSize];
        runningTitle->getIcon(iconBuffer, iconSize);

        element::TitleInfo *titleInfo = new element::TitleInfo(iconBuffer, iconSize, runningTitle);

        delete[] iconBuffer;

        list->addView(new Header("edz.gui.main.running.general"_lang, true));
        list->addView(titleInfo);
        list->addView(new Header("edz.gui.main.running.memory"_lang, true));

        Table *regionsTable = new Table();

        bool foundHeap = false;
        u16 currCodeRegion = 0;
        u16 codeRegionCnt = 0;

        for (MemoryInfo region : cheat::CheatManager::getMemoryRegions())
            if (region.type == MemType_CodeStatic && region.perm == Perm_Rx)
                codeRegionCnt++;

        auto memoryRegions = cheat::CheatManager::getMemoryRegions();
        for (MemoryInfo region : memoryRegions) {
            std::string regionName = "";

            if (region.type == MemType_Heap && !foundHeap) {
                foundHeap = true;
                regionName = "heap";
            } else if (region.type == MemType_CodeStatic && region.perm == Perm_Rx) {
                if (currCodeRegion == 0 && codeRegionCnt == 1) {
                    regionName = "main";
                    continue;
                } else {
                    switch (currCodeRegion) {
                        case 0:
                            regionName = "rtld";
                            break;
                        case 1:
                            regionName = "main";
                            break;
                        default:
                            if (currCodeRegion == memoryRegions.size() - 1)
                                regionName = "sdk";
                            else
                                regionName = "subsdk" + std::to_string(currCodeRegion - 1);
                    }
                }

                currCodeRegion++;
            } else continue;

            regionsTable->addRow(TableRowType::BODY, regionName, "0x" + hlp::toHexString(region.addr) + " - 0x" + hlp::toHexString(region.addr + region.size));
        }
        list->addView(regionsTable);
        list->addView(new ListItem(""));
    }

    void GuiMain::createCheatsTab(List *list) {
        list->addView(new Header("edz.gui.main.cheats.header.desc"_lang, true));
        list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.cheats.label.desc"_lang, true));
        list->addView(new Header("edz.gui.main.cheats.header.cheats"_lang, cheat::CheatManager::getCheats().size() == 0));

        if (cheat::CheatManager::isCheatServiceAvailable()) {
            if (cheat::CheatManager::getCheats().size() > 0) {
                for (auto cheat : cheat::CheatManager::getCheats()) {
                    ToggleListItem *cheatItem = new ToggleListItem(hlp::limitStringLength(cheat->getName(), 60), cheat->isEnabled(), "",
                        "edz.widget.boolean.on"_lang, "edz.widget.boolean.off"_lang);

                    cheatItem->setClickListener([=](View *view){
                        cheat->setState(static_cast<ToggleListItem*>(view)->getToggleState());
                    });

                    list->addView(cheatItem);
                }
            } else {
                if (hlp::isInAppletMode())
                    list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.no_cheats"_lang, true));
                else
                    list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.application_mode"_lang, true));
            }
        } 
        else 
            list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.dmnt_cht_missing"_lang, true));
    }

    void GuiMain::createSettingsTab(LayerView *layerView) {
        static std::string email, password;

        List *loggedInList = new List();
        List *loggedOutList = new List();       

        // Logged out State
        ListItem *emailItem = new ListItem("edz.gui.main.settings.email"_lang);
        emailItem->setClickListener([&](View *view) {
            hlp::openSwkbdForText([&](std::string text) {
                email = text;
                static_cast<ListItem*>(view)->setValue(text);
            }, "edz.gui.main.settings.email"_lang, "edz.gui.main.settings.email.help"_lang);
        });
        
        ListItem *passwordItem = new ListItem("edz.gui.main.settings.password"_lang);
        passwordItem->setClickListener([&](View *view) {
            hlp::openSwkbdForPassword([&](std::string text) {
                password = text;

                std::string itemText = "";
                for (u8 i = 0; i < text.length(); i++)
                    itemText += "●";

                static_cast<ListItem*>(view)->setValue(itemText);
            }, "edz.gui.main.settings.password"_lang, "edz.gui.main.settings.password.help"_lang);
        });

        ListItem *loginButton = new ListItem("edz.gui.main.settings.login"_lang);
        loginButton->setClickListener([=](View *view) {
            api::SwitchCheatsDBAPI scdbApi;

            auto [result, token] = scdbApi.getToken(emailItem->getValue(), passwordItem->getValue());

            password = "";

            if (result != ResultSuccess) {
                return;
            }

            SET_CONFIG(Update.loggedIn, true);
            SET_CONFIG(Update.switchcheatsdbEmail, email);
            SET_CONFIG(Update.switchcheatsdbApiToken, token);

            email = "";

            layerView->changeLayer(1, true);
        });

        loggedOutList->addView(new Header("switchcheatsdb.name"_lang, true));
        loggedOutList->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.settings.not_logged_in"_lang, true));
        loggedOutList->addView(emailItem);
        loggedOutList->addView(passwordItem);
        loggedOutList->addView(loginButton);

        // Logged in state
        ListItem *logoutButton = new ListItem("edz.gui.main.settings.logout"_lang);
        logoutButton->setClickListener([=](View *view) {
            SET_CONFIG(Update.loggedIn, false);
            SET_CONFIG(Update.switchcheatsdbEmail, "");
            SET_CONFIG(Update.switchcheatsdbApiToken, "");
            
            layerView->changeLayer(0, true);
        });

        loggedInList->addView(new Header("switchcheatsdb.name"_lang, true));
        loggedInList->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.settings.logged_in"_lang + GET_CONFIG(Update.switchcheatsdbEmail)));
        loggedInList->addView(logoutButton);


        layerView->addLayer(loggedOutList);
        layerView->addLayer(loggedInList);
        layerView->changeLayer(GET_CONFIG(Update.loggedIn), false);
    }

    void GuiMain::createAboutTab(List *list) {
        list->addView(new Header("edz.name"_lang + " " VERSION_STRING " " + "edz.dev"_lang, true));
        list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.about.label.edz"_lang, true));

        list->addView(new Header("edz.gui.main.about.links"_lang, false));
        
        ListItem *scdbItem = new ListItem("switchcheatsdb.name"_lang, "", "https://switchcheatsdb.com");
        ListItem *patreonItem = new ListItem("patreon.name"_lang, "", "https://patreon.com/werwolv_");

        scdbItem->setThumbnail("romfs:/assets/icon_scdb.png");
        patreonItem->setThumbnail("romfs:/assets/icon_patreon.png");

        if (hlp::isInApplicationMode()) {
            scdbItem->setClickListener([](View *view){ openWebpage("https://www.switchcheatsdb.com"); });
            patreonItem->setClickListener([](View *view){ openWebpage("https://patreon.com/werwolv_"); });
        }
        
        list->addView(scdbItem);
        list->addView(patreonItem);

    }

    View* GuiMain::setupUI() {
        TabFrame *rootFrame = new TabFrame();
        rootFrame->setTitle("edz.name"_lang);
        
        if (Application::getThemeVariant() == ThemeVariant::ThemeVariant_LIGHT)
            rootFrame->setIcon("romfs:/assets/icon_edz_dark.png");
        else
            rootFrame->setIcon("romfs:/assets/icon_edz_light.png");

        this->m_titleList = new LayerView();
        this->m_cheatsList = new List();
        this->m_settingsList = new LayerView();
        this->m_aboutList = new List();

        createTitlesListTab(this->m_titleList);
        createCheatsTab(this->m_cheatsList);
        createSettingsTab(this->m_settingsList);
        createAboutTab(this->m_aboutList);

        rootFrame->addTab("edz.gui.main.titles.tab"_lang, this->m_titleList);

        if (hlp::isTitleRunning() && cheat::CheatManager::isCheatServiceAvailable()) {
            this->m_runningTitleInfoList = new List();
            createRunningTitleInfoTab(this->m_runningTitleInfoList);           
            rootFrame->addTab("edz.gui.main.running.tab"_lang, this->m_runningTitleInfoList);
        }

        rootFrame->addTab("edz.gui.main.cheats.tab"_lang, this->m_cheatsList);
        rootFrame->addTab("edz.gui.main.settings.tab"_lang, this->m_settingsList);
        rootFrame->addSeparator();
        rootFrame->addTab("edz.gui.main.about.tab"_lang, this->m_aboutList);

        return rootFrame;
    }

    void GuiMain::update() {

    }

}