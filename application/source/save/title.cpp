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

#include "save/title.hpp"

#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#include "save/save_data.hpp"
#include "helpers/utils.hpp"

namespace edz::save {

    Title::Title(titleid_t titleID, bool isInstalled) : m_titleID(titleID), m_isInstalled(isInstalled) {
        NsApplicationControlData appControlData;
        size_t appControlDataSize = 0;
        NacpLanguageEntry *languageEntry = nullptr;

        std::memset(&appControlData, 0x00, sizeof(NsApplicationControlData));
        
        if (EResult(nsGetApplicationControlData(NsApplicationControlSource_Storage, titleID, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize)).failed())
            throw std::runtime_error("Failed to load language data from title");
            
        if (EResult(nacpGetLanguageEntry(&appControlData.nacp, &languageEntry)))
            throw std::runtime_error("Failed to load icon data from title");

        this->m_titleName = std::string(languageEntry->name);
        this->m_titleAuthor = std::string(languageEntry->author);
        this->m_versionString = std::string(appControlData.nacp.display_version);
        
        std::memcpy(&this->m_nacp, &appControlData.nacp, sizeof(NacpStruct));

        size_t iconSize = appControlDataSize - sizeof(NsApplicationControlData::nacp);
        this->m_icon.reserve(iconSize);
        std::copy(appControlData.icon, appControlData.icon + iconSize, std::back_inserter(this->m_icon));

        for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
            PdmPlayStatistics playStatistics = { 0 };

            pdmqryQueryPlayStatisticsByApplicationIdAndUserAccountId(this->getID(), hlp::userIDToAccountUid(account->getID()), &playStatistics);
            this->m_playStatistics.insert({ userid, playStatistics });
        }
    }


    titleid_t Title::getID() {
        return this->m_titleID;
    }

    std::string Title::getIDString() {
        std::stringstream sstream;
        sstream << std::uppercase << std::setfill('0') << std::setw(sizeof(titleid_t) * 2) << std::hex << getID();

        return sstream.str();
    }

    std::string Title::getName() {
        return this->m_titleName;
    }

    std::string Title::getAuthor() {
        return this->m_titleAuthor;
    }

    std::string Title::getVersionString() {
        return this->m_versionString;
    }

    u32 Title::getVersion() {
        return this->m_version;
    }


    bool Title::isInstalled() {
        return this->m_isInstalled;
    }

    bool Title::hasSaveFile() {
        return this->getUserIDs().size() > 0 || this->m_hasCommonSaveFile;
    }

    bool Title::hasCommonSaveFile() {
        return this->m_hasCommonSaveFile;
    }

    bool Title::hasSaveFile(std::unique_ptr<Account> &account) {
        if (!hasSaveFile())
            return false;

        for (userid_t userID : getUserIDs())
            if (account->getID() == userID)
                return true;
        
        return false;
    }


    bool Title::isRunning() {
        static titleid_t runningTitleID = 0;

        // Running title needs to be loaded only once as it can't ever change without the user quiting edizon
        if (runningTitleID == 0) {
            u64 pid = 0;
            
            pmdmntGetApplicationProcessId(&pid);
            pminfoGetProgramId(&runningTitleID, pid);
        }

        return this->getID() == runningTitleID;
    }


    titleid_t Title::getRunningTitleID() {
        static titleid_t runningTitleID = 0;

        // Running title needs to be loaded only once as it can't ever change without the user quiting edizon
        if (runningTitleID == 0) {
            processid_t pid = Title::getRunningProcessID();
            pminfoGetProgramId(&runningTitleID, pid);
        }

        return runningTitleID;
    }

    processid_t Title::getRunningProcessID() {
        static processid_t runningProcessId = 0;

        // Running title needs to be loaded only once as it can't ever change without the user quiting edizon
        if (runningProcessId == 0)
            pmdmntGetApplicationProcessId(&runningProcessId);
        

        return runningProcessId;
    }

    std::unique_ptr<Title>& Title::getRunningTitle() {
        return SaveFileSystem::getAllTitles()[Title::getRunningTitleID()];
    }


    EResult Title::getLastTitleForgroundImage(u8 *buffer) {
        bool flag = false;

        ER_TRY(appletUpdateLastForegroundCaptureImage());
        ER_TRY(appletGetLastApplicationCaptureImageEx(buffer, 1280 * 720 * 4, &flag));

        return ResultSuccess;
    }


    std::vector<u8>& Title::getIcon() {
        return this->m_icon;
    }

    std::vector<userid_t> Title::getUserIDs() {
        return this->m_userIDs;
    }

    void Title::addUser(userid_t userID) {
        this->m_userIDs.push_back(userID);
    }

    void Title::setHasCommonSaveFile() {
        this->m_hasCommonSaveFile = true;
    }


    EResult Title::createSaveDataFileSystem(std::unique_ptr<Account> &account, FsSaveDataType saveDataType) {
        FsSaveDataAttribute save;
        FsSaveDataCreationInfo saveCreate;

        save.application_id = this->getID();
        save.uid = hlp::userIDToAccountUid(account->getID());
        save.system_save_data_id = 0;
        save.save_data_rank = 0;
        save.save_data_index = 0;
        save.save_data_type = saveDataType;

        if (saveDataType == FsSaveDataType_Device) {
            saveCreate.available_size = this->m_nacp.device_save_data_size;
            saveCreate.journal_size = this->m_nacp.device_save_data_journal_size;
        } else {
            saveCreate.available_size = this->m_nacp.user_account_save_data_size;
            saveCreate.journal_size = this->m_nacp.user_account_save_data_journal_size;
        }
        saveCreate.save_data_size = 0;
        saveCreate.owner_id = this->getID();
        saveCreate.flags = 0;
        saveCreate.save_data_space_id = FsSaveDataSpaceId_User;

        if (EResult res = _fsCreateSaveDataFileSystem(&save, &saveCreate); res.failed())
            return res;

        this->addUser(account->getID());

        return ResultSuccess;
    }

    void Title::launch() {
        appletRequestLaunchApplication(this->getID(), nullptr);
    }


    EResult Title::_fsCreateSaveDataFileSystem(const FsSaveDataAttribute* save, const FsSaveDataCreationInfo* create) {
        const struct {
            FsSaveDataAttribute save;
            FsSaveDataCreationInfo create;
            struct { u32 unk_x0; u8 unk_x4; u8 pad[0xB]; } data;
        } in = { *save, *create, { 0x40060, 0x01, 0} };

        return serviceDispatchIn(fsGetServiceSession(), 22, in);
    }

    time_t Title::getPlayTime(std::unique_ptr<Account> &account) {
        if (std::find(this->m_userIDs.begin(), this->m_userIDs.end(), account->getID()) == this->m_userIDs.end())
            return 0;
            
        return this->m_playStatistics[account->getID()].playtimeMinutes * 60;
    }

    time_t Title::getFirstPlayTime(std::unique_ptr<Account> &account) {
        return pdmPlayTimestampToPosix(this->m_playStatistics[account->getID()].first_timestampUser);
    }

    time_t Title::getLastPlayTime(std::unique_ptr<Account> &account) {
        return pdmPlayTimestampToPosix(this->m_playStatistics[account->getID()].last_timestampUser);
    }

    u32 Title::getLaunchCount(std::unique_ptr<Account> &account) {
        return this->m_playStatistics[account->getID()].totalLaunches;
    }
    
}