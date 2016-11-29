#include <d3dcompiler.h>

#include "Renderer.h"

#include "Camera.h"

#include <iostream>

#include "Mesh.h"
#include "WICTextureLoader.h"

#include "../ECS/TransformComponent.h"
#include "../ECS/TransformManager.h"

#include "../Util/Logger.h"

using namespace std;

using namespace DirectX;

namespace Farlor
{
    extern TransformManager g_TransformManager;
    extern Camera g_MainCamera;

    extern ParticleSystem g_WaveParticles;
    extern Camera g_WaveParticleCamera;

    std::map<std::string, RenderResourceType> Renderer::s_stringToTypeMapping =
    {
        { "render-target", RenderResourceType::RenderTarget }
    };

    std::map<std::string, RenderResourceFormat> Renderer::s_stringToFormatMapping =
    {
        { "R8G8B8A8", RenderResourceFormat::R8G8B8A8 },
        { "D24S8", RenderResourceFormat::D24S8 },
        { "R32G32B32A32F", RenderResourceFormat::R32G32B32A32F }
    };

    std::map<RenderResourceFormat, DXGI_FORMAT> Renderer::s_formatToDXGIFormatMapping
    {
        { RenderResourceFormat::R8G8B8A8, DXGI_FORMAT_R8G8B8A8_UNORM },
        { RenderResourceFormat::D24S8, DXGI_FORMAT_D24_UNORM_S8_UINT },
        { RenderResourceFormat::R32G32B32A32F, DXGI_FORMAT_R32G32B32A32_FLOAT }
    };


    void Renderer::Initialize(GameWindow gameWindow)
    {
        HRESULT result;

        m_width = gameWindow.GetWidth();
        m_height = gameWindow.GetHeight();

        DXGIModeDesc bufferDesc{m_width, m_height};

        // Describe swap chain
        DXGISwapChainDesc swapChainDesc{bufferDesc, gameWindow};

        // Create the swap chain
        result = D3D11CreateDeviceAndSwapChain(
            0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION,
            &swapChainDesc.m_dxgiSwapChainDesc, &m_pSwapChain, &m_pDevice, 0, &m_pDeviceContext);

        // Create back buffer
        ID3D11Texture2D* backBuffer;
        result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
            (void**)&backBuffer);

        // Create render target
        result = m_pDevice->CreateRenderTargetView(backBuffer, 0, &m_pRenderTargetView);
        backBuffer->Release();

        // Describe depth stencil buffer
        D3D11_TEXTURE2D_DESC depthStencilDesc = {0};
        depthStencilDesc.Width = m_width;
        depthStencilDesc.Height = m_height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(dsvDesc));
        dsvDesc.Flags = 0;
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        D3D11_SHADER_RESOURCE_VIEW_DESC dssrvDesc;
        ZeroMemory(&dssrvDesc, sizeof(dssrvDesc));
        dssrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        dssrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        dssrvDesc.Texture2D.MipLevels = 1;

        result = m_pDevice->CreateTexture2D(&depthStencilDesc, NULL, &m_depthStencilBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create depth stencil texture 2d")
        }

        result = m_pDevice->CreateDepthStencilView(m_depthStencilBuffer, &dsvDesc, &m_depthStencilView);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create depth stencil view")
        }

        result = m_pDevice->CreateShaderResourceView(m_depthStencilBuffer, &dssrvDesc, &m_pDepthScencilSRV);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create depth stencil SRV")
        }

        // Set render target
        m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_depthStencilView);


        D3D11_DEPTH_STENCIL_DESC dsDesc;

        // Depth test parameters
        dsDesc.DepthEnable = false;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

        // Stencil test parameters
        dsDesc.StencilEnable = false;
        dsDesc.StencilReadMask = 0xFF;
        dsDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing
        dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Create depth stencil state
        m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDSState);

        // Set raster desc
        RasterizerStateDesc rasterDesc;
        rasterDesc.m_rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.m_rasterDesc.FillMode = D3D11_FILL_SOLID;
        result = m_pDevice->CreateRasterizerState(&rasterDesc.m_rasterDesc, &m_rasterState);
        if (FAILED(result))
        {
            cout << "Failed to create raster state" << endl;
        }

        m_pDeviceContext->RSSetState(m_rasterState);

        // Create and set viewport
        D3D11_VIEWPORT viewport = {0};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = (float)m_width;
        viewport.Height = (float)m_height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        m_pDeviceContext->RSSetViewports(1, &viewport);

        // Create constatnt buffer
        D3D11_BUFFER_DESC cbd = {0};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(Farlor::cbTransforms);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;

        result = m_pDevice->CreateBuffer(&cbd, 0, &m_cbTransformsBuffer);
        if (FAILED(result))
        {
            cout << "Failed to create constant buffer layout" << endl;
        }

        // Create constatnt buffer
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(Farlor::cbMatProperties);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;

        result = m_pDevice->CreateBuffer(&cbd, 0, &m_cbMatPropertiesBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create constant buffer layout")
        }

        // Create constatnt buffer
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(Farlor::cbLightProperties);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;

        result = m_pDevice->CreateBuffer(&cbd, 0, &m_cbLightPropertiesBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create constant buffer layout")
        }

        // Create constatnt buffer
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(Farlor::cbCameraParams);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;

        result = m_pDevice->CreateBuffer(&cbd, 0, &m_cbCameraParamsBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create constant buffer layout")
        }

        // Create sampler state
        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    	samplerDesc.MipLODBias = 0.0f;
    	samplerDesc.MaxAnisotropy = 1;
    	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    	samplerDesc.BorderColor[0] = 0;
    	samplerDesc.BorderColor[1] = 0;
    	samplerDesc.BorderColor[2] = 0;
    	samplerDesc.BorderColor[3] = 0;
    	samplerDesc.MinLOD = 0;
    	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        m_pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);

        // Create blend states
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(blendDesc));

        blendDesc.RenderTarget[0].BlendEnable = true;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        result = m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBlendState);

        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create blend state")
        }


        // Wave Particle stuff

        D3D11_TEXTURE2D_DESC waveParticleTextureDesc = {0};
        waveParticleTextureDesc.Width = m_width;
        waveParticleTextureDesc.Height = m_height;
        waveParticleTextureDesc.MipLevels = 1;
        waveParticleTextureDesc.ArraySize = 1;
        waveParticleTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        waveParticleTextureDesc.SampleDesc.Count = 1;
        waveParticleTextureDesc.SampleDesc.Quality = 0;
        waveParticleTextureDesc.Usage = D3D11_USAGE_DEFAULT;
        waveParticleTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        waveParticleTextureDesc.CPUAccessFlags = 0;
        waveParticleTextureDesc.MiscFlags = 0;

        result = m_pDevice->CreateTexture2D(&waveParticleTextureDesc, 0, &m_pWaveParticleRenderTarget);
        if (FAILED(result))
        {
            cout << "Failed to create wave particle texture" << endl;
        }

        D3D11_TEXTURE2D_DESC waveParticleStagingTextureDesc = {0};
        waveParticleStagingTextureDesc.Width = m_width;
        waveParticleStagingTextureDesc.Height = m_height;
        waveParticleStagingTextureDesc.MipLevels = 1;
        waveParticleStagingTextureDesc.ArraySize = 1;
        waveParticleStagingTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        waveParticleStagingTextureDesc.SampleDesc.Count = 1;
        waveParticleStagingTextureDesc.SampleDesc.Quality = 0;
        waveParticleStagingTextureDesc.Usage = D3D11_USAGE_STAGING;
        waveParticleStagingTextureDesc.BindFlags = 0;
        waveParticleStagingTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        waveParticleStagingTextureDesc.MiscFlags = 0;

        result = m_pDevice->CreateTexture2D(&waveParticleStagingTextureDesc, 0, &m_pWaveParticleStagingResource);
        if (FAILED(result))
        {
            cout << "Failed to create wave particle staging texture" << endl;
        }

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
        renderTargetViewDesc.Format = waveParticleTextureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        m_pDevice->CreateRenderTargetView(m_pWaveParticleRenderTarget, &renderTargetViewDesc, &m_pWPRTView);

        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
        shaderResourceViewDesc.Format = waveParticleTextureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        // Create sampler state
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    	samplerDesc.MipLODBias = 0.0f;
    	samplerDesc.MaxAnisotropy = 1;
    	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    	samplerDesc.BorderColor[0] = 0;
    	samplerDesc.BorderColor[1] = 0;
    	samplerDesc.BorderColor[2] = 0;
    	samplerDesc.BorderColor[3] = 0;
    	samplerDesc.MinLOD = 0;
    	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        m_pDevice->CreateSamplerState(&samplerDesc, &m_pWPSampleState);



        m_pDevice->CreateShaderResourceView(m_pWaveParticleRenderTarget, &shaderResourceViewDesc, &m_pWPSRView);



        result = CreateWICTextureFromFile(m_pDevice, m_pDeviceContext, L"resources/Sprites/Particle.png", nullptr, &m_pParticleTextureResourceView);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to load texture")
        }

        // Create an alpha enabled blend state description.
        D3D11_BLEND_DESC blendStateDesc = {0};
    	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
    	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
    	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

        result = m_pDevice->CreateBlendState(&blendStateDesc, &m_pEnableAlphaBlending);
    	if(FAILED(result))
    	{
    		cout << "Failed to create alpha bend state active" << endl;
    	}

        blendStateDesc.RenderTarget[0].BlendEnable = FALSE;
    	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

        result = m_pDevice->CreateBlendState(&blendStateDesc, &m_pDisableAlphaBlending);
    	if(FAILED(result))
    	{
    		cout << "Failed to create alpha bend state active" << endl;
    	}


        // Depth test parameters
        dsDesc.DepthEnable = false;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

        // Stencil test parameters
        dsDesc.StencilEnable = false;
        dsDesc.StencilReadMask = 0xFF;
        dsDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing
        dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Create depth stencil state
        m_pDevice->CreateDepthStencilState(&dsDesc, &pDSState);


        g_WaveParticles.InitializeBuffers(m_pDevice, m_pDeviceContext);
        m_terrain.Initialize(100, 100, m_pDevice, m_pDeviceContext);
    }

    void Renderer::Render()
    {
        m_camView = g_WaveParticleCamera.m_camView;
        // Perspective
        m_camProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)m_width/m_height, 1.0f, 1000.0f);

        float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
        m_pDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

        m_pDeviceContext->OMSetRenderTargets(1, &m_pWPRTView, m_depthStencilView);
        m_pDeviceContext->ClearRenderTargetView(m_pWPRTView, clearColor);

        // Render the wave particles
        m_pDeviceContext->OMSetDepthStencilState(pDSState, 0);

        m_pDeviceContext->OMSetBlendState(m_pEnableAlphaBlending, 0, 0xffffffff);
        g_WaveParticles.Render(m_pDevice, m_pDeviceContext, m_pParticleTextureResourceView, m_pWPSampleState);
        m_pDeviceContext->OMSetBlendState(m_pDisableAlphaBlending, 0, 0xffffffff);

        // Render the terrain
        m_camView = g_MainCamera.m_camView;
        m_pDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
        m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_depthStencilView);
        m_pDeviceContext->RSSetState(m_rasterState);

        m_terrain.Render(m_pDevice, m_pDeviceContext, m_pWPSRView, m_pWPSampleState);

        m_pSwapChain->Present(0, 0);
    }

    void Renderer::Render(RenderingComponent& renderingComponent)
    {
        // cout << "Rendering Component: " << renderingComponent.m_mesh << endl;
        TransformComponent transformComponent = g_TransformManager.GetComponent(renderingComponent.m_entity);

        XMMATRIX world = XMMatrixIdentity();

        XMMATRIX scale = XMMatrixScaling(transformComponent.m_scale.x, transformComponent.m_scale.y, transformComponent.m_scale.z);
        // XMMATRIX translate = XMMatrixTranslation(, comp.m_position.y, comp.m_position.z);

        // XMVECTOR rotAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        XMMATRIX rotate = XMMatrixRotationRollPitchYaw(transformComponent.m_rotation.x, transformComponent.m_rotation.y, transformComponent.m_rotation.z);

        XMMATRIX translate = XMMatrixTranslation(transformComponent.m_position.x, transformComponent.m_position.y, transformComponent.m_position.z);
        world = scale * rotate *  translate;

        m_cbTransforms.World = XMMatrixTranspose(world);

        XMMATRIX wv = world * m_camView;

        m_cbTransforms.WorldView = XMMatrixTranspose(wv);

        XMMATRIX wvp = world * m_camView * m_camProjection;

        m_cbTransforms.WorldViewProjection = XMMatrixTranspose(wvp);

        m_pDeviceContext->UpdateSubresource(m_cbTransformsBuffer, 0, 0, &m_cbTransforms, 0, 0);

        // Material properties
        m_cbMatProperties.SpecularAlbedo = Vector3(1.0f, 1.0f, 1.0f);
        m_cbMatProperties.SpecularPower = 1.0f;
        m_pDeviceContext->UpdateSubresource(m_cbMatPropertiesBuffer, 0, 0, &m_cbMatProperties, 0, 0);

        ID3D11Buffer *pConstantBuffers[2];
        pConstantBuffers[0] = m_cbTransformsBuffer;
        pConstantBuffers[1] = m_cbMatPropertiesBuffer;

        m_pDeviceContext->VSSetConstantBuffers(0, 2, pConstantBuffers);
        m_pDeviceContext->PSSetConstantBuffers(0, 2, pConstantBuffers);

        m_meshes[renderingComponent.m_mesh].Render(m_pDevice, m_pDeviceContext);
    }

    void Renderer::RenderLight(LightComponent& lightComponent)
    {
        TransformComponent transformComponent = g_TransformManager.GetComponent(lightComponent.m_entity);

        m_cbLightProperties.m_position = transformComponent.m_position;
        m_cbLightProperties.m_color = lightComponent.m_color;
        m_cbLightProperties.m_direction = lightComponent.m_direction;
        m_cbLightProperties.m_spotlightAngles = lightComponent.m_spotlightAngles;
        m_cbLightProperties.m_lightRange = lightComponent.m_lightRange;

        m_pDeviceContext->UpdateSubresource(m_cbLightPropertiesBuffer, 0, 0, &m_cbLightProperties, 0, 0);

        XMFLOAT3 pos;
        XMStoreFloat3(&pos, g_MainCamera.m_camPosition);
        m_cbCameraParams.m_position = Vector3(pos.x, pos.y, pos.z);

        m_pDeviceContext->UpdateSubresource(m_cbCameraParamsBuffer, 0, 0, &m_cbCameraParams, 0, 0);

        ID3D11Buffer *pConstantBuffers[2];
        pConstantBuffers[0] = m_cbLightPropertiesBuffer;
        pConstantBuffers[1] = m_cbCameraParamsBuffer;

        m_pDeviceContext->PSSetConstantBuffers(0, 2, pConstantBuffers);

        m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        m_pDeviceContext->Draw(3, 0);
    }


    void Renderer::LoadMesh(HashedString meshName)
    {
        Mesh newMesh;
        std::string modelName = "resources/Models/";
        modelName += meshName.m_hashedString;
        FARLOR_LOG_INFO("Loading model: {}", modelName)
        Mesh::LoadObjTinyObj(modelName, newMesh);
        newMesh.InitializeBuffers(m_pDevice, m_pDeviceContext);
        m_meshes[meshName] = newMesh;
    }

    void Renderer::RegisterGlobalResource(std::string name, std::string resourceType,
        int widthScale, int heightScale, std::string format)
    {
        RenderResourceDesc resourceDesc;
        resourceDesc.m_name = name;
        resourceDesc.m_type = Renderer::TypeFromString(resourceType);
        resourceDesc.m_widthScale = widthScale;
        resourceDesc.m_heightScale = heightScale;
        resourceDesc.m_format = Renderer::FormatFromString(format);

        m_globalResourcesDescs.push_back(resourceDesc);
    }

    void Renderer::CreateGlobalResources()
    {
        for (auto& resourceDesc : m_globalResourcesDescs)
        {
            switch (resourceDesc.m_type)
            {
            case RenderResourceType::RenderTarget:
                {
                    FARLOR_LOG_INFO("Creating render target")

                    D3D11_TEXTURE2D_DESC rtDesc;
                    ZeroMemory(&rtDesc, sizeof(rtDesc));

                    RenderTarget newRT;

                    rtDesc.Width = m_width * resourceDesc.m_widthScale;
                    rtDesc.Height = m_height * resourceDesc.m_heightScale;
                    rtDesc.MipLevels = 1;
                    rtDesc.ArraySize = 1;
                    rtDesc.Format = FormatToDXGIFormat(resourceDesc.m_format);
                    rtDesc.SampleDesc.Count = 1;
                    rtDesc.Usage = D3D11_USAGE_DEFAULT;
                    rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
                    rtDesc.CPUAccessFlags = 0;
                    rtDesc.MiscFlags = 0;

                    HRESULT result = m_pDevice->CreateTexture2D(&rtDesc, 0, &newRT.m_pRTTexture);
                    if (FAILED(result))
                    {
                        FARLOR_LOG_ERROR("Failed to create global render target texture: {}", resourceDesc.m_name)
                    }

                    D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
                    ZeroMemory(&rtViewDesc, sizeof(rtViewDesc));
                    rtViewDesc.Format = rtDesc.Format;
                    rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                    rtViewDesc.Texture2D.MipSlice = 0;
                    result = m_pDevice->CreateRenderTargetView(newRT.m_pRTTexture, &rtViewDesc, &newRT.m_pRTView);
                    if (FAILED(result))
                    {
                        FARLOR_LOG_ERROR("Failed to create global render target view: {}", resourceDesc.m_name)
                    }

                    D3D11_SHADER_RESOURCE_VIEW_DESC rtSRViewDesc;
                    ZeroMemory(&rtSRViewDesc, sizeof(rtSRViewDesc));
                    rtSRViewDesc.Format = rtDesc.Format;
                    rtSRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    rtSRViewDesc.Texture2D.MostDetailedMip = 0;
                    rtSRViewDesc.Texture2D.MipLevels = 1;
                    result = m_pDevice->CreateShaderResourceView(newRT.m_pRTTexture, &rtSRViewDesc, &newRT.m_pRTShaderResourceView);
                    if (FAILED(result))
                    {
                        FARLOR_LOG_ERROR("Failed to create global render target shader resource view: {}", resourceDesc.m_name)
                    }

                    FARLOR_LOG_INFO("Successfuly created global render target: {}", resourceDesc.m_name);

                    m_renderTargets.insert(std::pair<std::string, RenderTarget>(resourceDesc.m_name, newRT));

                } break;

            default:
                {
                    FARLOR_LOG_WARNING("Incorrect resource type, cannot create")
                } break;
            }
        }
    }

    void Renderer::RegisterShader(string filename, string name, bool hasVS, bool hasPS, vector<string> defines)
    {
        Shader newShader(filename, hasVS, hasPS, defines);
        m_shaders[name] = newShader;
    }

void Renderer::CreateShaders()
    {
        for (auto& shaderPair : m_shaders)
        {
            shaderPair.second.Initialize(m_pDevice, m_pDeviceContext);
        }
    }

    void Renderer::AddComponent(RenderingComponent component)
    {
        LoadMesh(component.m_mesh);
        m_renderingComponents.push_back(component);
    }

    void Renderer::AddComponent(LightComponent component)
    {
        m_lightComponents.push_back(component);
    }

    RenderResourceType Renderer::TypeFromString(std::string strType)
    {
        return s_stringToTypeMapping[strType];
    }

    RenderResourceFormat Renderer::FormatFromString(std::string strFormat)
    {
        return s_stringToFormatMapping[strFormat];
    }

    DXGI_FORMAT Renderer::FormatToDXGIFormat(RenderResourceFormat format)
    {
        return s_formatToDXGIFormatMapping[format];
    }
}
