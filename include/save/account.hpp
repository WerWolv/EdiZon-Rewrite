#pragma once

#include "edizon.hpp"

#include <string>

namespace edz::save {

    class Account {
    public:
        Account(u128 userID, bool exists);
        ~Account();

        std::string getNickname();
        u128 getID();
        void getIcon(u8 *buffer, size_t bufferSize);
        size_t getIconSize();

        bool exists();

    private:
        u128 m_userID;
        bool m_exists;

        std::string m_nickname;
        u8 *m_userIcon;
        size_t m_userIconSize;
    };

}