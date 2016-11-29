#pragma once

#include <d3d11.h>

namespace Farlor
{
    class RasterizerStateDesc
    {
    public:
        RasterizerStateDesc();
        void Default();

    public:
        D3D11_RASTERIZER_DESC m_rasterDesc;
    };
}
