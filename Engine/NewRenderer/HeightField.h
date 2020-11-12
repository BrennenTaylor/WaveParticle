#pragma once

#include <d3d11.h>

#include "CBs.h"

#include <Math/Math.h>

namespace Farlor
{
    class HeightField
    {
    public:
        struct cbPerObject
        {
            DirectX::XMMATRIX WVP;
        };

    public:
        HeightField();

        void Initialize(int width, int height, const Farlor::Vector3& corner, uint32_t numGridCells, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

        void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pWPVB1SRV, ID3D11ShaderResourceView* pWPVB2SRV, ID3D11SamplerState* pWPSampleState, ID3D11Buffer* pCameraCBBuffer);

    public:
        int m_width;
        int m_height;

        Farlor::Vector3 m_corner;

        uint32_t m_numGridCells;

        int m_vertexCount;
        int m_indexCount;

    private:
        void InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

    private:
        ID3D11Buffer* m_pVertexBuffer;
        ID3D11Buffer* m_pIndexBuffer;

        ID3D11VertexShader* m_pPositionColorVertexShader;
        ID3D11PixelShader* m_pPositionColorPixelShader;
        ID3D11InputLayout* m_pPositionColorInputLayout;

        ID3D11Buffer* m_cbPerObjectBuffer;
        cbPerObject m_cbPerObject;
    };
}
