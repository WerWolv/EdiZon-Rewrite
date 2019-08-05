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

    std::map<titleid_t, Title*>& SaveFileSystem::getAllTitles() {
        static std::map<titleid_t, Title*> titles;

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
            Title *title = nullptr;

            try {
                title = new Title(appRecords[i].titleID, true);
            } catch (std::exception e) {
                continue;
            } 

            if (title != nullptr)
                titles.insert({ appRecords[i].titleID, title });
        }

        for (FsSaveDataInfo saveDataInfo : getTitleSaveFileData()) {
            titleid_t &titleID = saveDataInfo.titleID;

            // Add titles that are not installed but still have a save file on the system
            if (titles.find(titleID) == titles.end()) {
                Title *title = nullptr;

                try {
                    title = new Title(titleID, false);
                } catch (std::exception e) {
                    continue;
                }

                if (title != nullptr)
                    titles.insert({ titleID, title });
            }

            // Add what userIDs of all accounts that have a save file for this title
            titles[titleID]->addUser(saveDataInfo.userID);
        }

        delete[] appRecords;

        return titles;
    }

    std::map<userid_t, Account*>& SaveFileSystem::getAllAccounts() {
        static std::map<userid_t, Account*> accounts;

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
            Account *account = nullptr;

            try {
                account = new Account(userID, true);
            } catch (std::exception e) {
                continue;
            }

            if (account != nullptr)
                accounts.insert({userID, account});
        }

        for (FsSaveDataInfo saveDataInfo : getTitleSaveFileData()) {
            userid_t &userID = saveDataInfo.userID;

            // Add accounts that don't exist on the system anymore but still have some save files stored
            if (accounts.find(userID) == accounts.end()) {
                Account *account = nullptr;

                try {
                    account = new Account(userID, false);
                } catch (std::exception e) {
                    continue;
                }

                if (account != nullptr)
                    accounts.insert({userID, account});
            }
        }

        return accounts;
    }

    edz::helper::Folder SaveFileSystem::getSaveFolder() {
        return edz::helper::Folder(SAVE_DEVICE"_" + std::to_string(this->m_openFileSystemID) + ":");
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
        
        Result rc;
        do {
            rc = fsSaveDataIteratorRead(&iter, &saveDataInfo, 1, &totalSaveEntries);

            if (saveDataInfo.saveDataType == FsSaveDataType_SaveData)
                saveDataInfos.push_back(saveDataInfo);
        } while(R_SUCCEEDED(rc) && totalSaveEntries > 0);

        fsSaveDataIteratorClose(&iter);

        return saveDataInfos;
    }

}