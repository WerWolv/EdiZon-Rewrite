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

#include "edizon.hpp"
#include "cheat/dmntcht.hpp"

#include <string>
#include <vector>

namespace edz::cheat {

    class Cheat {
    public:
        Cheat(edz::dmntcht::DmntCheatEntry cheatEntry);

        std::string getName();
        u32 getID();

        bool toggle();
        bool isEnabled();

    private:
        std::string m_name;
        u32 m_id;
    };

    class FrozenAddress {
    public:
        FrozenAddress(edz::dmntcht::DmntFrozenAddressEntry);
        FrozenAddress(addr_t address, u8 width, u64 value);
        FrozenAddress(addr_t address, u8 width);

        addr_t getAddress();
        u8 getWidth();
        u64 getValue();
        u64 setValue(u64 value, u8 width);

        bool toggle();
        bool isFrozen();

    private:
        edz::dmntcht::DmntFrozenAddressEntry m_frozenAddressEntry;
        bool m_frozen;
    };


    class CheatManager {
    public:
        static CheatManager& getInstance() {
            static CheatManager instance;
            
            return instance;
        }

        CheatManager(CheatManager const&) = delete;
        void operator=(CheatManager const&) = delete;

        bool forceAttach();
        bool hasCheatProcess();

        titleid_t getTitleID();
        processid_t getProcessID();
        buildid_t getBuildID();

        std::vector<Cheat*>& getCheats();
        std::vector<FrozenAddress*>& getFrozenAddresses();

        MemoryInfo queryMemory(addr_t address);
        std::vector<MemoryInfo> getMemoryRegions();

        EResult readMemory(addr_t address, u8 *buffer, size_t bufferSize);
        EResult writeMemory(addr_t address, const u8 *buffer, size_t bufferSize);

    private:
        CheatManager();
        ~CheatManager();

        std::vector<Cheat*> m_cheats;
        std::vector<FrozenAddress*> m_frozenAddresses;

        edz::dmntcht::DmntCheatProcessMetadata m_processMetadata;
    };

} 