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

#include "save/save_data.hpp"
#include "helpers/utils.hpp"

namespace edz::save {

    SaveFileSystem::SaveFileSystem(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account) {
        static u16 usedIDs = 0;

        this->m_openFileSystemID = usedIDs++;
        if (EResult(fsdevMountSaveData((SAVE_DEVICE"_" + std::to_string(this->m_openFileSystemID)).c_str(), title->getID(), hlp::userIDToAccountUid(account->getID()))).failed())
            return;

        this->m_initialized = true;
    }

    SaveFileSystem::SaveFileSystem(std::unique_ptr<Title> &title) {
        static u16 usedIDs = 0;
        FsFileSystem fs;
        FsSaveDataAttribute attr;

        this->m_openFileSystemID = usedIDs++;
        
        memset(&attr, 0, sizeof(attr));
        attr.application_id = title->getID();
        attr.uid = hlp::userIDToAccountUid(0);
        attr.save_data_type = FsSaveDataType_Device;

        if (EResult(fsOpenSaveDataFileSystem(&fs, FsSaveDataSpaceId_User, &attr)).failed())
            return;
        
        if (EResult(fsdevMountDevice((SAVE_DEVICE"_" + std::to_string(this->m_openFileSystemID)).c_str(), fs)).failed())
            return;

        this->m_initialized = true;
    }

    SaveFileSystem::~SaveFileSystem() {
        if (!this->m_initialized) return;

        fsdevUnmountDevice((SAVE_DEVICE"_" + std::to_string(this->m_openFileSystemID)).c_str());
    }

    std::map<titleid_t, std::unique_ptr<Title>>& SaveFileSystem::getAllTitles() {
        static std::map<titleid_t, std::unique_ptr<Title>> titles;

        if (titles.size() > 0)
            return titles;

        NsApplicationRecord *appRecords = new NsApplicationRecord[1024]; // Nobody's going to have more than 1024 games hopefully...
        s32 actualAppRecordCnt = 0;

        if (EResult(nsListApplicationRecord(appRecords, 1024, 0, &actualAppRecordCnt)).failed()) {
            delete[] appRecords;
            return titles;  // Return empty titles map on error
        }
        
        
        // Get all installed games
        for (s32 i = 0; i < actualAppRecordCnt; i++) {
            try {
                titles.insert({ appRecords[i].application_id, std::make_unique<Title>(appRecords[i].application_id, true) });
            } catch (std::runtime_error& e) {
                continue;
            }                 
        }

        auto saveFileData = getTitleSaveFileData();
        for (FsSaveDataInfo saveDataInfo : saveFileData) {
            titleid_t &titleID = saveDataInfo.application_id;

            // Add titles that are not installed but still have a save file on the system
            if (saveFileData.size() < MAX_TITLE_FROM_SAVE_DATA && titles.find(titleID) == titles.end()) {
                try {
                    titles.insert({ titleID, std::make_unique<Title>(titleID, false) });
                } catch (std::runtime_error& e) {
                    Logger::error("Error with title %016lx", titleID);
                    continue;
                }
            }

            if (titles.find(titleID) == titles.end())
                continue;
            // Add userIDs of all accounts that have a save file for this title
            titles[titleID]->addUser(hlp::accountUidToUserID(saveDataInfo.uid));
        }

        for (FsSaveDataInfo saveDataInfo : getCommonSaveFileData()) {
            titleid_t &titleID = saveDataInfo.application_id;

            // Add titles that are not installed but still have a save file on the system
            if (saveFileData.size() < MAX_TITLE_FROM_SAVE_DATA && titles.find(titleID) == titles.end()) {
                try {
                    titles.insert({ titleID, std::make_unique<Title>(titleID, false) });
                } catch (std::runtime_error& e) {
                    Logger::error("Error with title %016lx", titleID);
                    continue;
                }
            }

            if (titles.find(titleID) == titles.end())
                continue;

            // Specify that this title has a common/device save file (Save file shared between users)
            titles[titleID]->setHasCommonSaveFile();
        }

        delete[] appRecords;

        return titles;
    }

    std::map<userid_t, std::unique_ptr<Account>>& SaveFileSystem::getAllAccounts() {
        static std::map<userid_t, std::unique_ptr<Account>> accounts;

        if (accounts.size() > 0)
            return accounts;   

        s32 userCount;
        s32 actualUserCount;
        if (EResult(accountGetUserCount(&userCount)).failed())
            return accounts;    // Return empty accounts map on error

        AccountUid userIDs[userCount];
        if (EResult(accountListAllUsers(userIDs, userCount, &actualUserCount)).failed())
            return accounts;    // Return empty accounts map on error

        // Get all existing accounts
        for (AccountUid userID : userIDs) {
            try {
                accounts.insert({ hlp::accountUidToUserID(userID), std::make_unique<Account>(hlp::accountUidToUserID(userID), true) });
            } catch (std::exception& e) {
                continue;
            }
        }

        for (FsSaveDataInfo saveDataInfo : getTitleSaveFileData()) {
            userid_t userID = hlp::accountUidToUserID(saveDataInfo.uid);

            // Add accounts that don't exist on the system anymore but still have some save files stored
            if (accounts.find(userID) == accounts.end()) {
                try {
                    accounts.insert({ userID, std::make_unique<Account>(userID, false) });
                } catch (std::exception& e) {
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
        FsSaveDataInfoReader reader;
        FsSaveDataInfo saveDataInfo;
        s64 totalSaveEntries = 0;
        std::vector<FsSaveDataInfo> saveDataInfos;

        if (EResult(fsOpenSaveDataInfoReader(&reader, FsSaveDataSpaceId_User)).failed())
            return EMPTY_RESPONSE;
        
        EResult res;
        do {
            res = fsSaveDataInfoReaderRead(&reader, &saveDataInfo, 1, &totalSaveEntries);

            if (saveDataInfo.save_data_type == FsSaveDataType_Account)
                saveDataInfos.push_back(saveDataInfo);
        } while(res.succeeded() && totalSaveEntries > 0);

        fsSaveDataInfoReaderClose(&reader);

        return saveDataInfos;
    }

    std::vector<FsSaveDataInfo> SaveFileSystem::getCommonSaveFileData() {
        FsSaveDataInfoReader reader;
        FsSaveDataInfo saveDataInfo;
        s64 totalSaveEntries = 0;
        std::vector<FsSaveDataInfo> saveDataInfos;

        if (EResult(fsOpenSaveDataInfoReader(&reader, FsSaveDataSpaceId_User)).failed())
            return EMPTY_RESPONSE;
        
        EResult res;
        do {
            res = fsSaveDataInfoReaderRead(&reader, &saveDataInfo, 1, &totalSaveEntries);

            if (saveDataInfo.save_data_type == FsSaveDataType_Device)
                saveDataInfos.push_back(saveDataInfo);
        } while(res.succeeded() && totalSaveEntries > 0);

        fsSaveDataInfoReaderClose(&reader);

        return saveDataInfos;
    }

}