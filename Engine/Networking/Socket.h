#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

#include "Packet.h"

namespace Farlor
{
    class Socket
    {
    public:
        Socket();

        bool SetupAsServer(int port, int numPending);
        bool ConnectToServer(char* pIP, int port);
        bool AcceptConnection(Socket& clientSocket);

        bool SendData(Packet& packet);
        int RecieveData(Packet& pPacket);

        void Disconnect();

        bool IsConnected();

        // Socket
        SOCKET m_socket;
    };
}
