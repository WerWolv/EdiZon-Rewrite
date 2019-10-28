/**
 * Copyright (C) 2019 WerWolv
 * 
 * This file is part of EdiZon.
 * 
 * EdiZon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * EdiZon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EdiZon.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "save/account.hpp"

#include <cstring>
#include <sstream>
#include <iomanip>

#include "helpers/utils.hpp"

namespace edz::save {

        Account::Account(userid_t userID, bool exists) : m_userID(userID), m_exists(exists) {
            AccountProfile profile;
            AccountUserData userData;
            AccountProfileBase profileBase;

            // If account doesn't existing anymore, use default properties
            if (!exists) {
                this->m_nickname = "< Unknown User >";

                return;
            }

            if (EResult(accountGetProfile(&profile, hlp::userIDToAccountUid(userID))).failed())
                throw std::exception();
            
            if (EResult(accountProfileGet(&profile, &userData, &profileBase)).failed())
                throw std::exception();

            u32 iconSize = 0;
            if (EResult(accountProfileGetImageSize(&profile, &iconSize)).failed())
                throw std::exception();


            this->m_nickname = std::string(profileBase.nickname);

            this->m_icon.resize(iconSize);
            if (EResult(accountProfileLoadImage(&profile, &this->m_icon[0], iconSize, &iconSize)).failed()) {
                throw std::exception();
            }

            accountProfileClose(&profile);
        }


        userid_t Account::getID() {
            return this->m_userID;
        }

        std::string Account::getIDString() {
            std::stringstream sstream;
            sstream << std::uppercase << std::hex << static_cast<u64>(getID() >> 64) << static_cast<u64>(getID() & 0xFFFFFFFFFFFFFFFF);

            return sstream.str();
        }

        std::string Account::getNickname() {
            return this->m_nickname;
        }

        std::vector<u8>& Account::getIcon() {
            return this->m_icon;
        }

        bool Account::exists() {
            return this->m_exists;
        }

}