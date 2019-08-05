#pragma once

#include <switch.h>
#include <stratosphere.hpp>

namespace edz::vc {
    
    using VirtualControllerButtonID = u32;

    enum class VirtualControllerAction {
        PAUSE,
        HOLD,
        PULSE,
        STROBE
    };


    struct VirtualControllerPause {
        u32 duration;
    };

    struct VirtualControllerHold {
        u64 buttons;
        u32 duration;
        bool state;
    };

    struct VirtualControllerPulse {
        u64 buttons;
        u32 duration;
    };

    struct VirtualControllerStrobe {
        u64 buttons;
        u32 amount;
    };


    struct VirtualControllerFrame {
        VirtualControllerAction action;

        union {
            VirtualControllerPause pauseAction;
            VirtualControllerHold holdAction;
            VirtualControllerPulse pulseAction;
            VirtualControllerStrobe strobeAction;
        };
    };

}