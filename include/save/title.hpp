#pragma once

#include "edizon.hpp"

#include <vector>
#include <string>

namespace edz::save {
    class Title {
    public:
        Title(u64 titleID);
        ~Title();

        void addUserID(u128 userID);

        u64 getTitleID();
        std::string getTitleName();
        std::string getVersionString();
        u32 getVersion();

        void getTitleIcon(u8 *buffer, size_t bufferSize);
        s32 getTitleIconSize();

        std::vector<u128> getUserIDs();

    private:
        u64 m_titleID;
        std::vector<u128> m_userIDs;

        std::string m_titleName, m_titleAuthor, m_versionString;
        u32 m_version;
        u8 *m_titleIcon;
        u32 m_iconSize;
        s32 m_iconWidth, m_iconHeight;
    };
}