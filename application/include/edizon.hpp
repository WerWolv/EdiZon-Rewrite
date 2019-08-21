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
#include <Logger.hpp>       // Borealis Logger

#include "edz_types.hpp"    // Types associated with EdiZon

#include "helpers/lang_entry.hpp"   // Translation helper
#include "helpers/macros.hpp"   // Helper macros

/* || EDIZON COMPILE TIME SETTINGS || */

/* The EdiZon folder base path on the SD card */
#define EDIZON_BASE_DIR             "sdmc:/switch/EdiZon"

/* Whether the splash screen should be displayed on launch */
#define SPLASH_ENABLED              true

/* Current web API version */
#define EDIZON_API_VERSION          "v4"
#define SWITCHCHEATSDB_API_VERSION  "v1"

/* Web API URL */
#define EDIZON_API_URL              "http://api.edizon.werwolv.net/" EDIZON_API_VERSION 
#define SWITCHCHEATSDB_API_URL      "https://www.switchcheatsdb.com/api/" SWITCHCHEATSDB_API_VERSION

/* Debugging */
#define DEBUG_MODE_ENABLED          true
