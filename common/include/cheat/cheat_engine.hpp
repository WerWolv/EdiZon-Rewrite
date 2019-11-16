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

#include <functional>

namespace edz::cheat {

    enum class DataType {
        UNSIGNED,
        SIGNED,
        FLOATING_POINT,
        STRING
    };

    constexpr offset_t REGION_SIZE = INT64_MAX;

    using ReadFunction    = std::function<std::tuple<EResult, addr_t, size_t>(void*, size_t, offset_t)>;
    using WriteFunction   = std::function<EResult(void*, size_t, offset_t)>;
    using CompareFunction = std::function<bool(DataType, size_t, void*, void*)>;

    class CheatEngine {
    public:
        static EResult findPattern(void *pattern, size_t patternSize, DataType dataType, CompareFunction comparator, ReadFunction memRegion);
        static EResult dumpMemory(ReadFunction memRegion);
        static EResult compareMemoryWithDump(DataType dataType, CompareFunction comparator);

        static EResult patch(void* data, size_t size, offset_t offset, WriteFunction memRegion);
        static EResult nop(offset_t offset, WriteFunction memRegion);

        static MemoryInfo getHeapRegion();
        static std::vector<MemoryInfo> getMainRegions();

        static void setProgressCallback(std::function<void(u8)> callback);

    private:
        CheatEngine();
        CheatEngine& operator=(CheatEngine&) = delete;

        static inline std::function<void(u8)> s_progressCallback = [](u8){};
    };

    namespace cmp {

        bool equals(DataType dataType, size_t size, void* l, void* r);
        bool notEquals(DataType dataType, size_t size, void* l, void* r);

    }

    namespace region {

        // Read functions

        std::tuple<EResult, addr_t, size_t> readHeap(void* buffer, size_t size, offset_t offset);
        std::tuple<EResult, addr_t, size_t> readMain(void* buffer, size_t size, offset_t offset);
        std::tuple<EResult, addr_t, size_t> readFile(void* buffer, size_t size, offset_t offset);

        // Write functions

        EResult writeHeap(void* buffer, size_t size, offset_t offset);
        EResult writeMain(void* buffer, size_t size, offset_t offset);

    }


}