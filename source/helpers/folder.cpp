#include "folder.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

namespace edz::helper {
    Folder::Folder(std::string path) : m_path(path) {
        
    }

    Folder::Folder(std::string path, std::string folderName) : m_path(path), m_folderName(folderName) {

    }

    Folder::~Folder() {

    }

    std::string Folder::path() {
        return m_path;
    }

    std::string Folder::folderName() {
        return m_folderName;
    }

    void Folder::copyTo(std::string newPath) {
        struct dirent *entry;

        rewindDirectory();

        mkdir(newPath.c_str(), 0777);

        while ((entry = readdir(m_dir)) != nullptr) {
            if(std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                continue;

            if (entry->d_type == DT_DIR) {
                Folder folder(m_path + std::string("/") + entry->d_name, entry->d_name);
                folder.copyTo(newPath + "/" + folder.folderName());
            } else if (entry->d_type == DT_REG) {
                File file(m_path + "/" + std::string(entry->d_name));
                file.copyTo(newPath + "/" + file.fileName());
            }
        }
    }

    void Folder::removeFolder() {
        struct dirent *entry;

        rewindDirectory();

        while ((entry = readdir(m_dir)) != nullptr) {
            if(std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                continue;

            if (entry->d_type == DT_DIR) {

                {
                    Folder folder(m_path + std::string("/") + entry->d_name, entry->d_name);
                    folder.removeFolder();
                }

                rmdir((m_path + std::string("/") + entry->d_name).c_str());
            } else if (entry->d_type == DT_REG) {
                File file(m_path + "/" + std::string(entry->d_name));
                file.removeFile();
            }
        }

        rmdir(m_path.c_str());

        closedir(m_dir);
        m_dir = nullptr;
    }


    void Folder::openDirectory() {
        if (m_dir != nullptr) return;

        m_dir = opendir(m_path.c_str());
    }

    void Folder::closeDirectory() {
        if (m_dir == nullptr) return;

        closedir(m_dir);
        m_dir = nullptr;
    }

    void Folder::rewindDirectory() {
        closeDirectory();
        openDirectory();
    }
}