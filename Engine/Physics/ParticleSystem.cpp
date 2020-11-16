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

        // Circle start
        {
            const int numPoints = 3;
            double angle = 0.0f;
            double deltaTheta = TWO_PI / numPoints;

            float amplitude = 10.0f;
            float radius = 0.5;
            float speed = 0.2f;

            Vector3 startPosition = Vector3(0.0f, 0.0f, 0.0f);
            for (int i = 0; i < numPoints; i++)
            {
                Vector3 planeOrigin = Vector3(0.0f, 0.0f, 0.0f);
                Vector3 direction = Vector3(cos(angle), sin(angle), 0.0f);
                planeOrigin += direction * (radius / 2.0f);
                direction = direction.Normalized();
                WaveParticle newParticle(planeOrigin, direction, (float)g_TimerGame.TotalTime(), radius, speed);
                newParticle.m_dispersionAngle = deltaTheta;
                newParticle.m_amplitude = amplitude;
                AddParticle(newParticle);

                //std::cout << "Particle direction: " << direction.Normalized() << std::endl;
                //std::cout << "Dispersion angle: " << deltaTheta << std::endl;

                angle += deltaTheta;
            }
        }

        // Straight line wave
        /*{
            {
                Farlor::Vector3 position(0.0f, 0.0f, 0.0);
                Farlor::Vector3 direction(1.0f, 0.0f, 0.0);
                float amplitude = 0.1f;
                float radius = 1.0;
                float speed = 0.2f;

                WaveParticle newParticle(position, direction.Normalized(), (float)g_TimerGame.TotalTime(), radius, speed);
                newParticle.m_amplitude = amplitude;
                AddParticle(newParticle);
            }

            {
                Farlor::Vector3 position(0.0f, (-1.0f / 10.0f) * 1.0f, 0.0);
                Farlor::Vector3 direction(1.0f, 0.0f, 0.0);
                float amplitude = 0.1f;
                float radius = 1.0;
                float speed = 0.2f;

                WaveParticle newParticle(position, direction.Normalized(), (float)g_TimerGame.TotalTime(), radius, speed);
                newParticle.m_amplitude = amplitude;
                AddParticle(newParticle);
            }

            {
                Farlor::Vector3 position(0.0f, (1.0f / 10.0f) * 1.0f, 0.0);
                Farlor::Vector3 direction(1.0f, 0.0f, 0.0);
                float amplitude = 0.1f;
                float radius = 1.0;
                float speed = 0.2f;

                WaveParticle newParticle(position, direction.Normalized(), (float)g_TimerGame.TotalTime(), radius, speed);
                newParticle.m_amplitude = amplitude;
                AddParticle(newParticle);
            }

            {
                Farlor::Vector3 position(0.0f, (-1.0f / 10.0f) * 2.0f, 0.0);
                Farlor::Vector3 direction(1.0f, 0.0f, 0.0);
                float amplitude = 0.1f;
                float radius = 1.0;
                float speed = 0.2f;

                WaveParticle newParticle(position, direction.Normalized(), (float)g_TimerGame.TotalTime(), radius, speed);
                newParticle.m_amplitude = amplitude;
                AddParticle(newParticle);
            }

            {
                Farlor::Vector3 position(0.0f, (1.0f / 10.0f) * 2.0f, 0.0);
                Farlor::Vector3 direction(1.0f, 0.0f, 0.0);
                float amplitude = 0.1f;
                float radius = 1.0;
                float speed = 0.2f;

                WaveParticle newParticle(position, direction.Normalized(), (float)g_TimerGame.TotalTime(), radius, speed);
                newParticle.m_amplitude = amplitude;
                AddParticle(newParticle);
            }
        }*/
    

        // Random
        //const uint32_t numWaveParticles = 10000;

        //std::uniform_real_distribution<float> uniformZeroToOne(0.0f, 1.0f);

        //for (int i = 0; i < numWaveParticles; i++)
        //{
        //    Farlor::Vector3 position(uniformZeroToOne(m_gen) * 10.f - 5.0f, uniformZeroToOne(m_gen) * 10.f - 5.0f, 0.0);
        //    Farlor::Vector3 direction( uniformZeroToOne(m_gen) * 10.f - 5.0f, uniformZeroToOne(m_gen) * 10.f - 5.0f, 0.0);
        //    float height = 0.1;
        //    float radius = 1.0;
        //    float speed = 0.2;
        //    
        //    WaveParticle newParticle(position, direction.Normalized(), (float)g_TimerGame.TotalTime(), radius, speed);
        //    newParticle.m_amplitude = height;
        //    AddParticle(newParticle);
        //}
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
        int sampleDensity = 1;
        std::vector<WaveParticle> m_newParticles;
        for (int i = 0; i < m_numActualParticles; i++)
        {

            float currentTime = g_TimerGame.TotalTime();
            float timeMoved = currentTime - m_waveParticles[i].m_birthTime;

            const Vector3 newPoint = m_waveParticles[i].m_birthPosition + m_waveParticles[i].m_direction * (timeMoved * m_waveParticles[i].m_speed);
            const Vector3 oldPoint = m_waveParticles[i].m_currentPosition;
            const Vector3 normDirection = m_waveParticles[i].m_direction.Normalized();

            // TODO: Figure this out...
        // NOTE: 'm not sure
        //    // Before Collisions, we do the radius subdivide step
        //    {
        //        // Generate triangle representing area that wave particle covers

        //        // We want left and right of the current m_position
        //        // Then we want left and right of the new position

        //        Vector2 oldPoint2d = Vector2(oldPoint.x, oldPoint.y);
        //        Vector2 newPoint2d = Vector2(newPoint.x, newPoint.y);

        //        Vector3 direction = m_waveParticles[i].m_direction;
        //        direction = direction.Normalized();
        //        Vector2 direction2d = Vector2(direction.x, direction.y);
        //        direction2d = direction2d.Normalized();

        //        Vector3 birthPoint = m_waveParticles[i].m_birthPosition;
        //        float dispersionAngle = m_waveParticles[i].m_dispersionAngle;
        //        float halfAngle = dispersionAngle / 2.0f;

        //        Vector3 perpendicularDir = Vector3(-direction.y, direction.x, direction.z);
        //        perpendicularDir = perpendicularDir.Normalized();

        //        // Distance traveled
        //        float oldDistTraveled = Vector3(oldPoint - birthPoint).Magnitude();
        //        float newDistTraveled = Vector3(newPoint - birthPoint).Magnitude();

        //        // Perpendicular Directions
        //        // Vector3 oldPerpDir = Vector3(perpendicularDir * oldDistTraveled) * tan(halfAngle);
        //        // Vector3 newPerpDir = Vector3(perpendicularDir * newDistTraveled) * tan(halfAngle);

        //        Vector3 oldPerpDir = perpendicularDir * (m_waveParticles[i].m_particleSize / 2.0f);
        //        Vector3 newPerpDir = perpendicularDir * (m_waveParticles[i].m_particleSize / 2.0f);

        //        // Calculate left and rights
        //        Vector3 oldLeft = oldPoint + oldPerpDir;
        //        Vector3 oldRight = oldPoint - oldPerpDir;

        //        Vector3 newLeft = newPoint + newPerpDir;
        //        Vector3 newRight = newPoint - newPerpDir;

        //        Vector2 B = Vector2(oldLeft.x, oldLeft.y);
        //        Vector2 C = Vector2(oldRight.x, oldRight.y);

        //        // Second points of rectangle
        //        Vector2 E = Vector2(newLeft.x, newLeft.y);
        //        Vector2 D = Vector2(newRight.x, newRight.y);

        //        // Check if end points of any obstacles intersect this area. If so, we have a place for diffraction
        //        std::vector<DiffractionCollisionPacket> m_diffractionCollisionPackets;

        //        // Need particle dir
        //        Vector2 particleDir = Vector2(direction.x, direction.y);
        //        particleDir = particleDir.Normalized();

        //        // Make sure that the triangle doesnt have an area of zero
        //        if (!((TriangleArea(B, C, D) < 0.00000001f) || (TriangleArea(B, D, E) < 0.000000001f)))
        //        {
        //            for (auto& lineSegment : m_collisionSegments)
        //            {
        //                Vector2 firstEndPoint = Vector2(lineSegment.m_first.x, lineSegment.m_first.y);
        //                Vector2 secondEndPoint = Vector2(lineSegment.m_second.x, lineSegment.m_second.y);

        //                bool endpointCollision = false;

        //                // First end point
        //                if (PointInTriangle(firstEndPoint, B, C, D))
        //                {
        //                    std::cout << "First" << std::endl;
        //                    std::cout << "Point: " << firstEndPoint << std::endl;
        //                    std::cout << "B: " << B << std::endl;
        //                    std::cout << "C: " << C << std::endl;
        //                    std::cout << "D: " << D << std::endl;
        //                    endpointCollision = true;
        //                }
        //                if (PointInTriangle(firstEndPoint, B, D, E))
        //                {
        //                    std::cout << "First" << std::endl;
        //                    std::cout << "Point: " << firstEndPoint << std::endl;
        //                    std::cout << "B: " << B << std::endl;
        //                    std::cout << "D: " << D << std::endl;
        //                    std::cout << "E: " << E << std::endl;
        //                    endpointCollision = true;
        //                }

        //                if (endpointCollision)
        //                {
        //                    std::cout << "OldPoint2d: " << oldPoint2d << std::endl;
        //                    std::cout << "NewPoint2d: " << newPoint2d << std::endl;

        //                    float distLeft = DistancePointToLine(B, E, firstEndPoint);
        //                    float distRight = DistancePointToLine(C, D, firstEndPoint);
        //                    float particleSize = m_waveParticles[i].m_particleSize;

        //                    std::cout << "Dist left: " << distLeft << std::endl;
        //                    std::cout << "Dist Right: " << distRight << std::endl;
        //                    std::cout << "Particle Radius: " << m_waveParticles[i].m_particleSize << std::endl;

        //                    float amountTowardB = distLeft / (particleSize * 2.0f);
        //                    float amountTowardC = distRight / (particleSize * 2.0f);

        //                    // Vector2 endpointDir = secondEndPoint - Vector2(birthPoint.x, birthPoint.y);
        //                    // endpointDir = endpointDir.Normalized();
        //                    // std::cout << "Endpoint dir: " <<  endpointDir << std::endl;
        //                    // std::cout << "Dir2D: " <<  direction2d << std::endl;

        //                    // float angleToParticleDir = acos(endpointDir.Dot(direction2d));
        //                    // std::cout << angleToParticleDir << std::endl;

        //                    // float angle1 = halfAngle - angleToParticleDir;
        //                    // float angle2 = halfAngle + angleToParticleDir;

        //                    // std::cout << dispersionAngle << std::endl; 
        //                    // std::cout << "Angle1: " << angle1 << std::endl;
        //                    // std::cout << "Angle2: " << angle2 << std::endl;
        //                    // std::cout << "Tot angle: " << angle1+angle2 << std::endl;

        //                    // float halfAngle1 = angle1 / 2.0f;
        //                    // float halfAngle2 = angle2 / 2.0f;

        //                    // std::cout << "Half Angle 1: " << halfAngle1 << std::endl;
        //                    // std::cout << "Half Angle 2: " << halfAngle2 << std::endl;

        //                    // Vector2 part1Dir(endpointDir.x*cos(halfAngle1) - endpointDir.y*sin(halfAngle1),
        //                    //     endpointDir.x*sin(halfAngle1) + endpointDir.y*cos(halfAngle1));
        //                    // part1Dir = part1Dir.Normalized();

        //                    // Vector2 part2Dir(endpointDir.x*cos(-1.0f*halfAngle2) - endpointDir.y*sin(-1.0f*halfAngle2),
        //                    //     endpointDir.x*sin(-1.0f*halfAngle2) + endpointDir.y*cos(-1.0f*halfAngle2));
        //                    // part2Dir = part2Dir.Normalized();

        //                    // std::cout << "Part1Dir: " << part1Dir.Normalized() << std::endl;
        //                    // std::cout << "Part2Dir: " << part2Dir.Normalized() << std::endl;


        //                    Vector2 part1Dir = Vector2(particleDir.x, particleDir.y);
        //                    Vector2 part2Dir = Vector2(particleDir.x, particleDir.y);

        //                    part1Dir = part1Dir.Normalized();
        //                    part2Dir = part2Dir.Normalized();

        //                    Vector2 bc = C - B;

        //                    float part1Size = amountTowardB * m_waveParticles[i].m_particleSize;
        //                    float part2Size = amountTowardC * m_waveParticles[i].m_particleSize;

        //                    WaveParticle part1 = m_waveParticles[i];
        //                    WaveParticle part2 = m_waveParticles[i];

        //                    part1.m_particleSize = part1Size * 2.0f;
        //                    part2.m_particleSize = part2Size * 2.0f;

        //                    part1.m_direction = Vector3(part1Dir.x, part1Dir.y, 0.0f);
        //                    part2.m_direction = Vector3(part2Dir.x, part2Dir.y, 0.0f);

        //                    part1.m_currentPosition = Vector3(B.x, B.y, 0.0f) + Vector3(bc.x, bc.y, 0.0f) * (amountTowardB / 2.0f);
        //                    part2.m_currentPosition = Vector3(C.x, C.y, 0.0f) - Vector3(bc.x, bc.y, 0.0f) * (amountTowardC / 2.0f);

        //                    part1.m_birthPosition = part1.m_currentPosition - part1.m_direction * m_waveParticles[i].m_timeMoved;
        //                    part2.m_birthPosition = part2.m_currentPosition - part2.m_direction * m_waveParticles[i].m_timeMoved;

        //                    std::cout << "Time Moved: " << m_waveParticles[i].m_timeMoved << std::endl;

        //                    part1.m_dispersionAngle = m_waveParticles[i].m_dispersionAngle * amountTowardB;
        //                    part2.m_dispersionAngle = m_waveParticles[i].m_dispersionAngle * amountTowardC;

        //                    // part1.m_particleSize = part1Size;
        //                    // part2.m_particleSize = part2Size;

        //                    m_waveParticles[i].m_amplitude = 0.0f;
        //                    KillParticles();

        //                    AddParticle(part1);
        //                    AddParticle(part2);

        //                    i = 0;
        //                    continue;
        //                }

        //                // Second end point
        //                if (PointInTriangle(secondEndPoint, B, C, D))
        //                {
        //                    std::cout << "Second" << std::endl;
        //                    std::cout << "Point: " << secondEndPoint << std::endl;
        //                    std::cout << "B: " << B << std::endl;
        //                    std::cout << "C: " << C << std::endl;
        //                    std::cout << "D: " << D << std::endl;
        //                    endpointCollision = true;
        //                }
        //                if (PointInTriangle(secondEndPoint, B, D, E))
        //                {
        //                    std::cout << "Second" << std::endl;
        //                    std::cout << "Point: " << secondEndPoint << std::endl;
        //                    std::cout << "B: " << B << std::endl;
        //                    std::cout << "D: " << D << std::endl;
        //                    std::cout << "E: " << E << std::endl;
        //                    endpointCollision = true;
        //                }

        //                if (endpointCollision)
        //                {
        //                    std::cout << "OldPoint2d: " << oldPoint2d << std::endl;
        //                    std::cout << "NewPoint2d: " << newPoint2d << std::endl;

        //                    float distLeft = DistancePointToLine(B, E, secondEndPoint);
        //                    float distRight = DistancePointToLine(C, D, secondEndPoint);
        //                    float particleSize = m_waveParticles[i].m_particleSize;

        //                    std::cout << "Dist left: " << distLeft << std::endl;
        //                    std::cout << "Dist Right: " << distRight << std::endl;
        //                    std::cout << "Particle Radius: " << m_waveParticles[i].m_particleSize << std::endl;

        //                    float amountTowardB = distLeft / (particleSize * 2.0f);
        //                    float amountTowardC = distRight / (particleSize * 2.0f);

        //                    // Vector2 endpointDir = secondEndPoint - Vector2(birthPoint.x, birthPoint.y);
        //                    // endpointDir = endpointDir.Normalized();
        //                    // std::cout << "Endpoint dir: " <<  endpointDir << std::endl;
        //                    // std::cout << "Dir2D: " <<  direction2d << std::endl;

        //                    // float angleToParticleDir = acos(endpointDir.Dot(direction2d));
        //                    // std::cout << angleToParticleDir << std::endl;

        //                    // float angle1 = halfAngle - angleToParticleDir;
        //                    // float angle2 = halfAngle + angleToParticleDir;

        //                    // std::cout << dispersionAngle << std::endl; 
        //                    // std::cout << "Angle1: " << angle1 << std::endl;
        //                    // std::cout << "Angle2: " << angle2 << std::endl;
        //                    // std::cout << "Tot angle: " << angle1+angle2 << std::endl;

        //                    // float halfAngle1 = angle1 / 2.0f;
        //                    // float halfAngle2 = angle2 / 2.0f;

        //                    // std::cout << "Half Angle 1: " << halfAngle1 << std::endl;
        //                    // std::cout << "Half Angle 2: " << halfAngle2 << std::endl;

        //                    // Vector2 part1Dir(endpointDir.x*cos(halfAngle1) - endpointDir.y*sin(halfAngle1),
        //                    //     endpointDir.x*sin(halfAngle1) + endpointDir.y*cos(halfAngle1));
        //                    // part1Dir = part1Dir.Normalized();

        //                    // Vector2 part2Dir(endpointDir.x*cos(-1.0f*halfAngle2) - endpointDir.y*sin(-1.0f*halfAngle2),
        //                    //     endpointDir.x*sin(-1.0f*halfAngle2) + endpointDir.y*cos(-1.0f*halfAngle2));
        //                    // part2Dir = part2Dir.Normalized();

        //                    // std::cout << "Part1Dir: " << part1Dir.Normalized() << std::endl;
        //                    // std::cout << "Part2Dir: " << part2Dir.Normalized() << std::endl;


        //                    Vector2 part1Dir = Vector2(particleDir.x, particleDir.y);
        //                    Vector2 part2Dir = Vector2(particleDir.x, particleDir.y);

        //                    part1Dir = part1Dir.Normalized();
        //                    part2Dir = part2Dir.Normalized();

        //                    Vector2 bc = C - B;

        //                    float part1Size = amountTowardB * m_waveParticles[i].m_particleSize;
        //                    float part2Size = amountTowardC * m_waveParticles[i].m_particleSize;

        //                    WaveParticle part1 = m_waveParticles[i];
        //                    WaveParticle part2 = m_waveParticles[i];

        //                    part1.m_particleSize = part1Size * 2.0f;
        //                    part2.m_particleSize = part2Size * 2.0f;

        //                    part1.m_direction = Vector3(part1Dir.x, part1Dir.y, 0.0f);
        //                    part2.m_direction = Vector3(part2Dir.x, part2Dir.y, 0.0f);

        //                    part1.m_currentPosition = Vector3(B.x, B.y, 0.0f) + Vector3(bc.x, bc.y, 0.0f) * (amountTowardB / 2.0f);
        //                    part2.m_currentPosition = Vector3(C.x, C.y, 0.0f) - Vector3(bc.x, bc.y, 0.0f) * (amountTowardC / 2.0f);

        //                    part1.m_birthPosition = part1.m_currentPosition - part1.m_direction * m_waveParticles[i].m_timeMoved;
        //                    part2.m_birthPosition = part2.m_currentPosition - part2.m_direction * m_waveParticles[i].m_timeMoved;

        //                    std::cout << "Time Moved: " << m_waveParticles[i].m_timeMoved << std::endl;

        //                    part1.m_dispersionAngle = m_waveParticles[i].m_dispersionAngle * amountTowardB;
        //                    part2.m_dispersionAngle = m_waveParticles[i].m_dispersionAngle * amountTowardC;

        //                    // part1.m_particleSize = part1Size;
        //                    // part2.m_particleSize = part2Size;

        //                    m_waveParticles[i].m_amplitude = 0.0f;
        //                    KillParticles();

        //                    AddParticle(part1);
        //                    AddParticle(part2);

        //                    i = 0;
        //                    continue;
        //                }
        //            }
        //        }
        //    }

            // TODO: Extract to its own function
            // NOTE: Do collisions with line segments
            //for (auto& lineSegment : m_collisionSegments)
            //{
            //    Vector3 first = lineSegment.m_first;
            //    Vector3 second = lineSegment.m_second;

            //    Vector3 lineDir = second - first;
            //    lineDir = lineDir.Normalized();
            //    Vector3 lineDirNorm = Vector3(-1.0f * lineDir.y, lineDir.x, 0.0f);

            //    float distanceOld = (oldPoint - first).Dot(lineDirNorm);
            //    float distanceNew = (newPoint - first).Dot(lineDirNorm);


            //    // std::cout << "distancenew * old: " << (distanceNew * distanceOld) << std::endl;
            //    if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
            //    {
            //        float s1_x, s1_y, s2_x, s2_y;
            //        s1_x = newPoint.x - oldPoint.x;
            //        s1_y = newPoint.y - oldPoint.y;

            //        s2_x = second.x - first.x;
            //        s2_y = second.y - first.y;

            //        float s, t;
            //        s = (-s1_y * (oldPoint.x - first.x) + s1_x * (oldPoint.y - first.y)) / (-s2_x * s1_y + s1_x * s2_y);
            //        t = (s2_x * (oldPoint.y - first.y) - s2_y * (oldPoint.x - first.x)) / (-s2_x * s1_y + s1_x * s2_y);

            //        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
            //        {
            //            // Collision detected
            //            // Vector3 collisionPoint(oldPoint.x + (t * s1_x), oldPoint.y + (t * s1_y), 0.0f);
            //            // std::cout << "Collision Detected at: " << collisionPoint << std::endl;
            //            //
            //            // std::cout << "LineDirNorm: " << lineDirNorm << std::endl;
            //            // lineDirNorm.Normalized();

            //            m_waveParticles[i].m_direction = m_waveParticles[i].m_direction -
            //                (2.0f * m_waveParticles[i].m_direction.Dot(lineDirNorm) * lineDirNorm);
            //            m_waveParticles[i].m_birthPosition = m_waveParticles[i].m_birthPosition +
            //                (2.0f * (first - m_waveParticles[i].m_birthPosition).Dot(lineDirNorm) * lineDirNorm);
            //            collision = true;
            //            break;

            //            // // Collision Response
            //            // m_waveParticles[i].m_direction = m_waveParticles[i].m_direction -
            //            //     (2.0f * m_waveParticles[i].m_direction.Dot(lineDirNorm) * lineDirNorm);
            //            // m_waveParticles[i].m_birthPosition = m_waveParticles[i].m_birthPosition +
            //            //     (2.0f * (first - m_waveParticles[i].m_birthPosition).Dot(lineDirNorm) * lineDirNorm);
            //            //     collision = true;
            //            //     break;
            //        }
            //    }
            //}


            // TODO: Call this in method
            // NOTE: Performs colision response with planes
            //for (auto& plane : m_collisionPlanes)
            //{
            //    float distanceOld = (m_waveParticles[i].m_currentPosition - plane.m_point).Dot(plane.m_normal);
            //    float distanceNew = (newPoint - plane.m_point).Dot(plane.m_normal);
            //    // std::cout << "Distance Old: " << distanceOld << std::endl;
            //    // std::cout << "Distance New: " << distanceNew << std::endl;
            //    // std::cout << "Old Point: " << m_waveParticles[i].m_currentPosition << std::endl;
            //    // std::cout << "New Point: " << newPoint << std::endl;

            //    // std::cout << "distancenew * old: " << (distanceNew * distanceOld) << std::endl;
            //    if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
            //    {
            //        // Collision with plane happened
            //        // std::cout << "Collision with plane happened" << std::endl;
            //        // std::cout << "Old Direction: " << m_waveParticles[i].m_direction << std::endl;
            //        m_waveParticles[i].m_direction = m_waveParticles[i].m_direction - (2.0f * m_waveParticles[i].m_direction.Dot(plane.m_normal) * plane.m_normal);
            //        m_waveParticles[i].m_birthPosition = m_waveParticles[i].m_birthPosition + (2.0f * (plane.m_point - m_waveParticles[i].m_birthPosition).Dot(plane.m_normal) * plane.m_normal);

            //        // std::cout << "New Direction: " << m_waveParticles[i].m_direction << std::endl;
            //        collision = true;
            //        break;
            //    }
            //}

            // Split Particles Into Smaller radius
            // if (!collision)
            // {
            //     // Generate triangle representing area that wave particle covers

            //     // We want left and right of the current m_position
            //     // Then we want left and right of the new position

            //     Vector2 oldPoint2d = Vector2(oldPoint.x, oldPoint.y);
            //     Vector2 newPoint2d = Vector2(newPoint.x, newPoint.y);

            //     Vector3 direction = m_waveParticles[i].m_direction;
            //     Vector3 birthPoint = m_waveParticles[i].m_birthPosition;
            //     float dispersionAngle = m_waveParticles[i].m_dispersionAngle;
            //     float halfAngle = dispersionAngle / 2.0f;

            //     Vector3 perpendicularDir = Vector3(-direction.y, direction.x, direction.z);
            //     perpendicularDir = perpendicularDir.Normalized();

            //     // Distance traveled
            //     float oldDistTraveled = Vector3(oldPoint - birthPoint).Magnitude();
            //     float newDistTraveled = Vector3(newPoint - birthPoint).Magnitude();

            //     // Perpendicular Directions
            //     Vector3 oldPerpDir = Vector3(perpendicularDir * oldDistTraveled) * tan(halfAngle);
            //     Vector3 newPerpDir = Vector3(perpendicularDir * newDistTraveled) * tan(halfAngle);

            //     // Calculate left and rights
            //     Vector3 oldLeft = oldPoint + oldPerpDir;
            //     Vector3 oldRight = oldPoint - oldPerpDir;

            //     Vector3 newLeft = newPoint + newPerpDir;
            //     Vector3 newRight = newPoint - newPerpDir;

            //     Vector2 B = Vector2(oldLeft.x, oldLeft.y);
            //     Vector2 C = Vector2(oldRight.x, oldRight.y);

            //     // Second points of rectangle
            //     Vector2 E = Vector2(newLeft.x, newLeft.y);
            //     Vector2 D = Vector2(newRight.x, newRight.y);

            //     // Check if end points of any obstacles intersect this area. If so, we have a place for diffraction
            //     vector<DiffractionCollisionPacket> m_diffractionCollisionPackets;

            //     // Need particle dir
            //     Vector2 particleDir = Vector2(direction.x, direction.y);
            //     particleDir = particleDir.Normalized();

            //     for (auto& lineSegment : m_collisionSegments)
            //     {
            //         Vector2 firstEndPoint = Vector2(lineSegment.m_first.x, lineSegment.m_first.y);
            //         Vector2 secondEndPoint = Vector2(lineSegment.m_second.x, lineSegment.m_second.y);

            //         if (PointInTriangle(firstEndPoint, B, C, D))
            //         {
            //             std::cout << "First" << std::endl;
            //             std::cout << "Point: " << firstEndPoint << std::endl;
            //             std::cout << "B: " << B << std::endl;
            //             std::cout << "C: " << C << std::endl;
            //             std::cout << "D: " << D << std::endl;
            //             Vector2 lineDir = secondEndPoint - firstEndPoint;
            //             lineDir = lineDir.Normalized();
            //             // m_diffractionCollisionPackets.push_back(DiffractionCollisionPacket(firstEndPoint, lineDir, particleDir));

            //             float dist = DistancePointToLine(oldPoint2d, newPoint2d, firstEndPoint);

            //         }

            //         if (PointInTriangle(secondEndPoint, B, D, E))
            //         {
            //             std::cout << "Second" << std::endl;
            //             std::cout << "Point: " << secondEndPoint << std::endl;
            //             std::cout << "B: " << B << std::endl;
            //             std::cout << "D: " << D << std::endl;
            //             std::cout << "E: " << E << std::endl;
            //             Vector2 lineDir = firstEndPoint - secondEndPoint;
            //             lineDir = lineDir.Normalized();
            //             // m_diffractionCollisionPackets.push_back(DiffractionCollisionPacket(secondEndPoint, lineDir, particleDir));

            //             float dist = DistancePointToLine(oldPoint2d, newPoint2d, firstEndPoint);
            //         }
            //     }

            //     if (m_diffractionCollisionPackets.size() > 0)
            //     {
            //         std::cout << "We had a collisions" << std::endl;

            //         for (auto itr = m_diffractionCollisionPackets.begin(); itr != m_diffractionCollisionPackets.end(); ++itr)
            //         {
            //             std::cout << "Collision with obstacle end point: " << itr->m_endPoint << std::endl;
            //             std::cout << "Obstacle direction: " << itr->m_obstacleDir << std::endl;
            //             std::cout << "ParticleDir: " << itr->m_particleDir << std::endl;

            //             // We need to spawn a new particle for this collision
            //             // First, lets identify the dispersion angle
            //             float diffractionDispersionAngle = acos(itr->m_obstacleDir.Dot(itr->m_particleDir));
            //             std::cout << "New dispersion angle: " << diffractionDispersionAngle << std::endl;

            //             // float tempAngle = DEGREE_TO_RAD(130);
            //             // if (diffractionDispersionAngle >= tempAngle)
            //             // {
            //             //     continue;
            //             // }

            //             Vector2 dispersionParticleDir = itr->m_obstacleDir + itr->m_particleDir;
            //             dispersionParticleDir = dispersionParticleDir.Normalized();

            //             float offset = 0.01f;
            //             Vector2 diffractionPos = itr->m_endPoint + (dispersionParticleDir * offset);

            //             dispersionParticleDir *= speed;

            //             WaveParticle diffractionParticle = WaveParticle(Vector3(diffractionPos.x, diffractionPos.y, 10), Vector3(dispersionParticleDir.x, dispersionParticleDir.y, 0.0f),
            //                 (float)g_TimerGame.TotalTime());
            //             diffractionParticle.m_dispersionAngle = diffractionDispersionAngle;
            //             // AddParticle(diffractionParticle);
            //             m_newParticles.push_back(diffractionParticle);
            //         }
            //     }
            // }


            // // Perform Diffraction Test
            // if (!collision)
            // {
            //     // Generate triangle representing area that wave particle covers

            //     // We want left and right of the current m_position
            //     // Then we want left and right of the new position

            //     Vector3 direction = m_waveParticles[i].m_direction;
            //     Vector3 birthPoint = m_waveParticles[i].m_birthPosition;
            //     float dispersionAngle = m_waveParticles[i].m_dispersionAngle;
            //     float halfAngle = dispersionAngle / 2.0f;

            //     Vector3 perpendicularDir = Vector3(-direction.y, direction.x, direction.z);
            //     perpendicularDir = perpendicularDir.Normalized();

            //     // Distance traveled
            //     float oldDistTraveled = Vector3(oldPoint - birthPoint).Magnitude();
            //     float newDistTraveled = Vector3(newPoint - birthPoint).Magnitude();

            //     // Perpendicular Directions
            //     Vector3 oldPerpDir = Vector3(perpendicularDir * oldDistTraveled) * tan(halfAngle);
            //     Vector3 newPerpDir = Vector3(perpendicularDir * newDistTraveled) * tan(halfAngle);

            //     // Calculate left and rights
            //     Vector3 oldLeft = oldPoint + oldPerpDir;
            //     Vector3 oldRight = oldPoint - oldPerpDir;

            //     Vector3 newLeft = newPoint + newPerpDir;
            //     Vector3 newRight = newPoint - newPerpDir;

            //     Vector2 B = Vector2(oldLeft.x, oldLeft.y);
            //     Vector2 C = Vector2(oldRight.x, oldRight.y);

            //     // Second points of rectangle
            //     Vector2 E = Vector2(newLeft.x, newLeft.y);
            //     Vector2 D = Vector2(newRight.x, newRight.y);

            //     // Check if end points of any obstacles intersect this area. If so, we have a place for diffraction
            //     vector<DiffractionCollisionPacket> m_diffractionCollisionPackets;

            //     // Need particle dir
            //     Vector2 particleDir = Vector2(direction.x, direction.y);
            //     particleDir = particleDir.Normalized();

            //     for (auto& lineSegment : m_collisionSegments)
            //     {
            //         Vector2 firstEndPoint = Vector2(lineSegment.m_first.x, lineSegment.m_first.y);
            //         Vector2 secondEndPoint = Vector2(lineSegment.m_second.x, lineSegment.m_second.y);

            //         if (PointInTriangle(firstEndPoint, B, C, D))
            //         {
            //             std::cout << "First" << std::endl;
            //             std::cout << "Point: " << firstEndPoint << std::endl;
            //             std::cout << "B: " << B << std::endl;
            //             std::cout << "C: " << C << std::endl;
            //             std::cout << "D: " << D << std::endl;
            //             Vector2 lineDir = secondEndPoint - firstEndPoint;
            //             lineDir = lineDir.Normalized();
            //             m_diffractionCollisionPackets.push_back(DiffractionCollisionPacket(firstEndPoint, lineDir, particleDir));
            //         }

            //         if (PointInTriangle(secondEndPoint, B, D, E))
            //         {
            //             std::cout << "Second" << std::endl;
            //             std::cout << "Point: " << secondEndPoint << std::endl;
            //             std::cout << "B: " << B << std::endl;
            //             std::cout << "D: " << D << std::endl;
            //             std::cout << "E: " << E << std::endl;
            //             Vector2 lineDir = firstEndPoint - secondEndPoint;
            //             lineDir = lineDir.Normalized();
            //             m_diffractionCollisionPackets.push_back(DiffractionCollisionPacket(secondEndPoint, lineDir, particleDir));
            //         }
            //     }

            //     if (m_diffractionCollisionPackets.size() > 0)
            //     {
            //         std::cout << "We had a collisions" << std::endl;

            //         for (auto itr = m_diffractionCollisionPackets.begin(); itr != m_diffractionCollisionPackets.end(); ++itr)
            //         {
            //             std::cout << "Collision with obstacle end point: " << itr->m_endPoint << std::endl;
            //             std::cout << "Obstacle direction: " << itr->m_obstacleDir << std::endl;
            //             std::cout << "ParticleDir: " << itr->m_particleDir << std::endl;

            //             // We need to spawn a new particle for this collision
            //             // First, lets identify the dispersion angle
            //             float diffractionDispersionAngle = acos(itr->m_obstacleDir.Dot(itr->m_particleDir));
            //             std::cout << "New dispersion angle: " << diffractionDispersionAngle << std::endl;

            //             // float tempAngle = DEGREE_TO_RAD(130);
            //             // if (diffractionDispersionAngle >= tempAngle)
            //             // {
            //             //     continue;
            //             // }

            //             Vector2 dispersionParticleDir = itr->m_obstacleDir + itr->m_particleDir;
            //             dispersionParticleDir = dispersionParticleDir.Normalized();

            //             float offset = 0.01f;
            //             Vector2 diffractionPos = itr->m_endPoint + (dispersionParticleDir * offset);

            //             dispersionParticleDir *= speed;

            //             WaveParticle diffractionParticle = WaveParticle(Vector3(diffractionPos.x, diffractionPos.y, 10), Vector3(dispersionParticleDir.x, dispersionParticleDir.y, 0.0f),
            //                 (float)g_TimerGame.TotalTime());
            //             diffractionParticle.m_dispersionAngle = diffractionDispersionAngle;
            //             // AddParticle(diffractionParticle);
            //             m_newParticles.push_back(diffractionParticle);
            //         }
            //     }
            // }

            // Perform subdivide
            {
                // Do subdivide
                float theta = m_waveParticles[i].m_dispersionAngle;
                float phi = theta / 2.0f;
                float distanceTraveled = m_waveParticles[i].m_speed * timeMoved; // Assumes normalized direction of travel
                float tempOpposite = distanceTraveled * tan(phi);
                float distanceBetweenParticles = 2.0f * tempOpposite * cos(phi);

                int numSubdividing = 0;

                // std::cout << "Distance Moved: " << distanceMoved.Magnitude() << std::endl;
                // Perform subdivide

                //if (i == 0)
                //{
                //    std::cout << "Angle owned: " << m_waveParticles[i].m_dispersionAngle << std::endl;
                //    std::cout << "distanceBetweenParticles: " << distanceBetweenParticles << std::endl;
                //    std::cout << "(m_waveParticles[i].m_particleSize / 3.0f): " << (m_waveParticles[i].m_particleSize / 3.0f) << std::endl;
                //    std::cout << "Distance traveled" << distanceTraveled << std::endl;
                //}

                if (distanceBetweenParticles > (m_waveParticles[i].m_particleSize / 9.0f))
                {
                    //std::cout << "Particle Subdividing: " << i << std::endl;
                    // std::cout << "AngleOwned: " << angleOwned << std::endl;
                    // std::cout << "i" << i << std::endl;
                    // std::cout << "Performing Subdivide" << std::endl;
                    // std::cout << "Subdivide" << std::endl;
                    numSubdividing++;

                    // m_waveParticles[i].m_active = true;

                    Vector3 pos = m_waveParticles[i].m_birthPosition;
                    Vector3 direction = m_waveParticles[i].m_direction;
                    float originalAngle = m_waveParticles[i].m_dispersionAngle;
                    float angle = originalAngle / 2.0f * (1.0f / 3.0f);
                    float negAngle = -1.0f * angle;

                    // std::cout << "Angle: " << angle << std::endl;

                    float xLeft = direction.x * cos(angle) - direction.y * sin(angle);
                    float yLeft = direction.y * cos(angle) + direction.x * sin(angle);

                    float xRight = direction.x * cos(negAngle) - direction.y * sin(negAngle);
                    float yRight = direction.y * cos(negAngle) + direction.x * sin(negAngle);

                    Vector2 left(xLeft, yLeft);
                    left = left.Normalized();

                    Vector2 right(xRight, yRight);
                    right = right.Normalized();

                    //std::cout << "original Direction: " << direction << std::endl;
                    //std::cout << "Left Direction: <" << left.x << ", " << left.y << ">" << std::endl;
                    //std::cout << "Right Direction: <" << right.x << ", " << right.y << ">" << std::endl;

                    // cin.get();

                    // std::cout << "original Particle Current Position: " << m_waveParticles[i].m_currentPosition << std::endl;

                    WaveParticle particleLeft = WaveParticle(pos, Vector3(left.x, left.y, 0.0f), m_waveParticles[i].m_birthTime, m_waveParticles[i].m_particleSize,
                        m_waveParticles[i].m_speed);
                    WaveParticle particleRight = WaveParticle(pos, Vector3(right.x, right.y, 0.0f), m_waveParticles[i].m_birthTime, m_waveParticles[i].m_particleSize,
                        m_waveParticles[i].m_speed);
                    WaveParticle particleSame = WaveParticle(pos, Vector3(direction.x, direction.y, 0.0f), m_waveParticles[i].m_birthTime, m_waveParticles[i].m_particleSize,
                        m_waveParticles[i].m_speed);

                    particleLeft.m_currentPosition = particleLeft.m_birthPosition + particleLeft.m_direction * timeMoved;
                    particleRight.m_currentPosition = particleRight.m_birthPosition + particleRight.m_direction * timeMoved;
                    particleSame.m_currentPosition = particleSame.m_birthPosition + particleSame.m_direction * timeMoved;

                    // std::cout << "Left Current Position: " <<  particleLeft.m_currentPosition << std::endl;
                    // std::cout << "Right Current Position: " << particleRight.m_currentPosition << std::endl;


                    particleLeft.m_dispersionAngle = originalAngle / 3.0f;
                    particleRight.m_dispersionAngle = originalAngle / 3.0f;
                    particleSame.m_dispersionAngle = originalAngle / 3.0f;

                    particleLeft.m_amplitude = m_waveParticles[i].m_amplitude / 3.0f;
                    particleRight.m_amplitude = m_waveParticles[i].m_amplitude / 3.0f;
                    particleSame.m_amplitude = m_waveParticles[i].m_amplitude / 3.0f;

                    m_waveParticles[i].m_amplitude = 0.0f;

                    m_newParticles.push_back(particleSame);
                    m_newParticles.push_back(particleLeft);
                    m_newParticles.push_back(particleRight);
                }
            }

            m_waveParticles[i].m_currentPosition = newPoint;
            m_waveParticles[i].m_amplitude = m_waveParticles[i].m_amplitude - (g_amplitudeDropRate * timeMoved);

            m_waveParticles[i].m_timeMoved = timeMoved;
        }

        for (int i = 0; i < m_newParticles.size(); i++)
        {
            //std::cout << "Adding particle" << std::endl;
            AddParticle(m_newParticles[i]);
        }

        // std::cout << "New Particle Number: " << m_numActualParticles << std::endl;

        KillParticles();

        // std::cout << "Number of particles: " << m_numActualParticles << std::endl;
    }


    /*void ParticleSystem::Update(float timestep)
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
    }*/

    void ParticleSystem::KillParticles()
    {
        int numBefore = m_numActualParticles;
        int i = 0;
        while (i < m_maxParticles)
        {
            if (m_waveParticles[i].m_active && m_waveParticles[i].m_amplitude <= 0.01f)
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
