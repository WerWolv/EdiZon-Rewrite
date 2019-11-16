/**
 * Copyright (C) 2019 averne
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

#include <switch.h>

#include "helpers/vi_shim.hpp"

Result viDestroyManagedLayer(u64 layer_id) {
    Service *serv = viGetSession_IManagerDisplayService();
    if (!serviceIsActive(serv))
        return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        u32 layer_id; // u64? Gives the same result
    } *raw;

    raw = static_cast<decltype(raw)>(ipcPrepareHeader(&c, sizeof(*raw)));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 2011;
    raw->layer_id = (u32)layer_id;

    Result rc = serviceIpcDispatch(serv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = static_cast<decltype(resp)>(r.Raw);

        rc = resp->result;
    }

    return rc;
}
