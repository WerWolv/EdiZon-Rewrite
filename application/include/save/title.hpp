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

        void addUser(userid_t userID);
        void setHasCommonSaveFile();

        titleid_t getID();
        std::string getIDString();

        std::string getName();
        std::string getAuthor();
        std::string getVersionString();
        u32 getVersion();

        bool isInstalled();
        bool hasSaveFile();
        bool hasSaveFile(std::unique_ptr<Account> &account);
        bool hasCommonSaveFile();

        bool isRunning();
        static titleid_t getRunningTitleID();
        static processid_t getRunningProcessID();
        static std::unique_ptr<Title>& getRunningTitle();
        
        static EResult getLastTitleForgroundImage(u8 *buffer);

        std::vector<u8>& getIcon();

        std::vector<userid_t> getUserIDs();

        EResult createSaveDataFileSystem(std::unique_ptr<Account> &account, FsSaveDataType saveDataType);
        void launch();

        time_t getPlayTime(std::unique_ptr<Account> &account);
        time_t getFirstPlayTime(std::unique_ptr<Account> &account);
        time_t getLastPlayTime(std::unique_ptr<Account> &account);
        u32 getLaunchCount(std::unique_ptr<Account> &account);

    private:
        titleid_t m_titleID;
        bool m_isInstalled;
        std::vector<userid_t> m_userIDs;
        std::map<userid_t, PdmPlayStatistics> m_playStatistics;
        bool m_hasCommonSaveFile;

        NacpStruct m_nacp;
        std::string m_titleName, m_titleAuthor, m_versionString;
        u32 m_version;
        std::vector<u8> m_icon;

        EResult _fsCreateSaveDataFileSystem(const FsSaveDataAttribute* save, const FsSaveDataCreationInfo* create);
    };
}