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

#include <string>
#include <fstream>
#include <filesystem>

namespace edz::helper {

    class File {
    public:
        File(std::string filePath);
        File(const File &file);
        ~File();

        std::string fileName();
        std::string filePath();
        std::string basePath();

        size_t fileSize();

        void removeFile();
        File copyTo(std::string path);
        void createDirectories();

        bool exists();

        s32 read(u8 *buffer, size_t bufferSize);
        s32 write(u8 *buffer, size_t bufferSize);

    private:
        FILE *m_file;
        std::string m_filePath;

        void openFile();
        void closeFile();
    };

}