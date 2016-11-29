#pragma once

#include "Socket.h"

#include <vector>

namespace Farlor
{
    class NetworkManager
    {
    public:
        NetworkManager();
        ~NetworkManager();

        bool Initialize();
        void Process();

    private:
        Socket m_assetPipelineReciever;
        Socket m_assetPipelineClient;
    };
}
