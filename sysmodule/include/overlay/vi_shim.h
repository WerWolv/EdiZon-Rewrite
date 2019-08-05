#ifndef _VI_SHIM_H
#define _VI_SHIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <switch.h>

// See libtransistor vi.c
Result viDestroyManagedLayer(u64 layer_id);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _VI_SHIM_H
