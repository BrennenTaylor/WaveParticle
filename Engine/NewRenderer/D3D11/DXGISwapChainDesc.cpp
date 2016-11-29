#include "DXGISwapChainDesc.h"

namespace Farlor
{
    DXGISwapChainDesc::DXGISwapChainDesc(DXGIModeDesc dxgiModeDesc, GameWindow& window)
    {
        ZeroMemory(&m_dxgiSwapChainDesc, sizeof(m_dxgiSwapChainDesc));

        Default(dxgiModeDesc, window);
    }

    void DXGISwapChainDesc::Default(DXGIModeDesc dxgiModeDesc, GameWindow& window)
    {
        m_dxgiSwapChainDesc.BufferDesc = dxgiModeDesc.m_dxgiModeDesc;
        m_dxgiSwapChainDesc.SampleDesc.Count = 1;
        m_dxgiSwapChainDesc.SampleDesc.Quality = 0;
        m_dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        m_dxgiSwapChainDesc.BufferCount = 1;
        m_dxgiSwapChainDesc.OutputWindow = window.GetWindowHandle();
        m_dxgiSwapChainDesc.Windowed = true;
        m_dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    }
}
