#pragma once

#include <d3d11.h>

#include <vector>
#include <string>

namespace Farlor
{
    class Shader
    {
    public:
        Shader();
        Shader(std::string filename, bool hasVS, bool hasPS, std::vector<std::string> defines);
        void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

        void SetPipeline(ID3D11DeviceContext* pDeviceContext);

    private:
        std::string m_filename;

        ID3D11VertexShader* m_pVertexShader;
        ID3D11PixelShader* m_pPixelShader;
        ID3D11InputLayout* m_pInputLayout;

        bool m_hasVS;
        bool m_hasPS;

        std::vector<std::string> m_defines;
    };
}
