/**
 * Copyright (C) 2019 - 2020 WerWolv
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
#include <algorithm>
#include <iomanip>
#include <regex>

#include "cheat/cheat.hpp"
#include "cheat/cheat_parser.hpp"

#include "helpers/config_manager.hpp"
#include "save/edit/config.hpp"
#include "save/edit/editor.hpp"
#include "ui/elements/focusable_table.hpp"
#include "ui/elements/title_list_item.hpp"
#include "ui/elements/credit_view.hpp"
#include "ui/pages/page_login.hpp"

#include "api/edizon_api.hpp"
#include "api/save_repo_api.hpp"
#include "api/switchcheatsdb_api.hpp"

#include "ui/elements/hex_editor.hpp"

namespace edz::ui {

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

    void GuiMain::createTitlePopup(std::unique_ptr<save::Title> &title) {
        brls::TabFrame *rootFrame = new brls::TabFrame();

        brls::List *softwareInfoList = new brls::List();

        brls::ListItem *launchItem = new brls::ListItem("edz.gui.popup.information.launch.title"_lang, "", "edz.gui.popup.information.launch.subtitle"_lang);
        launchItem->getClickEvent()->subscribe([&title](brls::View *view){
            title->launch();
        });

        softwareInfoList->addView(new brls::Header("edz.gui.popup.information.header.general"_lang, false));
        softwareInfoList->addView(launchItem);
        softwareInfoList->addView(new brls::Header("edz.gui.popup.information.header.save"_lang, false));
        
        // TODO: Fix new save FS not being detected by edizon
        if (title->hasSaveFile()) {
            for (userid_t userid : title->getUserIDs()) {
                auto &account = save::SaveFileSystem::getAllAccounts()[userid];

                if (title->hasSaveFile(account)) {
                    brls::ListItem *userItem = new brls::ListItem(account->getNickname(), "", hlp::formatString("edz.gui.popup.information.playtime"_lang, title->getPlayTime(account) / 3600, (title->getPlayTime(account) % 3600) / 60));
                    userItem->setThumbnail(account->getIcon().data(), account->getIcon().size());

                    softwareInfoList->addView(userItem);
                }
            }

            if (title->hasCommonSaveFile()) {
                brls::ListItem *userItem = new brls::ListItem("edz.gui.popup.information.save.common"_lang, "", " ");
                userItem->setThumbnail("romfs:/assets/info/icon_save_common.png");

                softwareInfoList->addView(userItem);                
            }
        }
        else {
            brls::ListItem *createSaveFSItem = new brls::ListItem("edz.gui.popup.management.createfs.title"_lang, "", "edz.gui.popup.management.createfs.subtitle"_lang);
            createSaveFSItem->getClickEvent()->subscribe([&title](brls::View *view) {
                hlp::openPlayerSelect([&title](std::unique_ptr<save::Account> &account) {
                    title->createSaveDataFileSystem(account, FsSaveDataType_Account);
                    brls::Application::popView();
                });
            });

            softwareInfoList->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.popup.error.nosavefs"_lang, true));
            softwareInfoList->addView(createSaveFSItem);
        }


        brls::List *saveManagementList = new brls::List();

        brls::ListItem *backupItem = new brls::ListItem("edz.gui.popup.management.backup.title"_lang, "", "edz.gui.popup.management.backup.subtitle"_lang);
        backupItem->getClickEvent()->subscribe([&title](brls::View *view) {
            hlp::openPlayerSelect([&title](std::unique_ptr<save::Account> &account) {
                std::string backupName;
                if (hlp::openSwkbdForText([&title, &backupName](std::string str) { backupName = str; }, "edz.gui.popup.management.backup.keyboard.title"_lang, "", 32, hlp::getCurrentDateTimeString()))
                    Gui::runAsyncWithDialog([&](){ save::SaveManager::backup(title, account, backupName, ""); }, "edz.gui.popup.management.backup.dialog"_lang);
             });

        });

        brls::ListItem *restoreItem = new brls::ListItem("edz.gui.popup.management.restore.title"_lang, "", "edz.gui.popup.management.restore.subtitle"_lang);
        restoreItem->getClickEvent()->subscribe([&title](brls::View *view) {
            auto list = save::SaveManager::getLocalBackupList(title).second;
            if (list.size() == 0)
                return;
                
            if (hlp::openPctlPrompt([]{})) {
                brls::Dropdown::open("edz.gui.popup.management.backup.dropdown.title"_lang, list, [&title, list](int selection) {

                    if (selection != -1) {
                        hlp::openPlayerSelect([&, list, selection](std::unique_ptr<save::Account> &account) { 
                            Gui::runAsyncWithDialog([&] { save::SaveManager::restore(title, account, list[selection]); }, "edz.gui.popup.management.restore.dialog"_lang);
                        });
                    }
                    
                });
            }
        });

        brls::ListItem *deleteItem = new brls::ListItem("edz.gui.popup.management.delete.title"_lang, "", "edz.gui.popup.management.delete.subtitle"_lang);
        deleteItem->getClickEvent()->subscribe([&title, this](brls::View *view) {

            brls::Dialog *confirmationDialog = new brls::Dialog("edz.gui.popup.management.delete.confirm"_lang);
            confirmationDialog->addButton("edz.dialog.no"_lang, [confirmationDialog](brls::View *view) { confirmationDialog->close(); });
            confirmationDialog->addButton("edz.dialog.yes"_lang, [confirmationDialog, &title, this](brls::View *view) {
                if (hlp::openPctlPrompt([this]{})) {
                    hlp::openPlayerSelect([&, this](std::unique_ptr<save::Account> &account) {
                        Gui::runLater([&]() { Gui::runAsyncWithDialog([&] { save::SaveManager::remove(title, account); }, "edz.gui.popup.management.delete.deleting"_lang); }, 1);
                    });
                }
                confirmationDialog->close();
            });

            confirmationDialog->open();
        });


        saveManagementList->addView(new brls::Header("edz.gui.popup.management.header.basic"_lang, false));
        saveManagementList->addView(backupItem);
        saveManagementList->addView(restoreItem);
        saveManagementList->addView(new brls::Header("edz.gui.popup.management.header.advanced"_lang, false));
        saveManagementList->addView(deleteItem);


        rootFrame->addTab("edz.gui.popup.information.tab"_lang, softwareInfoList);
        rootFrame->addSeparator();
        rootFrame->addTab("edz.gui.popup.management.tab"_lang, saveManagementList);


        brls::PopupFrame::open(title->getName(), title->getIcon().data(), title->getIcon().size(), rootFrame, "edz.gui.popup.version"_lang + " " + title->getVersionString(), title->getAuthor());
    }

    EResult GuiMain::createSaveRepoPopup(std::string saveRepoUrl) {
        api::SaveRepoAPI saveRepoApi(saveRepoUrl);
        std::string name, motd;
        std::vector<u8> icon;
        std::vector<api::SaveRepoAPI::save_file_t> saveFiles;
    
        if (auto [result, _name] = saveRepoApi.getName(); result.succeeded())
            name = _name;
        else return result;

        if (auto [result, _motd] = saveRepoApi.getMOTD(); result.succeeded())
            motd = _motd;
        else return result;

        if (auto [result, _icon] = saveRepoApi.getIcon(); result.succeeded())
            icon = _icon;
        else return result;

        if (auto [result, _saveFiles] = saveRepoApi.listFiles(); result.succeeded())
            saveFiles = _saveFiles;
        else return result;

        Gui::runLater([=]{

        brls::AppletFrame *rootFrame = new brls::AppletFrame(false, false);
        brls::List *list = new brls::List();

        auto &allTitles = save::SaveFileSystem::getAllTitles();
        for (auto saveFile : saveFiles) {
            if (allTitles.find(saveFile.titleID) == allTitles.end())
                continue;

            auto item = new brls::ListItem(hlp::formatString("%s [ %s ]", saveFile.name.c_str(), allTitles[saveFile.titleID]->getName().c_str()), "", saveFile.date);
            auto thisIcon = allTitles[saveFile.titleID]->getIcon();
            item->setThumbnail(thisIcon.data(), thisIcon.size());
            item->getClickEvent()->subscribe([saveRepoUrl, saveFile](brls::View *view) {
                hlp::openPlayerSelect([=](auto &account) {
                    Gui::runAsyncWithDialog([=, &account] {
                        auto &allTitles = save::SaveFileSystem::getAllTitles();
                        api::SaveRepoAPI saveRepoApi(saveRepoUrl);

                        hlp::File downloadedFile = hlp::File(hlp::createTmpFolder().path() + saveFile.name);
                        if (saveRepoApi.getFile(saveFile.name, downloadedFile.path()).succeeded()) {
                            std::string backupName = saveFile.name.substr(0, saveFile.name.length() - 4);
                            save::SaveManager::restore(allTitles[saveFile.titleID], account, backupName, downloadedFile.parent());
                        }
                        else
                            brls::Logger::error("Failed to download save file");
                    }, "edz.gui.main.repos.dialog.download"_lang);
                });
            });

            list->addView(item);
        }

        rootFrame->setContentView(list);

        brls::PopupFrame::open(name, const_cast<u8*>(&icon[0]), icon.size(), rootFrame, motd);

        }, 0);

        return ResultSuccess;
    }

    bool GuiMain::handleSorting(SortingStyle sorting, std::unique_ptr<save::Title> &l, std::unique_ptr<save::Title> &r) {
        u32 lLowestTime = 0, rLowestTime = 0;
        u32 lLatestTime = 0, rLatestTime = 0;
        u32 lHighestPlayTime = 0, rHighestPlayTime = 0;
        u32 lLaunches = 0, rLaunches = 0;

        switch (sorting) {
            case SortingStyle::TITLE_ID:
                return l->getID() < r->getID();
            case SortingStyle::ALPHABETICAL_NAME:
                return l->getName().compare(r->getName()) < 0;
            case SortingStyle::ALPHABETICAL_AUTHOR:
                return l->getAuthor().compare(r->getAuthor()) < 0;
            case SortingStyle::NUM_LAUNCHES:
                for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
                    lLaunches += l->getLaunchCount(account);
                    rLaunches += r->getLaunchCount(account);
                }

                return lLaunches > rLaunches;
            case SortingStyle::FIRST_PLAYED:
                for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
                    if (u32 lTime = l->getFirstPlayTime(account); lTime < lLowestTime)
                        lLowestTime = lTime;
                    if (u32 rTime = r->getFirstPlayTime(account); rTime < rLowestTime)
                        rLowestTime = rTime;
                }

                return lLowestTime < rLowestTime;
            case SortingStyle::LAST_PLAYED:
                for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
                    if (u32 lTime = l->getLastPlayTime(account); lTime > lLatestTime)
                        lLatestTime = lTime;
                    if (u32 rTime = r->getLastPlayTime(account); rTime > rLatestTime)
                        rLatestTime = rTime;
                }

                return lLatestTime > rLatestTime;
            case SortingStyle::PLAY_TIME:
                for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
                    lHighestPlayTime += l->getPlayTime(account);
                    rHighestPlayTime += r->getPlayTime(account);
                }

                return lHighestPlayTime > rHighestPlayTime;
        }

        return true;
    }

    void GuiMain::sortTitleGrid(brls::List *list, SortingStyle sorting) {
        std::vector<ui::element::TitleButton*> titleButtons;

        for (size_t i = 0; i < list->getViewsCount(); i++) {
            ui::element::HorizontalTitleList *hTitleList = static_cast<ui::element::HorizontalTitleList*>(list->getChild(i));

            for (const auto& titleButton : hTitleList->getChildren())
                titleButtons.push_back(static_cast<ui::element::TitleButton*>(titleButton->view));

            hTitleList->clear(false);
        }

        std::sort(titleButtons.begin(), titleButtons.end(), [sorting, this](ui::element::TitleButton* lItem, ui::element::TitleButton* rItem) {
            auto &l = lItem->getTitle();
            auto &r = rItem->getTitle();

            return this->handleSorting(sorting, l, r);
        });

        u16 index = 0;
        for (ui::element::TitleButton* titleButton : titleButtons) {
            titleButton->setColumn(index % 4);
            static_cast<ui::element::HorizontalTitleList*>(list->getChild(index / 4))->addView(titleButton);
            static_cast<ui::element::HorizontalTitleList*>(list->getChild(index / 4))->invalidate();
            index++;
        }

        list->invalidate();
    }

    void GuiMain::sortTitleList(std::vector<brls::BoxLayoutChild*>& list, SortingStyle sorting) {
        std::sort(list.begin(), list.end(), [sorting, this](brls::BoxLayoutChild* lItem, brls::BoxLayoutChild* rItem) {
            auto &l = static_cast<ui::element::TitleListItem*>(lItem->view)->getTitle();
            auto &r = static_cast<ui::element::TitleListItem*>(rItem->view)->getTitle();

            return this->handleSorting(sorting, l, r);
        });
    }

    void GuiMain::createTitleListView(brls::List *list, SortingStyle sorting) {
        for (auto &titlePair : save::SaveFileSystem::getAllTitles()) {
            auto &title = titlePair.second;

            brls::ListItem *titleItem = new ui::element::TitleListItem(title, hlp::limitStringLength(title->getName(), 45), "", title->getIDString());
            
            titleItem->setValue(Fonts::MaterialIcons::SUBMENU);
            titleItem->setThumbnail(title->getIcon().data(), title->getIcon().size()); 

            titleItem->getClickEvent()->subscribe([&](brls::View *view) {
                createTitlePopup(title);
            });


            list->addView(titleItem);
        }

        // sortTitleList(list->getChildren(), sorting);
        list->setSpacing(30);
    }

    void GuiMain::createTitleCondensedView(brls::List *list, SortingStyle sorting) {
        for (auto &titlePair : save::SaveFileSystem::getAllTitles()) {
            auto &title = titlePair.second;
            brls::ListItem *titleItem = new ui::element::TitleListItem(title, hlp::limitStringLength(title->getName(), 35));
            
            titleItem->setValue(Fonts::MaterialIcons::SUBMENU);
                        
            titleItem->getClickEvent()->subscribe([&](brls::View *view) {
                createTitlePopup(title);
            });


            list->addView(titleItem);
        }
        
        // sortTitleList(list->getChildren(), sorting);
        list->setSpacing(30);
    }

    void GuiMain::createTitleGridView(brls::List *list, SortingStyle sorting) {
        std::vector<element::HorizontalTitleList*> hLists;
        u16 column = 0;
        
        for (auto &titlePair : save::SaveFileSystem::getAllTitles()) {
            auto &title = titlePair.second;

            if (column % 4 == 0)
                hLists.push_back(new element::HorizontalTitleList());
            
            element::TitleButton *titleButton = new element::TitleButton(title, column % 4);

            titleButton->getClickEvent()->subscribe([&](brls::View *view) {
                createTitlePopup(title);
            });

            hLists[hLists.size() - 1]->addView(titleButton);

            column++;
        }
        
        for (element::HorizontalTitleList*& hList : hLists) {
            list->addView(hList);
        }

        GuiMain::sortTitleGrid(list, static_cast<SortingStyle>(GET_CONFIG(Settings.titlesSortingStyle)));
        list->setSpacing(30);

        list->invalidate();
        
    }

    void GuiMain::createTitlesListTab(brls::LayerView *layerView, SortingStyle sorting) {
        brls::List *listDisplayList = new brls::List();
        brls::List *condensedDisplayList = new brls::List();
        brls::List *gridDisplayList = new brls::List();        

        GuiMain::createTitleListView(listDisplayList, sorting);
        layerView->addLayer(listDisplayList);

        GuiMain::createTitleCondensedView(condensedDisplayList, sorting);
        layerView->addLayer(condensedDisplayList);

        if (!GET_CONFIG(Settings.fennecMode)) {
            GuiMain::createTitleGridView(gridDisplayList, sorting);
            layerView->addLayer(gridDisplayList);
        }

        layerView->changeLayer(GET_CONFIG(Settings.titlesDisplayStyle), false);
    }

    void GuiMain::createSaveReposTab(brls::List *list) {
        static bool addedUnofficialReposHeader = false;

        api::EdiZonAPI edizonApi;

        list->addView(new brls::Label(brls::LabelStyle::SMALL, "edz.gui.main.repos.label.desc"_lang, true));
        list->addView(new brls::Header("edz.gui.main.repos.header.official"_lang));

        auto [result, providers] = edizonApi.getOfficialProviders();

        if (providers.size() == 0)
            list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.repos.label.norepos"_lang, true));
        else {
            for (auto provider : providers) {
                brls::ListItem *listItem = new brls::ListItem(provider.name + "edz.by"_lang + provider.owner, "", provider.description);
                listItem->getClickEvent()->subscribe([=](brls::View *view) { 
                    Gui::runAsync([this, provider] {
                        brls::Application::blockInputs();
                        this->createSaveRepoPopup(provider.url);
                        brls::Application::unblockInputs();

                    });
                });
                list->addView(listItem);
            }
        }

        auto &unofficialProviders = GET_CONFIG(Settings.saveFileRepos);
        if (unofficialProviders.size() > 0) {

            for (auto providerUrl : unofficialProviders) {
                api::SaveRepoAPI saveRepoApi(providerUrl);

                auto [result1, name] = saveRepoApi.getName();
                if (result1.failed())
                    continue;

                auto [result2, motd] = saveRepoApi.getMOTD();
                if (result2.failed())
                    continue;

                if (!addedUnofficialReposHeader)
                    list->addView(new brls::Header("edz.gui.main.repos.header.unofficial"_lang));

                addedUnofficialReposHeader = true;

                brls::ListItem *listItem = new brls::ListItem(name, "", motd);
                listItem->getClickEvent()->subscribe([this, providerUrl](brls::View *view) { 
                    Gui::runAsync([this, providerUrl] {
                        brls::Application::blockInputs();
                        this->createSaveRepoPopup(providerUrl);
                        brls::Application::unblockInputs();
                    });
                });
                list->addView(listItem);
            }
        }

        list->registerAction("edz.gui.main.repos.button.add"_lang, brls::Key::X, [this, list]{
            hlp::openSwkbdForText([this, list](std::string input) {
                if (!std::regex_match(input, std::regex("^(?:http(s)?:\\/\\/)?[\\w.-]+(?:\\.[\\w\\.-]+)+[\\w\\-\\._~:/?#[\\]@!\\$&'\\(\\)\\*\\+,;=.]+$"))) {
                    //TODO: ERROR
                    return;
                }

                api::SaveRepoAPI saveRepoApi(input);

                auto [result1, name] = saveRepoApi.getName();
                if (result1.failed()) {
                    //TODO: ERROR
                    return;
                }

                auto [result2, motd] = saveRepoApi.getMOTD();
                if (result2.failed()) {
                    //TODO: ERROR
                    return;
                }

                if (!addedUnofficialReposHeader)
                    list->addView(new brls::Header("edz.gui.main.repos.header.unofficial"_lang));

                auto &unofficialProviders = GET_CONFIG(Settings.saveFileRepos);
                unofficialProviders.push_back(input);
                hlp::ConfigManager::store();

                brls::ListItem *listItem = new brls::ListItem(name, "", motd);
                listItem->getClickEvent()->subscribe([this, input](brls::View *view) { 
                    Gui::runAsync([this, input] {
                        brls::Application::blockInputs();
                        this->createSaveRepoPopup(input);
                        brls::Application::unblockInputs();
                    });
                });
                list->addView(listItem);
                list->invalidate();
            }, "edz.gui.main.repos.dialog.title"_lang, "edz.gui.main.repos.dialog.desc"_lang, 32, "http://");

            return true;
        });

        list->invalidate();
    }

    void GuiMain::createRunningTitleInfoTab(brls::List *list) {
        auto &runningTitle = save::Title::getRunningTitle();
                
        element::TitleInfo *titleInfo = new element::TitleInfo(runningTitle->getIcon(), runningTitle);

        brls::ListItem *cheatEngineItem = new brls::ListItem("edz.gui.main.running.cheatengine.title"_lang, "", "edz.gui.main.running.cheatengine.subtitle"_lang);
        cheatEngineItem->setValue(Fonts::MaterialIcons::SUBMENU);
        cheatEngineItem->getClickEvent()->subscribe([](brls::View *view){
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

            regionsTable->addRow(brls::TableRowType::BODY, regionName, "0x" + hlp::toHexString(region.addr) + " - 0x" + hlp::toHexString(region.addr + region.size));
        }

        list->addView(new brls::Header("edz.gui.main.running.general"_lang, true));
        list->addView(cheatEngineItem);
        list->addView(titleInfo);
        list->addView(new brls::Header("edz.gui.main.running.memory"_lang, true));
        list->addView(regionsTable);
    }

    void GuiMain::createCheatsTab(brls::List *list) {
        auto *edizonOverlayInstalledItem = new brls::ToggleListItem("edz.gui.main.cheats.overlay"_lang, hlp::File(OVERLAYS_PATH "/" EDIZON_OVERLAY_FILENAME).exists(), "edz.gui.main.cheats.overlay.desc"_lang);
        edizonOverlayInstalledItem->getClickEvent()->subscribe([edizonOverlayInstalledItem](brls::View *view) {
            brls::ToggleListItem* listItem = static_cast<brls::ToggleListItem*>(view);
            
            if (listItem->getToggleState()) {
                hlp::File overlayFile(EDIZON_ROMFS_OVERLAY_PATH "/" EDIZON_OVERLAY_FILENAME);
                overlayFile.copyTo(OVERLAYS_PATH "/" EDIZON_OVERLAY_FILENAME);
            }
            else {
                hlp::File overlayFile(OVERLAYS_PATH "/" EDIZON_OVERLAY_FILENAME);
                overlayFile.remove();
            }

            Gui::runLater([edizonOverlayInstalledItem] {
                bool actuallExtracted = hlp::File(OVERLAYS_PATH "/" EDIZON_OVERLAY_FILENAME).exists();

                if (actuallExtracted != edizonOverlayInstalledItem->getToggleState())
                    edizonOverlayInstalledItem->onClick();  // TODO: This probably won't work
            }, 10);
        });


        list->addView(edizonOverlayInstalledItem);
        list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.cheats.overlay.note"_lang, true));
        list->addView(new brls::Header("edz.gui.main.cheats.header.cheats"_lang, cheat::CheatManager::getCheats().size() == 0));

        GuiMain::m_cheatToggleListItems.clear();

        if (cheat::CheatManager::isCheatServiceAvailable()) {
            if (cheat::CheatManager::getCheats().size() > 0) {
                for (auto cheat : cheat::CheatManager::getCheats()) {
                    brls::ToggleListItem *cheatItem = new brls::ToggleListItem(hlp::limitStringLength(cheat->getName(), 60), cheat->isEnabled(), "",
                        "edz.widget.boolean.on"_lang, "edz.widget.boolean.off"_lang);

                    cheatItem->getClickEvent()->subscribe([=](brls::View *view){
                        printf("%d\n", cheat->isEnabled());
                        cheat->setState(static_cast<brls::ToggleListItem*>(view)->getToggleState());
                    });

                    list->addView(cheatItem);
                    GuiMain::m_cheatToggleListItems.push_back(cheatItem);
                }
                Gui::runRepetiviely([this] {
                    u16 i = 0;
                    for (auto &cheatToggle : GuiMain::m_cheatToggleListItems) {
                        bool isEnabled = cheat::CheatManager::getCheats()[i++]->isEnabled();
                        if (cheatToggle->getToggleState() != isEnabled)
                            cheatToggle->onClick(); // TODO: This should work properly without setToggle, but need to verify
                    }
                }, 20);

            } else {
                if (!hlp::isTitleRunning())
                    list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.no_title"_lang, true));
                else if (hlp::isInAppletMode()) {
                    auto [result, cheatFileName] = cheat::CheatManager::getCheatFile();

                    if (result.failed()) {
                        
                        if (this->m_onlineCheats.size() == 0) {
                            list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.no_cheats_online"_lang, true));
                        } else {
                            list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.no_cheats"_lang, true));
                            
                            brls::ListItem *applyOnlineCheatsItem = new brls::ListItem("edz.gui.main.cheats.button.load.online"_lang, "edz.gui.main.cheats.button.load.online.desc"_lang);
                            applyOnlineCheatsItem->getClickEvent()->subscribe([=](brls::View *view) {
                                brls::Application::blockInputs();
                                brls::Application::giveFocus(nullptr);

                                list->removeView(4);
                                list->removeView(3);

                                for (auto &onlineCheatDef : this->m_onlineCheats)
                                    cheat::CheatManager::addCheat(onlineCheatDef, false);

                                for (auto cheat : cheat::CheatManager::getCheats()) {
                                    brls::ToggleListItem *cheatItem = new brls::ToggleListItem(hlp::limitStringLength(cheat->getName(), 60), cheat->isEnabled(), "",
                                        "edz.widget.boolean.on"_lang, "edz.widget.boolean.off"_lang);

                                    cheatItem->getClickEvent()->subscribe([=](brls::View *view){
                                        cheat->setState(static_cast<brls::ToggleListItem*>(view)->getToggleState());
                                    });

                                    if (cheatItem != nullptr) {
                                        list->addView(cheatItem);
                                        GuiMain::m_cheatToggleListItems.push_back(cheatItem);
                                    }
                                }

                                brls::Application::giveFocus(list);
                                brls::Application::unblockInputs();
                            });

                            list->addView(applyOnlineCheatsItem);
                        }
                    }
                    else {
                        brls::ListItem *cheatFileSelectionItem = new brls::ListItem("edz.gui.main.cheats.button.load.offline"_lang, "edz.gui.main.cheats.button.load.offline.desc"_lang);
                        
                        cheatFileSelectionItem->getClickEvent()->subscribe([this](brls::View *view) {
                            auto [result, cheatDefs] = cheat::CheatParser::parseFile(EDIZON_CHEATS_DIR "/" + cheat::CheatManager::getCheatFile().second);

                            if (result.succeeded()) {
                                for (auto &cheatDef : cheatDefs)
                                    cheat::CheatManager::addCheat(cheatDef, false);

                                Gui::runLater([this] {
                                    brls::Application::blockInputs();
                                    if (brls::Application::getCurrentFocus() == this->m_cheatsList)
                                        brls::Application::giveFocus(nullptr);

                                    this->m_cheatsList->clear();
                                    createCheatsTab(this->m_cheatsList);

                                    brls::Application::giveFocus(this->m_cheatsList);
                                    brls::Application::unblockInputs();
                                }, 20);
                            }                                
                            else {
                                ;//TODO: Display error
                            }
                        });
                        
                        list->addView(cheatFileSelectionItem);
                    }
                }
                else
                    list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.application_mode"_lang, true));
            }
        } 
        else 
            list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.dmnt_cht_missing"_lang, true));
    }

    void GuiMain::createSettingsTab(brls::List *list) {
        // General Options
        std::vector<std::string> langCodes, langNames;

        // Add default language option (Copies HOS settings)
        langCodes.push_back("auto");
        langNames.push_back("edz.gui.main.settings.language.default"_lang);

        // Add all languages that have translations available
        for (const auto &[code, name] : LangEntry::getSupportedLanguages()) {
            langCodes.push_back(code);
            langNames.push_back(name);
        }

        brls::SelectListItem *languageOptionItem = new brls::SelectListItem("edz.gui.main.settings.language"_lang, langNames);

        languageOptionItem->getValueSelectedEvent()->subscribe([=](size_t selection) {
            SET_CONFIG(Settings.langCode, langCodes[selection]);

            brls::Application::blockInputs();

            Gui::runLater([]() {
                LangEntry::clearCache();
                brls::Application::giveFocus(nullptr);
                brls::Application::popView();
            }, 10);

            Gui::runLater([]() {
                Gui::changeTo<GuiMain>();
                brls::Application::unblockInputs();
            }, 40);
        });

        if (auto currLanguageIndex = std::find(langCodes.begin(), langCodes.end(), GET_CONFIG(Settings.langCode)); currLanguageIndex != langCodes.end())
            languageOptionItem->setSelectedValue(currLanguageIndex - langCodes.begin());

        brls::ToggleListItem *pctlOptionItem = nullptr;
        
        if (hlp::isPctlEnabled()) {
            pctlOptionItem = new brls::ToggleListItem("edz.gui.main.settings.pctl"_lang, GET_CONFIG(Settings.pctlChecksEnabled), "edz.gui.main.settings.pctl.desc"_lang, "edz.widget.boolean.on"_lang, "edz.widget.boolean.off"_lang);
            pctlOptionItem->getClickEvent()->subscribe([](brls::View *view) {
                static bool rejected = false;

                if (rejected) {
                    rejected = false;
                    return;
                }

                if (hlp::openPctlPrompt([]{})) {
                    SET_CONFIG(Settings.pctlChecksEnabled, static_cast<brls::ToggleListItem*>(view)->getToggleState());
                }
                else {
                    rejected = true;
                    static_cast<brls::ToggleListItem*>(view)->onClick();
                }
            });
        }

        // Title Options
        std::vector<std::string> displayOptions = { "edz.gui.main.settings.style.list"_lang, 
                                                    "edz.gui.main.settings.style.condensed"_lang };
                                
        std::vector<std::string> sortingOptions = { "edz.gui.main.settings.sorting.titleid"_lang,
                                                    "edz.gui.main.settings.sorting.alphaname"_lang,
                                                    "edz.gui.main.settings.sorting.alphaauthor"_lang,
                                                    "edz.gui.main.settings.sorting.firstplayed"_lang,
                                                    "edz.gui.main.settings.sorting.lastplayed"_lang,
                                                    "edz.gui.main.settings.sorting.playtime"_lang,
                                                    "edz.gui.main.settings.sorting.numlaunches"_lang };

        if (!GET_CONFIG(Settings.fennecMode))
            displayOptions.push_back("edz.gui.main.settings.style.grid"_lang);

        brls::SelectListItem *displayOptionsItem = new brls::SelectListItem("edz.gui.main.settings.style"_lang, displayOptions);
        brls::SelectListItem *sortingOptionsItem = new brls::SelectListItem("edz.gui.main.settings.sorting"_lang, sortingOptions);

        displayOptionsItem->getValueSelectedEvent()->subscribe([=](size_t selection) {
            this->m_titleList->changeLayer(selection, false);
            SET_CONFIG(Settings.titlesDisplayStyle, selection);
        });


        // sortingOptionsItem->getValueSelectedEvent()->subscribe([=](size_t selection) {
        //     SET_CONFIG(Settings.titlesSortingStyle, selection);
        //     Gui::runLater([this]() { 
        //         GuiMain::sortTitleList(static_cast<brls::List*>(this->m_titleList->getLayer(0))->getChildren(), static_cast<SortingStyle>(GET_CONFIG(Settings.titlesSortingStyle)));
        //         GuiMain::sortTitleList(static_cast<brls::List*>(this->m_titleList->getLayer(1))->getChildren(), static_cast<SortingStyle>(GET_CONFIG(Settings.titlesSortingStyle)));
                
        //         if (!GET_CONFIG(Settings.fennecMode))
        //              GuiMain::sortTitleGrid(static_cast<brls::List*>(this->m_titleList->getLayer(2)), static_cast<SortingStyle>(GET_CONFIG(Settings.titlesSortingStyle)));
        //     }, 1);
        // });

        displayOptionsItem->setSelectedValue(static_cast<int>(GET_CONFIG(Settings.titlesDisplayStyle)));
        sortingOptionsItem->setSelectedValue(static_cast<int>(GET_CONFIG(Settings.titlesSortingStyle)));


        // SwitchCheatsDB Login
        brls::ListItem *scdbLoginItem = new brls::ListItem("edz.gui.main.settings.account.title"_lang, "edz.gui.main.settings.scdbinfo"_lang);
        scdbLoginItem->setValue(GET_CONFIG(Online.loggedIn) ? GET_CONFIG(Online.switchcheatsdbEmail) : "edz.gui.main.settings.account.nologin"_lang);
        scdbLoginItem->getClickEvent()->subscribe([=](brls::View *view) { 
            if (GET_CONFIG(Online.loggedIn)) {
                brls::Dialog *dialog = new brls::Dialog("edz.gui.main.settings.dialog.logout"_lang);
                dialog->addButton("edz.dialog.no"_lang, [=](brls::View *view) {
                    dialog->close();
                });
                dialog->addButton("edz.dialog.yes"_lang, [=](brls::View *view) {
                    SET_CONFIG(Online.loggedIn, false);
                    SET_CONFIG(Online.switchcheatsdbEmail, "");
                    SET_CONFIG(Online.switchcheatsdbApiToken, "");
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

        list->addView(new brls::Header("edz.gui.main.settings.header.generaloptions"_lang));
        list->addView(languageOptionItem);
        if (pctlOptionItem != nullptr)
            list->addView(pctlOptionItem);

        list->addView(new brls::Header("edz.gui.main.settings.header.titleoptions"_lang));
        list->addView(displayOptionsItem);
        list->addView(sortingOptionsItem);
        
        list->addView(new brls::Header("edz.gui.main.settings.header.accountoptions"_lang));
        list->addView(scdbLoginItem);
    }

    void GuiMain::createAboutTab(brls::List *list) {
        list->addView(new brls::Header("edz.name"_lang + " v" + EDIZON_VERSION.getString() + (SNAPSHOT ? " - Snapshot" : "") + "edz.by"_lang + "edz.dev"_lang, true));
        list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.about.label.edz"_lang, true));

        list->addView(new brls::Header("edz.gui.main.about.header.links"_lang, false));
        
        brls::ListItem *scdbItem = new brls::ListItem("edz.switchcheatsdb.name"_lang, "", "https://switchcheatsdb.com");
        brls::ListItem *patreonItem = new brls::ListItem("edz.patreon.name"_lang, "", "https://patreon.com/werwolv");
        brls::ListItem *guideItem = new brls::ListItem("edz.gui.main.about.guide.title"_lang, "", "https://edizon.werwolv.net");

        scdbItem->setThumbnail("romfs:/assets/about/icon_scdb.png");
        patreonItem->setThumbnail("romfs:/assets/about/icon_patreon.png");
        guideItem->setThumbnail("romfs:/assets/about/icon_guide.png");

        scdbItem->setValue(Fonts::MaterialIcons::SUBMENU);
        patreonItem->setValue(Fonts::MaterialIcons::SUBMENU);
        guideItem->setValue(Fonts::MaterialIcons::SUBMENU);

        if (hlp::isInApplicationMode()) {
            scdbItem->getClickEvent()->subscribe([](brls::View *view)    { openWebpage("https://www.switchcheatsdb.com"); });
            patreonItem->getClickEvent()->subscribe([](brls::View *view) { openWebpage("https://patreon.com/werwolv");    });
            guideItem->getClickEvent()->subscribe([](brls::View *view)   { openWebpage("http://edizon.werwolv.net");      });
        }

        list->addView(scdbItem);
        list->addView(patreonItem);
        list->addView(guideItem);

        list->addView(new brls::Header("edz.gui.main.about.header.credits"_lang, true));
        list->addView(new element::CreditView());
    }

    void GuiMain::loadOnlineCheats() {
        if (GET_CONFIG(Online.loggedIn)) {           
            //Gui::runAsync([=]() {
                api::SwitchCheatsDBAPI switchCheatsDBAPI;

                auto [result, response] = switchCheatsDBAPI.getCheats(save::Title::getRunningTitleID());

                for (auto &cheat : response.cheats) {
                    if (cheat.buildID != cheat::CheatManager::getBuildID())
                        continue;
                    
                    auto [result, cheatDefs] = cheat::CheatParser::parseString(cheat.content);
                    if (result.failed())
                        continue;

                    for (auto &cheatDef : cheatDefs)
                        this->m_onlineCheats.push_back(cheatDef);
                }
            //});
        }
    }


    brls::View* GuiMain::setupUI() {
        brls::TabFrame *rootFrame = new brls::TabFrame();
        rootFrame->setTitle("edz.name"_lang);

        if (brls::Application::getThemeVariant() == brls::ThemeVariant::ThemeVariant_LIGHT)
            rootFrame->setIcon("romfs:/assets/icon_edz_dark.png");
        else
            rootFrame->setIcon("romfs:/assets/icon_edz_light.png");

        this->m_titleList = new brls::LayerView();
        this->m_saveReposList = new brls::List();
        this->m_cheatsList = new brls::List();
        this->m_playTimeStatsList = new brls::LayerView();
        this->m_settingsList = new brls::List();
        this->m_aboutList = new brls::List();

        loadOnlineCheats();

        createTitlesListTab(this->m_titleList, static_cast<SortingStyle>(GET_CONFIG(Settings.titlesSortingStyle)));
        createCheatsTab(this->m_cheatsList);
        createSaveReposTab(this->m_saveReposList);
        createSettingsTab(this->m_settingsList);
        createAboutTab(this->m_aboutList);

        rootFrame->addTab("edz.gui.main.titles.tab"_lang, this->m_titleList);

        if (hlp::isTitleRunning() && cheat::CheatManager::isCheatServiceAvailable()) {
            this->m_runningTitleInfoList = new brls::List();
            createRunningTitleInfoTab(this->m_runningTitleInfoList);           
            rootFrame->addTab("edz.gui.main.running.tab"_lang, this->m_runningTitleInfoList);
        }

        rootFrame->addTab("edz.gui.main.cheats.tab"_lang, this->m_cheatsList);
        rootFrame->addTab("edz.gui.main.repos.tab"_lang, this->m_saveReposList);
        rootFrame->addSeparator();
        rootFrame->addTab("edz.gui.main.settings.tab"_lang, this->m_settingsList);
        rootFrame->addTab("edz.gui.main.about.tab"_lang, this->m_aboutList);

        #if DEBUG_MODE_ENABLED
            rootFrame->setFooterText("edz.debugmode"_lang);
        #endif

        return rootFrame;
    }

    void GuiMain::update() {

    }   

}