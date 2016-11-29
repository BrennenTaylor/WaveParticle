#pragma once

#include <d3d11.h>

namespace Farlor
{
    struct RenderTarget
    {
        ID3D11Texture2D *m_pRTTexture;
        ID3D11RenderTargetView *m_pRTView;
        ID3D11ShaderResourceView *m_pRTShaderResourceView;
    };
}
