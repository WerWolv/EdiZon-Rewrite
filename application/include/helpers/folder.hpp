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

#include "helpers/file.hpp"

#include <string>
#include <vector>
#include <map>
#include <dirent.h>
#include <functional>

namespace edz::hlp {
    class Folder {
    public:
        Folder(std::string path);
        Folder(std::string path, std::string folderName);
        ~Folder();

        std::string path();
        std::string name();
        bool exists();

        void copyTo(std::string newPath);
        void copyFrom(std::string oldPath);

        EResult remove();
        EResult createDirectories();

        std::map<std::string, File> getFiles();
        std::map<std::string, Folder> getFolders();

        void foreach(std::function<void(struct dirent*)> callback);

    private:
        DIR *m_dir;
        std::string m_path;
        std::string m_folderName;

        void openDirectory();
        void closeDirectory();
        void rewindDirectory();
    };
}