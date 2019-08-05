#include <switch.h>

#include "overlay/vi_shim.h"

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

    raw = ipcPrepareHeader(&c, sizeof(*raw));
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
        } *resp = r.Raw;

        rc = resp->result;
    }

    return rc;
}
