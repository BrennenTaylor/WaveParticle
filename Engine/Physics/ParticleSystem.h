#pragma once

#include <vector>

#include "Particle.h"

#include "../NewRenderer/Vertex.h"

#include "../Physics/Plane.h"
#include "../Physics/LineSegment.h"

#include <DirectXMath.h>

namespace Farlor
{
    class ParticleSystem
    {
    public:
        struct cbPerObject
        {
            DirectX::XMMATRIX WVP;
        };

        ParticleSystem(int numParticles);

        void AddParticle(WaveParticle particle);
        void AddPlane(Plane plane);
        inline void AddSegments(LineSegment segment)
        {
            m_collisionSegments.push_back(segment);
        }

        void Update(float timestep);

        void InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
        void UpdateBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
        void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pWSRView, ID3D11SamplerState* pWPSampleState);

        void StartRandomWave();

        void KillParticles();

    private:
        std::vector<WaveParticle> m_waveParticles;
        std::vector<Plane> m_collisionPlanes;
        std::vector<LineSegment> m_collisionSegments;
        int m_maxParticles;
        int m_numActualParticles;

        int m_particleSize;

        int m_vertexCount;
        int m_indexCount;

        WaveParticleVertex* m_vertices;
        unsigned int* m_indices;
        ID3D11Buffer* m_vertexBuffer;
        ID3D11Buffer* m_indexBuffer;

        ID3D11VertexShader* m_pVertexShader;
        ID3D11PixelShader* m_pPixelShader;

        ID3D11InputLayout* m_inputLayout;

        ID3D11Buffer* m_cbPerObjectBuffer;
        cbPerObject m_cbPerObject;

        ID3D11RasterizerState* m_rasterState;
    };
}
