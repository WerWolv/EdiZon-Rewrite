#include "save/save_data.hpp"

namespace edz::save {

    SaveFileSystem::SaveFileSystem(Title *title, Account *account) {

        if (R_FAILED(fsMount_SaveData(&m_saveFileSystem, title->getTitleID(), account->getUserID())))
            return;

        if (fsdevMountDevice(SAVE_DEVICE, m_saveFileSystem) == -1) {
            fsFsClose(&m_saveFileSystem);
            return;
        }

        m_initialized = true;
    }

    SaveFileSystem::~SaveFileSystem() {
        if (!m_initialized) return;

        fsdevUnmountDevice(SAVE_DEVICE);
        fsFsClose(&m_saveFileSystem);
    }

    std::map<u64, Title*>& SaveFileSystem::getAllTitles() {
        static std::map<u64, Title*> titles;

        if (titles.size() > 0)
            return titles;

        for (FsSaveDataInfo saveDataInfo : getTitleSaveFileData()) {
            u64 &titleID = saveDataInfo.titleID;

            if (titles.find(titleID) == titles.end()) {
                titles.insert({ (u64)titleID, new Title(titleID) });
            }

            titles[titleID]->addUserID(saveDataInfo.userID);

        }

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

        for (u128 userID : userIDs)
            accounts.insert({userID, new Account(userID)});

        return accounts;
    }

    edz::helper::Folder SaveFileSystem::getSaveFolder() {
        return edz::helper::Folder(SAVE_DEVICE ":");
    }

    void SaveFileSystem::commit() {
        fsdevCommitDevice(SAVE_DEVICE);
    }


    std::vector<FsSaveDataInfo> SaveFileSystem::getTitleSaveFileData() {
        FsSaveDataIterator iter;
        FsSaveDataInfo saveDataInfo;
        size_t totalSaveEntries = 0;
        std::vector<FsSaveDataInfo> saveDataInfos;

        if (R_FAILED(fsOpenSaveDataIterator(&iter, FsSaveDataSpaceId_NandUser)))
            return saveDataInfos;
        
        Result rc;
        do {
            rc = fsSaveDataIteratorRead(&iter, &saveDataInfo, 1, &totalSaveEntries);

            if (saveDataInfo.SaveDataType == FsSaveDataType_SaveData)
                saveDataInfos.push_back(saveDataInfo);
        } while(R_SUCCEEDED(rc) && totalSaveEntries > 0);

        fsSaveDataIteratorClose(&iter);

        return saveDataInfos;
    }

}