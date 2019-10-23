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
#include "ui/elements/focusable_table.hpp"
#include "ui/elements/title_list_item.hpp"
#include "ui/pages/page_login.hpp"

#include "api/edizon_api.hpp"
#include "api/save_repo_api.hpp"
#include "api/switchcheatsdb_api.hpp"

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

    void GuiMain::createTitlePopup(save::Title *title) {
        size_t iconSize = title->getIconSize();
        u8 *iconBuffer = new u8[iconSize];
        title->getIcon(iconBuffer, iconSize);

        brls::TabFrame *rootFrame = new brls::TabFrame();

        brls::List *softwareInfoList = new brls::List();

        brls::ListItem *launchItem = new brls::ListItem("edz.gui.popup.information.launch.title"_lang, "", "edz.gui.popup.information.launch.subtitle"_lang);
        launchItem->setClickListener([title](brls::View *view){
            AppletStorage s = { 0 };
            appletCreateStorage(&s, 0);
            appletRequestLaunchApplication(title->getID(), &s);
        });

        softwareInfoList->addView(new brls::Header("edz.gui.popup.information.header.general"_lang, false));
        softwareInfoList->addView(launchItem);
        softwareInfoList->addView(new brls::Header("edz.gui.popup.information.header.save"_lang, false));

        
        // TODO: Fix new save FS not being detected by edizon
        if (title->hasSaveFile()) {
            for (userid_t userid : title->getUserIDs()) {
                save::Account *account = save::SaveFileSystem::getAllAccounts()[userid].get();

                if (title->hasSaveFile(account)) {
                    brls::ListItem *userItem = new brls::ListItem(account->getNickname(), "", hlp::formatString("edz.gui.popup.information.playtime"_lang, title->getLaunchCount(account), title->getPlayTime(account) / 3600, (title->getPlayTime(account) % 3600) / 60));
                    
                    size_t accountIconBufferSize = account->getIconSize();
                    u8 *accountIconBuffer = new u8[accountIconBufferSize];
                    account->getIcon(accountIconBuffer, accountIconBufferSize);
                    userItem->setThumbnail(accountIconBuffer, accountIconBufferSize);

                    delete[] accountIconBuffer;

                    softwareInfoList->addView(userItem);
                }
            }
        }
        else {
            brls::ListItem *createSaveFSItem = new brls::ListItem("edz.gui.popup.management.createfs.title"_lang, "", "edz.gui.popup.management.createfs.subtitle"_lang);
            createSaveFSItem->setClickListener([=](brls::View *view) {
                hlp::openPlayerSelect([=](save::Account *account) {
                    title->createSaveDataFileSystem(account);
                });
            });

            softwareInfoList->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.popup.error.nosavefs"_lang, true));
            softwareInfoList->addView(createSaveFSItem);
        }

        brls::List *saveManagementList = new brls::List();
        
        brls::ListItem *backupItem = new brls::ListItem("edz.gui.popup.management.backup.title"_lang, "", "edz.gui.popup.management.backup.subtitle"_lang);
        backupItem->setClickListener([=](brls::View *view) {
            save::Account *currUser = nullptr;
            std::string backupName;
            
            if (hlp::openPlayerSelect([&](save::Account *account) { currUser = account; }))
                if (hlp::openSwkbdForText([&](std::string str) { backupName = str; }, "edz.gui.popup.management.backup.keyboard.title"_lang)) {
                    Gui::runAsyncWithDialog(std::async(std::launch::async, save::SaveManager::backup, title, currUser, backupName, ""), "Creating a save data backup...");
                }
        });

        brls::ListItem *restoreItem = new brls::ListItem("edz.gui.popup.management.restore.title"_lang, "", "edz.gui.popup.management.restore.subtitle"_lang);
        restoreItem->setClickListener([=](brls::View *view) {
            auto [result, list] = save::SaveManager::getLocalBackupList(title);

            brls::Dropdown::open("edz.gui.popup.management.backup.dropdown.title"_lang, list, [=](int selection) {
                if (selection != -1)
                    hlp::openPlayerSelect([=](save::Account *account) { 
                        Gui::runAsyncWithDialog(std::async(std::launch::async, save::SaveManager::restore, title, account, list[selection], ""), "Restoring a save data backup...");
                    });
            });
        });

        brls::ListItem *deleteItem = new brls::ListItem("edz.gui.popup.management.delete.title"_lang, "", "edz.gui.popup.management.delete.subtitle"_lang);
        deleteItem->setClickListener([=](brls::View *view) {
            
            brls::Dialog *confirmationDialog = new brls::Dialog("Are your sure you want to delete your save data? This cannot be undone without a backup.");
            confirmationDialog->addButton("No", [confirmationDialog](brls::View *view) { confirmationDialog->close(); });
            confirmationDialog->addButton("Yes", [confirmationDialog, title](brls::View *view) {
                hlp::openPlayerSelect([&](save::Account *account) {
                    Gui::runLater([=]() { Gui::runAsyncWithDialog(std::async(std::launch::async, save::SaveManager::remove, title, account), "Deleting save data..."); }, 1);
                });
                confirmationDialog->close();
            });

            confirmationDialog->open();
        });

        /*brls::ListItem *editItem = new brls::ListItem("Edit Save File", "", "Edit your save file");
        editItem->setClickListener([=](brls::View *view) {
            save::Account *currUser = nullptr;
            if (hlp::openPlayerSelect([&](save::Account *account) { currUser = account; })) {
                auto [result, config] = save::edit::Editor::loadConfig(title, currUser);

                if (result.succeeded()) {
                    std::vector<std::string> saveFilePaths = config->getSaveFilePaths();

                    brls::Dropdown::open("edz.gui.popup.management.backup.dropdown.title"_lang, saveFilePaths, [=](int selection) {
                        if (selection != -1) {
                            auto [result, script] = save::edit::Editor::loadScript(title, currUser, saveFilePaths[selection]);
                            brls::TabFrame *editorFrame = new brls::TabFrame();
                            config->setScript(script);
                            config->createUI(editorFrame);

                            brls::Application::pushView(editorFrame);
                        }                   
                    });   
                }

            }
        });*/

        saveManagementList->addView(new brls::Header("edz.gui.popup.management.header.basic"_lang, false));
        saveManagementList->addView(backupItem);
        saveManagementList->addView(restoreItem);
        saveManagementList->addView(new brls::Header("edz.gui.popup.management.header.advanced"_lang, false));
        saveManagementList->addView(deleteItem);
        //saveManagementList->addView(editItem);

        rootFrame->addTab("edz.gui.popup.information.tab"_lang, softwareInfoList);
        rootFrame->addSeparator();
        rootFrame->addTab("edz.gui.popup.management.tab"_lang, saveManagementList);

        brls::PopupFrame::open(title->getName(), iconBuffer, iconSize, rootFrame, "edz.gui.popup.version"_lang + " " + title->getVersionString(), title->getAuthor());

        delete[] iconBuffer;
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

        brls::AppletFrame *rootFrame = new brls::AppletFrame(false, false);
        brls::List *list = new brls::List();

        for (auto saveFile : saveFiles)
            list->addView(new brls::ListItem(saveFile.name, "", saveFile.date));

        rootFrame->setContentView(list);

        brls::PopupFrame::open(name, &icon[0], icon.size(), rootFrame, motd);

        return ResultSuccess;
    }

    bool GuiMain::handleSorting(SortingStyle sorting, save::Title *l, save::Title *r) {
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
    }

    void GuiMain::sortTitleGrid(brls::List *list, SortingStyle sorting) {
        std::vector<ui::element::TitleButton*> titleButtons;

        for (brls::BoxLayoutChild *child : list->getChildren()) {
            ui::element::HorizontalTitleList *hTitleList = static_cast<ui::element::HorizontalTitleList*>(child->view);

            for (const auto& titleButton : hTitleList->getChildren())
                titleButtons.push_back(static_cast<ui::element::TitleButton*>(titleButton->view));

            hTitleList->clear(false);
        }

        std::sort(titleButtons.begin(), titleButtons.end(), [sorting, this](ui::element::TitleButton* lItem, ui::element::TitleButton* rItem) {
            save::Title *l = lItem->getTitle();
            save::Title *r = rItem->getTitle();

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
            save::Title *l = static_cast<ui::element::TitleListItem*>(lItem->view)->getTitle().get();
            save::Title *r = static_cast<ui::element::TitleListItem*>(rItem->view)->getTitle().get();

            return this->handleSorting(sorting, l, r);
        });
    }

    void GuiMain::createTitleListView(brls::List *list, SortingStyle sorting) {
        for (auto &[titleid, title] : save::SaveFileSystem::getAllTitles()) {
            brls::ListItem *titleItem = new ui::element::TitleListItem(title, hlp::limitStringLength(title->getName(), 45), "", title->getIDString());
            
            size_t iconSize = title->getIconSize();
            u8 *iconBuffer = new u8[iconSize];
            title->getIcon(iconBuffer, iconSize);
            
            titleItem->setThumbnail(iconBuffer, iconSize);

            delete[] iconBuffer;
            
            titleItem->setClickListener([&](brls::View *view) {
                createTitlePopup(title.get());
            });


            list->addView(titleItem);
        }

        sortTitleList(list->getChildren(), sorting);
        list->setSpacing(30);
    }

    void GuiMain::createTitleCondensedView(brls::List *list, SortingStyle sorting) {
        for (auto &[titleid, title] : save::SaveFileSystem::getAllTitles()) {
            brls::ListItem *titleItem = new ui::element::TitleListItem(title, hlp::limitStringLength(title->getName(), 35));
            titleItem->setValue(title->getIDString(), true, false);
                        
            titleItem->setClickListener([&](brls::View *view) {
                createTitlePopup(title.get());
            });


            list->addView(titleItem);
        }
        
        sortTitleList(list->getChildren(), sorting);
        list->setSpacing(30);
    }

    void GuiMain::createTitleGridView(brls::List *list, SortingStyle sorting) {
        std::vector<element::HorizontalTitleList*> hLists;
        u16 column = 0;
        
        for (auto &[titleid, title] : save::SaveFileSystem::getAllTitles()) {         
            if (column % 4 == 0)
                hLists.push_back(new element::HorizontalTitleList());
            
            element::TitleButton *titleButton = new element::TitleButton(title.get(), column % 4);

            titleButton->setClickListener([&](brls::View *view) {
                createTitlePopup(title.get());
            });

            hLists[hLists.size() - 1]->addView(titleButton);

            column++;
        }
        
        for (element::HorizontalTitleList*& hList : hLists) {
            list->addView(hList);
        }

        GuiMain::sortTitleGrid(list, static_cast<SortingStyle>(GET_CONFIG(Save.titlesSortingStyle)));
        list->setSpacing(30);

        list->invalidate();
        
    }

    void GuiMain::createTitlesListTab(brls::LayerView *layerView, SortingStyle sorting) {
        brls::List *listDisplayList = new brls::List();
        brls::List *condensedDisplayList = new brls::List();
        brls::List *gridDisplayList = new brls::List();        

        GuiMain::createTitleListView(listDisplayList, sorting);
        GuiMain::createTitleCondensedView(condensedDisplayList, sorting);
        GuiMain::createTitleGridView(gridDisplayList, sorting);

        layerView->addLayer(listDisplayList);
        layerView->addLayer(condensedDisplayList);
        layerView->addLayer(gridDisplayList);

        layerView->changeLayer(GET_CONFIG(Save.titlesDisplayStyle), false);
    }

    void GuiMain::createSaveReposTab(brls::List *list) {
        api::EdiZonAPI edizonApi;

        list->addView(new brls::Label(brls::LabelStyle::SMALL, "edz.gui.main.repos.label.desc"_lang, true));
        list->addView(new brls::Header("edz.gui.main.repos.header.official"_lang));

        auto [result, providers] = edizonApi.getOfficialProviders();

        if (providers.size() == 0)
            list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.repos.label.norepos"_lang, true));
        else {
            for (auto provider : providers) {
                brls::ListItem *listItem = new brls::ListItem(provider.name + "edz.by"_lang + provider.owner, "", provider.description);
                listItem->setClickListener([=](brls::View *view) { this->createSaveRepoPopup(provider.url); });
                list->addView(listItem);
            }
        }

        list->invalidate();
    }

    void GuiMain::createRunningTitleInfoTab(brls::List *list) {
        std::shared_ptr<save::Title> runningTitle = save::SaveFileSystem::getAllTitles()[save::Title::getRunningTitleID()];
                
        size_t iconSize = runningTitle->getIconSize();
        u8 *iconBuffer = new u8[iconSize];

        runningTitle->getIcon(iconBuffer, iconSize);

        element::TitleInfo *titleInfo = new element::TitleInfo(iconBuffer, iconSize, runningTitle);
        delete[] iconBuffer;

        brls::ListItem *cheatEngineItem = new brls::ListItem("edz.gui.main.running.cheatengine.title"_lang, "", "edz.gui.main.running.cheatengine.subtitle"_lang);
        cheatEngineItem->setClickListener([](brls::View *view){
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
        list->addView(new brls::Label(brls::LabelStyle::SMALL, "edz.gui.main.cheats.label.desc"_lang, true));
        list->addView(new brls::Header("edz.gui.main.cheats.header.cheats"_lang, cheat::CheatManager::getCheats().size() == 0));

        if (cheat::CheatManager::isCheatServiceAvailable()) {
            if (cheat::CheatManager::getCheats().size() > 0) {
                for (auto cheat : cheat::CheatManager::getCheats()) {
                    brls::ToggleListItem *cheatItem = new brls::ToggleListItem(hlp::limitStringLength(cheat->getName(), 60), cheat->isEnabled(), "",
                        "edz.widget.boolean.on"_lang, "edz.widget.boolean.off"_lang);

                    cheatItem->setClickListener([=](brls::View *view){
                        cheat->setState(static_cast<brls::ToggleListItem*>(view)->getToggleState());
                    });

                    list->addView(cheatItem);
                }
            } else {
                if (!hlp::isTitleRunning())
                    list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.no_title"_lang, true));
                else if (hlp::isInAppletMode())
                    list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.cheats.error.no_cheats"_lang, true));
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

        languageOptionItem->setListener([=](size_t selection) {
            SET_CONFIG(Save.langCode, langCodes[selection]);
            
            brls::Application::blockInputs();

            Gui::runLater([]() {
                LangEntry::clearCache();
                brls::Application::removeFocus();
                brls::Application::popView();
            }, 10);

            Gui::runLater([]() {
                Gui::changeTo<GuiMain>();
                brls::Application::unblockInputs();
            }, 25);
        });

        if (auto currLanguageIndex = std::find(langCodes.begin(), langCodes.end(), GET_CONFIG(Save.langCode)); currLanguageIndex != langCodes.end())
            languageOptionItem->setSelectedValue(currLanguageIndex - langCodes.begin());

        // Title Options
        auto displayOptions = { "edz.gui.main.settings.style.list"_lang, 
                                "edz.gui.main.settings.style.condensed"_lang,
                                "edz.gui.main.settings.style.grid"_lang };
        auto sortingOptions = { "edz.gui.main.settings.sorting.titleid"_lang,
                                "edz.gui.main.settings.sorting.alphaname"_lang,
                                "edz.gui.main.settings.sorting.alphaauthor"_lang,
                                "edz.gui.main.settings.sorting.firstplayed"_lang,
                                "edz.gui.main.settings.sorting.lastplayed"_lang,
                                "edz.gui.main.settings.sorting.playtime"_lang,
                                "edz.gui.main.settings.sorting.numlaunches"_lang };

        brls::SelectListItem *displayOptionsItem = new brls::SelectListItem("edz.gui.main.settings.style"_lang, displayOptions);
        brls::SelectListItem *sortingOptionsItem = new brls::SelectListItem("edz.gui.main.settings.sorting"_lang, sortingOptions);

        displayOptionsItem->setListener([=](size_t selection) {
            this->m_titleList->changeLayer(selection, false);
            SET_CONFIG(Save.titlesDisplayStyle, selection);
        });


        sortingOptionsItem->setListener([=](size_t selection) {
            SET_CONFIG(Save.titlesSortingStyle, selection);
            Gui::runLater([this]() { 
                GuiMain::sortTitleList(static_cast<brls::List*>(this->m_titleList->getLayer(0))->getChildren(), static_cast<SortingStyle>(GET_CONFIG(Save.titlesSortingStyle)));
                GuiMain::sortTitleList(static_cast<brls::List*>(this->m_titleList->getLayer(1))->getChildren(), static_cast<SortingStyle>(GET_CONFIG(Save.titlesSortingStyle)));
                GuiMain::sortTitleGrid(static_cast<brls::List*>(this->m_titleList->getLayer(2)), static_cast<SortingStyle>(GET_CONFIG(Save.titlesSortingStyle)));
            }, 1);
        });

        displayOptionsItem->setSelectedValue(static_cast<int>(GET_CONFIG(Save.titlesDisplayStyle)));
        sortingOptionsItem->setSelectedValue(static_cast<int>(GET_CONFIG(Save.titlesSortingStyle)));


        // SwitchCheatsDB Login
        brls::ListItem *scdbLoginItem = new brls::ListItem("edz.gui.main.settings.account.title"_lang);
        scdbLoginItem->setValue(GET_CONFIG(Update.loggedIn) ? GET_CONFIG(Update.switchcheatsdbEmail) : "Not logged in");
        scdbLoginItem->setClickListener([=](brls::View *view) { 
            if (GET_CONFIG(Update.loggedIn)) {
                brls::Dialog *dialog = new brls::Dialog("edz.gui.main.settings.dialog.logout"_lang);
                dialog->addButton("edz.dialog.no"_lang, [=](brls::View *view) {
                    dialog->close();
                });
                dialog->addButton("edz.dialog.yes"_lang, [=](brls::View *view) {
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

        list->addView(new brls::Header("edz.gui.main.settings.header.generaloptions"_lang));
        list->addView(languageOptionItem);

        list->addView(new brls::Header("edz.gui.main.settings.header.titleoptions"_lang));
        list->addView(displayOptionsItem);
        list->addView(sortingOptionsItem);
        
        list->addView(new brls::Header("edz.gui.main.settings.header.accountoptions"_lang));
        list->addView(new brls::Label(brls::LabelStyle::MEDIUM, "edz.gui.main.settings.scdbinfo"_lang, true));
        list->addView(scdbLoginItem);
    }

    void GuiMain::createAboutTab(brls::List *list) {
        list->addView(new brls::Header("edz.name"_lang + " " VERSION_STRING + "edz.by"_lang + "edz.dev"_lang, true));
        list->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, "edz.gui.main.about.label.edz"_lang, true));

        list->addView(new brls::Header("edz.gui.main.about.links"_lang, false));
        
        brls::ListItem *scdbItem = new brls::ListItem("edz.switchcheatsdb.name"_lang, "", "https://switchcheatsdb.com");
        brls::ListItem *patreonItem = new brls::ListItem("edz.patreon.name"_lang, "", "https://patreon.com/werwolv");
        brls::ListItem *guideItem = new brls::ListItem("edz.gui.main.about.guide.title"_lang, "", "https://edizon.werwolv.net");

        scdbItem->setThumbnail("romfs:/assets/icon_scdb.png");
        patreonItem->setThumbnail("romfs:/assets/icon_patreon.png");
        guideItem->setThumbnail("romfs:/assets/icon_guide.png");

        if (hlp::isInApplicationMode()) {
            scdbItem->setClickListener([](brls::View *view)    { openWebpage("https://www.switchcheatsdb.com"); });
            patreonItem->setClickListener([](brls::View *view) { openWebpage("https://patreon.com/werwolv");    });
            guideItem->setClickListener([](brls::View *view)   { openWebpage("http://edizon.werwolv.net");      });
        }

        list->addView(scdbItem);
        list->addView(patreonItem);
        list->addView(guideItem);
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
        this->m_settingsList = new brls::List();
        this->m_aboutList = new brls::List();

        createTitlesListTab(this->m_titleList, SortingStyle::TITLE_ID);
        createSaveReposTab(this->m_saveReposList);

        createCheatsTab(this->m_cheatsList);
        createSettingsTab(this->m_settingsList);
        createAboutTab(this->m_aboutList);

        rootFrame->addTab("edz.gui.main.titles.tab"_lang, this->m_titleList);
        rootFrame->addTab("edz.gui.main.repos.tab"_lang, this->m_saveReposList);

        if (hlp::isTitleRunning() && cheat::CheatManager::isCheatServiceAvailable()) {
            this->m_runningTitleInfoList = new brls::List();
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