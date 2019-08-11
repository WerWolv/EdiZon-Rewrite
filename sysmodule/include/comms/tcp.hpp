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
#include "helpers/result.hpp"
#include "comms/packets.hpp"

#include <functional>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>

namespace edz::comms::tcp {

    class TCPManager {
    public:
        TCPManager() = delete;

        static EResult initialize();
        static void exit();

        static void process();

        static void setHandleCallback(std::function<void(edz::comms::common_packet_t*, edz::comms::common_packet_t*)> handleCallback);

    private:
        static inline bool s_initialized = false;
        static inline bool s_busy = false;
        static inline bool s_listening = false;

        static inline int s_serverSocket = 0, s_clientSocket;
        static inline struct sockaddr_in s_serverAddress = { 0 };
        static inline struct sockaddr_in s_clientAddress = { 0 };

        static inline std::function<void(edz::comms::common_packet_t*, edz::comms::common_packet_t*)> s_handleCallback;
    };

}