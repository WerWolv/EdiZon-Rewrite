#include "save/account.hpp"

#include <cstring>

namespace edz::save {

        Account::Account(u128 userID, bool exists) : m_userID(userID), m_exists(exists) {
            AccountProfile profile;
            AccountUserData userData;
            AccountProfileBase profileBase;

            // If account doesn't existing anymore, use default properties
            if (!exists) {
                this->m_userIcon = nullptr;
                this->m_userIconSize = 0;
                this->m_nickname = "< Unknown User >";

                return;
            }

            accountGetProfile(&profile, userID);
            accountProfileGet(&profile, &userData, &profileBase);
            accountProfileGetImageSize(&profile, &this->m_userIconSize);

            this->m_nickname = std::string(profileBase.username);

            this->m_userIcon = new u8[this->m_userIconSize];
            accountProfileLoadImage(&profile, this->m_userIcon, this->m_userIconSize, &this->m_userIconSize);

            accountProfileClose(&profile);
        }

        Account::~Account() {
            delete[] this->m_userIcon;
        }


        std::string Account::getNickname() {
            return this->m_nickname;
        }

        u128 Account::getID() {
            return this->m_userID;
        }

        void Account::getIcon(u8 *buffer, size_t bufferSize) {
            std::memcpy(buffer, this->m_userIcon, bufferSize);
        }

        size_t Account::getIconSize() {
            return this->m_userIconSize;
        }


        bool Account::exists() {
            return this->m_exists;
        }

}