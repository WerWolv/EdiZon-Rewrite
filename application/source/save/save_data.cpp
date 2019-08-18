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

#include "save/save_data.hpp"

namespace edz::save {

    SaveFileSystem::SaveFileSystem(Title *title, Account *account) {
        static u16 usedIDs = 0;

        if (R_FAILED(fsMount_SaveData(&this->m_saveFileSystem, title->getID(), account->getID())))
            return;

        this->m_openFileSystemID = usedIDs++;

        if (fsdevMountDevice((SAVE_DEVICE"_" + std::to_string(this->m_openFileSystemID)).c_str(), this->m_saveFileSystem) == -1) {
            fsFsClose(&this->m_saveFileSystem);
            return;
        }

        this->m_initialized = true;
    }

    SaveFileSystem::~SaveFileSystem() {
        if (!this->m_initialized) return;

        fsdevUnmountDevice((SAVE_DEVICE"_" + std::to_string(this->m_openFileSystemID)).c_str());
        fsFsClose(&this->m_saveFileSystem);
    }

    std::map<titleid_t, std::unique_ptr<Title>>& SaveFileSystem::getAllTitles() {
        static std::map<titleid_t, std::unique_ptr<Title>> titles;

        if (titles.size() > 0)
            return titles;

        NsApplicationRecord *appRecords = new NsApplicationRecord[1024]; // Nobody's going to have more than 1024 games hopefully...
        size_t actualAppRecordCnt = 0;

        if (EResult(nsListApplicationRecord(appRecords, 1024, 0, &actualAppRecordCnt)).failed()) {
            delete[] appRecords;
            return titles;  // Return empty titles map on error
        }
        

        // Get all installed games
        for (u32 i = 0; i < actualAppRecordCnt; i++) {
            try {
                titles.insert({ appRecords[i].titleID, std::make_unique<Title>(appRecords[i].titleID, true) });
            } catch (std::exception e) {
                continue;
            }                 
        }

        for (FsSaveDataInfo saveDataInfo : getTitleSaveFileData()) {
            titleid_t &titleID = saveDataInfo.titleID;

            // Add titles that are not installed but still have a save file on the system
            if (titles.find(titleID) == titles.end()) {
                try {
                    titles.insert({ titleID, std::make_unique<Title>(titleID, false) });
                } catch (std::exception e) {
                    continue;
                }
            }

            // Add what userIDs of all accounts that have a save file for this title
            titles[titleID]->addUser(saveDataInfo.userID);
        }

        delete[] appRecords;

        return titles;
    }

    std::map<userid_t, std::unique_ptr<Account>>& SaveFileSystem::getAllAccounts() {
        static std::map<userid_t, std::unique_ptr<Account>> accounts;

        if (accounts.size() > 0)
            return accounts;   

        s32 userCount;
        size_t actualUserCount;
        if (EResult(accountGetUserCount(&userCount)).failed())
            return accounts;    // Return empty accounts map on error

        userid_t userIDs[userCount];
        if (EResult(accountListAllUsers(userIDs, userCount, &actualUserCount)).failed())
            return accounts;    // Return empty accounts map on error

        // Get all existing accounts
        for (userid_t userID : userIDs) {
            try {
                accounts.insert({userID, std::make_unique<Account>(userID, true) });
            } catch (std::exception e) {
                continue;
            }
        }

        for (FsSaveDataInfo saveDataInfo : getTitleSaveFileData()) {
            userid_t &userID = saveDataInfo.userID;

            // Add accounts that don't exist on the system anymore but still have some save files stored
            if (accounts.find(userID) == accounts.end()) {
                try {
                    accounts.insert({ userID, std::make_unique<Account>(userID, false) });
                } catch (std::exception e) {
                    continue;
                }
            }
        }

        return accounts;
    }

    edz::hlp::Folder SaveFileSystem::getSaveFolder() {
        return edz::hlp::Folder(SAVE_DEVICE"_" + std::to_string(this->m_openFileSystemID) + ":");
    }

    void SaveFileSystem::commit() {
        if (!this->m_initialized) return;

        fsdevCommitDevice((SAVE_DEVICE"_" + std::to_string(this->m_openFileSystemID)).c_str());
    }


    std::vector<FsSaveDataInfo> SaveFileSystem::getTitleSaveFileData() {
        FsSaveDataIterator iter;
        FsSaveDataInfo saveDataInfo;
        size_t totalSaveEntries = 0;
        std::vector<FsSaveDataInfo> saveDataInfos;

        if (R_FAILED(fsOpenSaveDataIterator(&iter, FsSaveDataSpaceId_NandUser)))
            return saveDataInfos;   // Return empty saveDataInfos array
        
        EResult res;
        do {
            res = fsSaveDataIteratorRead(&iter, &saveDataInfo, 1, &totalSaveEntries);

            if (saveDataInfo.saveDataType == FsSaveDataType_SaveData)
                saveDataInfos.push_back(saveDataInfo);
        } while(res.succeeded() && totalSaveEntries > 0);

        fsSaveDataIteratorClose(&iter);

        return saveDataInfos;
    }

}