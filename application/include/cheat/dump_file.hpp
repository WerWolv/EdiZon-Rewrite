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

#pragma once

#include <edizon.hpp>
#include "cheat/cheat_engine_types.hpp"
#include "helpers/file.hpp"

#include <vector>

namespace edz::cheat::types {

    struct DumpFileHeader {
        u32 magic;
        titleid_t titleId;
        bool isKnownInitialPattern;
        Region searchRegion;
        size_t searchPatternSize;
        union { size_t addressCount; size_t dumpedSize; };
    };

    class DumpFile {
    public:
        DumpFile(std::string path);
        ~DumpFile();

        std::string getFilePath() const;

        void setTitleId(titleid_t titleId);
        void setIsKnownInitialPattern(bool known);
        void setRegion(Region &region);
        void setPatternSize(size_t patternSize);

        void addAddress(addr_t addr);
        size_t getAddressCount();

        void addBuffer(u8 *buffer, size_t size);
        size_t getDumpedSize();

        void writeHeader();
        void flush();

        EResultVal<addr_t> getAddress(s32 index);
        EResultVal<size_t> getBuffer(u8 *buffer, size_t bufferSize, offset_t offset);

    private:
        hlp::File m_file;

        DumpFileHeader m_header;
        std::vector<addr_t> m_addresses;

        addr_t m_readBuffer[512];
        s32 m_readBufferOffset;

        bool m_newDump = false;
    };

}