#include <d3dcompiler.h>

#include "Shader.h"

#include "../Util/StringUtil.h"

#include <iostream>

#include "../Farlor.h"

#include "../Util/Logger.h"

using namespace std;

namespace Farlor
{
    Shader::Shader()
        : m_filename{""}
        , m_pVertexShader{nullptr}
        , m_pPixelShader{nullptr}
        , m_hasVS{false}
        , m_hasPS{false}
    {
    }

    Shader::Shader(string filename, bool hasVS, bool hasPS, vector<string> defines)
        : m_filename{filename}
        , m_pVertexShader{nullptr}
        , m_pPixelShader{nullptr}
        , m_hasVS{hasVS}
        , m_hasPS{hasPS}
        , m_defines{defines}
    {
    }

    void Shader::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result;

        ID3DBlob* vertexShaderBuffer = nullptr;
        ID3DBlob* pixelShaderBuffer = nullptr;
        ID3DBlob* errorMessage = nullptr;

        wstring vertexShaderName = L"resources/shaders/";
        vertexShaderName += StringToWideString(m_filename);
        wstring pixelShaderName = L"resources/shaders/";
        pixelShaderName += StringToWideString(m_filename);

        FARLOR_LOG_INFO("Loading shader: {}", m_filename)

        // Create shader defines
        D3D_SHADER_MACRO* macros = nullptr;

        if (m_defines.size() > 0)
        {
            macros = new D3D_SHADER_MACRO[m_defines.size()+1];
            for (u32 i = 0; i < m_defines.size(); ++i)
            {
                macros[i].Name = m_defines[i].c_str();
                macros[i].Definition = "1";
            }

            // NOTE: Must cap off with null defines!!!!
            macros[m_defines.size()].Name = NULL;
            macros[m_defines.size()].Definition = NULL;
        }

        if (m_hasVS)
        {
            result = D3DCompileFromFile(vertexShaderName.c_str(), macros, 0, "VSMain", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to compile vertex shader")
                FARLOR_LOG_ERROR("{}", (char*)errorMessage->GetBufferPointer())
                return;
            }

            result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 0, &m_pVertexShader);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create vertex shader")
                return;
            }

            // Reflection shader info
            ID3D11ShaderReflection* pVertexShaderReflection = nullptr;
            result = D3DReflect(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &pVertexShaderReflection);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to get shader reflection")
            }

            D3D11_SHADER_DESC shaderDesc;
            pVertexShaderReflection->GetDesc(&shaderDesc);

            std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
            for (u32 i = 0; i < shaderDesc.InputParameters; i++)
            {
                D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
                pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

                D3D11_INPUT_ELEMENT_DESC elementDesc;
                elementDesc.SemanticName = paramDesc.SemanticName;
                elementDesc.SemanticIndex = paramDesc.SemanticIndex;
                elementDesc.InputSlot = 0;
                elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
                elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                elementDesc.InstanceDataStepRate = 0;

                // determine DXGI format
                if ( paramDesc.Mask == 1 )
                {
                    if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32_UINT;
                    else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32_SINT;
                    else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
                }
                else if ( paramDesc.Mask <= 3 )
                {
                    if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
                    else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
                    else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
                }
                else if ( paramDesc.Mask <= 7 )
                {
                    if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
                    else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
                    else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                }
                else if ( paramDesc.Mask <= 15 )
                {
                    if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
                    else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
                    else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                }

                //save element desc
                inputLayoutDesc.push_back(elementDesc);
            }

            result = pDevice->CreateInputLayout(&inputLayoutDesc[0], (unsigned int)inputLayoutDesc.size(), vertexShaderBuffer->GetBufferPointer(),
                vertexShaderBuffer->GetBufferSize(), &m_pInputLayout);
        }

        if (m_hasPS)
        {
            result = D3DCompileFromFile(pixelShaderName.c_str(), macros, 0, "PSMain", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to compile pixel shader")
                FARLOR_LOG_ERROR("{}", (char*)errorMessage->GetBufferPointer())
                return;
            }

            result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), 0, &m_pPixelShader);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create pixel shader")
                return;
            }
        }

        SAFE_RELEASE(vertexShaderBuffer);
        SAFE_RELEASE(pixelShaderBuffer);
        SAFE_RELEASE(errorMessage);
        SAFE_DELETE_ARRAY(macros);
    }

    void Shader::SetPipeline(ID3D11DeviceContext* pDeviceContext)
    {
        if (m_hasVS)
        {
            pDeviceContext->VSSetShader(m_pVertexShader, 0, 0);
            pDeviceContext->IASetInputLayout(m_pInputLayout);
        }
        if (m_hasPS)
        {
            pDeviceContext->PSSetShader(m_pPixelShader, 0, 0);
        }
    }
}
