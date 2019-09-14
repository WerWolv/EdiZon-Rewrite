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

#include <switch.h>                 // libnx

#ifndef __SYSMODULE__
    #include <borealis/logger.hpp>      // Borealis Logger
    using Log = brls::Logger;
    
    #include "helpers/lang_entry.hpp"   // Translation helper
#endif

#include "edz_types.hpp"            // Types associated with EdiZon

#include "helpers/macros.hpp"       // Helper macros

using namespace std::string_literals;   // String user defined literals

/* || EDIZON COMPILE TIME SETTINGS || */

/* The EdiZon paths on the SD card */
#define EDIZON_BASE_DIR                 "sdmc:/switch/EdiZon"
    #define EDIZON_BACKUP_DIR               EDIZON_BASE_DIR "/backups"
    #define EDIZON_BATCH_BACKUP_DIR         EDIZON_BASE_DIR "/batch_backups"
    #define EDIZON_COMMON_RESTORE_DIR       EDIZON_BASE_DIR "/restore"
    #define EDIZON_CONFIGS_DIR              EDIZON_BASE_DIR "/configs"
    #define EDIZON_SCRIPTS_DIR              EDIZON_BASE_DIR "/scripts"
    #define EDIZON_LIBS_DIR                 EDIZON_BASE_DIR "/scripts/libs"
    #define EDIZON_CHEATS_DIR               EDIZON_BASE_DIR "/cheats"
    #define EDIZON_TMP_DIR                  EDIZON_BASE_DIR "/tmp"

/* Whether the splash screen should be displayed on launch */
#define SPLASH_ENABLED              true

/* Current web API version */
#define EDIZON_API_VERSION          "v4"
#define SWITCHCHEATSDB_API_VERSION  "v1"

/* Web API URLs */
#define EDIZON_API_URL              "http://api.edizon.werwolv.net/" EDIZON_API_VERSION 
#define SWITCHCHEATSDB_API_URL      "https://www.switchcheatsdb.com/api/" SWITCHCHEATSDB_API_VERSION

/* Debugging */
#define DEBUG_MODE_ENABLED          true
