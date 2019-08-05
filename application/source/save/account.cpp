#include "save/account.hpp"

#include <cstring>
#include <sstream>
#include <iomanip>

namespace edz::save {

        Account::Account(userid_t userID, bool exists) : m_userID(userID), m_exists(exists) {
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

            if (EResult(accountGetProfile(&profile, userID)).failed())
                throw std::exception();
            
            if (EResult(accountProfileGet(&profile, &userData, &profileBase)).failed())
                throw std::exception();

            if (EResult(accountProfileGetImageSize(&profile, &this->m_userIconSize)).failed())
                throw std::exception();


            this->m_nickname = std::string(profileBase.username);

            this->m_userIcon = new u8[this->m_userIconSize];
            if (EResult(accountProfileLoadImage(&profile, this->m_userIcon, this->m_userIconSize, &this->m_userIconSize)).failed()) {
                delete[] this->m_userIcon;
                throw std::exception();
            }

            accountProfileClose(&profile);
        }

        Account::~Account() {
            delete[] this->m_userIcon;
        }


        userid_t Account::getID() {
            return this->m_userID;
        }

        std::string Account::getIDString() {
            std::stringstream sstream;
            sstream << std::uppercase << std::setfill('0') << std::setw(sizeof(userid_t) * 2) << std::hex << static_cast<u64>(getID() >> 64) << static_cast<u64>(getID() & 0xFFFFFFFFFFFFFFFF);

            return sstream.str();
        }

        std::string Account::getNickname() {
            return this->m_nickname;
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