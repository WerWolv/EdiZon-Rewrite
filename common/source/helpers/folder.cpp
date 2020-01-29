/**
 * Copyright (C) 2020 WerWolv
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

#include "helpers/folder.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include "helpers/utils.hpp"

namespace edz::hlp {

    Folder::Folder()  : m_dir(nullptr), m_path(""), m_folderName("") {

    }

    Folder::Folder(std::string path) : m_dir(nullptr), m_path(path) {
        if (this->m_path.back() != '/')
            this->m_path += "/";
    }

    Folder::Folder(std::string path, std::string folderName) : m_dir(nullptr), m_path(path), m_folderName(folderName) {
        if (this->m_path.back() != '/')
            this->m_path += "/";
    }

    Folder::~Folder() {

    }

    std::string Folder::path() const {
        return this->m_path;
    }

    std::string Folder::name() const {
        return this->m_folderName;
    }

    void Folder::copyTo(std::string newPath) const {
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
                Folder folder(hlp::formatString("%s%s", this->m_path.c_str(), entry->d_name), entry->d_name);
                folder.copyTo(hlp::formatString("%s%s", newPath.c_str(), folder.name().c_str()));
            } else if (entry->d_type == DT_REG) {
                File file(hlp::formatString("%s%s", this->m_path.c_str(), entry->d_name));
                file.copyTo(hlp::formatString("%s%s", newPath.c_str(), entry->d_name));
            }
        }

        closeDirectory();
    }

    void Folder::copyFrom(std::string oldPath) const {
        Folder oldFolder(oldPath);

        this->remove();
        oldFolder.copyTo(this->m_path);
    }

    EResult Folder::remove() const {
        EResult res;
        struct dirent *entry;

        openDirectory();

        if (this->m_dir == nullptr) return ResultSuccess;

        while ((entry = readdir(this->m_dir)) != nullptr && res == 0) {
            if(std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                continue;

            if (entry->d_type == DT_DIR) {

                {
                    Folder folder(this->m_path + entry->d_name + ""s, entry->d_name);
                    res = folder.remove();

                    if (res.failed())
                        return res;
                }

                res = rmdir((this->m_path + entry->d_name).c_str());
                if (res.failed())
                    return res;
            } else if (entry->d_type == DT_REG) {
                File file(this->m_path + "/" + std::string(entry->d_name));
                file.remove();
            }
        }

        res = rmdir(this->m_path.c_str());

        closeDirectory();

        return res;
    }

    EResult Folder::createDirectories() const {
        EResult res;

        std::vector<char> path(this->m_path.c_str(), this->m_path.c_str() + this->m_path.size() + 1);
        
        for (u8 i = 0; i < path.size() && res == 0; i++) {
            if (path[i] != '/') continue;

            path[i] = '\0';
            res = mkdir(&path[0], 0777);
            path[i] = '/';
        }

        return res;
    }


    std::map<std::string, File> Folder::getFiles() const {
        struct dirent *entry;
        std::map<std::string, File> files;

        openDirectory();

        if (this->m_dir == nullptr) return files;

        while ((entry = readdir(this->m_dir)) != nullptr) {
            if(std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                continue;

            if (entry->d_type == DT_REG) {
                files.insert({ entry->d_name, File(this->m_path + std::string(entry->d_name)) });
            }
        }

        closeDirectory();

        return files;
    }

    std::map<std::string, Folder> Folder::getFolders() const {
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

    void Folder::foreach(std::function<void(struct dirent*)> callback) const {
        struct dirent *ent;

        openDirectory();

        while((ent = readdir(m_dir)) != nullptr)
            callback(ent);

        closeDirectory();
    }

    bool Folder::exists() const {
        DIR* dir = opendir(this->m_path.c_str());
        
        if (dir) {
            closedir(dir);
            return true;
        } else return false;
    }


    void Folder::openDirectory() const {
        if (this->m_dir != nullptr) return;

        this->m_dir = opendir(m_path.c_str());
    }

    void Folder::closeDirectory() const {
        if (this->m_dir == nullptr) return;

        closedir(this->m_dir);
        this->m_dir = nullptr;
    }

    void Folder::rewindDirectory() const {
        closeDirectory();
        openDirectory();
    }

}