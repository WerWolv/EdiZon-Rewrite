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

#include "comms/tcp.hpp"

#include "helpers/results.hpp"
#include <arpa/inet.h>
#include <cstring>

namespace edz::comms::tcp {

    EResult TCPManager::initialize() {
        TCPManager::s_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (TCPManager::s_serverSocket == -1)
            return edz::ResultEdzSocketInitFailed;

        TCPManager::s_serverAddress.sin_family = AF_INET;
        TCPManager::s_serverAddress.sin_addr.s_addr = INADDR_ANY;
        TCPManager::s_serverAddress.sin_port = htons(555);

        while (bind(TCPManager::s_serverSocket, (struct sockaddr*)&TCPManager::s_serverAddress, sizeof(TCPManager::s_serverAddress)) < 0)
            svcSleepThread(1E9);

        return listen(TCPManager::s_serverSocket, 3);
    }

    void TCPManager::exit() {
        close(TCPManager::s_serverSocket);
    }


    void TCPManager::process() {
        socklen_t addrLength = sizeof(struct sockaddr_in);

        TCPManager::s_clientSocket = accept(TCPManager::s_serverSocket, (struct sockaddr*)&TCPManager::s_clientAddress, &addrLength);

        if (TCPManager::s_clientSocket <= 0) {
            svcSleepThread(1E9);
            TCPManager::exit();
            TCPManager::initialize();

            return;
        }

        while (true) {
            size_t readSize = 0;
            edz::comms::common_packet_t recvPacket;
            memset(&recvPacket, 0, sizeof(edz::comms::common_packet_t));

            while ((readSize = ::recv(TCPManager::s_clientSocket, &recvPacket, sizeof(edz::comms::common_packet_t), 0)) > 0) {
                edz::comms::common_packet_t sendPacket;
                TCPManager::s_handleCallback(&recvPacket, &sendPacket);
                if (send(TCPManager::s_clientSocket, &sendPacket, sizeof(edz::comms::common_packet_t), 0) <= 0) {
                    readSize = -1;
                    break;
                }

                svcSleepThread(10E6);
            }

            svcSleepThread(10E6);

            if (readSize < 1)
                break;
        }
    }

    void TCPManager::setHandleCallback(std::function<void(edz::comms::common_packet_t*, edz::comms::common_packet_t*)> handleCallback) {
        TCPManager::s_handleCallback = handleCallback;
    }

}