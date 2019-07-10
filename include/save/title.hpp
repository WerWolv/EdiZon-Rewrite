#pragma once

#include "edizon.hpp"

#include "account.hpp"

#include <vector>
#include <string>

namespace edz::save {
    class Title {
    public:
        Title(u64 titleID, bool isInstalled);
        ~Title();

        void addUser(u128 userID);

        u64 getID();
        std::string getName();
        std::string getVersionString();
        u32 getVersion();

        bool isInstalled();
        bool hasSaveFile();

        void getIcon(u8 *buffer, size_t bufferSize);
        size_t getIconSize();

        std::vector<u128> getUserIDs();

        EResult createSaveDataFileSystem(Account *account, u64 fileSystemSize);


    private:
        u64 m_titleID;
        bool m_isInstalled;
        std::vector<u128> m_userIDs;

        std::string m_titleName, m_titleAuthor, m_versionString;
        u32 m_version;
        u8 *m_titleIcon;
        size_t m_iconSize;

        EResult _fsCreateSaveDataFileSystem(const FsSave* save, const FsSaveCreate* create);
    };
}