#pragma once

#include <switch.h>

namespace edz {

    static constexpr u32 Module_Edz = 555;

    static constexpr Result ResultEdzAlreadyRegistered              = MAKERESULT(Module_Edz, 1);
    static constexpr Result ResultEdzUnknownButtonID                = MAKERESULT(Module_Edz, 2);
    static constexpr Result ResultEdzInvalidButtonCombination       = MAKERESULT(Module_Edz, 3);

}