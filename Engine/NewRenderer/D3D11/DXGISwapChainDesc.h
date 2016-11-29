#pragma once

#include <dxgi.h>

#include "../../Core/Window.h"

#include "DXGIModeDesc.h"

namespace Farlor
{
    struct DXGISwapChainDesc
    {
    public:
        DXGISwapChainDesc(DXGIModeDesc dxgiModeDesc, GameWindow& gameWindow);

        void Default(DXGIModeDesc dxgiModeDesc, GameWindow& gameWindow);

    public:
        DXGI_SWAP_CHAIN_DESC m_dxgiSwapChainDesc;
    };
}
