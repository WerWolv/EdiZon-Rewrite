#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef __uint128_t u128;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64; 
typedef __int128_t s128;


#include "helpers/result.hpp"


namespace edz {
    typedef u64 titleid_t;
    typedef u128 userid_t;

    typedef u64 processid_t;
    typedef u64 buildid_t;

    typedef u64 addr_t;

    typedef struct {
        u64 magic;  // EDZN
        enum class PacketType : u16 { HEART_BEAT = 0, READ_MEMORY, WRITE_MEMORY } type;
        enum class Origin : u8 { TCP = 'T', USB = 'U' } origin;
        u16 payloadSize;
        u8 payload[0xFFFF - sizeof(magic) - sizeof(PacketType) - sizeof(Origin) - sizeof(payloadSize)];
    } PACKED bridge_packet_t;
}