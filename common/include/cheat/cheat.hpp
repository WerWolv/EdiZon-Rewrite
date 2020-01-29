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

#include "edizon.hpp"
#include "cheat/dmntcht.hpp"
#include "cheat/cheat_engine_types.hpp"

#include <string>
#include <vector>

namespace edz::cheat {

    class Cheat {
    public:
        Cheat(dmntcht::CheatEntry cheatEntry);

        std::string getName();
        u32 getID();

        bool toggle();
        bool setState(bool state);
        bool isEnabled();

    private:
        std::string m_name;
        u32 m_id;
    };

    class FrozenAddress {
    public:
        FrozenAddress(dmntcht::FrozenAddressEntry);
        FrozenAddress(addr_t address, u8 width, u64 value);
        FrozenAddress(addr_t address, u8 width);

        addr_t getAddress();
        u8 getWidth();
        u64 getValue();
        u64 setValue(u64 value, u8 width);

        bool toggle();
        bool isFrozen();

    private:
        dmntcht::FrozenAddressEntry m_frozenAddressEntry;
        bool m_frozen;
    };


    class CheatManager {
    public:
        static EResult initialize();
        static void exit();

        static bool isCheatServiceAvailable();

        static bool forceAttach();
        static bool hasCheatProcess();

        static titleid_t getTitleID();
        static processid_t getProcessID();
        static buildid_t getBuildID();

        static types::Region getBaseRegion();
        static types::Region getHeapRegion();
        static types::Region getMainRegion();
        static types::Region getAliasRegion();

        static EResultVal<std::string> getCheatFile();

        static EResultVal<u32> addCheat(dmntcht::CheatDefinition cheatDefinition, bool enabled);
        static EResult removeCheat(u32 cheatID);

        static std::vector<Cheat*>& getCheats();
        static std::vector<FrozenAddress*>& getFrozenAddresses();

        static MemoryInfo queryMemory(addr_t address);
        static std::vector<MemoryInfo> getMemoryRegions();

        static EResult readMemory(addr_t address, void *buffer, size_t bufferSize);
        static EResult writeMemory(addr_t address, const void *buffer, size_t bufferSize);

        static EResult reload();

    private:
        static inline std::vector<Cheat*> s_cheats;
        static inline std::vector<FrozenAddress*> s_frozenAddresses;

        static inline dmntcht::CheatProcessMetadata s_processMetadata;
    };

} 