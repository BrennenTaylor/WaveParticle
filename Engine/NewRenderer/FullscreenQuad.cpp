#include "FullScreenQuad.h"

#include <string>

#include <d3dcompiler.h>

#include <iostream>

using namespace std;

namespace Farlor
{
    void FullScreenQuad::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result;
        // Lets load ball shaders
        ID3DBlob* errorMessage = nullptr;
        ID3DBlob* vertexShaderBuffer = nullptr;
        ID3DBlob* pixelShaderBuffer = nullptr;
        D3D11_BUFFER_DESC matrixBufferDesc = {0};

        wstring vertexShaderName = L"resources/shaders/FullScreenQuad.hlsl";
        wstring pixelShaderName = L"resources/shaders/FullScreenQuad.hlsl";

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
            cout << "Failed to compile ball pixel shader" << endl;
            return;
        }

        result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 0, &m_pVertexShader);
        if (FAILED(result))
        {
            cout << "Failed to createe ball vertex shader" << endl;
            return;
        }

        result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), 0, &m_pPixelShader);
        if (FAILED(result))
        {
            cout << "Failed to create ball pixel shader" << endl;
            return;
        }
    }

    void FullScreenQuad::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pWSRView, ID3D11SamplerState* pWPSampleState)
    {
        pDeviceContext->VSSetShader(m_pVertexShader, 0, 0);
        pDeviceContext->PSSetShader(m_pPixelShader, 0, 0);
        pDeviceContext->IASetInputLayout(nullptr);

        pDeviceContext->PSSetShaderResources(0, 1, &pWSRView);
        pDeviceContext->PSSetSamplers(0, 1, &pWPSampleState);

        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        pDeviceContext->Draw(3, 0);
    }
}
