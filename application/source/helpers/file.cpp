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

#include "helpers/file.hpp"

#include <cstring>
#include <helpers/folder.hpp>

namespace edz::helper {
        File::File(std::string filePath) : m_filePath(filePath) {
            this->m_file = nullptr;
        }

        File::File(const File &file) {
            this->m_file = file.m_file;
            this->m_filePath = file.m_filePath;
        }

        File::~File() {
            closeFile();
        }
        

        std::string File::fileName() {
            return std::filesystem::path(this->m_filePath).filename();
        }

        std::string File::filePath() {
            return this->m_filePath;
        }

        std::string File::basePath() {
            return std::filesystem::path(this->m_filePath).parent_path();
        }

        size_t File::fileSize() {
            if (!exists()) return -1;

            openFile();

            size_t fileSize;
            fseek(this->m_file, 0, SEEK_END);
            fileSize = ftell(this->m_file);
            rewind(this->m_file);

            return fileSize;
        }

        void File::removeFile() {
            if (!exists()) return;

            remove(this->m_filePath.c_str());
        }

        File File::copyTo(std::string path) {
            Folder dstFolder(File(path).basePath());
            dstFolder.createDirectories();

            FILE *dst = fopen(path.c_str(), "w");

            size_t size = 0;
            u64 offset = 0;
            u8 *buffer = new u8[0x50000];

            openFile();

            while ((size = fread(buffer, 1, 0x50000, this->m_file)) > 0) {
                fwrite(buffer, 1, size, dst);
                offset += size;
            }
            
            delete[] buffer;

            closeFile();
            fclose(dst);

            return File(path);
        }

        void File::createDirectories() {
            Folder folder(this->basePath());

            folder.createDirectories();
        }

        bool File::exists() {
            this->m_file = fopen(this->m_filePath.c_str(), "rb");

            if (this->m_file != nullptr) {
                closeFile();
                return true;
            }

            return false;
        }

        s32 File::read(u8 *buffer, size_t bufferSize) {
            size_t readSize = 0;
            
            openFile();
            if (buffer != nullptr && bufferSize != 0)
                readSize = fread(buffer, 1, bufferSize, this->m_file);
            closeFile();

            return readSize;
        }

        s32 File::write(u8 *buffer, size_t bufferSize) {
            size_t writeSize = 0;

            openFile();
            if (buffer != nullptr && bufferSize != 0)
                writeSize = fwrite(buffer, 1, bufferSize, this->m_file);
            closeFile();

            return writeSize;            
        }


        void File::openFile() {
            if (this->m_file != nullptr) return;

            this->m_file = fopen(this->m_filePath.c_str(), "rb");

            if (this->m_file == nullptr)
                this->m_file = fopen(this->m_filePath.c_str(), "w+");
        }

        void File::closeFile() {
            if (this->m_file == nullptr) return;

            fclose(m_file);
            this->m_file = nullptr;

        }
}