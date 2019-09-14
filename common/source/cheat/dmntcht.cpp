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

#include "cheat/dmntcht.hpp"

namespace edz::dmntcht {
    
    static Service g_dmntchtService;
    static u64 g_refCnt;

    static EResult _getCount(u64 cmd_id, u64 *out_count);
    static EResult _getEntries(u64 cmd_id, void *buffer, u64 buffer_size, u64 offset, u64 *out_count);

    EResult initialize(void) {
        atomicIncrement64(&g_refCnt);

        if (serviceIsActive(&g_dmntchtService)) {
            return 0;
        }

        return smGetService(&g_dmntchtService, "dmnt:cht");
    }

    void exit(void) {
        if (atomicIncrement64(&g_refCnt) == 0)  {
            serviceClose(&g_dmntchtService);
        }
    }

    Service* getServiceSession(void) {
        return &g_dmntchtService;
    }

    EResult hasCheatProcess(bool *out) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65000;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
                bool out;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
            if (R_SUCCEEDED(rc)) {
                if (out) *out = resp->out;
            }
        }

        return rc;
    }

    EResult getCheatProcessEvent(Event *event) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65001;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;

            if (R_SUCCEEDED(rc)) {
                eventLoadRemote(event, r.Handles[0], true);
            }
        }

        return rc;
    }

    EResult getCheatProcessMetadata(CheatProcessMetadata *out_metadata) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65002;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
                CheatProcessMetadata metadata;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
            if (R_SUCCEEDED(rc)) {
                if (out_metadata) *out_metadata = resp->metadata;
            }
        }

        return rc;
    }

    EResult forceOpenCheatProcess(void) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65003;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
        }

        return rc;
    }

    static EResult _getCount(u64 cmd_id, u64 *out_count) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = cmd_id;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
                u64 count;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
            *out_count = resp->count;
        }

        return rc;
    }

    static EResult _getEntries(u64 cmd_id, void *buffer, u64 buffer_size, u64 offset, u64 *out_count) {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, buffer, buffer_size, BufferType::BufferType_Normal);

        struct {
            u64 magic;
            u64 cmd_id;
            u64 offset;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = cmd_id;
        raw->offset = offset;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
                u64 count;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
            if (R_SUCCEEDED(rc)) {
                if (out_count) *out_count = resp->count;
            }
        }

        return rc;
    }

    EResult getCheatProcessMappingCount(u64 *out_count) {
        return _getCount(65100, out_count);
    }

    EResult GetCheatProcessMappings(MemoryInfo *buffer, u64 max_count, u64 offset, u64 *out_count) {
        return _getEntries(65101, buffer, sizeof(*buffer) * max_count, offset, out_count);
    }

    EResult readCheatProcessMemory(u64 address, void *buffer, size_t size) {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, buffer, size, BufferType::BufferType_Normal);

        struct {
            u64 magic;
            u64 cmd_id;
            u64 address;
            u64 size;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65102;
        raw->address = address;
        raw->size = size;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
        }

        return rc;
    }

    EResult writeCheatProcessMemory(u64 address, const void *buffer, size_t size) {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddSendBuffer(&c, buffer, size, BufferType::BufferType_Normal);

        struct {
            u64 magic;
            u64 cmd_id;
            u64 address;
            u64 size;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65103;
        raw->address = address;
        raw->size = size;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
        }

        return rc;
    }

    EResult queryCheatProcessMemory(MemoryInfo *mem_info, u64 address){
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
            u64 address;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65104;
        raw->address = address;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
                MemoryInfo mem_info;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
            if (R_SUCCEEDED(rc)) {
                if (mem_info) *mem_info = resp->mem_info;
            }
        }

        return rc;
    }

    EResult getCheatCount(u64 *out_count) {
        return _getCount(65200, out_count);
    }

    EResult getCheats(CheatEntry *buffer, u64 max_count, u64 offset, u64 *out_count) {
        return _getEntries(65201, buffer, sizeof(*buffer) * max_count, offset, out_count);
    }

    EResult getCheatById(CheatEntry *buffer, u32 cheat_id) {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, buffer, sizeof(*buffer), BufferType::BufferType_Normal);

        struct {
            u64 magic;
            u64 cmd_id;
            u32 cheat_id;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65202;
        raw->cheat_id = cheat_id;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
        }

        return rc;
    }

    EResult toggleCheat(u32 cheat_id) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
            u32 cheat_id;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65203;
        raw->cheat_id = cheat_id;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
        }

        return rc;
    }

    EResult addCheat(CheatDefinition *buffer, bool enabled, u32 *out_cheat_id) {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddSendBuffer(&c, buffer, sizeof(*buffer), BufferType::BufferType_Normal);

        struct {
            u64 magic;
            u64 cmd_id;
            u8 enabled;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65204;
        raw->enabled = enabled;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
                u32 cheat_id;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
            if (R_SUCCEEDED(rc)) {
                if (out_cheat_id) *out_cheat_id = resp->cheat_id;
            }
        }

        return rc;
    }

    EResult removeCheat(u32 cheat_id) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
            u32 cheat_id;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65205;
        raw->cheat_id = cheat_id;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
        }

        return rc;
    }


    EResult getFrozenAddressCount(u64 *out_count) {
        return _getCount(65300, out_count);
    }

    EResult getFrozenAddresses(FrozenAddressEntry *buffer, u64 max_count, u64 offset, u64 *out_count) {
        return _getEntries(65301, buffer, sizeof(*buffer) * max_count, offset, out_count);
    }

    EResult getFrozenAddress(FrozenAddressEntry *out, u64 address) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
            u64 address;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65302;
        raw->address = address;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
                FrozenAddressEntry entry;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
            if (R_SUCCEEDED(rc)) {
                if (out) *out = resp->entry;
            }
        }

        return rc;
    }

    EResult enableFrozenAddress(u64 address, u64 width, u64 *out_value) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
            u64 address;
            u64 width;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65303;
        raw->address = address;
        raw->width = width;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
                u64 value;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
            if (R_SUCCEEDED(rc)) {
                if (out_value) *out_value = resp->value;
            }
        }

        return rc;
    }

    EResult disableFrozenAddress(u64 address) {
        IpcCommand c;
        ipcInitialize(&c);

        struct {
            u64 magic;
            u64 cmd_id;
            u64 address;
        } *raw;

        raw = static_cast<decltype(raw)>(serviceIpcPrepareHeader(&g_dmntchtService, &c, sizeof(*raw)));

        raw->magic = SFCI_MAGIC;
        raw->cmd_id = 65304;
        raw->address = address;

        Result rc = serviceIpcDispatch(&g_dmntchtService);

        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            struct {
                u64 magic;
                u64 result;
                u64 value;
            } *resp;

            serviceIpcParse(&g_dmntchtService, &r, sizeof(*resp));
            resp = static_cast<decltype(resp)>(r.Raw);

            rc = resp->result;
        }

        return rc;
    }
}