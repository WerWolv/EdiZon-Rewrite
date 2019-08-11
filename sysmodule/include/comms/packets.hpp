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

#include <switch.h>

namespace edz::comms {

    typedef struct {
        u32 magic;  // EDZN
        enum class PacketType : u16 { HEART_BEAT = 0, GET = 1, SET = 2, CONTINUE = 3 } type;
        enum class Origin : u8 { TCP = 'T', USB = 'U' } origin;
        u16 payloadSize;
        u8 payload[0xFFFF - sizeof(magic) - sizeof(PacketType) - sizeof(Origin) - sizeof(payloadSize)];
    } PACKED common_packet_t;

    static_assert(sizeof(common_packet_t) == 0xFFFF, "Incorrect bridge packet definition!");

}