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
#include "helpers/result.hpp"

namespace edz {

    #define EDZRES(desc) MAKERESULT(MODULE_EDIZONSYSMODULE, desc)

    static constexpr Result ResultSuccess                           = MAKERESULT(0, 0);

    static constexpr Result ResultEdzAlreadyRegistered              = EDZRES(1);
    static constexpr Result ResultEdzUnknownButtonID                = EDZRES(2);
    static constexpr Result ResultEdzInvalidButtonCombination       = EDZRES(3);
    static constexpr Result ResultEdzAttachFailed                   = EDZRES(4);
    static constexpr Result ResultEdzDetachFailed                   = EDZRES(5);
    static constexpr Result ResultEdzInvalidBuffer                  = EDZRES(6);

    static constexpr Result ResultEdzTCPInitFailed                  = EDZRES(101);
    static constexpr Result ResultEdzUSBInitFailed                  = EDZRES(102);
    static constexpr Result ResultEdzScreenInitFailed               = EDZRES(103);
    static constexpr Result ResultEdzFontInitFailed                 = EDZRES(104);

    static constexpr Result ResultEdzSocketInitFailed               = EDZRES(201);

    static constexpr Result ResultEdzAbortFailed                    = EDZRES(301);

}