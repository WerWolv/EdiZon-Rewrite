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
#include "save/save_manager.hpp"

#include "ui/elements/title_info.hpp"
#include "ui/elements/title_button.hpp"

namespace edz::ui {

    class GuiMain : public Gui {
    public:
        GuiMain() : Gui() { }
        ~GuiMain() { }

        enum class DisplayStyle { LIST, CONDENSED, GRID };
        enum class SortingStyle { ALPHABETICAL_NAME, ALPHABETICAL_AUTHOR, FIRST_PLAYED, LAST_PLAYED, PLAY_TIME, NUM_LAUNCHES };

        brls::View* setupUI() override;
        void update() override;

        private:
            std::string m_email, m_password;

            brls::LayerView *m_titleList = nullptr;
            brls::List *m_runningTitleInfoList = nullptr;
            brls::List *m_cheatsList = nullptr;
            brls::LayerView *m_settingsList = nullptr;
            brls::List *m_aboutList = nullptr;

            void createTitlePopup(save::Title *title);
            void createTitleListView(brls::List *list);
            void createTitleGridView(brls::List *list);

            void createTitlesListTab(brls::LayerView *layerView);
            void createSaveReposTab(brls::List *list);
            void createRunningTitleInfoTab(brls::List *list);
            void createCheatsTab(brls::List *list);
            void createSettingsTab(brls::LayerView *layerView);
            void createAboutTab(brls::List *list);
    };

}