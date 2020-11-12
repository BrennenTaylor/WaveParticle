#pragma once

#include "../NewRenderer/Vertex.h"

#include "../Physics/Plane.h"
#include "../Physics/LineSegment.h"

#include "Math/Math.h"

#include <DirectXMath.h>

#include <vector>

namespace Farlor
{
    class ParticleSystem
    {
    public:

        struct WaveParticle
        {
            Vector3 m_birthPosition; // Tracks where particle was first created
            Vector3 m_currentPosition; // Tracks where the particle currently is in physical sim

            Vector3 m_direction; // Tracks direction of particle travel

            float m_amplitude; // How much energy is in the particle? Usually the center of the particle is full amplitude
            float m_dispersionAngle; // What angle of space is the particle covering, i.e. angle of circle based ripple covered by this particle

            float m_birthTime; // Time particle was created at
            float m_particleSize; // How big is the thing?
            bool m_active; // Is the particle active?

            float m_timeMoved; // How long has the thing moved

            WaveParticle(const Vector3& birthPos, const Vector3& direction, float birthTime, float size = 20.0f, bool isActive = false);

            bool ShouldKill();
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

        WaveParticleVertex* m_vertices = nullptr;
        unsigned int* m_indices = nullptr;
        ID3D11Buffer* m_pVertexBuffer = nullptr;
        ID3D11Buffer* m_pIndexBuffer = nullptr;

        ID3D11VertexShader* m_pVertexShader = nullptr;
        ID3D11PixelShader* m_pPixelShader = nullptr;

        ID3D11InputLayout* m_inputLayout = nullptr;

        ID3D11RasterizerState* m_rasterState = nullptr;
    };
}
