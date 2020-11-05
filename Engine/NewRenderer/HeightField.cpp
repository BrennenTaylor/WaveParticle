// NOTE: For some fucking reason this must be included before any d3d11 stuff gets included. Why you ask? No clue.
#include <d3dcompiler.h>
#include "HeightField.h"

#include "Renderer.h"
#include "Vertex.h"

#include <DirectXMath.h>

#include "../Util/Logger.h"

namespace Farlor
{
    extern Renderer g_RenderingSystem;

    HeightField::HeightField()
        : m_width{0}
        , m_height{0}
        , m_vertexCount{0}
        , m_indexCount{0}
        , m_pVertexBuffer{nullptr}
        , m_pIndexBuffer{nullptr}
        , m_pPositionColorVertexShader{nullptr}
        , m_pPositionColorPixelShader{nullptr}
        , m_pPositionColorInputLayout{nullptr}
        , m_cbPerObjectBuffer{nullptr}
        , m_cbPerObject{}
    {
    }

    void HeightField::Initialize(int width, int height, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result = ERROR_SUCCESS;

        // Bottom left corner is at 0, 0
        // Top right is (m_width, m_height)
        // For mapping purposes, bottom left uv is (0, 0)
        // Top right is (1, 1)
        m_width = width;
        m_height = height;

        ID3DBlob* pErrorMessage = nullptr;
        ID3DBlob* pVertexShaderBuffer = nullptr;
        ID3DBlob* pPixelShaderBuffer = nullptr;
        D3D11_BUFFER_DESC matrixBufferDesc = {0};

        std::wstring vertexShaderName = L"resources/shaders/RenderHeightmap.hlsl";
        std::wstring pixelShaderName = L"resources/shaders/RenderHeightmap.hlsl";

        result = D3DCompileFromFile(vertexShaderName.c_str(), 0, 0, "VSMain", "vs_5_0", 0, 0, &pVertexShaderBuffer, &pErrorMessage);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to compile vertex shader: resources/shaders/RenderHeightmap.hlsl")
            FARLOR_LOG_ERROR((char*)pErrorMessage->GetBufferPointer())
            return;
        }

        result = D3DCompileFromFile(pixelShaderName.c_str(), 0, 0, "PSMain", "ps_5_0", 0, 0, &pPixelShaderBuffer, &pErrorMessage);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to compile pixel shader: resources/shaders/RenderHeightmap.hlsl")
            FARLOR_LOG_ERROR((char*)pErrorMessage->GetBufferPointer())
            return;
        }

        result = pDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), 0, &m_pPositionColorVertexShader);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create vertex shader: resources/shaders/RenderHeightmap.hlsl")
            return;
        }

        result = pDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), 0, &m_pPositionColorPixelShader);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create pixel shader: resources/shaders/RenderHeightmap.hlsl")
            return;
        }

        // Create input layout
        result = pDevice->CreateInputLayout(VertexPositionColorUV::s_layout, VertexPositionColorUV::s_numElements,
            pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(),
            &m_pPositionColorInputLayout);

        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create input layout: VertexPositionColorUV::s_layout")
        }

        // Create constatnt buffer
        D3D11_BUFFER_DESC cbd = {0};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(ParticleSystem::cbPerObject);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;

        result = pDevice->CreateBuffer(&cbd, 0, &m_cbPerObjectBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create ParticleSystem::cbPerObject: HeightField")
        }

        InitializeBuffers(pDevice, pDeviceContext);
    }

    void HeightField::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pWSRView, ID3D11SamplerState* pWPSampleState)
    {
        unsigned int stride = sizeof(VertexPositionColorUV);
        unsigned int offset = 0;

        pDeviceContext->VSSetShader(m_pPositionColorVertexShader, 0, 0);
        pDeviceContext->PSSetShader(m_pPositionColorPixelShader, 0, 0);
        pDeviceContext->IASetInputLayout(m_pPositionColorInputLayout);

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

        pDeviceContext->VSSetShaderResources(0, 1, &pWSRView);
        pDeviceContext->VSSetSamplers(0, 1, &pWPSampleState);

        pDeviceContext->DrawIndexed(m_indexCount, 0, 0);
    }

    void HeightField::InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result = ERROR_SUCCESS;

        // Number of grid cells within mesh?
        int numSubX = 500;
        int numSubZ = 500;

        // Only 6 verts per square, and (x-1)*(z-1) squares
        // TODO: Optimize this to reuse vertices
        m_vertexCount = (numSubX - 1) * (numSubZ - 1) * 6;
        m_indexCount = m_vertexCount;

        VertexPositionColorUV* pRawVertices = new VertexPositionColorUV[m_vertexCount];
        unsigned int* pRawIndices = new unsigned int[m_indexCount];

        int index = 0;
        float positionX = 0.0f;
        float positionZ = 0.0f;
        for(int j = 0; j < (numSubZ - 1); j++)
        {
            for (int i = 0; i < (numSubX - 1); i++)
            {
                // LINE 1
    			// Upper left.
    			positionX = (float)i;
    			positionZ = (float)(j+1);
                positionX = (positionX/numSubX)*m_width;
                positionZ = (positionZ/numSubZ)*m_height;

                pRawVertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
                pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                pRawVertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/m_height);
                pRawIndices[index] = index;
    			index++;

    			// Upper right.
    			positionX = (float)(i+1);
    			positionZ = (float)(j+1);
                positionX = (positionX/numSubX)*m_width;
                positionZ = (positionZ/numSubZ)*m_height;

                pRawVertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
                pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                pRawVertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
                pRawIndices[index] = index;
    			index++;

    			// LINE 2
    			// Upper right.
    			// positionX = (float)(i+1);
    			// positionZ = (float)(j+1);
                //
    			// vertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
    			// vertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                // vertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
    			// indices[index] = index;
    			// index++;

    			// Bottom right.
    			positionX = (float)(i+1);
    			positionZ = (float)j;
                positionX = (positionX/numSubX)*m_width;
                positionZ = (positionZ/numSubZ)*m_height;


                pRawVertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
                pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                pRawVertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
                pRawIndices[index] = index;
    			index++;

    			// LINE 3
    			// Bottom right.
    			positionX = (float)(i+1);
    			positionZ = (float)j;
                positionX = (positionX/numSubX)*m_width;
                positionZ = (positionZ/numSubZ)*m_height;


                pRawVertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
                pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                pRawVertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
                pRawIndices[index] = index;
    			index++;

    			// Bottom left.
    			// positionX = (float)i;
    			// positionZ = (float)j;
                //
    			// vertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
    			// vertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                // vertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
    			// indices[index] = index;
    			// index++;

    			// LINE 4
    			// Bottom left.
    			positionX = (float)i;
    			positionZ = (float)j;
                positionX = (positionX/numSubX)*m_width;
                positionZ = (positionZ/numSubZ)*m_height;


                pRawVertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
                pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                pRawVertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
                pRawIndices[index] = index;
    			index++;

    			// Upper left.
    			positionX = (float)i;
    			positionZ = (float)(j+1);
                positionX = (positionX/numSubX)*m_width;
                positionZ = (positionZ/numSubZ)*m_height;


                pRawVertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
                pRawVertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                pRawVertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
                pRawIndices[index] = index;
    			index++;
            }
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
            FARLOR_LOG_ERROR("Failed to create HeightField vertex buffer")
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
            FARLOR_LOG_ERROR("Failed to create HeightField index buffer")
        }

        delete[] pRawVertices;
        pRawVertices = nullptr;
        delete[] pRawIndices;
        pRawIndices = nullptr;
    }
}
