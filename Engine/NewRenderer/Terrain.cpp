#include "Terrain.h"

#include "Vertex.h"

#include "Renderer.h"

#include <DirectXMath.h>
using namespace DirectX;

namespace Farlor
{
    extern Renderer g_RenderingSystem;

    Terrain::Terrain()
        : m_width{0}
        , m_height{0}
    {
    }

    void Terrain::Initialize(int width, int height, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result;
        m_width = width;
        m_height = height;

        ID3DBlob* errorMessage = nullptr;
        ID3DBlob* vertexShaderBuffer = nullptr;
        ID3DBlob* pixelShaderBuffer = nullptr;
        D3D11_BUFFER_DESC matrixBufferDesc = {0};

        wstring vertexShaderName = L"resources/shaders/RenderHeightmap.hlsl";
        wstring pixelShaderName = L"resources/shaders/RenderHeightmap.hlsl";

        result = D3DCompileFromFile(vertexShaderName.c_str(), 0, 0, "VSMain", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
        if (FAILED(result))
        {
            cout << "Failed to compile vertex shader" << endl;
            cout << (char*)errorMessage->GetBufferPointer() << endl;
            return;
        }

        result = D3DCompileFromFile(pixelShaderName.c_str(), 0, 0, "PSMain", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
        if (FAILED(result))
        {
            cout << "Failed to compile pixel shader" << endl;
            cout << (char*)errorMessage->GetBufferPointer() << endl;
            return;
        }

        result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 0, &m_pPositionColorVertexShader);
        if (FAILED(result))
        {
            cout << "Failed to create ball vertex shader" << endl;
            return;
        }

        result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), 0, &m_pPositionColorPixelShader);
        if (FAILED(result))
        {
            cout << "Failed to create ball pixel shader" << endl;
            return;
        }

        // Create input layout
        result = pDevice->CreateInputLayout(VertexPositionColorUV::s_layout, VertexPositionColorUV::s_numElements,
            vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
            &m_pPositionColorInputLayout);

        if (FAILED(result))
        {
            cout << "Failed to create input layout" << endl;
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
            cout << "Failed to create constant buffer layout" << endl;
        }

        InitializeBuffers(pDevice, pDeviceContext);
    }

    void Terrain::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pWSRView, ID3D11SamplerState* pWPSampleState)
    {
        unsigned int stride = sizeof(VertexPositionColorUV);
        unsigned int offset = 0;

        pDeviceContext->VSSetShader(m_pPositionColorVertexShader, 0, 0);
        pDeviceContext->PSSetShader(m_pPositionColorPixelShader, 0, 0);
        pDeviceContext->IASetInputLayout(m_pPositionColorInputLayout);

        XMMATRIX world = XMMatrixIdentity();
        XMMATRIX wvp = world * g_RenderingSystem.m_camView * g_RenderingSystem.m_camProjection;

        m_cbPerObject.WVP = XMMatrixTranspose(wvp);
        pDeviceContext->UpdateSubresource(m_cbPerObjectBuffer, 0, 0, &m_cbPerObject, 0, 0);
        pDeviceContext->VSSetConstantBuffers(0, 1, &m_cbPerObjectBuffer);

        pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
        pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        pDeviceContext->VSSetShaderResources(0, 1, &pWSRView);
        pDeviceContext->VSSetSamplers(0, 1, &pWPSampleState);

        pDeviceContext->DrawIndexed(m_indexCount, 0, 0);
    }

    void Terrain::InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result;

        int numSubX = 500;
        int numSubZ = 500;

        m_vertexCount = (numSubX - 1) * (numSubZ - 1) * 8;
        m_indexCount = m_vertexCount;

        VertexPositionColorUV* vertices = new VertexPositionColorUV[m_vertexCount];
        unsigned int* indices = new unsigned int[m_indexCount];

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

    			vertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
    			vertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                vertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/m_height);
                indices[index] = index;
    			index++;

    			// Upper right.
    			positionX = (float)(i+1);
    			positionZ = (float)(j+1);
                positionX = (positionX/numSubX)*m_width;
                positionZ = (positionZ/numSubZ)*m_height;


    			vertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
    			vertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                vertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
                indices[index] = index;
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


    			vertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
    			vertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                vertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
    			indices[index] = index;
    			index++;

    			// LINE 3
    			// Bottom right.
    			positionX = (float)(i+1);
    			positionZ = (float)j;
                positionX = (positionX/numSubX)*m_width;
                positionZ = (positionZ/numSubZ)*m_height;


    			vertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
    			vertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                vertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
    			indices[index] = index;
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


    			vertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
    			vertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                vertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
    			indices[index] = index;
    			index++;

    			// Upper left.
    			positionX = (float)i;
    			positionZ = (float)(j+1);
                positionX = (positionX/numSubX)*m_width;
                positionZ = (positionZ/numSubZ)*m_height;


    			vertices[index].m_position = Vector3(positionX, 0.0f, positionZ);
    			vertices[index].m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                vertices[index].m_uv = Vector2(positionX/(float)m_width, positionZ/(float)m_height);
    			indices[index] = index;
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
        vertexData.pSysMem = vertices;
    	vertexData.SysMemPitch = 0;
    	vertexData.SysMemSlicePitch = 0;

        result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
        if (FAILED(result))
        {
            cout << "Failed to create terrain vertex buffer" << endl;
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
        indexData.pSysMem = indices;
    	indexData.SysMemPitch = 0;
    	indexData.SysMemSlicePitch = 0;

        result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
        if (FAILED(result))
        {
            cout << "Failed to create terrain index buffer" << endl;
        }

        delete[] vertices;
        vertices = nullptr;
        delete[] indices;
        indices = nullptr;
    }
}
