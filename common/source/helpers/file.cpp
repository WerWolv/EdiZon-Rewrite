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

#include "helpers/file.hpp"

#include <cstring>
#include <helpers/folder.hpp>

namespace edz::hlp {

    File::File() : m_file(nullptr), m_filePath("") {

    }

    File::File(std::string filePath) : m_file(nullptr), m_filePath(filePath) {

    }

    File::File(const File &file) : m_file(file.m_file), m_filePath(file.m_filePath) {

    }

    File::~File() {
        closeFile();
    }
    

    std::string File::name() const {
        return std::filesystem::path(this->m_filePath).filename();
    }

    std::string File::path() const {
        return this->m_filePath;
    }

    std::string File::parent() const {
        return std::filesystem::path(this->m_filePath).parent_path();
    }

    size_t File::size() const {
        if (!exists()) return -1;

        if (openFile().failed())
            return 0;

        size_t fileSize;
        fseek(this->m_file, 0, SEEK_END);
        fileSize = ftell(this->m_file);
        rewind(this->m_file);

        return fileSize;
    }

    void File::remove() const {
        if (!exists()) return;

        ::remove(this->m_filePath.c_str());
    }

    File File::copyTo(std::string path) const {
        Folder dstFolder(File(path).parent());
        dstFolder.createDirectories();

        FILE *dst = fopen(path.c_str(), "w");

        if (dst == nullptr)
            return *this;


        size_t size = 0;
        u64 offset = 0;
        u8 *buffer = new u8[0x5000];

        if (openFile().failed())
            return *this;

        while ((size = fread(buffer, 1, 0x5000, this->m_file)) > 0) {
            fwrite(buffer, 1, size, dst);
            offset += size;
        }
        
        delete[] buffer;

        closeFile();
        fclose(dst);

        return File(path);
    }

    void File::createDirectories() const {
        Folder folder(this->parent());

        folder.createDirectories();
    }

    bool File::exists() const {
        this->m_file = fopen(this->m_filePath.c_str(), "rb");

        if (this->m_file != nullptr) {
            closeFile();
            return true;
        }

        return false;
    }

    s32 File::read(u8 *buffer, size_t bufferSize) {
        size_t readSize = 0;
        
        if (openFile().failed())
            return 0;

        fseek(this->m_file, this->m_position, this->m_seekOperation);
        this->m_seekOperation = SEEK_SET;

        if (buffer != nullptr && bufferSize != 0) {
            rewind(this->m_file);
            readSize = fread(buffer, 1, bufferSize, this->m_file);
        }

        this->m_position = ftell(this->m_file);
        closeFile();

        return readSize;
    }
    
    std::string File::read() {
        size_t fileSize = this->size() + 1;
        std::string content(fileSize, '\0');

        read(reinterpret_cast<u8*>(&content[0]), fileSize);

        return content;
    }

    s32 File::write(const u8 *buffer, size_t bufferSize) {
        size_t writeSize = 0;

        if (openFile().failed())
            return 0;

        fseek(this->m_file, this->m_position, this->m_seekOperation);
        this->m_seekOperation = SEEK_SET;

        if (buffer != nullptr && bufferSize != 0)
            writeSize = fwrite(buffer, 1, bufferSize, this->m_file);

        this->m_position = ftell(this->m_file);
        closeFile();

        return writeSize;            
    }

    void File::write(std::string &buffer) {
        write(reinterpret_cast<u8*>(&buffer[0]), buffer.size());
    }


    EResult File::openFile() const {
        if (this->m_file != nullptr) return ResultSuccess;

        this->m_file = fopen(this->m_filePath.c_str(), "rb");

        if (this->m_file == nullptr)
            this->m_file = fopen(this->m_filePath.c_str(), "w+");

        if (this->m_file == nullptr)
            return ResultEdzOperationFailed;

        return ResultSuccess;
    }

    void File::closeFile() const {
        if (this->m_file == nullptr) return;

        fclose(m_file);
        this->m_file = nullptr;

    }

    u32 File::seek(s32 position, s32 operation) {
        this->m_position = position;
        this->m_seekOperation = operation;

        return this->m_position;
    }

}