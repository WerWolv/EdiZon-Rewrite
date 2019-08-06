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

#include "helpers/folder.hpp"
#include "save/account.hpp"
#include "save/title.hpp"

#include <map>

#define SAVE_DEVICE "save"

namespace edz::save {

    class SaveFileSystem {
    public:
        SaveFileSystem(Title *title, Account *account);
        ~SaveFileSystem();

        static std::map<titleid_t, std::unique_ptr<Title>>& getAllTitles();
        static std::map<userid_t, std::unique_ptr<Account>>& getAllAccounts();

        edz::helper::Folder getSaveFolder();
        void commit();

    private:
        static std::vector<FsSaveDataInfo> getTitleSaveFileData();

        u16 m_openFileSystemID;

        FsFileSystem m_saveFileSystem;
        bool m_initialized = false;
    };

}