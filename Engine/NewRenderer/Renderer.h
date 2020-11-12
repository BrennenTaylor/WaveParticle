#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>

#include "RenderingComponent.h"
#include "LightComponents.h"

#include "RenderResourceDesc.h"
#include "RenderTarget.h"

#include "../Physics/Particlesystem.h"

#include "D3D11/DXGIModeDesc.h"
#include "D3D11/DXGISwapChainDesc.h"
#include "D3D11/RasterizerStateDesc.h"

#include "../Core/Window.h"
#include "../ECS/Entity.h"
#include "Mesh.h"
#include "CBs.h"
#include "Shader.h"
#include "ViewMode.h"


//// Wave Particle Stuff
#include "HeightField.h"
//
#include <DirectXMath.h>

namespace Farlor
{
class Renderer
    {
    public:
        void Initialize(GameWindow gameWindow);
        void Render();
        void Render(RenderingComponent& renderingComponent);
        void RenderLight(LightComponent& lightComponent);

        void AddComponent(RenderingComponent component);
        void AddComponent(LightComponent component);

        void RegisterGlobalResource(std::string name, std::string resourceType, int widthScale, int heightScale, std::string format);
        void CreateGlobalResources();
        void RegisterShader(std::string filename, std::string name, bool hasVS, bool hasPS, std::vector<std::string> m_shaders);
        void CreateShaders();

        static RenderResourceType TypeFromString(std::string strType);
        static RenderResourceFormat FormatFromString(std::string strFormat);

        DXGI_FORMAT FormatToDXGIFormat(RenderResourceFormat format);


        // Wave particle stuff
        void AddParticleSystem();

    public:
        DirectX::XMMATRIX m_camView;
        DirectX::XMMATRIX m_camProjection;

        int m_width, m_height;
        std::vector<RenderingComponent> m_renderingComponents;
        std::vector<LightComponent> m_lightComponents;

        std::vector<RenderResourceDesc> m_globalResourcesDescs;

    private:
        void InitializeShaders();
        void RenderParticleSystems();

        void LoadMesh(HashedString meshName);

    private:
        HWND m_gameWindowHandle;

        IDXGISwapChain* m_pSwapChain = nullptr;

        ID3D11Device* m_pDevice = nullptr;
        ID3D11DeviceContext* m_pDeviceContext = nullptr;
        ID3D11Texture2D* m_pBackBuffer = nullptr;
        ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;
        
        ID3D11Texture2D* m_pWindowDSB = nullptr;
        ID3D11DepthStencilView* m_pWindowDSV = nullptr;
        ID3D11ShaderResourceView* m_pWindowDSSRV = nullptr;

        ID3D11Texture2D* m_pWaveParticleDSB = nullptr;
        ID3D11DepthStencilView* m_pWaveParticleDSV = nullptr;
        ID3D11ShaderResourceView* m_pWaveParticleDSSRV = nullptr;

        D3D11_VIEWPORT m_mainViewport;
        D3D11_VIEWPORT m_waveParticleViewport;


        ID3D11RasterizerState* m_pCullRS = nullptr;
        ID3D11RasterizerState* m_pNoCullRS = nullptr;

        ID3D11Buffer* m_cbTransformsBuffer;
        cbTransforms m_cbTransforms;

        ID3D11Buffer* m_cbMatPropertiesBuffer;
        cbMatProperties m_cbMatProperties;

        ID3D11Buffer* m_cbLightPropertiesBuffer;
        cbLightProperties m_cbLightProperties;

        ID3D11Buffer* m_cbCameraParamsBuffer;
        cbCameraParams m_cbCameraParams;

        std::map<std::string, RenderTarget> m_renderTargets;

        ID3D11ShaderResourceView* m_pHouseTextureSRV;
        ID3D11SamplerState* m_pSamplerState;

        ID3D11DepthStencilState* m_pDSState;

        ID3D11BlendState* m_pAdditiveBlendState = nullptr;
        ID3D11BlendState* m_pNoBlendState = nullptr;

        std::vector<std::string> m_shaderFilenames;
        std::map<std::string, Shader> m_shaders;

        std::map<HashedString, Mesh> m_meshes;

        ViewMode m_currentViewMode;

        // Wave Particle stuff
        ID3D11Texture2D* m_pWaveParticleRenderTarget;
        ID3D11RenderTargetView* m_pWPRTView;
        ID3D11ShaderResourceView* m_pWPSRView;

        ID3D11Texture2D* m_pWaveParticleStagingResource;

        ID3D11ShaderResourceView* m_pParticleTextureResourceView;

        std::vector<ParticleSystem> m_particleSystems;

        ID3D11SamplerState* m_pWPSampleState;

        ID3D11DepthStencilState * pDSState;

        HeightField m_waterSurface;

        Vector4* m_pHeightmapPositions;


        static std::map<std::string, RenderResourceType> s_stringToTypeMapping;
        static std::map<std::string, RenderResourceFormat> s_stringToFormatMapping;

        static std::map<RenderResourceFormat, DXGI_FORMAT> s_formatToDXGIFormatMapping;
    };
}
