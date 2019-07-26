#pragma once

#include <edizon.hpp>

namespace edz::helper {

    namespace CFWEmulation {

        enum class CFW : u8 {
            NONE = 0,
            AMS = 1,
            RNX = 2,
            SXOS = 4
        };

        CFW getEmulatableCFWs();
        void emulate(CFW cfw);

    }

}