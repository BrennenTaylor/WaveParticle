#include "Socket.h"

#include <iostream>
#include <string>

using namespace std;

namespace Farlor
{
    Socket::Socket()
        : m_socket{INVALID_SOCKET}
    {
    }

    bool Socket::SetupAsServer(int port, int numPending)
    {
        sockaddr_in address;
        u_long arg = 1;

        m_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (m_socket == INVALID_SOCKET)
        {
            return false;
        }

        memset(&address, 0, sizeof(sockaddr_in));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(port);

        // Turn off blocking
        ioctlsocket(m_socket, FIONBIO, &arg);

        if (bind(m_socket, (sockaddr*)&address, sizeof(sockaddr)) == SOCKET_ERROR)
        {
            cout << "Failed to bind" << endl;
            Disconnect();
            return false;
        }

        if (listen(m_socket, numPending) == SOCKET_ERROR)
        {
            cout << "Failed to listen" << endl;
            return false;
        }

        return true;
    }

    bool Socket::ConnectToServer(char* pIP, int port)
    {
        return true;
    }

    bool Socket::AcceptConnection(Socket& clientSocket)
    {
        sockaddr_in addr;
        int size = sizeof(sockaddr_in);
        memset(&addr, 0, size);
        clientSocket.m_socket = accept(m_socket, (struct sockaddr*)&addr, &size);

        if (clientSocket.m_socket == INVALID_SOCKET)
        {
            return false;
        }

        return true;
    }

    bool Socket::SendData(Packet& packet)
    {
        int sent = -1, size = 0;

        if (packet.m_dataLength <= 0)
        {
            cout << "No data to send" << endl;
            return false;
        }

        size = packet.m_dataLength;
        sent = send(m_socket, packet.m_data, size, 0);

        if (sent == SOCKET_ERROR)
        {
            cout << "Failed to send packet" << endl;
            return false;
        }

        if (sent != size)
        {
            cout << "Only particla data sent" << endl;
        }

        return true;
    }

    int Socket::RecieveData(Packet& packet)
    {
        int lengthToRead = 0;
        // ioctlsocket(m_socket, FIONREAD, &lengthToRead);

        if (lengthToRead > packet.m_dataLength)
        {
            lengthToRead = packet.m_dataLength;
        }

        if (lengthToRead > 0)
        {
            // lengthToRead = recv(m_socket, packet.m_data, lengthToRead);
        }

        return lengthToRead;
    }

    void Socket::Disconnect()
    {
        cout << "Disconnecting Socket" << endl;
        closesocket(m_socket);

        m_socket = INVALID_SOCKET;
    }

    bool Socket::IsConnected()
    {
        return !(m_socket == INVALID_SOCKET);
    }
}
