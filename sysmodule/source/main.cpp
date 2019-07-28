#include <switch.h>

#include <stratosphere.hpp>

#include "edz_vc_service.hpp"

extern "C" {

    u32 __nx_applet_type = AppletType_None;

    #define INNER_HEAP_SIZE 0x80000
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_initheap(void) {
        void*  addr = nx_inner_heap;
        size_t size = nx_inner_heap_size;

        // Newlib
        extern char* fake_heap_start;
        extern char* fake_heap_end;

        fake_heap_start = (char*)addr;
        fake_heap_end   = (char*)addr + size;
    }

    void __attribute__((weak)) __appInit(void) {
        Result rc;

        rc = hidInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

        rc = fsInitialize();
        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

        fsdevMountSdmc();
    }

    void __attribute__((weak)) userAppExit(void);

    void __attribute__((weak)) __appExit(void) {
        fsdevUnmountAll();
        fsExit();
        timeExit();
        hidExit();
        smExit();
    }

}

static void netLoop(void *args) {
    while (true) {
        svcSleepThread(1E6); // Sleep 1ms
    }
} 

static void usbLoop(void *args) {
    while (true) {
        svcSleepThread(1E6); // Sleep 1ms
    }
} 

static void hidLoop(void *args) {
    while (true) {
        svcSleepThread(1E6); // Sleep 1ms
    }
} 

int main(int argc, char* argv[]) {
    Thread netThread, usbThread, hidThread;

    threadCreate(&netThread, netLoop, nullptr, 0x500, 0x2C, -2);
    threadCreate(&usbThread, usbLoop, nullptr, 0x500, 0x2C, -2);
    threadCreate(&hidThread, hidLoop, nullptr, 0x500, 0x2C, -2);

    threadStart(&netThread);
    threadStart(&usbThread);
    threadStart(&hidThread);

    consoleDebugInit(debugDevice_SVC);

    static auto serverManager = WaitableManager(1);
    serverManager.AddWaitable(new ServiceServer<edz::vc::EdzVCService>("edz:vc", 1));
    
    serverManager.Process();
    
    return 0;
}
