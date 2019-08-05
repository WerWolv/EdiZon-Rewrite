#pragma once

#include "overlay/color.hpp"

// Handheld and docked
#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define TS_WIDTH  1280
#define TS_HEIGHT 720

// Aligned width * height -> page-aligned (0x1000)
#define FB_WIDTH  256 // Aligned (*bpp) to 64
#define FB_HEIGHT 512 // Aligned to 128

#define LAYER_WIDTH  (SCREEN_HEIGHT * FB_WIDTH / FB_HEIGHT)
#define LAYER_HEIGHT SCREEN_HEIGHT
#define LAYER_X      0.0f
#define LAYER_Y      0.0f

#define TS_LAYER_WIDTH  (LAYER_WIDTH  * TS_WIDTH  / SCREEN_WIDTH)
#define TS_LAYER_HEIGHT (LAYER_HEIGHT * TS_HEIGHT / SCREEN_HEIGHT)
#define TS_LAYER_X      (LAYER_X      * TS_WIDTH  / SCREEN_WIDTH)
#define TS_LAYER_Y      (LAYER_Y      * TS_HEIGHT / SCREEN_HEIGHT)

#define BPP sizeof(rgba4444_t)

#define DEFAULT_FONT_HEIGHT 16
