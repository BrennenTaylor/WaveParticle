#include "Renderer.h"

#include "WICTextureLoader.h"

#include "Mesh.h"

#include "Camera.h"

#include "../ECS/TransformComponent.h"
#include "../ECS/TransformManager.h"

#include "../Util/Logger.h"

#include "imgui.h"i
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include <iostream>

namespace Farlor
{
    const uint32_t WaveParticleHeightmapRenderTargetSize = 512;


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

    Renderer::Renderer()
        : m_TexturedQuad(
            Farlor::Vector3(-5.0f, -1.0f, -5.0f),
            Farlor::Vector3(-5.0f, -1.0f, 5.0f),
            Farlor::Vector3(5.0f, -1.0f, 5.0f),
            Farlor::Vector3(5.0f, -1.0f, -5.0f)
        )
    {
    }

    void Renderer::Initialize(GameWindow gameWindow)
    {
        HRESULT result = ERROR_SUCCESS;

        m_width = gameWindow.GetWidth();
        m_height = gameWindow.GetHeight();

        DXGIModeDesc bufferDesc{ m_width, m_height };

        // Describe swap chain
        DXGISwapChainDesc swapChainDesc{ bufferDesc, gameWindow };

        // Create the swap chain
        result = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION,
            &swapChainDesc.m_dxgiSwapChainDesc, &m_pSwapChain, &m_pDevice, 0, &m_pDeviceContext);



        // After device and context made, lets make imgui stuff
        ImGui_ImplWin32_Init(gameWindow.GetWindowHandle());
        ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);





        // Create back buffer
        m_pBackBuffer = nullptr;
        result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pBackBuffer);

        // Create render target
        assert(m_pBackBuffer != nullptr);
        result = m_pDevice->CreateRenderTargetView(m_pBackBuffer, 0, &m_pBackBufferRTV);
        m_pBackBuffer->Release();

        // Describe depth stencil buffer
        // Dimensions need to be the size of the window
        {
            D3D11_TEXTURE2D_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
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

            result = m_pDevice->CreateTexture2D(&depthStencilDesc, NULL, &m_pWindowDSB);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create depth stencil texture 2d")
            }

            result = m_pDevice->CreateDepthStencilView(m_pWindowDSB, &dsvDesc, &m_pWindowDSV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create depth stencil view")
            }

            result = m_pDevice->CreateShaderResourceView(m_pWindowDSB, &dssrvDesc, &m_pWindowDSSRV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create depth stencil SRV")
            }
        }
        // Set render target
        m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pWindowDSV);

        // Describe wave particle texture depth stencil buffer
        // Dimensions need to be the size of the wave particle render target
        {
            D3D11_TEXTURE2D_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.Width = WaveParticleHeightmapRenderTargetSize;
            depthStencilDesc.Height = WaveParticleHeightmapRenderTargetSize;
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

            result = m_pDevice->CreateTexture2D(&depthStencilDesc, NULL, &m_pWaveParticleDSB);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create depth stencil texture 2d")
            }

            result = m_pDevice->CreateDepthStencilView(m_pWaveParticleDSB, &dsvDesc, &m_pWaveParticleDSV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create depth stencil view")
            }

            result = m_pDevice->CreateShaderResourceView(m_pWaveParticleDSB, &dssrvDesc, &m_pWaveParticleDSSRV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create depth stencil SRV")
            }
        }

        D3D11_DEPTH_STENCIL_DESC dsDesc;
        ZeroMemory(&dsDesc, sizeof(dsDesc));
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
        result = m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDSState);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create depth stencil state")
        }

        // Set raster desc
        {
            RasterizerStateDesc rasterDesc;
            ZeroMemory(&rasterDesc, sizeof(rasterDesc));
            rasterDesc.m_rasterDesc.CullMode = D3D11_CULL_BACK;
            rasterDesc.m_rasterDesc.FillMode = D3D11_FILL_SOLID;
            result = m_pDevice->CreateRasterizerState(&rasterDesc.m_rasterDesc, &m_pCullRS);
            if (FAILED(result))
            {
                std::cout << "Failed to create raster state: D3D11_CULL_BACK, D3D11_FILL_SOLID" << std::endl;
            }
        }
        m_pDeviceContext->RSSetState(m_pCullRS);

        {
            RasterizerStateDesc rasterDesc;
            ZeroMemory(&rasterDesc, sizeof(rasterDesc));
            rasterDesc.m_rasterDesc.CullMode = D3D11_CULL_NONE;
            rasterDesc.m_rasterDesc.FillMode = D3D11_FILL_SOLID;
            result = m_pDevice->CreateRasterizerState(&rasterDesc.m_rasterDesc, &m_pNoCullRS);
            if (FAILED(result))
            {
                std::cout << "Failed to create raster state: D3D11_CULL_BACK, D3D11_FILL_SOLID" << std::endl;
            }
        }

        // Create and set viewport
        // TODO: Drive this from the camera
        m_mainViewport = { 0 };
        m_mainViewport.TopLeftX = 0.0f;
        m_mainViewport.TopLeftY = 0.0f;
        m_mainViewport.Width = (float)m_width;
        m_mainViewport.Height = (float)m_height;
        m_mainViewport.MinDepth = 0.0f;
        m_mainViewport.MaxDepth = 1.0f;
        m_pDeviceContext->RSSetViewports(1, &m_mainViewport);

        m_waveParticleViewport = { 0 };
        m_waveParticleViewport.TopLeftX = 0.0f;
        m_waveParticleViewport.TopLeftY = 0.0f;
        m_waveParticleViewport.Width = (float)WaveParticleHeightmapRenderTargetSize;
        m_waveParticleViewport.Height = (float)WaveParticleHeightmapRenderTargetSize;
        m_waveParticleViewport.MinDepth = 0.0f;
        m_waveParticleViewport.MaxDepth = 1.0f;


        // Create constatnt buffer
        D3D11_BUFFER_DESC cbd = { 0 };
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(Farlor::cbTransforms);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;

        result = m_pDevice->CreateBuffer(&cbd, 0, &m_cbTransformsBuffer);
        if (FAILED(result))
        {
            std::cout << "Failed to create cbTransforms buffer" << std::endl;
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
            FARLOR_LOG_ERROR("Failed to create cbMatProperties buffer")
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
            FARLOR_LOG_ERROR("Failed to create cbLightProperties buffer")
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
            FARLOR_LOG_ERROR("Failed to create cbCameraParams buffer")
        }

        // Create sampler state
        {
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
            result = m_pDevice->CreateSamplerState(&samplerDesc, &m_pWrapSamplerState);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to sampler state: D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS")
            }
        }

        // Create clamp sampler state
        {
            D3D11_SAMPLER_DESC samplerDesc;
            ZeroMemory(&samplerDesc, sizeof(samplerDesc));
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.MipLODBias = 0.0f;
            samplerDesc.MaxAnisotropy = 1;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
            samplerDesc.BorderColor[0] = 0;
            samplerDesc.BorderColor[1] = 0;
            samplerDesc.BorderColor[2] = 0;
            samplerDesc.BorderColor[3] = 0;
            samplerDesc.MinLOD = 0;
            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
            result = m_pDevice->CreateSamplerState(&samplerDesc, &m_pClampSamplerState);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create sampler state: D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_ALWAYS")
            }
        }

        // Create blend states
        {
            D3D11_BLEND_DESC additiveBlendDesc;
            ZeroMemory(&additiveBlendDesc, sizeof(additiveBlendDesc));
            additiveBlendDesc.RenderTarget[0].BlendEnable = true;
            additiveBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
            additiveBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
            additiveBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            additiveBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            additiveBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            additiveBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            additiveBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            result = m_pDevice->CreateBlendState(&additiveBlendDesc, &m_pAdditiveBlendState);

            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create additive blend state")
            }
        }


        // Create blend states
        {
            D3D11_BLEND_DESC noBlendDesc;
            ZeroMemory(&noBlendDesc, sizeof(noBlendDesc));
            noBlendDesc.RenderTarget[0].BlendEnable = false;
            //noBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
            //noBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
            //noBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            //noBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            //noBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            //noBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            noBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            result = m_pDevice->CreateBlendState(&noBlendDesc, &m_pNoBlendState);

            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create no blend state")
            }
        }


        // Wave Particle stuff
        // This can be any size, but it just cant be bigger than the window as a dsv is shared.
        // TODO: Have our own dsv
         // Wave particle main render target
        {
            D3D11_TEXTURE2D_DESC textureDesc = { 0 };
            textureDesc.Width = m_width;
            textureDesc.Height = m_height;
            textureDesc.MipLevels = 1;
            textureDesc.ArraySize = 1;
            textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Usage = D3D11_USAGE_DEFAULT;
            textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            textureDesc.CPUAccessFlags = 0;
            textureDesc.MiscFlags = 0;

            result = m_pDevice->CreateTexture2D(&textureDesc, 0, &m_pDebugBuffer);
            if (FAILED(result))
            {
                std::cout << "Failed to create wave particle texture" << std::endl;
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = textureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            result = m_pDevice->CreateRenderTargetView(m_pDebugBuffer, &renderTargetViewDesc, &m_pDebugRTV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create debug RTV")
            }
        }




        // Wave particle main render target
        {
            D3D11_TEXTURE2D_DESC waveParticleTextureDesc = { 0 };
            waveParticleTextureDesc.Width = WaveParticleHeightmapRenderTargetSize;
            waveParticleTextureDesc.Height = WaveParticleHeightmapRenderTargetSize;
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
                std::cout << "Failed to create wave particle texture" << std::endl;
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = waveParticleTextureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            result = m_pDevice->CreateRenderTargetView(m_pWaveParticleRenderTarget, &renderTargetViewDesc, &m_pWPRTView);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle RT RTV")
            }



            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
            shaderResourceViewDesc.Format = waveParticleTextureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            result = m_pDevice->CreateShaderResourceView(m_pWaveParticleRenderTarget, &shaderResourceViewDesc, &m_pWPSRView);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle RT SRV")
            }
        }

        // Wave particle first horizontal blur texture
        {
            D3D11_TEXTURE2D_DESC waveParticleBlurH1TextureDesc = { 0 };
            waveParticleBlurH1TextureDesc.Width = WaveParticleHeightmapRenderTargetSize;
            waveParticleBlurH1TextureDesc.Height = WaveParticleHeightmapRenderTargetSize;
            waveParticleBlurH1TextureDesc.MipLevels = 1;
            waveParticleBlurH1TextureDesc.ArraySize = 1;
            waveParticleBlurH1TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            waveParticleBlurH1TextureDesc.SampleDesc.Count = 1;
            waveParticleBlurH1TextureDesc.SampleDesc.Quality = 0;
            waveParticleBlurH1TextureDesc.Usage = D3D11_USAGE_DEFAULT;
            waveParticleBlurH1TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            waveParticleBlurH1TextureDesc.CPUAccessFlags = 0;
            waveParticleBlurH1TextureDesc.MiscFlags = 0;

            result = m_pDevice->CreateTexture2D(&waveParticleBlurH1TextureDesc, 0, &m_pWPHB1B);
            if (FAILED(result))
            {
                std::cout << "Failed to create wave particle texture: waveParticleBlurH1Texture" << std::endl;
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = waveParticleBlurH1TextureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            result = m_pDevice->CreateRenderTargetView(m_pWPHB1B, &renderTargetViewDesc, &m_pWPHB1RTV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle H1 RTV")
            }



            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
            shaderResourceViewDesc.Format = waveParticleBlurH1TextureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            result = m_pDevice->CreateShaderResourceView(m_pWPHB1B, &shaderResourceViewDesc, &m_pWPHB1SRV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle H1 SRV")
            }
        }

        // Wave particle second horizontal blur texture
        {
            D3D11_TEXTURE2D_DESC waveParticleBlurH2TextureDesc = { 0 };
            waveParticleBlurH2TextureDesc.Width = WaveParticleHeightmapRenderTargetSize;
            waveParticleBlurH2TextureDesc.Height = WaveParticleHeightmapRenderTargetSize;
            waveParticleBlurH2TextureDesc.MipLevels = 1;
            waveParticleBlurH2TextureDesc.ArraySize = 1;
            waveParticleBlurH2TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            waveParticleBlurH2TextureDesc.SampleDesc.Count = 1;
            waveParticleBlurH2TextureDesc.SampleDesc.Quality = 0;
            waveParticleBlurH2TextureDesc.Usage = D3D11_USAGE_DEFAULT;
            waveParticleBlurH2TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            waveParticleBlurH2TextureDesc.CPUAccessFlags = 0;
            waveParticleBlurH2TextureDesc.MiscFlags = 0;

            result = m_pDevice->CreateTexture2D(&waveParticleBlurH2TextureDesc, 0, &m_pWPHB2B);
            if (FAILED(result))
            {
                std::cout << "Failed to create wave particle texture: waveParticleBlurH2Texture" << std::endl;
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = waveParticleBlurH2TextureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            result = m_pDevice->CreateRenderTargetView(m_pWPHB2B, &renderTargetViewDesc, &m_pWPHB2RTV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle H2 RTV")
            }



            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
            shaderResourceViewDesc.Format = waveParticleBlurH2TextureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            result = m_pDevice->CreateShaderResourceView(m_pWPHB2B, &shaderResourceViewDesc, &m_pWPHB2SRV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle H2 SRV")
            }
        }

        // Wave particle first vertical blur texture
        {
            D3D11_TEXTURE2D_DESC waveParticleBlurV1TextureDesc = { 0 };
            waveParticleBlurV1TextureDesc.Width = WaveParticleHeightmapRenderTargetSize;
            waveParticleBlurV1TextureDesc.Height = WaveParticleHeightmapRenderTargetSize;
            waveParticleBlurV1TextureDesc.MipLevels = 1;
            waveParticleBlurV1TextureDesc.ArraySize = 1;
            waveParticleBlurV1TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            waveParticleBlurV1TextureDesc.SampleDesc.Count = 1;
            waveParticleBlurV1TextureDesc.SampleDesc.Quality = 0;
            waveParticleBlurV1TextureDesc.Usage = D3D11_USAGE_DEFAULT;
            waveParticleBlurV1TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            waveParticleBlurV1TextureDesc.CPUAccessFlags = 0;
            waveParticleBlurV1TextureDesc.MiscFlags = 0;

            result = m_pDevice->CreateTexture2D(&waveParticleBlurV1TextureDesc, 0, &m_pWPVB1B);
            if (FAILED(result))
            {
                std::cout << "Failed to create wave particle texture: waveParticleBlurV1Texture" << std::endl;
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = waveParticleBlurV1TextureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            result = m_pDevice->CreateRenderTargetView(m_pWPVB1B, &renderTargetViewDesc, &m_pWPVB1RTV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle V1 RTV")
            }



            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
            shaderResourceViewDesc.Format = waveParticleBlurV1TextureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            result = m_pDevice->CreateShaderResourceView(m_pWPVB1B, &shaderResourceViewDesc, &m_pWPVB1SRV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle V1 SRV")
            }
        }

        // Wave particle second vertical blur texture
        {
            D3D11_TEXTURE2D_DESC waveParticleBlurV2TextureDesc = { 0 };
            waveParticleBlurV2TextureDesc.Width = WaveParticleHeightmapRenderTargetSize;
            waveParticleBlurV2TextureDesc.Height = WaveParticleHeightmapRenderTargetSize;
            waveParticleBlurV2TextureDesc.MipLevels = 1;
            waveParticleBlurV2TextureDesc.ArraySize = 1;
            waveParticleBlurV2TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            waveParticleBlurV2TextureDesc.SampleDesc.Count = 1;
            waveParticleBlurV2TextureDesc.SampleDesc.Quality = 0;
            waveParticleBlurV2TextureDesc.Usage = D3D11_USAGE_DEFAULT;
            waveParticleBlurV2TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            waveParticleBlurV2TextureDesc.CPUAccessFlags = 0;
            waveParticleBlurV2TextureDesc.MiscFlags = 0;

            result = m_pDevice->CreateTexture2D(&waveParticleBlurV2TextureDesc, 0, &m_pWPVB2B);
            if (FAILED(result))
            {
                std::cout << "Failed to create wave particle texture: waveParticleBlurV2Texture" << std::endl;
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = waveParticleBlurV2TextureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            result = m_pDevice->CreateRenderTargetView(m_pWPVB2B, &renderTargetViewDesc, &m_pWPVB2RTV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle V2 RTV")
            }



            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
            shaderResourceViewDesc.Format = waveParticleBlurV2TextureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            result = m_pDevice->CreateShaderResourceView(m_pWPVB2B, &shaderResourceViewDesc, &m_pWPVB2SRV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create wave particle V2 SRV")
            }
        }

        // Caustic Texture
        {
            D3D11_TEXTURE2D_DESC causticTextureDesc = { 0 };
            causticTextureDesc.Width = WaveParticleHeightmapRenderTargetSize;
            causticTextureDesc.Height = WaveParticleHeightmapRenderTargetSize;
            causticTextureDesc.MipLevels = 1;
            causticTextureDesc.ArraySize = 1;
            causticTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            causticTextureDesc.SampleDesc.Count = 1;
            causticTextureDesc.SampleDesc.Quality = 0;
            causticTextureDesc.Usage = D3D11_USAGE_DEFAULT;
            causticTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            causticTextureDesc.CPUAccessFlags = 0;
            causticTextureDesc.MiscFlags = 0;

            result = m_pDevice->CreateTexture2D(&causticTextureDesc, 0, &m_pCausticTextureBuffer);
            if (FAILED(result))
            {
                std::cout << "Failed to create caustic texture: m_pCausticTextureBuffer" << std::endl;
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = causticTextureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            result = m_pDevice->CreateRenderTargetView(m_pCausticTextureBuffer, &renderTargetViewDesc, &m_pCausticRTV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create caustic texture RTV")
            }



            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
            shaderResourceViewDesc.Format = causticTextureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            result = m_pDevice->CreateShaderResourceView(m_pCausticTextureBuffer, &shaderResourceViewDesc, &m_pCausticSRV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create caustic texture SRV")
            }
        }


        // Mid caustics
        {
            D3D11_TEXTURE2D_DESC causticTextureDesc = { 0 };
            causticTextureDesc.Width = WaveParticleHeightmapRenderTargetSize;
            causticTextureDesc.Height = WaveParticleHeightmapRenderTargetSize;
            causticTextureDesc.MipLevels = 1;
            causticTextureDesc.ArraySize = 1;
            causticTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            causticTextureDesc.SampleDesc.Count = 1;
            causticTextureDesc.SampleDesc.Quality = 0;
            causticTextureDesc.Usage = D3D11_USAGE_DEFAULT;
            causticTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            causticTextureDesc.CPUAccessFlags = 0;
            causticTextureDesc.MiscFlags = 0;

            result = m_pDevice->CreateTexture2D(&causticTextureDesc, 0, &m_pCausticsMidPass00TextureBuffer);
            if (FAILED(result))
            {
                std::cout << "Failed to create caustic texture: m_pCausticsMidPass00TextureBuffer" << std::endl;
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = causticTextureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            result = m_pDevice->CreateRenderTargetView(m_pCausticsMidPass00TextureBuffer, &renderTargetViewDesc, &m_pCausticsMidPass00RTV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create mid caustic texture 00 RTV")
            }



            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
            shaderResourceViewDesc.Format = causticTextureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            result = m_pDevice->CreateShaderResourceView(m_pCausticsMidPass00TextureBuffer, &shaderResourceViewDesc, &m_pCausticsMidPass00SRV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create mid caustic texture 00 SRV")
            }
        }
        {
            D3D11_TEXTURE2D_DESC causticTextureDesc = { 0 };
            causticTextureDesc.Width = WaveParticleHeightmapRenderTargetSize;
            causticTextureDesc.Height = WaveParticleHeightmapRenderTargetSize;
            causticTextureDesc.MipLevels = 1;
            causticTextureDesc.ArraySize = 1;
            causticTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            causticTextureDesc.SampleDesc.Count = 1;
            causticTextureDesc.SampleDesc.Quality = 0;
            causticTextureDesc.Usage = D3D11_USAGE_DEFAULT;
            causticTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            causticTextureDesc.CPUAccessFlags = 0;
            causticTextureDesc.MiscFlags = 0;

            result = m_pDevice->CreateTexture2D(&causticTextureDesc, 0, &m_pCausticsMidPass01TextureBuffer);
            if (FAILED(result))
            {
                std::cout << "Failed to create caustic texture: m_pCausticsMidPass01TextureBuffer" << std::endl;
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = causticTextureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            result = m_pDevice->CreateRenderTargetView(m_pCausticsMidPass01TextureBuffer, &renderTargetViewDesc, &m_pCausticsMidPass01RTV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create mid caustic texture 01 RTV")
            }



            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
            shaderResourceViewDesc.Format = causticTextureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            result = m_pDevice->CreateShaderResourceView(m_pCausticsMidPass01TextureBuffer, &shaderResourceViewDesc, &m_pCausticsMidPass01SRV);
            if (FAILED(result))
            {
                FARLOR_LOG_ERROR("Failed to create mid caustic texture 01 SRV")
            }
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
        result = m_pDevice->CreateDepthStencilState(&dsDesc, &pDSState);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create depth stencil state: wave particle")
        }

        // ensure that our particle system and terrain systems initialized
        g_WaveParticles.InitializeBuffers(m_pDevice, m_pDeviceContext);
        m_waterSurface.Initialize(10.0, 10.0, Farlor::Vector3(-5.0, 0.0, -5.0), 500, m_pDevice, m_pDeviceContext);
        m_TexturedQuad.Initialize(m_pDevice, m_pDeviceContext);
    }

    void Renderer::Render()
    {

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        bool showDemoWindow = true;
        ImGui::ShowDemoWindow(&showDemoWindow);

        ImGui::Render();

        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        // Clear main render target
        {

            m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, clearColor);
            m_pDeviceContext->ClearRenderTargetView(m_pDebugRTV, clearColor);
            m_pDeviceContext->ClearDepthStencilView(m_pWindowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }

        // Do wave particel rendering
        
        // Clear wave particle stuff
        {
            m_pDeviceContext->ClearRenderTargetView(m_pWPRTView, clearColor);
            m_pDeviceContext->ClearDepthStencilView(m_pWaveParticleDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }

        m_pDeviceContext->RSSetViewports(1, &m_waveParticleViewport);

        m_pDeviceContext->OMSetRenderTargets(1, &m_pWPRTView, m_pWaveParticleDSV);

        // Render the wave particles
        // m_pDeviceContext->RSSetState(m_rasterState);
        m_pDeviceContext->OMSetDepthStencilState(pDSState, 0);

        m_pDeviceContext->OMSetBlendState(m_pAdditiveBlendState, 0, 0xffffffff);
        g_WaveParticles.Render(m_pDevice, m_pDeviceContext);
        m_pDeviceContext->OMSetBlendState(m_pNoBlendState, 0, 0xffffffff);

        // Render horizontal blur
        {
            m_pDeviceContext->ClearRenderTargetView(m_pWPHB1RTV, clearColor);
            m_pDeviceContext->ClearRenderTargetView(m_pWPHB2RTV, clearColor);
            m_pDeviceContext->ClearDepthStencilView(m_pWaveParticleDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



            ID3D11RenderTargetView* pRenderTargets[2];
            pRenderTargets[0] = m_pWPHB1RTV;
            pRenderTargets[1] = m_pWPHB2RTV;
            m_pDeviceContext->OMSetRenderTargets(2, pRenderTargets, m_pWaveParticleDSV);

            m_pDeviceContext->PSSetShaderResources(0, 1, &m_pWPSRView);
            m_pDeviceContext->PSSetSamplers(0, 1, &m_pWrapSamplerState);

            m_shaders["WaveParticleHB"].SetPipeline(m_pDeviceContext);

            m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            m_pDeviceContext->Draw(3, 0);
        }

        // Render vertical blur
        {
            m_pDeviceContext->ClearRenderTargetView(m_pWPVB1RTV, clearColor);
            m_pDeviceContext->ClearRenderTargetView(m_pWPVB2RTV, clearColor);
            m_pDeviceContext->ClearDepthStencilView(m_pWaveParticleDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            ID3D11RenderTargetView* pRenderTargets[2];
            pRenderTargets[0] = m_pWPVB1RTV;
            pRenderTargets[1] = m_pWPVB2RTV;
            m_pDeviceContext->OMSetRenderTargets(2, pRenderTargets, m_pWaveParticleDSV);


            ID3D11ShaderResourceView* pSRVs[2];
            pSRVs[0] = m_pWPHB1SRV;
            pSRVs[1] = m_pWPHB2SRV;
            m_pDeviceContext->PSSetShaderResources(0, 2, pSRVs);
            m_pDeviceContext->PSSetSamplers(0, 1, &m_pWrapSamplerState);

            m_shaders["WaveParticleVB"].SetPipeline(m_pDeviceContext);

            m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            m_pDeviceContext->Draw(3, 0);
        }


        // Render caustic map
        {
            m_pDeviceContext->ClearRenderTargetView(m_pCausticsMidPass00RTV, clearColor);
            m_pDeviceContext->ClearRenderTargetView(m_pCausticsMidPass01RTV, clearColor);
        }
        {
            float causticClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
            m_pDeviceContext->ClearRenderTargetView(m_pCausticRTV, causticClearColor);
        }

        // Caustics Pass 00
        {
            ID3D11RenderTargetView* pRenderTargets[8];
            pRenderTargets[0] = m_pCausticsMidPass00RTV;
            pRenderTargets[1] = m_pCausticsMidPass01RTV;
            pRenderTargets[2] = nullptr;
            pRenderTargets[3] = nullptr;
            pRenderTargets[4] = nullptr;
            pRenderTargets[5] = nullptr;
            pRenderTargets[6] = nullptr;
            pRenderTargets[7] = nullptr;
            m_pDeviceContext->OMSetRenderTargets(8, pRenderTargets, m_pWaveParticleDSV);

            ID3D11ShaderResourceView* pSRVs[8];
            pSRVs[0] = m_pWPVB1SRV;
            pSRVs[1] = m_pWPVB2SRV;
            pSRVs[2] = nullptr;
            pSRVs[3] = nullptr;
            pSRVs[4] = nullptr;
            pSRVs[5] = nullptr;
            pSRVs[6] = nullptr;
            pSRVs[7] = nullptr;
            m_pDeviceContext->PSSetShaderResources(0, 8, pSRVs);
            m_pDeviceContext->PSSetSamplers(0, 1, &m_pClampSamplerState);

            m_shaders["Caustics00"].SetPipeline(m_pDeviceContext);

            m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            m_pDeviceContext->Draw(3, 0);
        }


        // Caustics Pass 01
        {
            ID3D11RenderTargetView* pRenderTargets[8];
            pRenderTargets[0] = m_pCausticRTV;
            pRenderTargets[1] = nullptr;
            pRenderTargets[2] = nullptr;
            pRenderTargets[3] = nullptr;
            pRenderTargets[4] = nullptr;
            pRenderTargets[5] = nullptr;
            pRenderTargets[6] = nullptr;
            pRenderTargets[7] = nullptr;
            m_pDeviceContext->OMSetRenderTargets(8, pRenderTargets, m_pWaveParticleDSV);

            ID3D11ShaderResourceView* pSRVs[2];
            pSRVs[0] = m_pCausticsMidPass00SRV;
            pSRVs[1] = m_pCausticsMidPass01SRV;
            pSRVs[2] = nullptr;
            pSRVs[3] = nullptr;
            pSRVs[4] = nullptr;
            pSRVs[5] = nullptr;
            pSRVs[6] = nullptr;
            pSRVs[7] = nullptr;
            m_pDeviceContext->PSSetShaderResources(0, 8, pSRVs);
            m_pDeviceContext->PSSetSamplers(0, 1, &m_pClampSamplerState);

            m_shaders["Caustics01"].SetPipeline(m_pDeviceContext);

            m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            m_pDeviceContext->Draw(3, 0);
        }






        // Set back to main viewport
        // Perspective
        m_camProjection = DirectX::XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)m_width / m_height, 1.0f, 1000.0f);


        m_pDeviceContext->RSSetViewports(1, &m_mainViewport);

        m_camView = g_MainCamera.m_camView;

        m_pDeviceContext->ClearRenderTargetView(m_renderTargets["G-Normals"].m_pRTView, clearColor);
        m_pDeviceContext->ClearRenderTargetView(m_renderTargets["G-Diffuse"].m_pRTView, clearColor);
        m_pDeviceContext->ClearRenderTargetView(m_renderTargets["G-Specular"].m_pRTView, clearColor);
        m_pDeviceContext->ClearRenderTargetView(m_renderTargets["G-Position"].m_pRTView, clearColor);
        m_pDeviceContext->ClearDepthStencilView(m_pWindowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        m_pDeviceContext->OMSetDepthStencilState(nullptr, 0);


        m_shaders["GBufferBasePass"].SetPipeline(m_pDeviceContext);

        m_pDeviceContext->PSSetShaderResources(0, 1, &m_pHouseTextureSRV);
        m_pDeviceContext->PSSetSamplers(0, 1, &m_pWrapSamplerState);

        ID3D11Buffer* pConstantBuffers[2];
        pConstantBuffers[0] = m_cbTransformsBuffer;
        pConstantBuffers[1] = m_cbMatPropertiesBuffer;

        m_pDeviceContext->VSSetConstantBuffers(0, 2, pConstantBuffers);
        m_pDeviceContext->PSSetConstantBuffers(0, 2, pConstantBuffers);

        ID3D11RenderTargetView* deferredRTViews[4];
        deferredRTViews[0] = m_renderTargets["G-Normals"].m_pRTView;
        deferredRTViews[1] = m_renderTargets["G-Diffuse"].m_pRTView;
        deferredRTViews[2] = m_renderTargets["G-Specular"].m_pRTView;
        deferredRTViews[3] = m_renderTargets["G-Position"].m_pRTView;

        m_pDeviceContext->OMSetRenderTargets(4, deferredRTViews, m_pWindowDSV);

        // Render all objects
        for (auto& renderComponent : m_renderingComponents)
        {
            Render(renderComponent);
        }

        m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, nullptr);

        ID3D11ShaderResourceView* resourceViews[4];

        resourceViews[0] = m_renderTargets["G-Normals"].m_pRTShaderResourceView;
        resourceViews[1] = m_renderTargets["G-Diffuse"].m_pRTShaderResourceView;
        resourceViews[2] = m_renderTargets["G-Specular"].m_pRTShaderResourceView;
        resourceViews[3] = m_renderTargets["G-Position"].m_pRTShaderResourceView;

        m_pDeviceContext->OMSetBlendState(m_pAdditiveBlendState, 0, 0xffffffff);

        // Render ambient light
        m_shaders["GBufferLightAmbient"].SetPipeline(m_pDeviceContext);
        m_pDeviceContext->PSSetShaderResources(0, 1, &m_renderTargets["G-Diffuse"].m_pRTShaderResourceView);

        // Turn on alpha blending
        m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        m_pDeviceContext->Draw(3, 0);


        m_shaders["GBufferLightPassDirectional"].SetPipeline(m_pDeviceContext);
        m_pDeviceContext->PSSetShaderResources(0, 4, resourceViews);

        // Render Lights
        for (auto& lightComponent : m_lightComponents)
        {
            RenderLight(lightComponent);
        }

        m_pDeviceContext->OMSetBlendState(m_pNoBlendState, 0, 0xffffffff);


        // Render the height field
        ID3D11RenderTargetView* pRenderTargets[2];
        pRenderTargets[0] = m_pBackBufferRTV;
        pRenderTargets[1] = m_pDebugRTV;

        m_pDeviceContext->OMSetRenderTargets(2, pRenderTargets, m_pWindowDSV);

        m_pDeviceContext->RSSetState(m_pNoCullRS);

        // Note: Render lights handled this, so we probably dont need this, but we will do it again to be sure
        DirectX::XMFLOAT3 pos;
        DirectX::XMStoreFloat3(&pos, g_MainCamera.m_camPosition);
        m_cbCameraParams.m_eyePosition = Vector3(pos.x, pos.y, pos.z);
        m_pDeviceContext->UpdateSubresource(m_cbCameraParamsBuffer, 0, 0, &m_cbCameraParams, 0, 0);

        m_waterSurface.Render(m_pDevice, m_pDeviceContext, m_pWPVB1SRV, m_pWPVB2SRV, m_pClampSamplerState, m_cbCameraParamsBuffer);




        // Render textured quad

        m_TexturedQuad.Render(m_pDevice, m_pDeviceContext, m_pCausticSRV, m_pClampSamplerState, m_cbCameraParamsBuffer);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        m_pSwapChain->Present(0, 0);
    }

    void Renderer::Render(RenderingComponent& renderingComponent)
    {
        // std::cout << "Rendering Component: " << renderingComponent.m_mesh << std::endl;
        TransformComponent transformComponent = g_TransformManager.GetComponent(renderingComponent.m_entity);

        DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

        DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(transformComponent.m_scale.x, transformComponent.m_scale.y, transformComponent.m_scale.z);
        // XMMATRIX translate = XMMatrixTranslation(, comp.m_position.y, comp.m_position.z);

        // XMVECTOR rotAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationRollPitchYaw(transformComponent.m_rotation.x, transformComponent.m_rotation.y, transformComponent.m_rotation.z);

        DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(transformComponent.m_position.x, transformComponent.m_position.y, transformComponent.m_position.z);
        world = scale * rotate * translate;

        m_cbTransforms.World = DirectX::XMMatrixTranspose(world);

        DirectX::XMMATRIX wv = world * m_camView;

        m_cbTransforms.WorldView = DirectX::XMMatrixTranspose(wv);

        DirectX::XMMATRIX wvp = world * m_camView * m_camProjection;

        m_cbTransforms.WorldViewProjection = DirectX::XMMatrixTranspose(wvp);

        m_pDeviceContext->UpdateSubresource(m_cbTransformsBuffer, 0, 0, &m_cbTransforms, 0, 0);

        // Material properties
        m_cbMatProperties.SpecularAlbedo = Vector3(1.0f, 1.0f, 1.0f);
        m_cbMatProperties.SpecularPower = 1.0f;
        m_pDeviceContext->UpdateSubresource(m_cbMatPropertiesBuffer, 0, 0, &m_cbMatProperties, 0, 0);

        ID3D11Buffer* pConstantBuffers[2];
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

        DirectX::XMFLOAT3 pos;
        DirectX::XMStoreFloat3(&pos, g_MainCamera.m_camPosition);
        m_cbCameraParams.m_eyePosition = Vector3(pos.x, pos.y, pos.z);

        m_pDeviceContext->UpdateSubresource(m_cbCameraParamsBuffer, 0, 0, &m_cbCameraParams, 0, 0);

        ID3D11Buffer* pConstantBuffers[2];
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
                rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
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

    void Renderer::RegisterShader(std::string filename, std::string name, bool hasVS, bool hasPS, std::vector<std::string> defines)
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
