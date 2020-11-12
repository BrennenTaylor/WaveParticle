// NOTE: For some fucking reason this must be included before any d3d11 stuff gets included. Why you ask? No clue.
#include <d3dcompiler.h>

#include  "ParticleSystem.h"

#include "../NewRenderer/Renderer.h"

#include "Sample.h"

#include <time.h>
#include <math.h>

#include "../Math/CollisionFunctions.h"

#include "../Core/Timer.h"

#include "DiffractionCollisionPacket.h"

#include <DirectXMath.h>

#include "../Util/Logger.h"

#include <time.h>

namespace Farlor
{
    extern Renderer g_RenderingSystem;
    extern Farlor::Timer g_TimerGame;

    static float g_amplitudeDropRate = 0.001f;

    const float PI = 3.141592f;
    const float TWO_PI = 2.0f * PI;

    ParticleSystem::WaveParticle::WaveParticle(const Vector3& birthPos, const Vector3& direction, float birthTime, float size, float speed, bool isActive)
        : m_birthPosition(birthPos)
        , m_currentPosition(m_birthPosition)
        , m_direction(direction)
        , m_amplitude{ 1.0f }
        , m_dispersionAngle{ 0.0f }
        , m_speed{ speed }
        , m_birthTime(birthTime)
        , m_particleSize(size)
        , m_active(isActive)
        , m_timeMoved{ 0.0f }
    {
    }

    bool ParticleSystem::WaveParticle::ShouldKill()
    {
        const float amplitudeCutoffThreshold = 0.1f;
        return (m_amplitude <= amplitudeCutoffThreshold);
    }

    ParticleSystem::ParticleSystem(int numParticles)
        : m_waveParticles()
        , m_collisionPlanes()
        , m_collisionSegments()
        , m_maxParticles(numParticles)
        , m_numActualParticles(0)
        , m_particleSize(0.0)
        , m_vertexCount(0)
        , m_indexCount(0)
    {
        m_gen = std::mt19937(time(0));

        m_vertices = nullptr;
        m_indices = nullptr;
        m_maxParticles = numParticles;

        // Initialize all the possible particles
        for (int i = 0; i < numParticles; i++)
        {
            m_waveParticles.push_back(
                WaveParticle(
                    Vector3(0.0f, 0.0f, 0.0f), // Birth Pos
                    Vector3(1.0f, 0.0f, 0.0f), // Dir of Travel
                    0.0f, // Birth time
                    1.0f, // Particle Size
                    false // isActive
                )
            );
        }

        // for (int i = 0; i < 1000; i++)
        // {
        //     static float zCoord = 10.0f;
        //     static float speed = 10.0f;
        //     Vector3 startPosition = Vector3((float)genIntInRange(0, 50)-25.0f, (float)genIntInRange(0, 50)-25.0f, zCoord);
        //     Vector3 planeOrigin = Vector3(0.0f, 0.0f, zCoord);
        //     Vector3 direction = startPosition - planeOrigin;
        //     direction = direction.Normalized();
        //     direction *= speed;
        //     AddParticle(WaveParticle(startPosition, direction, 0.0f));
        // }

        // m_waveParticles[0].m_active = true;
        // m_numActualParticles++;
    }

    void ParticleSystem::StartRandomWave()
    {

        //const int numPoints = 1;
        //const float initialParticleRadius = 1;

        //Vector3 direction(0.0f, 1.0f, 0.0f);
        //direction *= 1.0;
        //WaveParticle newParticle(Vector3(0.0, 0.0, 0.0), direction, (float)g_TimerGame.TotalTime(), initialParticleRadius);
        //// We need to figure out how to measure "angle" for  parallel waves. Important for diffraction.
        //newParticle.m_amplitude = 0.15;
        //newParticle.m_particleSize = 1.0;
        //AddParticle(newParticle);

        const uint32_t numWaveParticles = 10000;

        std::uniform_real_distribution<float> uniformZeroToOne(0.0f, 1.0f);

        for (int i = 0; i < numWaveParticles; i++)
        {
            Farlor::Vector3 position(uniformZeroToOne(m_gen) * 10.f - 5.0f, uniformZeroToOne(m_gen) * 10.f - 5.0f, 0.0);
            Farlor::Vector3 direction( uniformZeroToOne(m_gen) * 10.f - 5.0f, uniformZeroToOne(m_gen) * 10.f - 5.0f, 0.0);
            float height = uniformZeroToOne(m_gen) * 0.1 + 0.2;
            float radius = uniformZeroToOne(m_gen) * 0.05 + 0.1;
            float speed = uniformZeroToOne(m_gen) * 0.5;
            
            WaveParticle newParticle(position, direction.Normalized(), (float)g_TimerGame.TotalTime(), radius, speed);
            newParticle.m_amplitude = height;
            AddParticle(newParticle);
        }
    }

    void ParticleSystem::AddParticle(WaveParticle particle)
    {
        for (int i = 0; i < m_waveParticles.size(); i++)
        {
            if (m_waveParticles[i].m_active == false)
            {
                m_waveParticles[i] = particle;
                m_waveParticles[i].m_active = true;
                m_numActualParticles++;
                return;
            }
        }
        std::cout << "Failed to add particle, all particle slots active" << std::endl;
    }

    void ParticleSystem::AddPlane(Plane plane)
    {
        m_collisionPlanes.push_back(plane);
    }

    void ParticleSystem::Update(float timestep)
    {
        for (int i = 0; i < m_numActualParticles; i++)
        {
            bool collision = false;
            float currentTime = g_TimerGame.TotalTime();
            float timeMoved = currentTime - m_waveParticles[i].m_birthTime;            
            m_waveParticles[i].m_currentPosition = m_waveParticles[i].m_birthPosition + m_waveParticles[i].m_direction * (timeMoved * m_waveParticles[i].m_speed);
            m_waveParticles[i].m_timeMoved = timeMoved;
        }
        KillParticles();
    }

    void ParticleSystem::KillParticles()
    {
        int numBefore = m_numActualParticles;
        int i = 0;
        while (i < m_maxParticles)
        {
            if (m_waveParticles[i].m_active && m_waveParticles[i].m_amplitude <= 0.05f)
            {
                // std::cout << "Killing particle" << std::endl;
                m_waveParticles[i].m_active = false;

                // std::cout << "Killing Particle" << std::endl;
                // Shift all particles over
                for (int j = i; j < (m_maxParticles-1); j++)
                {
                    m_waveParticles[j].m_birthPosition = m_waveParticles[j+1].m_birthPosition;
                    m_waveParticles[j].m_currentPosition = m_waveParticles[j + 1].m_currentPosition;
                    m_waveParticles[j].m_direction = m_waveParticles[j + 1].m_direction;
                    m_waveParticles[j].m_amplitude = m_waveParticles[j + 1].m_amplitude;
                    m_waveParticles[j].m_dispersionAngle = m_waveParticles[j + 1].m_dispersionAngle;
                    m_waveParticles[j].m_birthTime = m_waveParticles[j + 1].m_birthTime;
                    m_waveParticles[j].m_particleSize = m_waveParticles[j + 1].m_particleSize;
                    m_waveParticles[j].m_active = m_waveParticles[j + 1].m_active;

                    m_waveParticles[j+1].m_active = false;
                    // std::cout << "Killing particle: " << std::endl;
                }
                m_numActualParticles--;
                // std::cout << "Removed Particle" << std::endl;
            }
            else
            {
                i++;
            }
        }
        // std::cout << "Killed off: " << (numBefore - m_numActualParticles) << std::endl;
    }

    // Take in renderer device and context and aquire and own buffers for particles
    // TODO: Seperate this so the particle system doesnt actually own the buffers.
    void ParticleSystem::InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result = ERROR_SUCCESS;

        // TODO: Why do we need 6 times the number of vertices
        // Number of vertices is 6 as each particle becomes a quad of 2 triangles
        m_vertexCount = m_maxParticles;
        m_indexCount = m_maxParticles;
        m_vertices = new WaveParticleVertex[m_vertexCount];
        assert(m_vertices);
        memset(m_vertices, 0, sizeof(WaveParticleVertex) * m_vertexCount);

        m_indices = new unsigned int[m_indexCount];
        assert(m_indices);
        for(int idx = 0; idx < m_indexCount; idx++)
        {
            m_indices[idx] = idx;
        }

        D3D11_BUFFER_DESC vertexBufferDesc{0};
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth = sizeof(WaveParticleVertex) * m_vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA vertexData = {0};
        vertexData.pSysMem = m_vertices;
        vertexData.SysMemPitch = 0;
        vertexData.SysMemSlicePitch = 0;

        result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create vertex buffer: particle system")
            return;
        }

        D3D11_BUFFER_DESC indexBufferDesc{0};
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_indexCount;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexData = {0};
        indexData.pSysMem = m_indices;
        indexData.SysMemPitch = 0;
        indexData.SysMemSlicePitch = 0;

        result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create index buffer: particle system")
            return;
        }

        delete[] m_indices;
        m_indices = nullptr;

        // Lets load shaders
        ID3DBlob* pErrorMessage = nullptr;
        ID3DBlob* pVertexShaderBuffer = nullptr;
        ID3DBlob* pPixelShaderBuffer = nullptr;
        D3D11_BUFFER_DESC matrixBufferDesc = {0};

        std::wstring vertexShaderName = L"resources/shaders/PositionUV.hlsl";
        std::wstring pixelShaderName = L"resources/shaders/PositionUV.hlsl";

        result = D3DCompileFromFile(vertexShaderName.c_str(), 0, 0, "VSMain", "vs_5_0", 0, 0, &pVertexShaderBuffer, &pErrorMessage);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to compile vertex shader: resources/shaders/PositionUV.hlsl")
            return;
        }

        result = D3DCompileFromFile(pixelShaderName.c_str(), 0, 0, "PSMain", "ps_5_0", 0, 0, &pPixelShaderBuffer, &pErrorMessage);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to compile pixel shader: resources/shaders/PositionUV.hlsl")
            return;
        }

        result = pDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), 0, &m_pVertexShader);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create vertex shader: resources/shaders/PositionUV.hlsl")
            return;
        }

        result = pDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), 0, &m_pPixelShader);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create pixel shader: resources/shaders/PositionUV.hlsl")
            return;
        }

        // Create input layout
        result = pDevice->CreateInputLayout(WaveParticleVertex::s_layout, WaveParticleVertex::s_numElements,
            pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(),
            &m_inputLayout);
        if (FAILED(result))
        {
            FARLOR_LOG_ERROR("Failed to create input layout: WaveParticleVertex::s_layout")
        }

        // Set raster desc
        D3D11_RASTERIZER_DESC rasterDesc;
        ZeroMemory(&rasterDesc, sizeof(rasterDesc));
        rasterDesc.AntialiasedLineEnable = false;
    	rasterDesc.CullMode = D3D11_CULL_NONE;
    	rasterDesc.DepthBias = 0;
    	rasterDesc.DepthBiasClamp = 0.0f;
    	rasterDesc.DepthClipEnable = true;
    	rasterDesc.FillMode = D3D11_FILL_SOLID;
    	rasterDesc.FrontCounterClockwise = false;
    	rasterDesc.MultisampleEnable = false;
    	rasterDesc.ScissorEnable = false;
    	rasterDesc.SlopeScaledDepthBias = 0.0f;
        result = pDevice->CreateRasterizerState(&rasterDesc, &m_rasterState);
        if (FAILED(result))
        {
            std::cout << "Failed to create raster state" << std::endl;
        }
    }

    void ParticleSystem::UpdateBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result = ERROR_SUCCESS;
        D3D11_MAPPED_SUBRESOURCE mappedVertices = {0};
        WaveParticleVertex* pGpuVertices = nullptr;

        // Reset array of vertices
        memset(m_vertices, 0, sizeof(WaveParticleVertex) * m_vertexCount);

        // Now build vertex array from particle list array
        // Each particle becomes a quad made out of two triangles
        // TODO: This likely can become gpu side function mapping or something, no vertices but the particle center needed or something
        int index = 0;
        for (int i = 0; i < m_numActualParticles; i++)
        {
            // Bottom Left
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = m_waveParticles[i].m_direction.x;
            m_vertices[index].m_uv.y = m_waveParticles[i].m_direction.z;
            m_vertices[index].m_amplitude = m_waveParticles[i].m_amplitude;
            index++;
        }

        // Lock vertex buffer and copy data to it
        result = pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices);
        if (FAILED(result))
        {
            std::cout << "Failed to map resource: m_vertexBuffer" << std::endl;
            return;
        }

        // Get pointer to data in vertex buffer
        pGpuVertices = (WaveParticleVertex*)mappedVertices.pData;

        memcpy(pGpuVertices, (void*)m_vertices, sizeof(WaveParticleVertex)*m_vertexCount);
        // Unlock buffer
        pDeviceContext->Unmap(m_pVertexBuffer, 0);
    }

    void ParticleSystem::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        UpdateBuffers(pDevice, pDeviceContext);

        unsigned int stride = sizeof(WaveParticleVertex);
        unsigned int offset = 0;

        pDeviceContext->VSSetShader(m_pVertexShader, 0, 0);
        pDeviceContext->PSSetShader(m_pPixelShader, 0, 0);
        pDeviceContext->IASetInputLayout(m_inputLayout);

        pDeviceContext->RSSetState(m_rasterState);

        pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
        pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

        pDeviceContext->DrawIndexed(m_numActualParticles, 0, 0);
    }
}
