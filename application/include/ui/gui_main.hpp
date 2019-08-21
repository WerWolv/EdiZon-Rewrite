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

#pragma once

#include <edizon.hpp>

#include "ui/gui.hpp"
#include "save/save_data.hpp"
#include "helpers/utils.hpp"
#include "cheat/cheat.hpp"

#include "ui/elements/title_info.hpp"
#include "ui/elements/title_button.hpp"

namespace edz::ui {

    class GuiMain : public Gui {
    public:
        GuiMain() : Gui() { }
        ~GuiMain() { }

        View* setupUI() override {
            TabFrame *rootFrame = new TabFrame();
            rootFrame->setTitle(edz::LangEntry("edz.name").get());

            // Title list
            this->m_titleList = new List();

            this->m_titleList->addView(new Header(edz::LangEntry("edz.gui.main.titles.options").get(), false));

            ListItem *sortingOption = new ListItem(edz::LangEntry("edz.gui.main.titles.style").get());
            sortingOption->setValue(edz::LangEntry("edz.gui.main.titles.style.list").get(), false, true);
            sortingOption->setClickListener([&](View *view) {

                for (u16 i = this->m_titleListEntries; i > 3; i--)
                    this->m_titleList->removeView(i - 1, true);

                this->m_titleListEntries = 3;

                if (this->m_sortingStyle == SortingStyle::GRID) {
                    this->m_sortingStyle = SortingStyle::LIST;
                    static_cast<ListItem*>(view)->setValue(edz::LangEntry("edz.gui.main.titles.style.list").get(), false, true);
                    constructTitleListView();
                }
                else if (this->m_sortingStyle == SortingStyle::LIST) {
                    this->m_sortingStyle = SortingStyle::GRID;
                    static_cast<ListItem*>(view)->setValue(edz::LangEntry("edz.gui.main.titles.style.grid").get(), false, true);
                    constructTitleGridView();
                }

            });

            this->m_titleList->addView(sortingOption);

            this->m_titleList->addView(new Header(edz::LangEntry("edz.gui.main.titles.tab").get(), false));

            this->m_titleListEntries = 3;

            constructTitleListView();

            rootFrame->addTab(edz::LangEntry("edz.gui.main.titles.tab").get(), this->m_titleList);


            // Running title information
            if (hlp::isTitleRunning() && cheat::CheatManager::get().isCheatServiceAvailable()) {
                cheat::CheatManager::get().forceAttach();
                List *titleInfoList = new List();

                std::shared_ptr<save::Title> runningTitle = save::SaveFileSystem::getAllTitles()[save::Title::getRunningTitleID()];
                
                size_t iconSize = runningTitle->getIconSize();
                u8 *iconBuffer = new u8[iconSize];
                runningTitle->getIcon(iconBuffer, iconSize);

                element::TitleInfo *titleInfo = new element::TitleInfo(iconBuffer, iconSize, runningTitle, save::Title::getRunningProcessID(), cheat::CheatManager::get().getBuildID());

                delete[] iconBuffer;

                titleInfoList->addView(new Header(edz::LangEntry("edz.gui.main.running.general").get(), true));
                titleInfoList->addView(titleInfo);
                titleInfoList->addView(new Header(edz::LangEntry("edz.gui.main.running.memory").get(), true));

                Table *regionsTable = new Table();

                bool foundHeap = false;
                u16 currCodeRegion = 0;
                u16 codeRegionCnt = 0;

                for (MemoryInfo region : cheat::CheatManager::get().getMemoryRegions())
                    if (region.type == MemType_CodeStatic && region.perm == Perm_Rx)
                        codeRegionCnt++;

                for (MemoryInfo region : cheat::CheatManager::get().getMemoryRegions()) {
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
                                case 2:
                                    regionName = "sdk";
                                    break;
                                default:
                                    regionName = "subsdk" + std::to_string(currCodeRegion - 2);
                            }
                        }

                        currCodeRegion++;
                    } else continue;

                    regionsTable->addRow(TableRowType::BODY, regionName, "0x" + hlp::toHexString(region.addr) + " - 0x" + hlp::toHexString(region.addr + region.size));
                }
                titleInfoList->addView(regionsTable);
                titleInfoList->addView(new ListItem(""));

                rootFrame->addTab(edz::LangEntry("edz.gui.main.running.tab").get(), titleInfoList);
            }


            // Cheats
            List *cheatsList = new List();

            cheatsList->addView(new Header(edz::LangEntry("edz.gui.main.cheats.header.desc").get(), true));
            cheatsList->addView(new Label(LabelStyle::DESCRIPTION, edz::LangEntry("edz.gui.main.cheats.label.desc").get(), true));
            cheatsList->addView(new Header(edz::LangEntry("edz.gui.main.cheats.header.cheats").get(), cheat::CheatManager::get().getCheats().size() == 0));

            if (cheat::CheatManager::get().isCheatServiceAvailable()) {
                if (cheat::CheatManager::get().getCheats().size() > 0) {
                    for (auto cheat : cheat::CheatManager::get().getCheats()) {
                        ToggleListItem *cheatItem = new ToggleListItem(cheat->getName(), cheat->isEnabled(), "",
                            edz::LangEntry("edz.widget.boolean.on").get(), edz::LangEntry("edz.widget.boolean.off").get());

                        cheatsList->addView(cheatItem);
                    }
                } else {
                    if (hlp::isInAppletMode())
                        cheatsList->addView(new Label(LabelStyle::DESCRIPTION, edz::LangEntry("edz.gui.main.cheats.error.no_cheats").get(), true));
                    else
                        cheatsList->addView(new Label(LabelStyle::DESCRIPTION, edz::LangEntry("edz.gui.main.cheats.error.application_mode").get(), true));
                }
            } 
            else 
                cheatsList->addView(new Label(LabelStyle::DESCRIPTION, edz::LangEntry("edz.gui.main.cheats.error.dmnt_cht_missing").get(), true));

            rootFrame->addTab(edz::LangEntry("edz.gui.main.cheats.tab").get(), cheatsList);


            // Settings
            List *settingsList = new List();

            settingsList->addView(new Header(edz::LangEntry("switchcheatsdb.name").get(), false));

            InputListItem *emailItem = new InputListItem(edz::LangEntry("edz.gui.main.about.email").get(), "", edz::LangEntry("edz.gui.main.about.email.help").get());
            ListItem *passwordItem = new ListItem(edz::LangEntry("edz.gui.main.about.password").get());
            passwordItem->setClickListener([&](View *view) {
                hlp::askSwkbdPassword([&](std::string text) {
                    this->m_password = text;

                    std::string itemText = "";
                    for (u8 i = 0; i < text.length(); i++)
                        itemText += "●";

                    static_cast<ListItem*>(view)->setValue(itemText);
                }, edz::LangEntry("edz.gui.main.about.password").get(), edz::LangEntry("edz.gui.main.about.password.help").get());
            });

            settingsList->addView(emailItem);
            settingsList->addView(passwordItem);
            
            ListItem *loginButton = new ListItem(edz::LangEntry("edz.gui.main.about.login").get());
            settingsList->addView(loginButton);
            settingsList->addView(new Label(LabelStyle::DESCRIPTION, edz::LangEntry("edz.gui.main.about.label.switchcheatsdb").get(), true));
            
            settingsList->addView(new ListItemGroupSpacing(true));

            rootFrame->addTab(edz::LangEntry("edz.gui.main.settings.tab").get(), settingsList);

            // About
            List *aboutList = new List();

            aboutList->addView(new Header(edz::LangEntry("edz.name").get() + " " VERSION_STRING + " "s + edz::LangEntry("edz.dev").get(), false));
            aboutList->addView(new Label(LabelStyle::DESCRIPTION, edz::LangEntry("edz.gui.main.about.label.edz").get(), true));

            rootFrame->addSeparator();
            rootFrame->addTab(edz::LangEntry("edz.gui.main.about.tab").get(), aboutList);

            return rootFrame;
        }

        void update() override {

        }

        private:
            std::string m_email, m_password;
            List *m_titleList;
            u16 m_titleListEntries;
            enum class SortingStyle { LIST, GRID } m_sortingStyle;

            void constructTitlePopup(save::Title *title) {
                size_t iconSize = title->getIconSize();
                u8 *iconBuffer = new u8[iconSize];
                title->getIcon(iconBuffer, iconSize);

                TabFrame *rootFrame = new TabFrame();
                rootFrame->addTab("Software Information", new Rectangle(nvgRGB(0xFF, 0x00, 0x00)));
                rootFrame->addSeparator();
                rootFrame->addTab("Save Management", new Rectangle(nvgRGB(0x00, 0xFF, 0x00)));
                rootFrame->addTab("Save Editing", new Rectangle(nvgRGB(0x00, 0x00, 0xFF)));

                PopupFrame::open(title->getName(), iconBuffer, iconSize, rootFrame, "Ver. " + title->getVersionString(), title->getAuthor());

                delete[] iconBuffer;
            }

            void constructTitleListView() {
                for (auto &[titleID, title] : save::SaveFileSystem::getAllTitles()) {
                    ListItem *titleItem = new ListItem(hlp::limitStringLength(title->getName(), 45), "", title->getIDString());
                    
                    size_t iconSize = title->getIconSize();
                    u8 *iconBuffer = new u8[iconSize];
                    title->getIcon(iconBuffer, iconSize);
                    
                    titleItem->setThumbnail(iconBuffer, iconSize);

                    delete[] iconBuffer;
                    
                    titleItem->setClickListener([&](View *view) {
                        constructTitlePopup(title.get());
                    });


                    this->m_titleList->addView(titleItem);
                    this->m_titleListEntries++;
                }

                this->m_titleList->invalidate();
            }

            void constructTitleGridView() {
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
                        constructTitlePopup(title.get());
                    });

                    hLists[hLists.size() - 1]->addView(titleButton);

                    delete[] iconBuffer;


                    column++;
                }
                
                for (element::HorizontalTitleList*& hList : hLists) {
                    this->m_titleList->addView(hList);
                    this->m_titleListEntries++;
                }

                this->m_titleList->setSpacing(30);

                this->m_titleList->invalidate();
                
            }
    };

}