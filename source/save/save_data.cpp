#include "save/save_data.hpp"

namespace edz::save {

    SaveFileSystem::SaveFileSystem(Title *title, Account *account) {

        if (R_FAILED(fsMount_SaveData(&this->m_saveFileSystem, title->getID(), account->getID())))
            return;

        if (fsdevMountDevice(SAVE_DEVICE, this->m_saveFileSystem) == -1) {
            fsFsClose(&this->m_saveFileSystem);
            return;
        }

        this->m_initialized = true;
    }

    SaveFileSystem::~SaveFileSystem() {
        if (!this->m_initialized) return;

        fsdevUnmountDevice(SAVE_DEVICE);
        fsFsClose(&this->m_saveFileSystem);
    }

    std::map<u64, Title*>& SaveFileSystem::getAllTitles() {
        static std::map<u64, Title*> titles;

        if (titles.size() > 0)
            return titles;

        NsApplicationRecord *appRecords = new NsApplicationRecord[1024]; // Nobody's going to have more than 1024 games hopefully...
        size_t actualAppRecordCnt = 0;

        EResult rc = nsListApplicationRecord(appRecords, 1024, 0, &actualAppRecordCnt);
        if (rc.failed()) {
            delete[] appRecords;
            return titles;  // Return empty titles map on error
        }
        

        // Get all installed games
        for (u32 i = 0; i < actualAppRecordCnt; i++)
            titles.insert({ appRecords[i].titleID, new Title(appRecords[i].titleID, true) });

        for (FsSaveDataInfo saveDataInfo : getTitleSaveFileData()) {
            u64 &titleID = saveDataInfo.titleID;

            // Add titles that are not installed but still have a save file on the system
            if (titles.find(titleID) == titles.end())
                titles.insert({ titleID, new Title(titleID, false) });

            // Add what userIDs of all accounts that have a save file for this title
            titles[titleID]->addUser(saveDataInfo.userID);
        }

        delete[] appRecords;

        return titles;
    }

    std::map<u128, Account*>& SaveFileSystem::getAllAccounts() {
        static std::map<u128, Account*> accounts;

        if (accounts.size() > 0)
            return accounts;   

        s32 userCount;
        size_t actualUserCount;
        if (R_FAILED(accountGetUserCount(&userCount)))
            return accounts;    // Return empty accounts map on error

        u128 userIDs[userCount];
        if (R_FAILED(accountListAllUsers(userIDs, userCount, &actualUserCount)))
            return accounts;    // Return empty accounts map on error

        // Get all existing accounts
        for (u128 userID : userIDs)
            accounts.insert({userID, new Account(userID, true)});

        for (FsSaveDataInfo saveDataInfo : getTitleSaveFileData()) {
            u128 &userID = saveDataInfo.userID;

            // Add accounts that don't exist on the system anymore but still have some save files stored
            if (accounts.find(userID) == accounts.end())
                accounts.insert({ userID, new Account(userID, false) });
        }

        return accounts;
    }

    edz::helper::Folder SaveFileSystem::getSaveFolder() {
        return edz::helper::Folder(SAVE_DEVICE ":");
    }

    void SaveFileSystem::commit() {
        if (!this->m_initialized) return;

        fsdevCommitDevice(SAVE_DEVICE);
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