#include "save/title.hpp"

#include <turbojpeg.h>
#include <cstring>
#include <string>

namespace edz::save {

    Title::Title(u64 titleID) : m_titleID(titleID) {
        NsApplicationControlData appControlData;
        size_t appControlDataSize = 0;
        NsApplicationContentMetaStatus appContentMetaStatus;
        NacpLanguageEntry *languageEntry = nullptr;

        std::memset(&appControlData, 0x00, sizeof(NsApplicationControlData));

        nsGetApplicationControlData(1, titleID, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize);
        nsListApplicationContentMetaStatus(titleID, 0, &appContentMetaStatus, sizeof(NsApplicationContentMetaStatus), nullptr);
        nacpGetLanguageEntry(&appControlData.nacp, &languageEntry);

        m_titleName = std::string(languageEntry->name);
        m_titleAuthor = std::string(languageEntry->author);
        m_versionString = std::string(appControlData.nacp.version);
        m_version = appContentMetaStatus.title_version;

        m_titleIcon = new u8[appControlDataSize - sizeof(NsApplicationControlData::nacp)];

        std::memcpy(m_titleIcon, appControlData.icon, appControlDataSize - sizeof(NsApplicationControlData::nacp));
        m_iconSize = appControlDataSize - sizeof(NsApplicationControlData::nacp);

        /*tjhandle jpegDecompressor = tjInitDecompress();
        s32 jpegSubsamp;

        tjDecompressHeader2(jpegDecompressor, appControlData.icon, appControlDataSize - sizeof(NsApplicationControlData::nacp), &m_iconWidth, &m_iconHeight, &jpegSubsamp);
        tjDecompress2(jpegDecompressor, appControlData.icon, appControlDataSize - sizeof(NsApplicationControlData::nacp), m_titleIcon, m_iconWidth, 0, m_iconHeight, TJPF_RGBA, TJFLAG_ACCURATEDCT);
        tjDestroy(jpegDecompressor);*/
    }

    Title::~Title() {
        delete[] m_titleIcon;
    }

    void Title::addUserID(u128 userID) {
        m_userIDs.push_back(userID);
    }

    std::string Title::getTitleName() {
        return m_titleName;
    }

    std::string Title::getVersionString() {
        return m_versionString;
    }

    u32 Title::getVersion() {
        return m_version;
    }

    void Title::getTitleIcon(u8 *buffer, size_t bufferSize) {
        std::memcpy(buffer, m_titleIcon, bufferSize);
    }

    s32 Title::getTitleIconSize() {
        return m_iconSize;
    }

    u64 Title::getTitleID() {
        return m_titleID;
    }

    std::vector<u128> Title::getUserIDs() {
        return m_userIDs;
    }
    
}