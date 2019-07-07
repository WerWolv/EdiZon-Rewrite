#pragma once

#include <switch.h>

#include "edz_types.hpp"

#define DEBUG_PRINT(x, ...) (printf("[DEBUG] %s:%d | " x "\n", __PRETTY_FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__))

/* || EDIZON COMPILE TIME SETTINGS || */

/* The EdiZon folder base path on the SD card */
#define EDIZON_BASE_DIR "/switch/EdiZon"

/* Whether the splash screen should be displayed on launch */
#define SPLASH_ENABLED 1

/* Current web API version */
#define API_VERSION "v4"

/* Web API URL */
#define EDIZON_URL "http://api.edizon.werwolv.net/" API_VERSION 