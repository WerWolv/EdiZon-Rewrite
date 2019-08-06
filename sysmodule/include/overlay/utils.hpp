/**
 * Copyright (C) 2019 averne
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

#define MAX(x, y) (((x) > (y)) ? (x) :  (y))
#define MIN(x, y) (((x) < (y)) ? (x) :  (y))
#define ABS(x)    (((x) > 0) ?   (x) : -(x))

#define VEC_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

#define ASSERT_SIZE(x, sz)        static_assert(sizeof(x) == sz, "Wrong size in " #x " (should be " #sz ")")
#define ASSERT_STANDARD_LAYOUT(x) static_assert(std::is_standard_layout_v<x>, #x " is not standard layout")

#define TRY_FATAL(exp) ({ \
    if (Result rc = exp; R_FAILED(rc)) \
        fatal(rc); \
})

#define INIT_SERV(s, ...) TRY_FATAL(s##Initialize(__VA_ARGS__))
