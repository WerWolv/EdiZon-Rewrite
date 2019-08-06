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

namespace edz {

    static constexpr u32 Module_Edz = 555;

    static constexpr Result ResultEdzAlreadyRegistered              = MAKERESULT(Module_Edz, 1);
    static constexpr Result ResultEdzUnknownButtonID                = MAKERESULT(Module_Edz, 2);
    static constexpr Result ResultEdzInvalidButtonCombination       = MAKERESULT(Module_Edz, 3);
    static constexpr Result ResultEdzAttachFailed                   = MAKERESULT(Module_Edz, 4);
    static constexpr Result ResultEdzDetachFailed                   = MAKERESULT(Module_Edz, 5);
    static constexpr Result ResultEdzInvalidBuffer                  = MAKERESULT(Module_Edz, 6);

}