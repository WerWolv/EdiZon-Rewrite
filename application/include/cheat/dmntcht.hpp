#pragma once

#include "edizon.hpp"

namespace edz::dmntcht {

    typedef struct {
        u64 base;
        u64 size;
    } DmntMemoryRegionExtents;

    typedef struct {
        u64 process_id;
        u64 title_id;
        DmntMemoryRegionExtents main_nso_extents;
        DmntMemoryRegionExtents heap_extents;
        DmntMemoryRegionExtents alias_extents;
        DmntMemoryRegionExtents address_space_extents;
        u8 main_nso_build_id[0x20];
    } DmntCheatProcessMetadata;

    typedef struct {
        char readable_name[0x40];
        uint32_t num_opcodes;
        uint32_t opcodes[0x100];
    } DmntCheatDefinition;

    typedef struct {
        bool enabled;
        uint32_t cheat_id;
        DmntCheatDefinition definition;
    } DmntCheatEntry;

    typedef struct {
        u64 value;
        u8 width;
    } DmntFrozenAddressValue;

    typedef struct {
        u64 address;
        DmntFrozenAddressValue value;
    } DmntFrozenAddressEntry;

    EResult initialize(void);
    void exit(void);
    Service* getServiceSession(void);

    EResult hasCheatProcess(bool *out);
    EResult getCheatProcessEvent(Event *event);
    EResult getCheatProcessMetadata(DmntCheatProcessMetadata *out_metadata);
    EResult forceOpenCheatProcess(void);

    EResult getCheatProcessMappingCount(u64 *out_count);
    EResult getCheatProcessMappings(MemoryInfo *buffer, u64 max_count, u64 offset, u64 *out_count);
    EResult readCheatProcessMemory(u64 address, void *buffer, size_t size);
    EResult writeCheatProcessMemory(u64 address, const void *buffer, size_t size);
    EResult queryCheatProcessMemory(MemoryInfo *mem_info, u64 address);

    EResult getCheatCount(u64 *out_count);
    EResult getCheats(DmntCheatEntry *buffer, u64 max_count, u64 offset, u64 *out_count);
    EResult getCheatById(DmntCheatEntry *out_cheat, u32 cheat_id);
    EResult toggleCheat(u32 cheat_id);
    EResult addCheat(DmntCheatDefinition *cheat, bool enabled, u32 *out_cheat_id);
    EResult removeCheat(u32 cheat_id);

    EResult getFrozenAddressCount(u64 *out_count);
    EResult getFrozenAddresses(DmntFrozenAddressEntry *buffer, u64 max_count, u64 offset, u64 *out_count);
    EResult getFrozenAddress(DmntFrozenAddressEntry *out, u64 address);
    EResult enableFrozenAddress(u64 address, u64 width, u64 *out_value);
    EResult disableFrozenAddress(u64 address);
    
}