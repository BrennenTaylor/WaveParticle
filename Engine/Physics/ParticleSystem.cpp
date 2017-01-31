#include  "ParticleSystem.h"

#include <DirectXMath.h>

#include "../NewRenderer/Renderer.h"

#include "Sample.h"

#include <time.h>
#include <math.h>

#include "../Math/CollisionFunctions.h"

#include "../Core/Timer.h"

using namespace DirectX;

namespace Farlor
{
    extern Renderer g_RenderingSystem;
    extern Farlor::Timer g_TimerGame;

    static float zCoord = 10.0f;
    static float speed = 20.0f;

    ParticleSystem::ParticleSystem(int numParticles)
    {
        m_vertices = nullptr;
        m_indices = nullptr;
        m_maxParticles = numParticles;

        // m_timer.Initialize();

        srand((unsigned int)time(0));

        // auto genIntInRange = [](int min, int max)
        // {
        //     return rand() % max + min;
        // };

        for (int i = 0; i < numParticles; i++)
        // for (int i = 0; i < numParticles; i++)
        {
            m_waveParticles.push_back(WaveParticle(
                Vector3(0.0f, 0.0f, 0.0f),
                Vector3(0.0f, 0.0f, 0.5f), 0.0f));
            m_waveParticles[i].m_active = false;
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

        const int numPoints = 20;

        for (int i = 0; i < numPoints; i++)
        {
            float x = -120.0f;
            float y = 0;

            Vector3 direction(0.0f, 1.0f, 0.0f);
            direction *= speed;
            WaveParticle newParticle(Vector3(x + (10.0f * i), y, zCoord), direction, (float)g_TimerGame.TotalTime());
            // We need to figure out how to measure "angle" for  parallel waves. Important for diffraction.
            newParticle.m_dispersionAngle = 1.0f;
            AddParticle(newParticle);
        }

        // auto angle = 0.0f;
        // auto deltaTheta = TWO_PI / numPoints;
        // // auto deltaTheta = TWO_PI / 8;
        //
        // auto genIntInRange = [](int min, int max)
        // {
        //     return rand() % max + min;
        // };
        //
        // Vector3 startPosition = Vector3(0.0f, 0.0f, zCoord);
        //
        // for (int i = 0; i < 1; i++)
        // // for (int i = 0; i < numPoints; i++)
        // {
        //     Vector3 planeOrigin = Vector3(0.0f, 0.0f, zCoord);
        //     Vector3 direction = Vector3(cos(angle), sin(angle), 0.0f);
        //     direction = direction.Normalized();
        //     direction *= speed;
        //     cout << "X: " << direction.x << endl;
        //     cout << "Y: " << direction.y << endl;
        //     WaveParticle newParticle(startPosition, direction, (float)g_TimerGame.TotalTime());
        //     newParticle.m_dispersionAngle = deltaTheta;
        //     AddParticle(newParticle);
        //
        //     angle += deltaTheta;
        // }
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
        cout << "Failed to add particle, all particle slots active" << endl;
    }

    void ParticleSystem::AddPlane(Plane plane)
    {
        m_collisionPlanes.push_back(plane);
    }

    void ParticleSystem::Update(float timestep)
    {
        int sampleDensity = 1;
        vector<WaveParticle> m_newParticles;

        for (int i = 0; i < m_numActualParticles; i++)
        {
            bool collision = false;
            float currentTime = g_TimerGame.TotalTime();
            float timeMoved = currentTime - m_waveParticles[i].m_birthTime;


            Vector3 newPoint = m_waveParticles[i].m_birthPosition + m_waveParticles[i].m_direction * timeMoved;
            Vector3 oldPoint = m_waveParticles[i].m_currentPosition;
            Vector3 normDirection = m_waveParticles[i].m_direction.Normalized();

            // Do collisions
            for (auto& lineSegment : m_collisionSegments)
            {
                Vector3 first = lineSegment.m_first;
                Vector3 second = lineSegment.m_second;

                Vector3 lineDir = second - first;
                lineDir = lineDir.Normalized();
                Vector3 lineDirNorm = Vector3(-1.0f * lineDir.y, lineDir.x, 0.0f);

                float distanceOld = (oldPoint - first).Dot(lineDirNorm);
                float distanceNew = (newPoint - first).Dot(lineDirNorm);


                // cout << "distancenew * old: " << (distanceNew * distanceOld) << endl;
                if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
                {
                    float s1_x, s1_y, s2_x, s2_y;
                    s1_x = newPoint.x - oldPoint.x;
                    s1_y = newPoint.y - oldPoint.y;

                    s2_x = second.x - first.x;
                    s2_y = second.y - first.y;

                    float s, t;
                    s = (-s1_y * (oldPoint.x - first.x) + s1_x * (oldPoint.y - first.y)) / (-s2_x * s1_y + s1_x * s2_y);
                    t = ( s2_x * (oldPoint.y - first.y) - s2_y * (oldPoint.x - first.x)) / (-s2_x * s1_y + s1_x * s2_y);

                    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
                    {
                        // Collision detected
                        // Vector3 collisionPoint(oldPoint.x + (t * s1_x), oldPoint.y + (t * s1_y), 0.0f);
                        // cout << "Collision Detected at: " << collisionPoint << endl;
                        //
                        // cout << "LineDirNorm: " << lineDirNorm << endl;
                        // lineDirNorm.Normalized();

                        m_waveParticles[i].m_direction = m_waveParticles[i].m_direction -
                                (2.0f * m_waveParticles[i].m_direction.Dot(lineDirNorm) * lineDirNorm);
                        m_waveParticles[i].m_birthPosition = m_waveParticles[i].m_birthPosition +
                            (2.0f * (first - m_waveParticles[i].m_birthPosition).Dot(lineDirNorm) * lineDirNorm);
                        collision = true;
                        break;

                        // // Collision Response
                        // m_waveParticles[i].m_direction = m_waveParticles[i].m_direction -
                        //     (2.0f * m_waveParticles[i].m_direction.Dot(lineDirNorm) * lineDirNorm);
                        // m_waveParticles[i].m_birthPosition = m_waveParticles[i].m_birthPosition +
                        //     (2.0f * (first - m_waveParticles[i].m_birthPosition).Dot(lineDirNorm) * lineDirNorm);
                        //     collision = true;
                        //     break;
                    }
                }
            }

            for (auto& plane : m_collisionPlanes)
            {
                float distanceOld = (m_waveParticles[i].m_currentPosition - plane.m_point).Dot(plane.m_normal);
                float distanceNew = (newPoint - plane.m_point).Dot(plane.m_normal);
                // cout << "Distance Old: " << distanceOld << endl;
                // cout << "Distance New: " << distanceNew << endl;
                // cout << "Old Point: " << m_waveParticles[i].m_currentPosition << endl;
                // cout << "New Point: " << newPoint << endl;

                // cout << "distancenew * old: " << (distanceNew * distanceOld) << endl;
                if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
                {
                    // Collision with plane happened
                    // cout << "Collision with plane happened" << endl;
                    // cout << "Old Direction: " << m_waveParticles[i].m_direction << endl;
                    m_waveParticles[i].m_direction = m_waveParticles[i].m_direction - (2.0f * m_waveParticles[i].m_direction.Dot(plane.m_normal) * plane.m_normal);
                    m_waveParticles[i].m_birthPosition = m_waveParticles[i].m_birthPosition + (2.0f * (plane.m_point - m_waveParticles[i].m_birthPosition).Dot(plane.m_normal) * plane.m_normal);

                    // cout << "New Direction: " << m_waveParticles[i].m_direction << endl;
                    collision = true;
                    break;
                }
            }

            // Perform Diffraction

            // Generate triangle representing area that wave particle covers
            // This is a triangle of the current position + dispersion angle (A, B, C)
            // in addition to a rectangle (B, C, D) and (B, D, E)
            float distanceTraveled = Vector3(m_waveParticles[i].m_direction * timeMoved).Magnitude();
            float dispersionAngle = m_waveParticles[i].m_dispersionAngle;
            float halfAngle = dispersionAngle / 2.0f;
            Vector3 direction = m_waveParticles[i].m_direction;

            float xLeft = direction.x * cos(halfAngle) - direction.y * sin(halfAngle);
            float yLeft = direction.y * cos(halfAngle) + direction.x * sin(halfAngle);

            float xRight = direction.x * cos(-1.0f * halfAngle) - direction.y * sin(-1.0f * halfAngle);
            float yRight = direction.y * cos(-1.0f * halfAngle) + direction.x * sin(-1.0f * halfAngle);
            
            float coveredDistance = 1.0f;

            Vector2 left(xLeft, yLeft);
            left = left.Normalized() * coveredDistance;
            Vector2 right(xRight, yRight);
            right = right.Normalized() * coveredDistance;

            // cout << "Left: " << left << endl;
            // cout << "Right: " << right << endl;

            // First points of rectangle
            Vector3 A3D = m_waveParticles[i].m_currentPosition;
            Vector2 A(A3D.x, A3D.y);
            Vector2 B = A + left;
            Vector2 C = B + right;

            // Second points of rectangle
            Vector3 newPos3D = m_waveParticles[i].m_birthPosition + m_waveParticles[i].m_direction * timeMoved;
            Vector2 newPos(newPos3D.x, newPos3D.y);
            Vector2 E = newPos + left;
            Vector2 D = newPos + right;

            // Check if end points of any obstacles intersect this area. If so, we have a place for diffraction
            vector<Vector2> m_collisions;

            for (auto& lineSegment : m_collisionSegments)
            {
                Vector2 firstEndPoint = Vector2(lineSegment.m_first.x, lineSegment.m_first.y);
                Vector2 secondEndPoint = Vector2(lineSegment.m_second.x, lineSegment.m_second.y);

                if (PointInTriangle(firstEndPoint, B, C, D))
                {
                    cout << "First" << endl;
                    cout << "Point: " << firstEndPoint << endl;
                    cout << "B: " << B << endl;
                    cout << "C: " << C << endl;
                    cout << "D: " << D << endl;
                    m_collisions.push_back(firstEndPoint);
                }
                
                if (PointInTriangle(secondEndPoint, B, D, E))
                {
                    cout << "Second" << endl;
                    cout << "Point: " << secondEndPoint << endl;
                    cout << "B: " << B << endl;
                    cout << "D: " << D << endl;
                    cout << "E: " << E << endl;
                    m_collisions.push_back(secondEndPoint);
                }
            }

            if (m_collisions.size() > 0)
            {
                cout << "We had a collision" << endl;
            }

            // Perform subdivide
            if (!collision)
            {
                // Do subdivide
                float angleOwned = m_waveParticles[i].m_dispersionAngle;
                float halfAngleOwned = angleOwned / 2.0f;
                Vector3 distanceMoved = (m_waveParticles[i].m_direction * ((float)g_TimerGame.TotalTime() - m_waveParticles[i].m_birthTime));
                // cout << "Distance Moved" << distanceMoved << endl;
                float totalDistanceMoved = distanceMoved.Magnitude();
                float distanceBetweenParticles = 2.0f * totalDistanceMoved * sin(halfAngleOwned);

                int numSubdividing = 0;

                // cout << "Distance Moved: " << distanceMoved.Magnitude() << endl;
                // Perform subdivide
                if (distanceBetweenParticles > 10.0f)
                {
                    // cout << "Particle Subdividing: " << i << endl;
                    // cout << "AngleOwned: " << angleOwned << endl;
                    // cout << "i" << i << endl;
                    // cout << "Performing Subdivide" << endl;
                    // cout << "Subdivide" << endl;
                    numSubdividing++;

                    // m_waveParticles[i].m_active = true;

                    Vector3 pos = m_waveParticles[i].m_birthPosition;
                    Vector3 direction = m_waveParticles[i].m_direction;
                    float origionalAngle = m_waveParticles[i].m_dispersionAngle;
                    float angle = origionalAngle/2.0f * (2.0f/3.0f);
                    float negAngle = -1.0f * angle;

                    // cout << "Angle: " << angle << endl;

                    float xLeft = direction.x * cos(angle) - direction.y * sin(angle);
                    float yLeft = direction.y * cos(angle) + direction.x * sin(angle);

                    float xRight = direction.x * cos(negAngle) - direction.y * sin(negAngle);
                    float yRight = direction.y * cos(negAngle) + direction.x * sin(negAngle);

                    Vector2 left(xLeft, yLeft);
                    left = left.Normalized() * speed;

                    Vector2 right(xRight, yRight);
                    right = right.Normalized() * speed;

                    // cout << "Origional Direction: " << direction << endl;
                    // cout << "Left Direction: <" << left.x << ", " << left.y << ">" << endl;
                    // cout << "Right Direction: <" << right.x << ", " << right.y << ">" << endl;

                    // cin.get();

                    // cout << "Origional Particle Current Position: " << m_waveParticles[i].m_currentPosition << endl;

                    WaveParticle particleLeft = WaveParticle(pos, Vector3(left.x, left.y, 0.0f), m_waveParticles[i].m_birthTime);
                    WaveParticle particleRight = WaveParticle(pos, Vector3(right.x, right.y, 0.0f), m_waveParticles[i].m_birthTime);
                    WaveParticle particleSame = WaveParticle(pos, Vector3(direction.x, direction.y, 0.0f), m_waveParticles[i].m_birthTime);

                    particleLeft.m_currentPosition = particleLeft.m_birthPosition + particleLeft.m_direction * timeMoved;
                    particleRight.m_currentPosition = particleRight.m_birthPosition + particleRight.m_direction * timeMoved;
                    particleSame.m_currentPosition = particleSame.m_birthPosition + particleSame.m_direction * timeMoved;

                    // cout << "Left Current Position: " <<  particleLeft.m_currentPosition << endl;
                    // cout << "Right Current Position: " << particleRight.m_currentPosition << endl;


                    particleLeft.m_dispersionAngle = origionalAngle / 3.0f;
                    particleRight.m_dispersionAngle = origionalAngle / 3.0f;
                    particleSame.m_dispersionAngle = origionalAngle / 3.0f;

                    // particleLeft.m_amplitude = m_waveParticles[i].m_amplitude / 3.0f;
                    // particleRight.m_amplitude = m_waveParticles[i].m_amplitude / 3.0f;
                    // particleSame.m_amplitude = m_waveParticles[i].m_amplitude / 3.0f;

                    m_waveParticles[i].m_amplitude = 0.0f;

                    m_newParticles.push_back(particleSame);
                    m_newParticles.push_back(particleLeft);
                    m_newParticles.push_back(particleRight);
                }
            }

            // timeMoved = (float)m_timer.GetCurrentTime() - m_waveParticles[i].m_birthTime;
            m_waveParticles[i].m_currentPosition = m_waveParticles[i].m_birthPosition + m_waveParticles[i].m_direction * timeMoved;

            // cout << "Num subdivided: " << numSubdividing << endl;
        }

        for (int i = 0; i < m_newParticles.size(); i++)
        {
            // cout << "Adding particle" << endl;
            AddParticle(m_newParticles[i]);
        }

        // cout << "New Particle Number: " << m_numActualParticles << endl;

        KillParticles();

        // cout << "Number of particles: " << m_numActualParticles << endl;
    }

    void ParticleSystem::KillParticles()
    {
        int numBefore = m_numActualParticles;
        int i = 0;
        while (i < m_maxParticles)
        {
            if (m_waveParticles[i].m_active && m_waveParticles[i].m_amplitude <= 0.05f)
            {
                cout << "Killing particle" << endl;
                m_waveParticles[i].m_active = false;

                // cout << "Killing Particle" << endl;
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
                    // m_waveParticles[j].m_sampleAngle = m_waveParticles[j + 1].m_sampleAngle;
                    // m_waveParticles[j].m_sampleDistance = m_waveParticles[j + 1].m_sampleDistance;
                    m_waveParticles[j].m_active = m_waveParticles[j + 1].m_active;

                    m_waveParticles[j+1].m_active = false;
                }
                m_numActualParticles--;
                // cout << "Removed Particle" << endl;
            }
            else
            {
                i++;
            }
        }
        // cout << "Killed off: " << (numBefore - m_numActualParticles) << endl;
    }

    void ParticleSystem::InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result;
        // Number of vertices is 6 as each particle becomes a quad of 2 triangles
        m_vertexCount = m_maxParticles * 6;
        m_indexCount = m_vertexCount;

        m_vertices = new VertexPositionUV[m_vertexCount];
        assert(m_vertices);

        m_indices = new unsigned int[m_indexCount];
        assert(m_vertices);

        memset(m_vertices, 0, sizeof(VertexPositionUV)*m_vertexCount);

        for(int i = 0; i < m_indexCount; i++)
        {
            m_indices[i] = i;
        }

        D3D11_BUFFER_DESC vertexBufferDesc{0};
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth = sizeof(VertexPositionUV) * m_vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA vertexData = {0};
        vertexData.pSysMem = m_vertices;
        vertexData.SysMemPitch = 0;
        vertexData.SysMemSlicePitch = 0;

        result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
        if (FAILED(result))
        {
            cout << "Failed to create vertex buffer" << endl;
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

        result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
        if (FAILED(result))
        {
            cout << "Failed to create index buffer" << endl;
            return;
        }

        delete[] m_indices;
        m_indices = nullptr;

        // Lets load ball shaders
        ID3DBlob* errorMessage = nullptr;
        ID3DBlob* vertexShaderBuffer = nullptr;
        ID3DBlob* pixelShaderBuffer = nullptr;
        D3D11_BUFFER_DESC matrixBufferDesc = {0};

        wstring vertexShaderName = L"resources/shaders/PositionUV.hlsl";
        wstring pixelShaderName = L"resources/shaders/PositionUV.hlsl";

        result = D3DCompileFromFile(vertexShaderName.c_str(), 0, 0, "VSMain", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
        if (FAILED(result))
        {
            cout << "Failed to compile vertex shader" << endl;
            return;
        }

        result = D3DCompileFromFile(pixelShaderName.c_str(), 0, 0, "PSMain", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
        if (FAILED(result))
        {
            cout << "Failed to compile ball pixel shader" << endl;
            return;
        }

        result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 0, &m_pVertexShader);
        if (FAILED(result))
        {
            cout << "Failed to createe ball vertex shader" << endl;
            return;
        }

        result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), 0, &m_pPixelShader);
        if (FAILED(result))
        {
            cout << "Failed to create ball pixel shader" << endl;
            return;
        }

        // Create input layout
        result = pDevice->CreateInputLayout(VertexPositionUV::s_layout, VertexPositionUV::s_numElements,
            vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
            &m_inputLayout);

        if (FAILED(result))
        {
            cout << "Failed to create input layout" << endl;
        }

        // Create constatnt buffer
        D3D11_BUFFER_DESC cbd = {0};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(ParticleSystem::cbPerObject);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;

        result = pDevice->CreateBuffer(&cbd, 0, &m_cbPerObjectBuffer);
        if (FAILED(result))
        {
            cout << "Failed to create constant buffer layout" << endl;
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
            cout << "Failed to create raster state" << endl;
        }
    }

    void ParticleSystem::UpdateBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT result;
        D3D11_MAPPED_SUBRESOURCE mappedVertices = {0};
        VertexPositionUV* verticesPtr;

        // Reset array of vertices
        memset(m_vertices, 0, sizeof(VertexPositionUV) * m_vertexCount);

        // Now build vertex array from particle list array
        // Each particle becomes a quad made out of two triangles
        int index = 0;
        for (int i = 0; i < m_numActualParticles; i++)
        {
            float particleSize = m_waveParticles[i].m_particleSize;

            // Bottom Left
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x - particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y - particleSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 0.0f;
            m_vertices[index].m_uv.y = 1.0f;
            index++;
            // Top Left
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x - particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y + particleSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 0.0f;
            m_vertices[index].m_uv.y = 0.0f;
            index++;
            // Bottom Right
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x + particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y - particleSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 1.0f;
            m_vertices[index].m_uv.y = 1.0f;
            index++;
            // Bottom Right
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x + particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y - particleSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 1.0f;
            m_vertices[index].m_uv.y = 1.0f;
            index++;
            // Top Left
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x - particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y + particleSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 0.0f;
            m_vertices[index].m_uv.y = 0.0f;
            index++;
            // Top Right
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x + particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y + particleSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 1.0f;
            m_vertices[index].m_uv.y = 0.0f;
            index++;
        }

        // Lock vertex buffer
        result = pDeviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices);
        if (FAILED(result))
        {
            cout << "Failed to map resource" << endl;
            return;
        }

        // Get pointer to data in vertex buffer
        verticesPtr = (VertexPositionUV*)mappedVertices.pData;

        memcpy(verticesPtr, (void*)m_vertices, sizeof(VertexPositionUV)*m_vertexCount);
        // Unlock buffer
        pDeviceContext->Unmap(m_vertexBuffer, 0);
    }

    void ParticleSystem::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pParticleTextureSRView, ID3D11SamplerState* pWPSampleState)
    {
        UpdateBuffers(pDevice, pDeviceContext);

        unsigned int stride = sizeof(VertexPositionUV);
        unsigned int offset = 0;

        pDeviceContext->VSSetShader(m_pVertexShader, 0, 0);
        pDeviceContext->PSSetShader(m_pPixelShader, 0, 0);
        pDeviceContext->IASetInputLayout(m_inputLayout);

        XMMATRIX world = XMMatrixIdentity();

        XMMATRIX wvp = world * g_RenderingSystem.m_camView * g_RenderingSystem.m_camProjection;

        m_cbPerObject.WVP = XMMatrixTranspose(wvp);
        pDeviceContext->UpdateSubresource(m_cbPerObjectBuffer, 0, 0, &m_cbPerObject, 0, 0);
        pDeviceContext->VSSetConstantBuffers(0, 1, &m_cbPerObjectBuffer);

        pDeviceContext->RSSetState(m_rasterState);

        pDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
        pDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        pDeviceContext->PSSetShaderResources(0, 1, &pParticleTextureSRView);
        pDeviceContext->PSSetSamplers(0, 1, &pWPSampleState);

        pDeviceContext->DrawIndexed(m_numActualParticles*6, 0, 0);
    }
}
