#include "helpers/cfw_emulation.hpp"
#include "helpers/utils.hpp"

namespace edz::helper {

    namespace CFWEmulation {
        
        CFW operator|(CFW l, CFW r) {
            return static_cast<CFW>(static_cast<u8>(l) | static_cast<u8>(r));
        }
        /*
                    Not emulated        Emulated
            AMS         edz                - 
            RNX     rnx, tx             rnx, tx
            SX      tx                    tx
        */
        bool canEmulateCFW() {
            // This feature is based on being able to launch the rnx and tx service. Since one or more of them are present on non-atmosphere, this is only supported on atmosphere
            return isServiceRunning("edz");
        }

        void emulate(CFW cfw) {
            if (!canEmulateCFW()) return;



        }

    }

}