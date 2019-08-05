#pragma once

#include <switch.h>
#include <result.hpp>

#include <functional>

namespace edz::comms {

    typedef struct {
        u64 magic;  // EDZN
        enum class PacketType : u16 { HEART_BEAT = 0, READ_MEMORY, WRITE_MEMORY } type;
        enum class Origin : u8 { TCP = 'T', USB = 'U' } origin;
        u16 payloadSize;
        u8 payload[0xFFFF - sizeof(magic) - sizeof(PacketType) - sizeof(Origin) - sizeof(payloadSize)];
    } PACKED bridge_packet_t;

    class USBManager {
    public:
        USBManager() = delete;

        static EResult initialize();
        static void exit();

        static void process();

        static void read(std::function<void(u8 *buffer, size_t size)> callback, size_t bufferSize, u32 timeout);
        static void write(u8 *buffer, size_t size, u32 timeout);
    };

}