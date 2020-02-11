/**
 * Copyright (C) 2019 - 2020 WerWolv
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

namespace edz::hlp {

    class File {
    public:
        File();
        File(std::string filePath);
        File(const File &file);
        ~File();

        std::string name() const;
        std::string path() const;
        std::string parent() const;

        size_t size() const;

        void remove() const;
        File copyTo(std::string path) const;
        void createDirectories() const;

        bool exists() const;

        s32 read(u8 *buffer, size_t bufferSize);
        std::string read();

        s32 write(const u8 *buffer, size_t bufferSize);
        void write(std::string &buffer);

        u32 seek(s32 position, s32 operation);

    private:
        mutable FILE *m_file;
        std::string m_filePath;

        u32 m_position = 0;
        s32 m_seekOperation = SEEK_SET;

        EResult openFile() const;
        void closeFile() const;
    };

}