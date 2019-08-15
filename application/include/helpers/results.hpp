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

    constexpr Result ResultSuccess                          = MAKERESULT(0, 0);

    constexpr Result ResultEdzBorealisInitFailed            = MAKERESULT(MODULE_EDIZON, 1);
    constexpr Result ResultEdzCurlInitFailed                = MAKERESULT(MODULE_EDIZON, 2);

    constexpr Result ResultEdzSysmoduleAlreadyRunning       = MAKERESULT(MODULE_EDIZON, 101);
    constexpr Result ResultEdzSysmoduleNotRunning           = MAKERESULT(MODULE_EDIZON, 102);
    constexpr Result ResultEdzSysmoduleLaunchFailed         = MAKERESULT(MODULE_EDIZON, 103);
    constexpr Result ResultEdzSysmoduleTerminationFailed    = MAKERESULT(MODULE_EDIZON, 104);

    constexpr Result ResultEdzEditorNoConfigFound           = MAKERESULT(MODULE_EDIZON, 201);
    constexpr Result ResultEdzEditorNoScriptFound           = MAKERESULT(MODULE_EDIZON, 202);
    constexpr Result ResultEdzEditorConfigOutdated          = MAKERESULT(MODULE_EDIZON, 203);
    constexpr Result ResultEdzEditorScriptSyntaxError       = MAKERESULT(MODULE_EDIZON, 204);
    constexpr Result ResultEdzEditorTooManyRedirects        = MAKERESULT(MODULE_EDIZON, 205);
    constexpr Result ResultEdzEditorAlreadyLoaded           = MAKERESULT(MODULE_EDIZON, 206);
    constexpr Result ResultEdzEditorNotLoaded               = MAKERESULT(MODULE_EDIZON, 207);
    constexpr Result ResultEdzEditorLoadFailed              = MAKERESULT(MODULE_EDIZON, 208);
    constexpr Result ResultEdzEditorStoreFailed             = MAKERESULT(MODULE_EDIZON, 209);

    constexpr Result ResultEdzScriptRuntimeError            = MAKERESULT(MODULE_EDIZON, 301);
    constexpr Result ResultEdzScriptInvalidArgument         = MAKERESULT(MODULE_EDIZON, 302);

}