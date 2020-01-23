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
#include <vector>

#include "cheat/cheat_engine_types.hpp"
#include "cheat/dump_file.hpp"

#include "cheat/cheat.hpp"
#include "helpers/file.hpp"

namespace edz::cheat {

    class CheatEngine {
    public:
        std::pair<EResult, std::string> findPattern(u8* pattern, std::size_t patternSize, types::Signedness signedness, types::Operation operation, types::Region region, bool alignedSearch);
        std::pair<EResult, std::string> findPattern(u8* pattern, std::size_t patternSize, types::Signedness signedness, types::Operation operation, types::Region region, types::DumpFile &dumpFile);
        std::pair<EResult, std::string> dumpRegion(types::Region region);
        std::pair<EResult, std::string> compareMemoryWithDump(std::size_t patternSize, types::Signedness signedness, types::Operation operation, types::Region region, types::DumpFile &dumpFile, bool alignedSearch);
    };

}