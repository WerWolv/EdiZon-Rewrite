/**
 * Copyright (C) 2018 libnx Authors
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

typedef struct {
    u8 bInterfaceClass;
    u8 bInterfaceSubClass;
    u8 bInterfaceProtocol;
} UsbCommsModInterfaceInfo;

typedef s32 (*AbortCallbackFn)();

/// Initializes usbCommsMod with the default number of interfaces (1)
Result usbCommsModInitialize(void);

/// Initializes usbCommsMod with a specific number of interfaces.
Result usbCommsModInitializeEx(u32 num_interfaces, const UsbCommsModInterfaceInfo *infos);

/// Exits usbCommsMod.
void usbCommsModExit(void);

/// Sets whether to throw a fatal error in usbCommsMod{Read/Write}* on failure, or just return the transferred size. By default (false) the latter is used.
void usbCommsModSetErrorHandling(bool flag);

/// Read data with the default interface.
size_t usbCommsModRead(void* buffer, size_t size, AbortCallbackFn callback);

/// Write data with the default interface.
size_t usbCommsModWrite(const void* buffer, size_t size, AbortCallbackFn callback);

/// Same as usbCommsModRead except with the specified interface.
size_t usbCommsModReadEx(void* buffer, size_t size, u32 interface, AbortCallbackFn callback);

/// Same as usbCommsModWrite except with the specified interface.
size_t usbCommsModWriteEx(const void* buffer, size_t size, u32 interface, AbortCallbackFn callback);
