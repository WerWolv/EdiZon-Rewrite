#pragma once

#include "edizon.hpp"

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

        static std::map<titleid_t, Title*>& getAllTitles();
        static std::map<userid_t, Account*>& getAllAccounts();

        edz::helper::Folder getSaveFolder();
        void commit();

    private:
        static std::vector<FsSaveDataInfo> getTitleSaveFileData();

        u16 m_openFileSystemID;

        FsFileSystem m_saveFileSystem;
        bool m_initialized = false;
    };

}