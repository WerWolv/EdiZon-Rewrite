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

namespace edz::save {

    Title::Title(titleid_t titleID, bool isInstalled) : m_titleID(titleID), m_isInstalled(isInstalled) {
        NsApplicationControlData appControlData;
        size_t appControlDataSize = 0;
        NacpLanguageEntry *languageEntry = nullptr;

        std::memset(&appControlData, 0x00, sizeof(NsApplicationControlData));
        
        if (EResult(nsGetApplicationControlData(1, titleID, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize)).failed())
            throw std::runtime_error("Failed to load language data from title");
            
        if (EResult(nacpGetLanguageEntry(&appControlData.nacp, &languageEntry)))
            throw std::runtime_error("Failed to load icon data from title");

        this->m_titleName = std::string(languageEntry->name);
        this->m_titleAuthor = std::string(languageEntry->author);
        this->m_versionString = std::string(appControlData.nacp.version);
        
        std::memcpy(&this->m_nacp, &appControlData.nacp, sizeof(NacpStruct));

        size_t iconSize = appControlDataSize - sizeof(NsApplicationControlData::nacp);
        this->m_icon.reserve(iconSize);
        std::copy(appControlData.icon, appControlData.icon + iconSize, std::back_inserter(this->m_icon));

        for (auto &[userid, account] : save::SaveFileSystem::getAllAccounts()) {
            PdmPlayStatistics playStatistics = { 0 };

            pdmqryQueryPlayStatisticsByApplicationIdAndUserAccountId(this->getID(), account->getID(), &playStatistics);
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
        return this->getUserIDs().size() > 0;
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

            pmdmntGetApplicationPid(&pid);
            pminfoGetTitleId(&runningTitleID, pid);
        }

        return this->getID() == runningTitleID;
    }

    titleid_t Title::getRunningTitleID() {
        static titleid_t runningTitleID = 0;

        // Running title needs to be loaded only once as it can't ever change without the user quiting edizon
        if (runningTitleID == 0) {
            processid_t pid = Title::getRunningProcessID();
            pminfoGetTitleId(&runningTitleID, pid);
        }

        return runningTitleID;
    }

    processid_t Title::getRunningProcessID() {
        static processid_t runningProcessId = 0;

        // Running title needs to be loaded only once as it can't ever change without the user quiting edizon
        if (runningProcessId == 0)
            pmdmntGetApplicationPid(&runningProcessId);
        

        return runningProcessId;
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


    EResult Title::createSaveDataFileSystem(std::unique_ptr<Account> &account) {
        FsSave save;
        save.titleID = this->getID();
        save.userID = account->getID();
        save.saveID = 0;
        save.rank = 0;
        save.index = 0;

        FsSaveCreate saveCreate;
        saveCreate.size = this->m_nacp.userAccountSaveDataSize;
        saveCreate.journalSize = this->m_nacp.userAccountSaveDataJournalSize;
        saveCreate.blockSize = 0x4000;
        saveCreate.ownerId = this->getID();
        saveCreate.flags = 0;
        saveCreate.saveDataSpaceId = FsSaveDataSpaceId_NandUser;

        return _fsCreateSaveDataFileSystem(&save, &saveCreate);
    }


    EResult Title::_fsCreateSaveDataFileSystem(const FsSave* save, const FsSaveCreate* create) {
        IpcCommand c;
        Service *fsService = fsGetServiceSession();
        ipcInitialize(&c);
        
        struct {
            u64 magic;
            u64 cmd_id;
            FsSave save;
            FsSaveCreate create;
            struct { u32 unk_x0; u8 unk_x4; u8 pad[0xB]; } data;
        } PACKED *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(fsService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 22;
        memcpy(&raw->save, save, sizeof(FsSave));
        memcpy(&raw->create, create, sizeof(FsSaveCreate));

        raw->data.unk_x0 = 0x40060;
        raw->data.unk_x4 = 0x01;

        Result rc = serviceIpcDispatch(fsService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
            } *resp;

            serviceIpcParse(fsService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
        }

        return rc;
    }

    time_t Title::getPlayTime(std::unique_ptr<Account> &account) {
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