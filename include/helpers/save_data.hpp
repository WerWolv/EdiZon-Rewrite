#pragma once

#include "edizon.hpp"

#include "folder.hpp"
#include "account.hpp"
#include "title.hpp"

#include <map>

#define SAVE_DEVICE "save:/"

namespace edz::helper {

    class SaveFileSystem {
    public:
        SaveFileSystem(Title title, Account account);
        ~SaveFileSystem();

        static std::map<u64, Title*> getAllTitles();
        static std::map<u128, Account*> getAllAccounts();

        Folder getSaveFolder();
        void commit();

    private:
        static std::vector<FsSaveDataInfo> getTitleSaveFileData();

        FsFileSystem m_saveFileSystem;
        bool m_initialized = false;
    };

}