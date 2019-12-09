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
#include <vector>

#include "cheat/cheat_engine_types.hpp"

#include "cheat/cheat.hpp"
#include "helpers/file.hpp"

namespace edz::cheat {

    class CheatEngine {
    public:
        static void findIn(types::RegionList regions, types::Operation operation, cheat::types::Value &value1, cheat::types::Value &value2) {
            u32 bufferSize = MEMORY_SEARCH_BUFFER_SIZE;
            u8 *buffer = new u8[bufferSize];

            for (const auto &region : regions) {
                offset_t offset = 0;

                while (offset < offset_t(region.getSize())) {
                    if (region.getSize() - offset < bufferSize)
                        bufferSize = region.getSize() - offset;

                    cheat::CheatManager::readMemory(region.getBase() + offset, buffer, bufferSize);

                    for (u32 i = 0; i < bufferSize; i += value1.getSize()) {
                        if ((types::Value(*(buffer + offset + i), value1.getType()).*operation)(value1)) {
                            //CheatEngine::s_foundAddresses.push_back(region.getBase() + offset);
                        }
                    }

                    offset += bufferSize;
                    //svcSleepThread(100E3); // Wait 100us
                }

            }

            delete[] buffer;
        }

        template<typename T>
        static void compareDumpWithMemory(types::RegionList regions, types::Operation operation) {

        }

        static std::vector<addr_t>& getFoundAddresses() {
            return CheatEngine::s_foundAddresses;
        }

        static void clearFoundAddresses() {
            CheatEngine::s_foundAddresses.clear();
        }

        private:
            static inline std::vector<addr_t> s_foundAddresses;
    };

}