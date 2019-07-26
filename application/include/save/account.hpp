#pragma once

#include "edizon.hpp"

#include <string>

namespace edz::save {

    class Account {
    public:
        Account(userid_t userID, bool exists);
        ~Account();

        userid_t getID();
        std::string getIDString();

        std::string getNickname();
        void getIcon(u8 *buffer, size_t bufferSize);
        size_t getIconSize();

        bool exists();

    private:
        userid_t m_userID;
        bool m_exists;

        std::string m_nickname;
        u8 *m_userIcon;
        size_t m_userIconSize;
    };

}