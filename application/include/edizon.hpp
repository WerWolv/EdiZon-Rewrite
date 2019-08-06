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

#include <switch.h>         // libnx
#include <Borealis.hpp>

#include "edz_types.hpp"

#include "helpers/lang_entry.hpp"

#define DEBUG_PRINT(x, ...) (printf("[DEBUG] %s:%d | " x "\n", __PRETTY_FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__))
#define TRY(x) if (::edz::EResult(x).failed()) return x; 

/* || EDIZON COMPILE TIME SETTINGS || */

/* The EdiZon folder base path on the SD card */
#define EDIZON_BASE_DIR "/switch/EdiZon"

/* Whether the splash screen should be displayed on launch */
#define SPLASH_ENABLED 1

/* Current web API version */
#define API_VERSION "v4"

/* Web API URL */
#define EDIZON_URL "http://api.edizon.werwolv.net/" API_VERSION 