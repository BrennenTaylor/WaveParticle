#include "DXGIModeDesc.h"

namespace Farlor
{
    DXGIModeDesc::DXGIModeDesc(int width, int height)
    {
        ZeroMemory(&m_dxgiModeDesc, sizeof(m_dxgiModeDesc));

        Default(width, height);
    }

    void DXGIModeDesc::Default(int width, int height)
    {
        m_dxgiModeDesc.Width = width;
        m_dxgiModeDesc.Height = height;
        m_dxgiModeDesc.RefreshRate.Numerator = 60;
        m_dxgiModeDesc.RefreshRate.Denominator = 1;
        m_dxgiModeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        m_dxgiModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        m_dxgiModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    }
}
