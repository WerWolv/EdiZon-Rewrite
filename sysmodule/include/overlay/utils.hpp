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
