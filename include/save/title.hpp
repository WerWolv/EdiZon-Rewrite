#pragma once

#include "edizon.hpp"

#include "account.hpp"

#include <vector>
#include <string>

namespace edz::save {
    class Title {
    public:
        Title(titleid_t titleID, bool isInstalled);
        ~Title();

        void addUser(userid_t userID);

        titleid_t getID();
        std::string getName();
        std::string getVersionString();
        u32 getVersion();

        bool isInstalled();
        bool hasSaveFile();

        void getIcon(u8 *buffer, size_t bufferSize);
        size_t getIconSize();

        std::vector<userid_t> getUserIDs();

        EResult createSaveDataFileSystem(Account *account, u64 fileSystemSize);


    private:
        titleid_t m_titleID;
        bool m_isInstalled;
        std::vector<userid_t> m_userIDs;

        std::string m_titleName, m_titleAuthor, m_versionString;
        u32 m_version;
        u8 *m_titleIcon;
        size_t m_iconSize;

        EResult _fsCreateSaveDataFileSystem(const FsSave* save, const FsSaveCreate* create);
    };
}