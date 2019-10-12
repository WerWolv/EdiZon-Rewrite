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

#pragma once

#include <edizon.hpp>

#include "save/account.hpp"

#include <vector>
#include <string>

namespace edz::save {
    class Title {
    public:
        Title(titleid_t titleID, bool isInstalled);
        ~Title();

        void addUser(userid_t userID);

        titleid_t getID();
        std::string getIDString();

        std::string getName();
        std::string getAuthor();
        std::string getVersionString();
        u32 getVersion();

        bool isInstalled();
        bool hasSaveFile();
        bool hasSaveFile(Account *account);

        bool isRunning();
        static titleid_t getRunningTitleID();
        static processid_t getRunningProcessID();
        static EResult getLastTitleForgroundImage(u8 *buffer);

        void getIcon(u8 *buffer, size_t size);
        size_t getIconSize();

        std::vector<userid_t> getUserIDs();

        EResult createSaveDataFileSystem(Account *account);

        time_t getPlayTime(Account *account);
        time_t getFirstPlayTime(Account *account);
        time_t getLastPlayTime(Account *account);
        u32 getLaunchCount(Account *account);

    private:
        titleid_t m_titleID;
        bool m_isInstalled;
        std::vector<userid_t> m_userIDs;

        NacpStruct m_nacp;
        std::string m_titleName, m_titleAuthor, m_versionString;
        u32 m_version;
        u8 *m_titleIcon;
        size_t m_iconSize;

        EResult _fsCreateSaveDataFileSystem(const FsSave* save, const FsSaveCreate* create);
    };
}