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

#include "helpers/cfw_emulation.hpp"
#include "helpers/utils.hpp"

namespace edz::hlp {

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