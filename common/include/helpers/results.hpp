/**
 * Copyright (C) 2020 WerWolv
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

    #define EDZHBRES(desc)  MAKERESULT(MODULE_EDIZONHB, desc)
    #define EDZSYSRES(desc) MAKERESULT(MODULE_EDIZONSYS, desc)

    /* Common error codes */
    constexpr Result ResultSuccess                          = MAKERESULT(0, 0);

    /* Homebrew error codes */
    constexpr Result ResultEdzBorealisInitFailed            = EDZHBRES(1);
    constexpr Result ResultEdzCurlInitFailed                = EDZHBRES(2);
    constexpr Result ResultEdzNotYetImplemented             = EDZHBRES(3);
    constexpr Result ResultEdzErrorDuringErrorHandling      = EDZHBRES(4);
    constexpr Result ResultEdzNotFound                      = EDZHBRES(5);

    constexpr Result ResultEdzSysmoduleAlreadyRunning       = EDZHBRES(101);
    constexpr Result ResultEdzSysmoduleNotRunning           = EDZHBRES(102);
    constexpr Result ResultEdzSysmoduleLaunchFailed         = EDZHBRES(103);
    constexpr Result ResultEdzSysmoduleTerminationFailed    = EDZHBRES(104);

    constexpr Result ResultEdzEditorNoConfigFound           = EDZHBRES(201);
    constexpr Result ResultEdzEditorNoScriptFound           = EDZHBRES(202);
    constexpr Result ResultEdzEditorConfigOutdated          = EDZHBRES(203);
    constexpr Result ResultEdzEditorScriptSyntaxError       = EDZHBRES(204);
    constexpr Result ResultEdzEditorTooManyRedirects        = EDZHBRES(205);
    constexpr Result ResultEdzEditorAlreadyLoaded           = EDZHBRES(206);
    constexpr Result ResultEdzEditorNotLoaded               = EDZHBRES(207);
    constexpr Result ResultEdzEditorLoadFailed              = EDZHBRES(208);
    constexpr Result ResultEdzEditorStoreFailed             = EDZHBRES(209);

    constexpr Result ResultEdzScriptRuntimeError            = EDZHBRES(301);
    constexpr Result ResultEdzScriptInvalidArgument         = EDZHBRES(302);

    constexpr Result ResultEdzCurlError                     = EDZHBRES(401);
    constexpr Result ResultEdzAPIError                      = EDZHBRES(402);

    constexpr Result ResultEdzSaveNoSaveFS                  = EDZHBRES(501);
    constexpr Result ResultEdzSaveNoSuchBackup              = EDZHBRES(502);
    constexpr Result ResultEdzSaveInvalidArgument           = EDZHBRES(503);

    constexpr Result ResultEdzCheatParsingFailed            = EDZHBRES(601);
    constexpr Result ResultEdzCheatServiceNotAvailable      = EDZHBRES(602);

    constexpr Result ResultEdzMemoryReadFailed              = EDZHBRES(701);
    constexpr Result ResultEdzMemoryOverflow                = EDZHBRES(702);
    constexpr Result ResultEdzOutOfRange                    = EDZHBRES(703);
    constexpr Result ResultEdzNoValuesFound                 = EDZHBRES(704);
    constexpr Result ResultEdzInvalidOperation              = EDZHBRES(705);

    /* Sysmodule error codes */
    constexpr Result ResultEdzAlreadyRegistered              = EDZSYSRES(1);
    constexpr Result ResultEdzUnknownButtonID                = EDZSYSRES(2);
    constexpr Result ResultEdzInvalidButtonCombination       = EDZSYSRES(3);
    constexpr Result ResultEdzAttachFailed                   = EDZSYSRES(4);
    constexpr Result ResultEdzDetachFailed                   = EDZSYSRES(5);
    constexpr Result ResultEdzInvalidBuffer                  = EDZSYSRES(6);

    constexpr Result ResultEdzTCPInitFailed                  = EDZSYSRES(101);
    constexpr Result ResultEdzUSBInitFailed                  = EDZSYSRES(102);
    constexpr Result ResultEdzScreenInitFailed               = EDZSYSRES(103);
    constexpr Result ResultEdzFontInitFailed                 = EDZSYSRES(104);

    constexpr Result ResultEdzSocketInitFailed               = EDZSYSRES(201);

    constexpr Result ResultEdzAbortFailed                    = EDZSYSRES(301);
}