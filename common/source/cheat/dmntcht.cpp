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

#include "cheat/dmntcht.hpp"

namespace edz::dmntcht {
    
    static Service g_dmntchtSrv;

    EResult initialize(void) {
        return smGetService(&g_dmntchtSrv, "dmnt:cht");
    }

    void exit(void) {
        serviceClose(&g_dmntchtSrv);
    }

    Service* getServiceSession(void) {
        return &g_dmntchtSrv;
    }

    EResult hasCheatProcess(bool *out) {
        u8 tmp = 0;
        EResult rc = serviceDispatchOut(&g_dmntchtSrv, 65000, tmp);
        if (R_SUCCEEDED(rc) && out) *out = tmp & 1;
        return rc;
    }

    EResult getCheatProcessEvent(Event *event) {
        Handle evt_handle = INVALID_HANDLE;
        EResult rc = serviceDispatch(&g_dmntchtSrv, 65001,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &evt_handle,
        );

        if (R_SUCCEEDED(rc)) {
            eventLoadRemote(event, evt_handle, true);
        }

        return rc;
    }

    EResult getCheatProcessMetadata(CheatProcessMetadata *out_metadata) {
        return serviceDispatchOut(&g_dmntchtSrv, 65002, *out_metadata);
    }

    EResult forceOpenCheatProcess(void) {
        return serviceDispatch(&g_dmntchtSrv, 65003);
    }

    static EResult getCount(u64 *out_count, u32 cmd_id) {
        return serviceDispatchOut(&g_dmntchtSrv, cmd_id, *out_count);
    }

    static EResult getEntries(void *buffer, u64 buffer_size, u64 offset, u64 *out_count, u32 cmd_id) {
        return serviceDispatchInOut(&g_dmntchtSrv, cmd_id, offset, *out_count,
            .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias },
            .buffers = { { buffer, buffer_size } },
        );
    }

    static EResult cmdInU32NoOut(u32 in, u32 cmd_id) {
        return serviceDispatchIn(&g_dmntchtSrv, cmd_id, in);
    }

    EResult getCheatProcessMappingCount(u64 *out_count) {
        return getCount(out_count, 65100);
    }

    EResult getCheatProcessMappings(MemoryInfo *buffer, u64 max_count, u64 offset, u64 *out_count) {
        return getEntries(buffer, sizeof(*buffer) * max_count, offset, out_count, 65101);
    }

    EResult readCheatProcessMemory(u64 address, void *buffer, size_t size) {
        const struct {
            u64 address;
            u64 size;
        } in = { address, size };
        return serviceDispatchIn(&g_dmntchtSrv, 65102, in,
            .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias },
            .buffers = { { buffer, size } },
        );
    }

    EResult writeCheatProcessMemory(u64 address, const void *buffer, size_t size) {
        const struct {
            u64 address;
            u64 size;
        } in = { address, size };
        return serviceDispatchIn(&g_dmntchtSrv, 65103, in,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcMapAlias },
            .buffers = { { buffer, size } },
        );
    }

    EResult queryCheatProcessMemory(MemoryInfo *mem_info, u64 address){
        return serviceDispatchInOut(&g_dmntchtSrv, 65104, address, *mem_info);
    }

    EResult getCheatCount(u64 *out_count) {
        return getCount(out_count, 65200);
    }

    EResult getCheats(CheatEntry *buffer, u64 max_count, u64 offset, u64 *out_count) {
        return getEntries(buffer, sizeof(*buffer) * max_count, offset, out_count, 65201);
    }

    EResult getCheatById(CheatEntry *out, u32 cheat_id) {
        return serviceDispatchIn(&g_dmntchtSrv, 65202, cheat_id,
            .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias | SfBufferAttr_FixedSize },
            .buffers = { { out, sizeof(*out) } },
        );
    }

    EResult toggleCheat(u32 cheat_id) {
        return cmdInU32NoOut(cheat_id, 65203);
    }

    EResult addCheat(CheatDefinition *cheat_def, bool enabled, u32 *out_cheat_id) {
        const u8 in = enabled != 0;
        return serviceDispatchInOut(&g_dmntchtSrv, 65204, in, *out_cheat_id,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcMapAlias | SfBufferAttr_FixedSize },
            .buffers = { { cheat_def, sizeof(*cheat_def) } },
        );
    }

    EResult removeCheat(u32 cheat_id) {
        return cmdInU32NoOut(cheat_id, 65205);
    }

    EResult getFrozenAddressCount(u64 *out_count) {
        return getCount(out_count, 65300);
    }

    EResult getFrozenAddresses(FrozenAddressEntry *buffer, u64 max_count, u64 offset, u64 *out_count) {
        return getEntries(buffer, sizeof(*buffer) * max_count, offset, out_count, 65301);
    }

    EResult getFrozenAddress(FrozenAddressEntry *out, u64 address) {
        return serviceDispatchInOut(&g_dmntchtSrv, 65302, address, *out);
    }

    EResult enableFrozenAddress(u64 address, u64 width, u64 *out_value) {
        const struct {
            u64 address;
            u64 width;
        } in = { address, width };
        return serviceDispatchInOut(&g_dmntchtSrv, 65303, in, *out_value);
    }

    EResult disableFrozenAddress(u64 address) {
        return serviceDispatchIn(&g_dmntchtSrv, 65304, address);
    }

}