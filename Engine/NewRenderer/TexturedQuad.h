#pragma once

#include <d3d11.h>

#include "CBs.h"

#include <Math/Math.h>

namespace Farlor
{
    class TexturedQuad
    {
    public:
        struct cbPerObject
        {
            DirectX::XMMATRIX WVP;
        };

    public:
        TexturedQuad(const Farlor::Vector3& bottomLeft, const Farlor::Vector3& topLeft,
            const Farlor::Vector3& topRight, const Farlor::Vector3& bottomRight);

        void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

        void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pTextureSRV,
            ID3D11SamplerState* pSampleState, ID3D11Buffer* pCameraCBBuffer);

    public:
        Farlor::Vector3 m_bottomLeft;
        Farlor::Vector3 m_topLeft;
        Farlor::Vector3 m_topRight;
        Farlor::Vector3 m_bottomRight;

        int m_vertexCount;
        int m_indexCount;

    private:
        void InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

    private:
        ID3D11Buffer* m_pVertexBuffer;
        ID3D11Buffer* m_pIndexBuffer;

        ID3D11VertexShader* m_pVertexShader;
        ID3D11PixelShader* m_pPixelShader;
        ID3D11InputLayout* m_pInputLayout;

        ID3D11Buffer* m_cbPerObjectBuffer;
        cbPerObject m_cbPerObject;
    };
}
