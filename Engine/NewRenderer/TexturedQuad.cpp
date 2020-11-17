// NOTE: For some fucking reason this must be included before any d3d11 stuff gets included. Why you ask? No clue.
#include <d3dcompiler.h>
#include "TexturedQuad.h"

#include "Renderer.h"
#include "Vertex.h"

#include <DirectXMath.h>

#include "../Util/Logger.h"

namespace Farlor
{
    extern Renderer g_RenderingSystem;

    TexturedQuad::TexturedQuad(const Farlor::Vector3& bottomLeft, const Farlor::Vector3& topLeft,
        const Farlor::Vector3& topRight, const Farlor::Vector3& bottomRight)
        : m_bottomLeft{ bottomLeft }
        , m_topLeft{ topLeft }
        , m_topRight{ topRight }
        , m_bottomRight{ bottomRight }
        , m_vertexCount{0}
        , m_indexCount{0}
        , m_pVertexBuffer{nullptr}
        , m_pIndexBuffer{nullptr}
        , m_pVertexShader{nullptr}
        , m_pPixelShader{nullptr}
        , m_pInputLayout{nullptr}
        , m_cbPerObjectBuffer{nullptr}
        , m_cbPerObject{}
    {
    }

    void TexturedQuad::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result = ERROR_SUCCESS;

        // NOTE: TexturedQuad is in world space, y axis is up
        // Bottom left corner is at (corner.x, corner.y, corner.z)
        // Top right is (corner.x + m_width, corner.y, corner.z + m_height)
        // For mapping purposes, bottom left uv is (0, 0)
        // Top right is (1, 1)
        ID3DBlob* pErrorMessage = nullptr;
        ID3DBlob* pVertexShaderBuffer = nullptr;
        ID3DBlob* pPixelShaderBuffer = nullptr;
        D3D11_BUFFER_DESC matrixBufferDesc = {0};

        std::wstring vertexShaderName = L"resources/shaders/TexturedQuad.hlsl";
        std::wstring pixelShaderName = L"resources/shaders/TexturedQuad.hlsl";

        result = D3DCompileFromFile(vertexShaderName.c_str(), 0, 0, "VSMain", "vs_5_0", 0, 0, &pVertexShaderBuffer, &pErrorMessage);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to compile vertex shader: resources/shaders/TexturedQuad.hlsl")
            FARLOR_LOG_ERROR((char*)pErrorMessage->GetBufferPointer())
            return;
        }

        result = D3DCompileFromFile(pixelShaderName.c_str(), 0, 0, "PSMain", "ps_5_0", 0, 0, &pPixelShaderBuffer, &pErrorMessage);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to compile pixel shader: resources/shaders/TexturedQuad.hlsl")
            FARLOR_LOG_ERROR((char*)pErrorMessage->GetBufferPointer())
            return;
        }

        result = pDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), 0, &m_pVertexShader);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create vertex shader: resources/shaders/TexturedQuad.hlsl")
            return;
        }

        result = pDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), 0, &m_pPixelShader);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create pixel shader: resources/shaders/TexturedQuad.hlsl")
            return;
        }

        // Create input layout
        result = pDevice->CreateInputLayout(VertexPositionColorUV::s_layout, VertexPositionColorUV::s_numElements,
            pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(),
            &m_pInputLayout);

        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create input layout: VertexPositionColorUV::s_layout")
        }

        // Create constatnt buffer
        D3D11_BUFFER_DESC cbd = {0};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(TexturedQuad::cbPerObject);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;

        result = pDevice->CreateBuffer(&cbd, 0, &m_cbPerObjectBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create ParticleSystem::cbPerObject: TexturedQuad")
        }

        InitializeBuffers(pDevice, pDeviceContext);
    }

    void TexturedQuad::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pTextureSRV,
        ID3D11SamplerState* pSampleState, ID3D11Buffer* pCameraCBBuffer)
    {
        unsigned int stride = sizeof(VertexPositionColorUV);
        unsigned int offset = 0;

        pDeviceContext->VSSetShader(m_pVertexShader, 0, 0);
        pDeviceContext->PSSetShader(m_pPixelShader, 0, 0);
        pDeviceContext->IASetInputLayout(m_pInputLayout);

        // Dont move grid at all for now
        // TODO: Attach this to camera possibly
        DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX wvp = world * g_RenderingSystem.m_camView * g_RenderingSystem.m_camProjection;

        m_cbPerObject.WVP = DirectX::XMMatrixTranspose(wvp);
        pDeviceContext->UpdateSubresource(m_cbPerObjectBuffer, 0, 0, &m_cbPerObject, 0, 0);
        pDeviceContext->VSSetConstantBuffers(0, 1, &m_cbPerObjectBuffer);

        pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
        pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        const uint32_t numSRVs = 1;
        ID3D11ShaderResourceView* pSRVs[numSRVs];
        pSRVs[0] = pTextureSRV;
        pDeviceContext->PSSetShaderResources(0, numSRVs, pSRVs);
        pDeviceContext->PSSetSamplers(0, 1, &pSampleState);

        // We need to set a PS contant buffer for camera data
        const uint32_t numConstantBuffers = 1;
        ID3D11Buffer *pConstantBuffers[numConstantBuffers];
        pConstantBuffers[0] = pCameraCBBuffer;
        pDeviceContext->PSSetConstantBuffers(0, numConstantBuffers, pConstantBuffers);

        pDeviceContext->DrawIndexed(m_indexCount, 0, 0);
    }

    void TexturedQuad::InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result = ERROR_SUCCESS;

        // Only 6 verts in quad
        m_vertexCount = 6;
        m_indexCount = m_vertexCount;

        VertexPositionColorUV* pRawVertices = new VertexPositionColorUV[m_vertexCount];
        unsigned int* pRawIndices = new unsigned int[m_indexCount];

        int index = 0;

        // LINE 1
        // Upper left.
        {
            pRawVertices[index].m_position = m_topLeft;
            pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            pRawVertices[index].m_uv = Vector2(0.0f, 1.0f);
            pRawIndices[index] = index;
            index++;
        }

        // Upper right.
        {
            pRawVertices[index].m_position = m_topRight;
            pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            pRawVertices[index].m_uv = Vector2(1.0f, 1.0f);
            pRawIndices[index] = index;
            index++;
        }

        // Bottom right.
        {
            pRawVertices[index].m_position = m_bottomRight;
            pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            pRawVertices[index].m_uv = Vector2(1.0f, 0.0f);
            pRawIndices[index] = index;
            index++;
        }
        // LINE 3
        // Bottom right.
        {
            pRawVertices[index].m_position = m_bottomRight;
            pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            pRawVertices[index].m_uv = Vector2(1.0f, 0.0f);
            pRawIndices[index] = index;
            index++;
        }

        // LINE 4
        // Bottom left.
        {
            pRawVertices[index].m_position = m_bottomLeft;
            pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            pRawVertices[index].m_uv = Vector2(0.0f, 0.0f);
            pRawIndices[index] = index;
            index++;
        }

        // Upper left.
        {
            pRawVertices[index].m_position = m_topLeft;
            pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            pRawVertices[index].m_uv = Vector2(1.0f, 0.0f);
            pRawIndices[index] = index;
            index++;
        }

        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    	vertexBufferDesc.ByteWidth = sizeof(VertexPositionColorUV) * m_vertexCount;
    	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    	vertexBufferDesc.CPUAccessFlags = 0;
    	vertexBufferDesc.MiscFlags = 0;
    	vertexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA vertexData;
        ZeroMemory(&vertexData, sizeof(vertexData));
        vertexData.pSysMem = pRawVertices;
    	vertexData.SysMemPitch = 0;
    	vertexData.SysMemSlicePitch = 0;

        result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create TexturedQuad vertex buffer")
        }

        D3D11_BUFFER_DESC indexBufferDesc;
        ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_indexCount;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA indexData;
        ZeroMemory(&indexData, sizeof(indexData));
        indexData.pSysMem = pRawIndices;
    	indexData.SysMemPitch = 0;
    	indexData.SysMemSlicePitch = 0;

        result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create TexturedQuad index buffer")
        }

        delete[] pRawVertices;
        pRawVertices = nullptr;
        delete[] pRawIndices;
        pRawIndices = nullptr;
    }
}
