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

#include "cheat/cheat_engine.hpp"

#include "cheat/cheat.hpp"
#include "helpers/file.hpp"

#include <cstring>

namespace edz::cheat {

    EResult CheatEngine::findPattern(void *pattern, size_t patternSize, DataType dataType, CompareFunction comparator, ReadFunction memRegion) {
        u8 *dataBuffer = new u8[patternSize];
        size_t regionSize = 0;
        size_t readProgress = 0;

        {
            auto [result, address, size] = memRegion(nullptr, 0, REGION_SIZE);
            regionSize = size;
        }

        while (true) {
            auto [result, address, readSize] = memRegion(dataBuffer, patternSize, -1);

            readProgress += readSize;
            CheatEngine::s_progressCallback((static_cast<float>(readProgress) / readSize) * 100);

            if (result.failed())
                break;

            if (comparator(dataType, patternSize, pattern, dataBuffer))
                ; //TODO: Cache and write address to file
        }

        return ResultSuccess;
    }

    EResult CheatEngine::dumpMemory(ReadFunction memRegion) {
        hlp::File dumpFile;

        u8 *dataBuffer = new u8[0xFFFF];

        // TODO: Add metadata to dumpFile, choose file name and add file path to config file

        while (true) {
            auto [result, address, readSize] = memRegion(dataBuffer, 0xFFFF, -1);

            if (readSize > 0)
                dumpFile.write(dataBuffer, readSize);

            if (result.failed())
                break;
        }

        return ResultSuccess;
    }


    EResult CheatEngine::patch(void* data, size_t size, offset_t offset, WriteFunction memRegion) {
        return memRegion(data, size, offset);
    }

    EResult CheatEngine::nop(offset_t offset, WriteFunction memRegion) {
        // AARCH64 NOP instruction code
        u32 NOP = 0b11010101000000110010000000011111;

        return CheatEngine::patch(&NOP, sizeof(NOP), offset, memRegion);
    }

    MemoryInfo CheatEngine::getHeapRegion() {
        for (auto memoryRegion : CheatManager::getMemoryRegions())
            if (memoryRegion.type == MemType_Heap)
                return memoryRegion;
    }

    std::vector<MemoryInfo> CheatEngine::getMainRegions() {
        std::vector<MemoryInfo> mainRegions;

        for (auto memoryRegion : CheatManager::getMemoryRegions())
            if (memoryRegion.type == MemType_CodeMutable && memoryRegion.perm == Perm_Rx)
                mainRegions.push_back(memoryRegion);
        
        return mainRegions;
    }

    void CheatEngine::setProgressCallback(std::function<void(u8)> callback) {
        CheatEngine::s_progressCallback = callback;
    }


    namespace cmp {

        bool equals(DataType dataType, size_t size, void* l, void* r) {
            return std::memcmp(l, r, size) == 0;
        }

        bool greaterThan(DataType dataType, size_t size, void* l, void* r) {

        }

        bool lessThan(DataType dataType, size_t size, void* l, void* r) {
            
        }

        bool between(DataType dataType, size_t size, void* l, void* r) {
            
        }

    }

    namespace region {

        // Read functions

        std::tuple<EResult, addr_t, size_t> readHeap(void* buffer, size_t size, offset_t offset) {
            static auto heap = CheatEngine::getHeapRegion();
            static offset_t currOffset = 0;

            if (offset == REGION_SIZE && buffer == nullptr)
                return { ResultSuccess, heap.addr, heap.size };

            if (offset != -1)
                currOffset = offset;

            if(CheatManager::readMemory(heap.addr + currOffset, buffer, size).failed())
                return { ResultEdzMemoryReadFailed, 0, 0 };

            if ((currOffset + size) > heap.size) 
                return { ResultEdzMemoryOverflow, heap.addr + currOffset, heap.size - currOffset };

            currOffset += size;

            return { ResultSuccess, heap.addr + currOffset - size, size };
        }

        std::tuple<EResult, addr_t, size_t> readMain(void* buffer, size_t size, offset_t offset) {
            static auto regions = CheatEngine::getMainRegions();
            static MemoryInfo main = { 0 };
            static offset_t currOffset = 0;

            if (offset != -1) {
                currOffset = offset;
                main = { 0 };
            }

            if (offset == REGION_SIZE && buffer == nullptr)
                return { ResultSuccess, main.addr, main.size };

            // Find right memory region depending of offset (Concatinating all code sections)
            if (static_cast<addr_t>(currOffset) >= main.size)
                for (auto const& region : regions) {
                    if (currOffset - region.size > 0) {
                        currOffset -= region.size;
                        main = region;
                    }
                    else break;
                }

            if(CheatManager::readMemory(main.addr + currOffset, buffer, size).failed())
                return { ResultEdzMemoryReadFailed, 0, 0 };

            if ((currOffset + size) > main.size) 
                return { ResultEdzMemoryOverflow, main.addr + currOffset, main.size - currOffset };

            currOffset += size;

            return { ResultSuccess, main.addr + currOffset - size, size };
        }

        std::tuple<EResult, addr_t, size_t> readFile(void* buffer, size_t size, offset_t offset) {
            return {};
        }

        // Write functions

        EResult writeHeap(void* buffer, size_t size, offset_t offset) {
            static auto heap = CheatEngine::getHeapRegion();

            return CheatManager::writeMemory(heap.addr + offset, buffer, size);
        }

        EResult writeMain(void* buffer, size_t size, offset_t offset) {
            static auto regions = CheatEngine::getMainRegions();
            static MemoryInfo main = { 0 };

            // Find right memory region depending of offset (Concatinating all code sections)
            if (static_cast<addr_t>(offset) >= main.size)
                for (auto const& region : regions) {
                    if (offset - region.size > 0) {
                        offset -= region.size;
                        main = region;
                    }
                    else break;
                }

            return CheatManager::writeMemory(main.addr + offset, buffer, size);
        }

    }

}