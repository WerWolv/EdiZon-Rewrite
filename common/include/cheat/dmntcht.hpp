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

namespace edz::dmntcht {

    typedef struct {
        u64 base;
        u64 size;
    } MemoryRegionExtents;

    typedef struct {
        u64 process_id;
        u64 title_id;
        MemoryRegionExtents main_nso_extents;
        MemoryRegionExtents heap_extents;
        MemoryRegionExtents alias_extents;
        MemoryRegionExtents address_space_extents;
        u8 main_nso_build_id[0x20];
    } CheatProcessMetadata;

    typedef struct {
        char readable_name[0x40];
        uint32_t num_opcodes;
        uint32_t opcodes[0x100];
    } CheatDefinition;

    typedef struct {
        bool enabled;
        uint32_t cheat_id;
        CheatDefinition definition;
    } CheatEntry;

    typedef struct {
        u64 value;
        u8 width;
    } FrozenAddressValue;

    typedef struct {
        u64 address;
        FrozenAddressValue value;
    } FrozenAddressEntry;

    EResult initialize(void);
    void exit(void);
    Service* getServiceSession(void);

    EResult hasCheatProcess(bool *out);
    EResult getCheatProcessEvent(Event *event);
    EResult getCheatProcessMetadata(CheatProcessMetadata *out_metadata);
    EResult forceOpenCheatProcess(void);

    EResult getCheatProcessMappingCount(u64 *out_count);
    EResult getCheatProcessMappings(MemoryInfo *buffer, u64 max_count, u64 offset, u64 *out_count);
    EResult readCheatProcessMemory(u64 address, void *buffer, size_t size);
    EResult writeCheatProcessMemory(u64 address, const void *buffer, size_t size);
    EResult queryCheatProcessMemory(MemoryInfo *mem_info, u64 address);

    EResult getCheatCount(u64 *out_count);
    EResult getCheats(CheatEntry *buffer, u64 max_count, u64 offset, u64 *out_count);
    EResult getCheatById(CheatEntry *out_cheat, u32 cheat_id);
    EResult toggleCheat(u32 cheat_id);
    EResult addCheat(CheatDefinition *cheat, bool enabled, u32 *out_cheat_id);
    EResult removeCheat(u32 cheat_id);

    EResult getFrozenAddressCount(u64 *out_count);
    EResult getFrozenAddresses(FrozenAddressEntry *buffer, u64 max_count, u64 offset, u64 *out_count);
    EResult getFrozenAddress(FrozenAddressEntry *out, u64 address);
    EResult enableFrozenAddress(u64 address, u64 width, u64 *out_value);
    EResult disableFrozenAddress(u64 address);
    
}