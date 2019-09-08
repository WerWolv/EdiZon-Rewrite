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

    #define EDZRES(desc) MAKERESULT(MODULE_EDIZON, desc)

    constexpr Result ResultSuccess                          = MAKERESULT(0, 0);

    constexpr Result ResultEdzBorealisInitFailed            = EDZRES(1);
    constexpr Result ResultEdzCurlInitFailed                = EDZRES(2);
    constexpr Result ResultEdzNotYetImplemented             = EDZRES(3);

    constexpr Result ResultEdzSysmoduleAlreadyRunning       = EDZRES(101);
    constexpr Result ResultEdzSysmoduleNotRunning           = EDZRES(102);
    constexpr Result ResultEdzSysmoduleLaunchFailed         = EDZRES(103);
    constexpr Result ResultEdzSysmoduleTerminationFailed    = EDZRES(104);

    constexpr Result ResultEdzEditorNoConfigFound           = EDZRES(201);
    constexpr Result ResultEdzEditorNoScriptFound           = EDZRES(202);
    constexpr Result ResultEdzEditorConfigOutdated          = EDZRES(203);
    constexpr Result ResultEdzEditorScriptSyntaxError       = EDZRES(204);
    constexpr Result ResultEdzEditorTooManyRedirects        = EDZRES(205);
    constexpr Result ResultEdzEditorAlreadyLoaded           = EDZRES(206);
    constexpr Result ResultEdzEditorNotLoaded               = EDZRES(207);
    constexpr Result ResultEdzEditorLoadFailed              = EDZRES(208);
    constexpr Result ResultEdzEditorStoreFailed             = EDZRES(209);

    constexpr Result ResultEdzScriptRuntimeError            = EDZRES(301);
    constexpr Result ResultEdzScriptInvalidArgument         = EDZRES(302);

    constexpr Result ResultEdzCurlError                     = EDZRES(401);
    constexpr Result ResultEdzAPIError                      = EDZRES(402);

    constexpr Result ResultEdzSaveNoSaveFS                  = EDZRES(501);
    constexpr Result ResultEdzSaveNoSuchBackup              = EDZRES(502);

    constexpr Result ResultEdzCheatParsingFailed            = EDZRES(601);
}