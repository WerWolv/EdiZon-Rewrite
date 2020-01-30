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

#include "cheat/dump_file.hpp"

namespace edz::cheat::types {

    DumpFile::DumpFile(std::string path) {
        this->m_file = hlp::File(path);
        this->m_readBufferOffset = -1;
        this->m_newDump = true;

        if (this->m_file.exists()) {
            this->m_newDump = false;
            this->m_file.seek(0, SEEK_SET);
            this->m_file.read(reinterpret_cast<u8*>(&this->m_header), sizeof(DumpFileHeader));
        }
    }

    DumpFile::~DumpFile() {

    }


    std::string DumpFile::getFilePath() const {
        return this->m_file.path();
    }


    void DumpFile::setTitleId(titleid_t titleId) {
        this->m_header.titleId = titleId;
    }

    void DumpFile::setIsKnownInitialPattern(bool known) {
        this->m_header.isKnownInitialPattern = known;
    }

    void DumpFile::setRegion(Region &region) {
        this->m_header.searchRegion = region;
    }

    void DumpFile::setPatternSize(size_t patternSize) {
        this->m_header.searchPatternSize = patternSize;
    }
        

    void DumpFile::addAddress(addr_t addr) {
        if (!this->m_header.isKnownInitialPattern)
            return;

        this->m_addresses.push_back(addr);

        if (this->m_addresses.size() >= 512)
            DumpFile::flush();
    }
    
    size_t DumpFile::getAddressCount() {
        if (!this->m_header.isKnownInitialPattern)
            return 0;

        return this->m_header.addressCount;
    }


    void DumpFile::addBuffer(u8 *buffer, size_t size) {
        if (this->m_header.isKnownInitialPattern)
            return;

        this->m_file.seek(0, SEEK_END);
        this->m_file.write(buffer, size);
        this->m_header.dumpedSize += size;

        DumpFile::writeHeader();
    }

    size_t DumpFile::getDumpedSize() {
        if (this->m_header.isKnownInitialPattern)
            return 0;

        return this->m_header.dumpedSize;
    }


    void DumpFile::writeHeader() {
        this->m_file.seek(0, SEEK_SET);
        this->m_file.write(reinterpret_cast<u8*>(&this->m_header), sizeof(DumpFileHeader));
    }

    void DumpFile::flush() {
        this->m_file.seek(0, SEEK_END);
        this->m_file.write(reinterpret_cast<u8*>(&this->m_addresses[0]), this->m_addresses.size() * sizeof(addr_t));
        this->m_header.addressCount += this->m_addresses.size();
        this->m_addresses.clear();

        DumpFile::writeHeader();
    }


    EResultVal<addr_t> DumpFile::getAddress(s32 index) {
        if (!this->m_header.isKnownInitialPattern)
            return { ResultEdzInvalidOperation, 0x00 };

        if (UNLIKELY(this->m_readBufferOffset == -1 || index < this->m_readBufferOffset || index > this->m_readBufferOffset + 512)) {
            if (index > ssize_t(this->m_header.addressCount))
                return { ResultEdzOutOfRange, 0 };
            
            this->m_file.seek(sizeof(DumpFileHeader) + index * sizeof(addr_t), SEEK_SET);
            this->m_file.read(reinterpret_cast<u8*>(this->m_readBuffer), std::min(this->m_header.addressCount - index, 512UL) * sizeof(addr_t));
        }

        return { ResultSuccess, this->m_readBuffer[index - this->m_readBufferOffset] };
    }

    EResultVal<size_t> DumpFile::getBuffer(u8 *buffer, size_t bufferSize, offset_t offset) {
        if (this->m_header.isKnownInitialPattern)
            return { ResultEdzInvalidOperation, 0x00 };

        size_t realSize = std::min(this->m_header.dumpedSize - offset, bufferSize);

        this->m_file.seek(sizeof(DumpFileHeader) + offset, SEEK_SET);
        this->m_file.read(buffer, realSize);

        return { ResultSuccess, realSize };
    }
    

}