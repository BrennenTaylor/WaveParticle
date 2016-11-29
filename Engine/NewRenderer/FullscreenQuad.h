#pragma once

#include <d3d11.h>

namespace Farlor
{
    class FullScreenQuad
    {
    public:
        void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
        void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pWSRView, ID3D11SamplerState* pWPSampleState);

    private:
        ID3D11VertexShader* m_pVertexShader;
        ID3D11PixelShader* m_pPixelShader;
    };
}
