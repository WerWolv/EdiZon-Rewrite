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
#include "edz_types.hpp"            // Types associated with EdiZon
#include "helpers/macros.hpp"       // Helper macros
#include "helpers/version.hpp"      // Version helper

#include <memory>

#ifndef __SYSMODULE__                           // EdiZon Homebrew context

    #include <borealis/logger.hpp>              // Borealis Logger
    using Logger = brls::Logger;
    
    #include "helpers/lang_entry.hpp"           // Translation helper

    /* || EDIZON COMPILE TIME SETTINGS || */

    #define BACKUP_FILE_EXTENSION           ".edz"

    /* Whether the splash screen should be displayed on launch */
    #define SPLASH_ENABLED                  true

    /* The buffer size used when searching a game's memory for values. Bigger values can increase speed but EdiZon might stop working in Applet mode */
    #define MEMORY_SEARCH_BUFFER_SIZE       0x40000

    /* Version settings */
    #define EDIZON_VERSION                  hlp::Version({ VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO })
    #define MINIMUM_AMS_VERSION_REQUIRED    hlp::Version({0,10,0})

    /* Current web API version */
    #define EDIZON_API_VERSION              "v4"
    #define SWITCHCHEATSDB_API_VERSION      "v1"

    /* Web API URLs */
    #define EDIZON_API_URL                  "http://werwolv.net:1337/" EDIZON_API_VERSION 
    #define SWITCHCHEATSDB_API_URL          "https://www.switchcheatsdb.com/api/" SWITCHCHEATSDB_API_VERSION
    #define ANONFILE_API_URL                "https://api.anonfile.com"

    /* Debugging */
    #define DEBUG_MODE_ENABLED              true
    #if DEBUG_MODE_ENABLED
        #define VERBOSE_LOG_OUTPUT              true        /* Enables more detailed log output. Might cause a lot of spam */
        #define REDIRECT_LOG_TO_FILE            false       /* Redirects all log output to a file on the SD card */
        #define LOG_ALLOCATION                  false       /* Overrides operator new and delete to log heap allocation and deallocation */
        #define DISABLE_EXCEPTION_HANDLER       false
    #endif

#else               // EdiZon Sysmodule context

#endif              // EdiZon Homebrew and Sysmodule context
    
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
        #define EDIZON_DUMP_DIR                 EDIZON_BASE_DIR "/dumps"

    #define EDIZON_OVERLAY_FILENAME         "ovlEdiZon.ovl"
    #define EDIZON_ROMFS_OVERLAY_PATH       "romfs:/overlay"
    #define OVERLAYS_PATH                   "sdmc:/switch/.overlays"
    
    #include <chrono>
    using namespace std::literals::chrono_literals;
    using namespace std::string_literals;       // String user defined literals

    /* TitleID of the bundled sysmodule */
    #define EDIZON_SYSMODULE_TITLEID        0x010000000007E51A