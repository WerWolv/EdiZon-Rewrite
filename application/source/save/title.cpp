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

namespace edz::save {

    Title::Title(titleid_t titleID, bool isInstalled) : m_titleID(titleID), m_isInstalled(isInstalled) {
        NsApplicationControlData appControlData;
        size_t appControlDataSize = 0;
        NsApplicationContentMetaStatus appContentMetaStatus;
        NacpLanguageEntry *languageEntry = nullptr;

        std::memset(&appControlData, 0x00, sizeof(NsApplicationControlData));

        if (EResult(nsGetApplicationControlData(1, titleID, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize)).failed())
            throw std::exception();
            
        if (EResult(nsListApplicationContentMetaStatus(titleID, 0, &appContentMetaStatus, sizeof(NsApplicationContentMetaStatus), nullptr)))
            throw std::exception();

        if (EResult(nacpGetLanguageEntry(&appControlData.nacp, &languageEntry)))
            throw std::exception();

        this->m_titleName = std::string(languageEntry->name);
        this->m_titleAuthor = std::string(languageEntry->author);
        this->m_versionString = std::string(appControlData.nacp.version);
        this->m_version = appContentMetaStatus.title_version;

        this->m_iconSize = appControlDataSize - sizeof(NsApplicationControlData::nacp);
        this->m_titleIcon = new u8[this->m_iconSize];
        std::memcpy(this->m_titleIcon, appControlData.icon, this->m_iconSize);
    }

    Title::~Title() {
        delete[] this->m_titleIcon;
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


    void Title::getIcon(u8 *buffer, size_t size) {
        std::memcpy(buffer, this->m_titleIcon, this->m_iconSize);
    }

    size_t Title::getIconSize() {
        return this->m_iconSize;
    }

    std::vector<userid_t> Title::getUserIDs() {
        return this->m_userIDs;
    }

    void Title::addUser(userid_t userID) {
        this->m_userIDs.push_back(userID);
    }


    EResult Title::createSaveDataFileSystem(Account *account, u64 fileSystemSize) {
        FsSave save;
        save.titleID = this->getID();
        save.userID = account->getID();
        save.saveID = 0;
        save.rank = 0;
        save.index = 0;

        FsSaveCreate saveCreate;
        saveCreate.size = fileSystemSize;
        saveCreate.journalSize = fileSystemSize;
        saveCreate.blockSize = fileSystemSize;
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
    
}