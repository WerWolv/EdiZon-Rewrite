#include "helpers/folder.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

namespace edz::helper {
    Folder::Folder(std::string path) {
        this->m_dir = nullptr;

        this->m_path = path;

        if (this->m_path.back() != '/')
            this->m_path += "/";
    }

    Folder::Folder(std::string path, std::string folderName) : m_path(path), m_folderName(folderName) {
        this->m_dir = nullptr;
    }

    Folder::~Folder() {

    }

    std::string Folder::path() {
        return this->m_path;
    }

    std::string Folder::folderName() {
        return this->m_folderName;
    }

    void Folder::copyTo(std::string newPath) {
        struct dirent *entry;

        if (newPath.back() != '/')
            newPath += "/";

        mkdir(newPath.c_str(), 0777);

        openDirectory();

        if (this->m_dir == nullptr) return;

        while ((entry = readdir(this->m_dir)) != nullptr) {

            if(std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                continue;

            if (entry->d_type == DT_DIR) {
                Folder folder(this->m_path + entry->d_name + "/"s, entry->d_name);
                folder.copyTo(newPath + folder.folderName());
            } else if (entry->d_type == DT_REG) {
                File file(this->m_path + std::string(entry->d_name));
                file.copyTo(newPath + file.fileName());
            }
        }

        closeDirectory();
    }

    void Folder::copyFrom(std::string oldPath) {
        Folder oldFolder(oldPath);

        this->removeFolder();
        oldFolder.copyTo(this->m_path);
    }

    void Folder::removeFolder() {
        struct dirent *entry;

        openDirectory();

        if (this->m_dir == nullptr) return;

        while ((entry = readdir(this->m_dir)) != nullptr) {
            if(std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                continue;

            if (entry->d_type == DT_DIR) {

                {
                    Folder folder(this->m_path + entry->d_name + "/"s, entry->d_name);
                    folder.removeFolder();
                }

                rmdir((this->m_path + entry->d_name).c_str());
            } else if (entry->d_type == DT_REG) {
                File file(this->m_path + "/" + std::string(entry->d_name));
                file.removeFile();
            }
        }

        rmdir(this->m_path.c_str());

        closeDirectory();
    }

    void Folder::createDirectories() {
        std::vector<char> path(this->m_path.c_str(), this->m_path.c_str() + this->m_path.size() + 1);
        
        for (u8 i = 0; i < path.size(); i++) {
            if (path[i] != '/') continue;

            path[i] = '\0';
            mkdir(&path[0], 0777);
            path[i] = '/';
        }
    }


    std::map<std::string, File> Folder::getFiles() {
        struct dirent *entry;
        std::map<std::string, File> files;

        openDirectory();

        if (this->m_dir == nullptr) return files;

        while ((entry = readdir(this->m_dir)) != nullptr) {
            if(std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                continue;

            if (entry->d_type == DT_REG)
                files.insert({ entry->d_name, File(this->m_path + "/" + std::string(entry->d_name)) });
        }

        closeDirectory();

        return files;
    }

    std::map<std::string, Folder> Folder::getFolders() {
        struct dirent *entry;
        std::map<std::string, Folder> folders;

        openDirectory();

        if (this->m_dir == nullptr) return folders;

        while ((entry = readdir(this->m_dir)) != nullptr) {
            if(std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                continue;

            if (entry->d_type == DT_DIR)
                folders.insert({ entry->d_name, Folder(this->m_path + "/"s + entry->d_name, entry->d_name) });
        }

        closeDirectory();

        return folders;
    }

    void Folder::foreach(std::function<void(struct dirent*)> callback) {
        struct dirent *ent;

        openDirectory();

        while((ent = readdir(m_dir)) != nullptr)
            callback(ent);

        closeDirectory();
    }


    void Folder::openDirectory() {
        if (this->m_dir != nullptr) return;

        this->m_dir = opendir(m_path.c_str());
    }

    void Folder::closeDirectory() {
        if (this->m_dir == nullptr) return;

        closedir(this->m_dir);
        this->m_dir = nullptr;
    }

    void Folder::rewindDirectory() {
        closeDirectory();
        openDirectory();
    }
}