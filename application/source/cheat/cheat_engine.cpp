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

#include "cheat/cheat_engine.hpp"

#include "helpers/utils.hpp"
#include "save/title.hpp"

namespace edz::cheat {

    EResultVal<std::string> CheatEngine::findPattern(u8* pattern, std::size_t patternSize, types::Signedness signedness, types::Operation operation, types::Region region, bool alignedSearch) {
        size_t bufferSize = MEMORY_SEARCH_BUFFER_SIZE;
        u8 *buffer = new u8[bufferSize];

        std::string dumpFilePath = EDIZON_DUMP_DIR + hlp::formatString("/region_%016lx_k1.bin", region.baseAddress);

        if (auto file = hlp::File(dumpFilePath); file.exists())
            file.remove();

        // Thanks to ASLR, the region base addresses should be pretty random
        types::DumpFile dumpFile(dumpFilePath);
        dumpFile.setIsKnownInitialPattern(true);
        dumpFile.setPatternSize(patternSize);
        dumpFile.setRegion(region);
        dumpFile.setTitleId(save::Title::getRunningTitleID());

        types::Pattern comparatorPattern(pattern, patternSize);
        comparatorPattern.setSize(patternSize);
        comparatorPattern.setSignedness(signedness);

        types::Pattern memoryPattern;
        memoryPattern.setSize(patternSize);
        memoryPattern.setSignedness(signedness);
        
        offset_t offset = 0;
        while (static_cast<u64>(offset) < region.size) {
            if (size_t(region.size - offset) < bufferSize)
                bufferSize = region.size - offset;

            if (CheatManager::readMemory(region.baseAddress + offset, buffer, bufferSize).failed())
                break;

            for (u32 i = 0; i < bufferSize; i += (alignedSearch ? patternSize : 1)) {
                memoryPattern.setPattern(buffer + i);
                if ((memoryPattern.*operation)(comparatorPattern)) {
                    dumpFile.addAddress(region.baseAddress + offset + i);
                }
            }
        }

        delete[] buffer;

        dumpFile.flush();

        return { ResultSuccess, dumpFilePath };
    }

    EResultVal<std::string> CheatEngine::findPattern(u8* pattern, std::size_t patternSize, types::Signedness signedness, types::Operation operation, types::Region region, types::DumpFile &dumpFile) {
        std::string dumpFilePath = EDIZON_DUMP_DIR + hlp::formatString("/region_%016lx_k2.bin", region.baseAddress);

        types::DumpFile newDumpFile(dumpFilePath);
        dumpFile.setIsKnownInitialPattern(true);
        dumpFile.setPatternSize(patternSize);
        dumpFile.setRegion(region);
        dumpFile.setTitleId(save::Title::getRunningTitleID());

        u8 *buffer = new u8[patternSize];

        types::Pattern comparatorPattern(pattern, patternSize);
        comparatorPattern.setSize(patternSize);
        comparatorPattern.setSignedness(signedness);

        types::Pattern memoryPattern(buffer, patternSize);
        memoryPattern.setSize(patternSize);
        memoryPattern.setSignedness(signedness);


        for (u32 i = 0; i < dumpFile.getAddressCount(); i++) {
            auto [result, address] = dumpFile.getAddress(i);

            if (result.failed())
                break;

            if (CheatManager::readMemory(address, buffer, patternSize).failed())
                break;

            if ((memoryPattern.*operation)(comparatorPattern)) {
                newDumpFile.addAddress(address);
            }
        }

        delete[] buffer;

        if (newDumpFile.getAddressCount() == 0) {
            hlp::File(dumpFilePath).remove();
            return { ResultEdzNoValuesFound, dumpFile.getFilePath() };
        }

        hlp::File(dumpFile.getFilePath()).remove();
        hlp::File(dumpFilePath).copyTo(dumpFile.getFilePath());

        return { ResultSuccess, dumpFilePath };
    }

    EResultVal<std::string> CheatEngine::dumpRegion(types::Region region) {
        std::string dumpFilePath = EDIZON_DUMP_DIR + hlp::formatString("/region_%016lx_u1.bin", region.baseAddress);

        if (auto file = hlp::File(dumpFilePath); file.exists())
            file.remove();

        types::DumpFile dumpFile(dumpFilePath);
        dumpFile.setIsKnownInitialPattern(false);
        dumpFile.setPatternSize(0);
        dumpFile.setRegion(region);
        dumpFile.setTitleId(save::Title::getRunningTitleID());

        size_t bufferSize = MEMORY_SEARCH_BUFFER_SIZE;
        u8 *buffer = new u8[bufferSize];

        offset_t offset = 0;
        while (static_cast<u64>(offset) < region.size) {
            if (size_t(region.size - static_cast<u64>(offset)) < bufferSize)
                bufferSize = region.size - offset;

            if (CheatManager::readMemory(region.baseAddress + offset, buffer, bufferSize).failed())
                break;

            dumpFile.addBuffer(buffer, bufferSize);
        }

        delete[] buffer;

        return { ResultSuccess, dumpFilePath };
    }

    EResultVal<std::string> CheatEngine::compareMemoryWithDump(std::size_t patternSize, types::Signedness signedness, types::Operation operation, types::Region region, types::DumpFile &dumpFile, bool alignedSearch) {
        size_t bufferSize = MEMORY_SEARCH_BUFFER_SIZE;
        u8 *memBuffer = new u8[bufferSize];
        u8 *dumpBuffer = new u8[bufferSize];

        std::string changedAddressesDumpFilePath = EDIZON_DUMP_DIR + hlp::formatString("/region_%016lx_u2.bin", region.baseAddress);
        types::DumpFile changedAddressesDumpFile(changedAddressesDumpFilePath);

        types::Pattern comparatorPattern;
        comparatorPattern.setSize(patternSize);
        comparatorPattern.setSignedness(signedness);

        types::Pattern memoryPattern;
        memoryPattern.setSize(patternSize);
        memoryPattern.setSignedness(signedness);
        
        offset_t offset = 0;
        while (static_cast<u64>(offset) < region.size) {
            if (size_t(region.size - static_cast<u64>(offset)) < bufferSize)
                bufferSize = region.size - offset;

            if (CheatManager::readMemory(region.baseAddress + offset, memBuffer, bufferSize).failed())
                break;

            auto [result, readSize] = changedAddressesDumpFile.getBuffer(dumpBuffer, bufferSize, offset);
            if (result.failed())
                break;

            for (u32 i = 0; i < bufferSize; i += (alignedSearch ? patternSize : 1)) {
                memoryPattern.setPattern(memBuffer + i);
                comparatorPattern.setPattern(dumpBuffer + i);

                if ((memoryPattern.*operation)(comparatorPattern)) {
                    changedAddressesDumpFile.addAddress(region.baseAddress + offset + i);
                }
            }
        }

        delete[] memBuffer;
        delete[] dumpBuffer;

        dumpFile.flush();

        return { ResultSuccess, changedAddressesDumpFilePath };
    }

}