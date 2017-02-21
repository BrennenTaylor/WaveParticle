#pragma once

#include <d3d11.h>
#include "../Math/Math.h"

namespace Farlor
{
    struct VertexPosition
    {
        static D3D11_INPUT_ELEMENT_DESC s_layout[];
        static u32 s_numElements;

        Vector3 m_position;

        VertexPosition();
        VertexPosition(float x, float y, float z);
    };

    struct VertexPositionUV
    {
        static D3D11_INPUT_ELEMENT_DESC s_layout[];
        static u32 s_numElements;

        Vector3 m_position;
        Vector2 m_uv;

        VertexPositionUV();
        VertexPositionUV(float x, float y, float z, float u, float v);
    };

    struct WaveParticleVertex
    {
        static D3D11_INPUT_ELEMENT_DESC s_layout[];
        static u32 s_numElements;

        Vector3 m_position;
        Vector2 m_uv;
        float m_amplitude;
        float m_angle;
        Vector3 m_origin;

        WaveParticleVertex();
        WaveParticleVertex(float x, float y, float z, float u, float v, float amplitude, float angle, Vector3 origin);
    };

    struct VertexPositionUVNormal
    {
        static D3D11_INPUT_ELEMENT_DESC s_layout[];
        static u32 s_numElements;

        Vector3 m_position;
        Vector2 m_uv;
        Vector3 m_normal;

        VertexPositionUVNormal();
        VertexPositionUVNormal(float x, float y, float z, float u, float v, float nx, float ny, float nz);

        bool operator==(const VertexPositionUVNormal& other) const;
    };
    //
    struct VertexPositionColorUV
    {
        static D3D11_INPUT_ELEMENT_DESC s_layout[];
        static u32 s_numElements;

        Vector3 m_position;
        Vector4 m_color;
        Vector2 m_uv;

        VertexPositionColorUV();
        VertexPositionColorUV(float x, float y, float z, float r, float g, float b, float a, float u, float v);

        bool operator==(const VertexPositionColorUV& other) const;
    };
}

namespace std
{
    template<> struct hash<Farlor::VertexPositionUVNormal>
    {
        size_t operator()(Farlor::VertexPositionUVNormal const& vertex) const
        {
            return ((hash<Farlor::Vector3>()(vertex.m_position) ^ (hash<Farlor::Vector3>()(vertex.m_normal) << 1)) >> 1) ^ (hash<Farlor::Vector2>()(vertex.m_uv) << 1);
        }
    };
}
