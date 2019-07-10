#include "save/title.hpp"

#include <cstring>
#include <string>

namespace edz::save {

    Title::Title(u64 titleID, bool isInstalled) : m_titleID(titleID), m_isInstalled(isInstalled) {
        NsApplicationControlData appControlData;
        size_t appControlDataSize = 0;
        NsApplicationContentMetaStatus appContentMetaStatus;
        NacpLanguageEntry *languageEntry = nullptr;

        std::memset(&appControlData, 0x00, sizeof(NsApplicationControlData));

        nsGetApplicationControlData(1, titleID, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize);
        nsListApplicationContentMetaStatus(titleID, 0, &appContentMetaStatus, sizeof(NsApplicationContentMetaStatus), nullptr);
        nacpGetLanguageEntry(&appControlData.nacp, &languageEntry);

        this->m_titleName = std::string(languageEntry->name);
        this->m_titleAuthor = std::string(languageEntry->author);
        this->m_versionString = std::string(appControlData.nacp.version);
        this->m_version = appContentMetaStatus.title_version;

        this->m_titleIcon = new u8[appControlDataSize - sizeof(NsApplicationControlData::nacp)];

        std::memcpy(this->m_titleIcon, appControlData.icon, appControlDataSize - sizeof(NsApplicationControlData::nacp));
        this->m_iconSize = appControlDataSize - sizeof(NsApplicationControlData::nacp);
    }

    Title::~Title() {
        delete[] this->m_titleIcon;
    }

    void Title::addUser(u128 userID) {
        this->m_userIDs.push_back(userID);
    }

    std::string Title::getName() {
        return this->m_titleName;
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


    void Title::getIcon(u8 *buffer, size_t bufferSize) {
        std::memcpy(buffer, this->m_titleIcon, bufferSize);
    }

    size_t Title::getIconSize() {
        return this->m_iconSize;
    }

    u64 Title::getID() {
        return this->m_titleID;
    }

    std::vector<u128> Title::getUserIDs() {
        return this->m_userIDs;
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