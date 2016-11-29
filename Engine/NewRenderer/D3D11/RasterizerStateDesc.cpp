#include "RasterizerStateDesc.h"

namespace Farlor
{
    RasterizerStateDesc::RasterizerStateDesc()
    {
        ZeroMemory(&m_rasterDesc, sizeof(m_rasterDesc));

        Default();
    }

    void RasterizerStateDesc::Default()
    {
        m_rasterDesc.AntialiasedLineEnable = false;
        m_rasterDesc.CullMode = D3D11_CULL_BACK;
        m_rasterDesc.DepthBias = 0;
        m_rasterDesc.DepthBiasClamp = 0.0f;
        m_rasterDesc.DepthClipEnable = true;
        m_rasterDesc.FillMode = D3D11_FILL_SOLID;
        m_rasterDesc.FrontCounterClockwise = false;
        m_rasterDesc.MultisampleEnable = false;
        m_rasterDesc.ScissorEnable = false;
        m_rasterDesc.SlopeScaledDepthBias = false;
    }
}
