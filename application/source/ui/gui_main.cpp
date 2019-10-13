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
#include "ui/gui_cheat_engine.hpp"

#include <cstring>

#include "helpers/config_manager.hpp"
#include "save/edit/config.hpp"
#include "save/edit/editor.hpp"
#include "api/switchcheatsdb_api.hpp"
#include "ui/elements/focusable_table.hpp"
#include "ui/elements/title_list_item.hpp"

#include "ui/pages/page_login.hpp"

namespace edz::ui {

    using namespace brls;

    EResult openWebpage(std::string url) {
        EResult res;
        WebCommonConfig config;

        res = webPageCreate(&config, url.c_str());

        if (res.succeeded()) {
            res = webConfigSetWhitelist(&config, "^.*");
            webConfigSetFooter(&config, false);
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

        brls::ListItem *launchItem = new ListItem("edz.gui.popup.information.launch.title"_lang, "", "edz.gui.popup.information.launch.subtitle"_lang);
        launchItem->setClickListener([title](View *view){
            AppletStorage s = { 0 };
            appletCreateStorage(&s, 0);
            appletRequestLaunchApplication(title->getID(), &s);
        });

        softwareInfoList->addView(new Header("edz.gui.popup.information.header.general"_lang, false));
        softwareInfoList->addView(launchItem);
        softwareInfoList->addView(new Header("edz.gui.popup.information.header.save"_lang, false));

        
        
        if (title->hasSaveFile()) {
            for (userid_t userid : title->getUserIDs()) {
                save::Account *account = save::SaveFileSystem::getAllAccounts()[userid].get();

                if (title->hasSaveFile(account)) {
                    ListItem *userItem = new ListItem(account->getNickname(), "", hlp::formatString("edz.gui.popup.information.playtime"_lang, title->getLaunchCount(account), title->getPlayTime(account) / 3600, (title->getPlayTime(account) % 3600) / 60));
                    
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
            ListItem *createSaveFSItem = new ListItem("edz.gui.popup.management.createfs.title"_lang, "", "edz.gui.popup.management.createfs.subtitle"_lang);
            createSaveFSItem->setClickListener([&](View *view) {
                hlp::openPlayerSelect([&](save::Account *account) {
                    title->createSaveDataFileSystem(account);
                    Application::popView();
                });
            });

            softwareInfoList->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.popup.error.nosavefs"_lang));
            softwareInfoList->addView(createSaveFSItem);
        }

        List *saveManagementList = new List();
        
        ListItem *backupItem = new ListItem("edz.gui.popup.management.backup.title"_lang, "", "edz.gui.popup.management.backup.subtitle"_lang);
        backupItem->setClickListener([=](View *view) {
            save::Account *currUser = nullptr;
            std::string backupName;
            
            if (hlp::openPlayerSelect([&](save::Account *account) { currUser = account; }))
                if (hlp::openSwkbdForText([&](std::string str) { backupName = str; }, "edz.gui.popup.management.backup.keyboard.title"_lang))
                    save::SaveManager::backup(title, currUser, backupName);
        });

        ListItem *restoreItem = new ListItem("edz.gui.popup.management.restore.title"_lang, "", "edz.gui.popup.management.restore.subtitle"_lang);
        restoreItem->setClickListener([=](View *view) {
            auto [result, list] = save::SaveManager::getLocalBackupList(title);

            Dropdown::open("edz.gui.popup.management.backup.dropdown.title"_lang, list, [=](int selection) {
                if (selection != -1)
                    hlp::openPlayerSelect([=](save::Account *account) { 
                        save::SaveManager::restore(title, account, list[selection]);
                    });
            });
        });

        ListItem *deleteItem = new ListItem("edz.gui.popup.management.delete.title"_lang, "", "edz.gui.popup.management.delete.subtitle"_lang);
        deleteItem->setClickListener([=](View *view) {
            save::Account *currUser = nullptr;
            
            if (hlp::openPlayerSelect([&](save::Account *account) { currUser = account; }))
                save::SaveManager::remove(title, currUser);
        });

        ListItem *editItem = new ListItem("Edit Save File", "", "Edit your save file");
        editItem->setClickListener([=](View *view) {
            save::Account *currUser = nullptr;
            if (hlp::openPlayerSelect([&](save::Account *account) { currUser = account; })) {
                auto [result, config] = save::edit::Editor::loadConfig(title, currUser);

                if (result.succeeded()) {
                    std::vector<std::string> saveFilePaths = config->getSaveFilePaths();

                    Dropdown::open("edz.gui.popup.management.backup.dropdown.title"_lang, saveFilePaths, [=](int selection) {
                        if (selection != -1) {
                            auto [result, script] = save::edit::Editor::loadScript(title, currUser, saveFilePaths[selection]);
                            brls::TabFrame *editorFrame = new brls::TabFrame();
                            config->setScript(script);
                            config->createUI(editorFrame);

                            Application::pushView(editorFrame);
                        }                   
                    });   
                }

            }
        });

        saveManagementList->addView(new Header("edz.gui.popup.management.header.basic"_lang, false));
        saveManagementList->addView(backupItem);
        saveManagementList->addView(restoreItem);
        saveManagementList->addView(new Header("edz.gui.popup.management.header.advanced"_lang, false));
        saveManagementList->addView(deleteItem);
        saveManagementList->addView(editItem);

        List *saveEditingList = new List();

        saveEditingList->addView(new Label(brls::LabelStyle::DESCRIPTION, "", true));

        rootFrame->addTab("edz.gui.popup.information.tab"_lang, softwareInfoList);
        rootFrame->addSeparator();
        rootFrame->addTab("edz.gui.popup.management.tab"_lang, saveManagementList);
        rootFrame->addTab("edz.gui.popup.editing.tab"_lang, saveEditingList);

        PopupFrame::open(title->getName(), iconBuffer, iconSize, rootFrame, "edz.gui.popup.version"_lang + " " + title->getVersionString(), title->getAuthor());

        delete[] iconBuffer;
    }

    void GuiMain::sortTitleList(std::vector<brls::BoxLayoutChild*>& list, SortingStyle sorting) {
        std::sort(list.begin(), list.end(), [sorting](brls::BoxLayoutChild* lItem, brls::BoxLayoutChild* rItem) {
            u32 lLowestTime = 0, rLowestTime = 0;
            u32 lLatestTime = 0, rLatestTime = 0;
            u32 lHighestPlayTime = 0, rHighestPlayTime = 0;
            u32 lLaunches = 0, rLaunches = 0;

            std::shared_ptr<save::Title> l = static_cast<ui::element::TitleListItem*>(lItem->view)->getTitle();
            std::shared_ptr<save::Title> r = static_cast<ui::element::TitleListItem*>(rItem->view)->getTitle();

            switch (sorting) {
                case SortingStyle::TITLE_ID:
                    return l->getID() < r->getID();
                case SortingStyle::ALPHABETICAL_NAME:
                    return l->getName().compare(r->getName()) < 0;
                case SortingStyle::ALPHABETICAL_AUTHOR:
                    return l->getAuthor().compare(r->getAuthor()) < 0;
                case SortingStyle::NUM_LAUNCHES:
                    for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
                        lLaunches += l->getLaunchCount(account.get());
                        rLaunches += r->getLaunchCount(account.get());
                    }

                    return lLaunches > rLaunches;
                case SortingStyle::FIRST_PLAYED:
                    for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
                        if (u32 lTime = l->getFirstPlayTime(account.get()); lTime < lLowestTime)
                            lLowestTime = lTime;
                        if (u32 rTime = r->getFirstPlayTime(account.get()); rTime < rLowestTime)
                            rLowestTime = rTime;
                    }

                    return lLowestTime < rLowestTime;
                case SortingStyle::LAST_PLAYED:
                    for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
                        if (u32 lTime = l->getLastPlayTime(account.get()); lTime > lLatestTime)
                            lLatestTime = lTime;
                        if (u32 rTime = r->getLastPlayTime(account.get()); rTime > rLatestTime)
                            rLatestTime = rTime;
                    }

                    return lLatestTime > rLatestTime;
                case SortingStyle::PLAY_TIME:
                    for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
                        lHighestPlayTime += l->getPlayTime(account.get());
                        rHighestPlayTime += r->getPlayTime(account.get());
                    }

                    return lHighestPlayTime > rHighestPlayTime;
            }

            return true;
        });
    }

    void GuiMain::createTitleListView(List *list, SortingStyle sorting) {
        for (auto &[titleid, title] : save::SaveFileSystem::getAllTitles()) {
            brls::ListItem *titleItem = new ui::element::TitleListItem(title, hlp::limitStringLength(title->getName(), 45), "", title->getIDString());
            
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

        sortTitleList(list->getChildren(), sorting);
        list->setSpacing(30);
    }

    void GuiMain::createTitleCondensedView(List *list, SortingStyle sorting) {
        for (auto &[titleid, title] : save::SaveFileSystem::getAllTitles()) {
            brls::ListItem *titleItem = new ui::element::TitleListItem(title, hlp::limitStringLength(title->getName(), 35));
            titleItem->setValue(title->getIDString(), true, false);
                        
            titleItem->setClickListener([&](View *view) {
                createTitlePopup(title.get());
            });


            list->addView(titleItem);
        }
        
        sortTitleList(list->getChildren(), sorting);
        list->setSpacing(30);
    }

    void GuiMain::createTitleGridView(List *list, SortingStyle sorting) {
        std::vector<element::HorizontalTitleList*> hLists;
        u16 column = 0;
        
        for (auto &[titleid, title] : save::SaveFileSystem::getAllTitles()) {         
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

    void GuiMain::createTitlesListTab(LayerView *layerView, SortingStyle sorting) {
        List *listDisplayList = new List();
        List *condensedDisplayList = new List();
        List *gridDisplayList = new List();        

        GuiMain::createTitleListView(listDisplayList, sorting);
        GuiMain::createTitleCondensedView(condensedDisplayList, sorting);
        GuiMain::createTitleGridView(gridDisplayList, sorting);

        layerView->addLayer(listDisplayList);
        layerView->addLayer(condensedDisplayList);
        layerView->addLayer(gridDisplayList);

        layerView->changeLayer(GET_CONFIG(Save.titlesDisplayStyle), false);
    }

    void GuiMain::createSaveReposTab(brls::List *list) {
        
    }

    void GuiMain::createRunningTitleInfoTab(List *list) {
        std::shared_ptr<save::Title> runningTitle = save::SaveFileSystem::getAllTitles()[save::Title::getRunningTitleID()];
                
        size_t iconSize = runningTitle->getIconSize();
        u8 *iconBuffer = new u8[iconSize];

        runningTitle->getIcon(iconBuffer, iconSize);

        element::TitleInfo *titleInfo = new element::TitleInfo(iconBuffer, iconSize, runningTitle);
        delete[] iconBuffer;

        brls::ListItem *cheatEngineItem = new brls::ListItem("edz.gui.main.running.cheatengine.title"_lang, "", "edz.gui.main.running.cheatengine.subtitle"_lang);
        cheatEngineItem->setClickListener([](View *view){
            Gui::changeTo<GuiCheatEngine>();
        });


        edz::ui::element::FocusableTable *regionsTable = new edz::ui::element::FocusableTable();

        bool foundHeap = false;
        u16 currCodeRegion = 0;
        u16 codeRegionCnt = 0;

        for (MemoryInfo region : cheat::CheatManager::getMemoryRegions())
            if (region.type == MemType_CodeStatic && region.perm == Perm_Rx)
                codeRegionCnt++;

        auto memoryRegions = cheat::CheatManager::getMemoryRegions();
        size_t numCodeRegions = 0;

        for (MemoryInfo region : memoryRegions)
            if (region.type == MemType_CodeStatic && region.perm == Perm_Rx)
                numCodeRegions++;

        for (MemoryInfo region : memoryRegions) {
            std::string regionName = "";

            if (region.type == MemType_Heap && !foundHeap) {
                foundHeap = true;
                regionName = "edz.gui.main.running.section.heap"_lang;
            } else if (region.type == MemType_CodeStatic && region.perm == Perm_Rx) {
                if (currCodeRegion == 0 && codeRegionCnt == 1) {
                    regionName = "edz.gui.main.running.section.main"_lang;
                    continue;
                } else {
                    switch (currCodeRegion) {
                        case 0:
                            regionName = "edz.gui.main.running.section.rtld"_lang;
                            break;
                        case 1:
                            regionName = "edz.gui.main.running.section.main"_lang;
                            break;
                        default:
                            if (currCodeRegion < (numCodeRegions - 1))
                                regionName = "edz.gui.main.running.section.subsdk"_lang + std::to_string(currCodeRegion - 2);
                            else
                                regionName = "edz.gui.main.running.section.sdk"_lang;
                    }
                }

                currCodeRegion++;
            } else continue;

            regionsTable->addRow(TableRowType::BODY, regionName, "0x" + hlp::toHexString(region.addr) + " - 0x" + hlp::toHexString(region.addr + region.size));
        }

        list->addView(new Header("edz.gui.main.running.general"_lang, true));
        list->addView(cheatEngineItem);
        list->addView(titleInfo);
        list->addView(new Header("edz.gui.main.running.memory"_lang, true));
        list->addView(regionsTable);
    }

    void GuiMain::createCheatsTab(List *list) {
        list->addView(new Label(LabelStyle::SMALL, "edz.gui.main.cheats.label.desc"_lang, true));
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
                if (!hlp::isTitleRunning())
                    list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.no_title"_lang, true));
                else if (hlp::isInAppletMode())
                    list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.no_cheats"_lang, true));
                else
                    list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.application_mode"_lang, true));
            }
        } 
        else 
            list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.dmnt_cht_missing"_lang, true));
    }

    void GuiMain::createSettingsTab(List *list) {
        // Title Options
        std::vector<std::string> displayOptions = { "edz.gui.main.titles.style.list"_lang, 
                                                    "edz.gui.main.titles.style.condensed"_lang,
                                                    "edz.gui.main.titles.style.grid"_lang };
        std::vector<std::string> sortingOptions = { "edz.gui.main.titles.sorting.titleid"_lang,
                                                    "edz.gui.main.titles.sorting.alphaname"_lang,
                                                    "edz.gui.main.titles.sorting.alphaauthor"_lang,
                                                    "edz.gui.main.titles.sorting.firstplayed"_lang,
                                                    "edz.gui.main.titles.sorting.lastplayed"_lang,
                                                    "edz.gui.main.titles.sorting.playtime"_lang,
                                                    "edz.gui.main.titles.sorting.numlaunches"_lang };

        SelectListItem *displayOptionsItem = new SelectListItem("edz.gui.main.titles.style"_lang, displayOptions);
        SelectListItem *sortingOptionsItem = new SelectListItem("edz.gui.main.titles.sorting"_lang, sortingOptions);

        displayOptionsItem->setListener([=](size_t selection) {
            this->m_titleList->changeLayer(selection, false);
            SET_CONFIG(Save.titlesDisplayStyle, selection);
        });


        sortingOptionsItem->setListener([=](size_t selection) {
            SET_CONFIG(Save.titlesSortingStyle, selection);
            this->m_reloadGui = true;
        });

        displayOptionsItem->setSelectedValue(static_cast<int>(GET_CONFIG(Save.titlesDisplayStyle)));
        sortingOptionsItem->setSelectedValue(static_cast<int>(GET_CONFIG(Save.titlesSortingStyle)));


        // SwitchCheatsDB Login
        ListItem *scdbLoginItem = new ListItem("edz.gui.main.settings.account.title"_lang);
        scdbLoginItem->setValue(GET_CONFIG(Update.loggedIn) ? GET_CONFIG(Update.switchcheatsdbEmail) : "Not logged in");
        scdbLoginItem->setClickListener([=](View *view) { 
            if (GET_CONFIG(Update.loggedIn)) {
                brls::Dialog *dialog = new brls::Dialog("edz.gui.main.settings.dialog.logout"_lang);
                dialog->addButton("edz.dialog.no"_lang, [=](View *view) {
                    dialog->close();
                });
                dialog->addButton("edz.dialog.yes"_lang, [=](View *view) {
                    SET_CONFIG(Update.loggedIn, false);
                    SET_CONFIG(Update.switchcheatsdbEmail, "");
                    SET_CONFIG(Update.switchcheatsdbApiToken, "");
                    scdbLoginItem->setValue("edz.gui.main.settings.account.nologin"_lang);
                    dialog->close();
                });

                dialog->open();
            } else {
                brls::AppletFrame *frame = new brls::AppletFrame(false, false);
                frame->setContentView(new ui::page::PageLogin(scdbLoginItem));
                frame->setTitle("edz.page.login.title"_lang);

                brls::Application::pushView(frame);
            }
        });


        list->addView(new Header("edz.gui.main.settings.header.titleoptions"_lang));
        list->addView(displayOptionsItem);
        list->addView(sortingOptionsItem);
        list->addView(new Header("edz.gui.main.settings.header.accountoptions"_lang));
        list->addView(new Label(LabelStyle::MEDIUM, "edz.gui.main.settings.scdbinfo"_lang, true));
        list->addView(scdbLoginItem);
    }

    void GuiMain::createAboutTab(List *list) {
        list->addView(new Header("edz.name"_lang + " " VERSION_STRING " " + "edz.dev"_lang, true));
        list->addView(new Label(LabelStyle::DESCRIPTION, "edz.gui.main.about.label.edz"_lang, true));

        list->addView(new Header("edz.gui.main.about.links"_lang, false));
        
        ListItem *scdbItem = new ListItem("edz.switchcheatsdb.name"_lang, "", "https://switchcheatsdb.com");
        ListItem *patreonItem = new ListItem("edz.patreon.name"_lang, "", "https://patreon.com/werwolv");
        ListItem *guideItem = new ListItem("edz.gui.main.about.guide.title"_lang, "", "https://edizon.werwolv.net");

        scdbItem->setThumbnail("romfs:/assets/icon_scdb.png");
        patreonItem->setThumbnail("romfs:/assets/icon_patreon.png");
        guideItem->setThumbnail("romfs:/assets/icon_guide.png");

        if (hlp::isInApplicationMode()) {
            scdbItem->setClickListener([](View *view)    { openWebpage("https://www.switchcheatsdb.com"); });
            patreonItem->setClickListener([](View *view) { openWebpage("https://patreon.com/werwolv");    });
            guideItem->setClickListener([](View *view)   { openWebpage("http://edizon.werwolv.net");      });
        }

        list->addView(scdbItem);
        list->addView(patreonItem);
        list->addView(guideItem);
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
        this->m_settingsList = new List();
        this->m_aboutList = new List();

        createTitlesListTab(this->m_titleList, SortingStyle::TITLE_ID);

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
        if (this->m_reloadGui) {
            this->m_reloadGui = false;

            GuiMain::sortTitleList(static_cast<brls::List*>(this->m_titleList->getLayer(0))->getChildren(), static_cast<SortingStyle>(GET_CONFIG(Save.titlesSortingStyle)));
            GuiMain::sortTitleList(static_cast<brls::List*>(this->m_titleList->getLayer(1))->getChildren(), static_cast<SortingStyle>(GET_CONFIG(Save.titlesSortingStyle)));
        }
    }

}