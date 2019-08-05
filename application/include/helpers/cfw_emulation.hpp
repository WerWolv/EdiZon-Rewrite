#pragma once

#include <edizon.hpp>

namespace edz::helper {

    namespace CFWEmulation {

        enum class CFW : u8 {
            AMS = 1,
            RNX = 2,
            SXOS = 4
        };

        bool canEmulateCFW();
        void emulate(CFW cfw);

    }

}