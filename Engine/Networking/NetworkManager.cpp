#include "NetworkManager.h"

#include <iostream>

using namespace std;

namespace Farlor
{
    NetworkManager::NetworkManager()
    {
    }

    NetworkManager::~NetworkManager()
    {
        WSACleanup();
    }

    bool NetworkManager::Initialize()
    {
        WSADATA wsaData;
        int error = WSAStartup(0x0202, &wsaData);

        if (error)
        {
            cout << "Failed to start up WSA" << endl;
            return false;
        }

        m_assetPipelineReciever.SetupAsServer(8888, 32);

        return true;
    }

    void NetworkManager::Process()
    {
        if (!m_assetPipelineClient.IsConnected())
        {
            if (m_assetPipelineReciever.AcceptConnection(m_assetPipelineClient))
            {
                cout << "We have accepted a connection" << endl;
            }
        }
        else
        {
            Packet recieveData(512);
            int numRecieved = m_assetPipelineClient.RecieveData(recieveData);
            if (numRecieved > 0)
            {
                Packet message("Confirmation Response", 21);

                cout << "Recieved Message: " << recieveData.m_data << endl;
                m_assetPipelineClient.SendData(message);
            }
        }
    }
}
