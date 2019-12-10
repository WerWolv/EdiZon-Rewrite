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

#include "save/title.hpp"
#include "save/account.hpp"
#include "helpers/folder.hpp"

namespace edz::save {

    class SaveManager {
    public:
        typedef struct {
            u32 magic;  // EDZN -> 0x4E5A4445
            titleid_t titleID;
            userid_t userID;
            time_t creationDate;
            bool deltaFile;
        } PACKED backup_header_t;

        static EResult backup(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account, std::string backupName, std::string basePath = "");
        static EResult backup(std::unique_ptr<Title> &title, std::string backupName, std::string basePath = "");
        static EResult restore(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account, std::string backupName, std::string basePath = "");
        static EResult restore(std::unique_ptr<Title> &title, std::string backupName, std::string basePath = "");
        static EResult remove(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account);
        static EResult remove(std::unique_ptr<Title> &title);
 
        static EResult swapSaveData(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account, std::string backupName);
        static EResult swapSaveData(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account1, std::unique_ptr<Account> &account2);
        static EResult duplicate(std::unique_ptr<Title> &title, std::unique_ptr<Account> &from, std::unique_ptr<Account> &to);
 
        static EResult upload(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account, std::string backupName);
        static EResult upload(std::unique_ptr<Title> &title, std::string backupName);
 
        static EResult download(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account, std::string url);
        static EResult download(std::unique_ptr<Title> &title, std::string localName, std::string url);
 
        static std::pair<EResult, std::vector<std::string>> getLocalBackupList(std::unique_ptr<Title> &title);
        static std::pair<EResult, std::map<std::string, std::string>> getOnlineBackupList(std::unique_ptr<Title> &title);
        static std::pair<EResult, bool> areBackupsUpToDate(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account);
        
    private:
        static std::string getBackupFolderName(std::unique_ptr<Title> &title);
    };

}
