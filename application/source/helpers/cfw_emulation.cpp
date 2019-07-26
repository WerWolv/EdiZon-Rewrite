#include "helpers/cfw_emulation.hpp"
#include "helpers/utils.hpp"

namespace edz::helper {

    namespace CFWEmulation {
        
        CFW operator|(CFW l, CFW r) {
            return static_cast<CFW>(static_cast<u8>(l) | static_cast<u8>(r));
        }

        CFW getEmulatableCFWs() {
            // ReiNX has both the rnx and tx servoce running already. Since those shouldn't be killed, CFW emulation cannot be used here
            if (isOnRNX())
                return CFW::RNX;

            // SXOS has only the tx service running an can therefor at least emulate ReiNX
            if (isOnSXOS())
                return CFW::SXOS | CFW::RNX;

            // Atmosphere has neither the rnx nor the tx service running and can therefor emulate ReiNX and SXOS
            if (isOnAMS())
                return CFW::AMS | CFW::RNX | CFW::SXOS;

            return CFW::NONE;
        }

        void emulate(CFW cfw) {
            if ((getEmulatableCFWs() | cfw) == CFW::NONE) return;



        }

    }

}