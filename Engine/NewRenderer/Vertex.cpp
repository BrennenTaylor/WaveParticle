#include "Vertex.h"

namespace Farlor
{
    // Vertex Position
    D3D11_INPUT_ELEMENT_DESC VertexPosition::s_layout[] =
    {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
    };

    u32 VertexPosition::s_numElements = 1;

    VertexPosition::VertexPosition()
        : m_position{0.0f, 0.0f, 0.0f}
    {
    }

    VertexPosition::VertexPosition(float x, float y, float z)
        : m_position{x, y, z}
    {
    }


    // Vertex Position UV Normal
    D3D11_INPUT_ELEMENT_DESC VertexPositionUVNormal::s_layout[] =
    {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
    };

    u32 VertexPositionUVNormal::s_numElements = 3;

    VertexPositionUVNormal::VertexPositionUVNormal()
        : m_position{0.0f, 0.0f, 0.0f}
        , m_uv{0.0f, 0.0f}
        , m_normal{0.0f, 0.0f, 0.0f}
    {
    }

    VertexPositionUVNormal::VertexPositionUVNormal(float x, float y, float z, float u, float v, float nx, float ny, float nz)
        : m_position{x, y, z}
        , m_uv{u, v}
        , m_normal{nx, ny, nz}
    {
    }

    bool VertexPositionUVNormal::operator==(const VertexPositionUVNormal& other) const {
        return (Vector3)m_position == (Vector3)other.m_position && (Vector3)m_normal == (Vector3)other.m_normal && (Vector2)m_uv == (Vector2)other.m_uv;
    }

    //  Wave Particle Vertex
    D3D11_INPUT_ELEMENT_DESC WaveParticleVertex::s_layout[] =
    {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "AMPLITUDE", 0, DXGI_FORMAT_R32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "ANGLE", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "ORIGIN", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
    };

    u32 WaveParticleVertex::s_numElements = 5;

    WaveParticleVertex::WaveParticleVertex()
        : m_position{0.0f, 0.0f, 0.0f}
        , m_uv{0.0f, 0.0f}
        , m_amplitude{1.0f}
        , m_angle{0.0f}
        , m_origin(0.0f, 0.0f, 0.0f)
    {
    }

    WaveParticleVertex::WaveParticleVertex(float x, float y, float z, float u, float v, float amplitude, float angle, Vector3 origin)
        : m_position{x, y, z}
        , m_uv{u, v}
        , m_amplitude{amplitude}
        , m_angle{angle}
        , m_origin(origin)
    {
    }

    //  Vertex Position UV
    D3D11_INPUT_ELEMENT_DESC VertexPositionUV::s_layout[] =
    {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
    };

    u32 VertexPositionUV::s_numElements = 2;

    VertexPositionUV::VertexPositionUV()
        : m_position{0.0f, 0.0f, 0.0f}
        , m_uv{0.0f, 0.0f}
    {
    }

    VertexPositionUV::VertexPositionUV(float x, float y, float z, float u, float v)
        : m_position{x, y, z}
        , m_uv{u, v}
    {
    }


    //  Vertex Position Color UV
    D3D11_INPUT_ELEMENT_DESC VertexPositionColorUV::s_layout[] =
    {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
    };
    
    u32 VertexPositionColorUV::s_numElements = 3;

    VertexPositionColorUV::VertexPositionColorUV()
        : m_position{0.0f, 0.0f, 0.0f}
        , m_color{1.0f, 0.0f, 1.0f, 1.0f}
        , m_uv{0.0f, 0.0f}
    {
    }

    VertexPositionColorUV::VertexPositionColorUV(float x, float y, float z, float r, float g, float b, float a, float u, float v)
        : m_position{x, y, z}
        , m_color{r, g, b, a}
        , m_uv{u, v}
    {
    }

    bool VertexPositionColorUV::operator==(const VertexPositionColorUV& other) const {
        return (Vector3)m_position == (Vector3)other.m_position && (Vector2)m_uv == (Vector2)other.m_uv;
    }
}
