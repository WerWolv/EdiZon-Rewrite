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