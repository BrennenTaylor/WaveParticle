#pragma once

#include <dxgi.h>

namespace Farlor
{
    struct DXGIModeDesc
    {
    public:
        DXGIModeDesc(int width, int height);

        void Default(int width, int height);

    public:
        DXGI_MODE_DESC m_dxgiModeDesc;
    };
}
