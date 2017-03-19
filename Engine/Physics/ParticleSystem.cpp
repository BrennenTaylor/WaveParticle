#include  "ParticleSystem.h"

#include <DirectXMath.h>

#include "../NewRenderer/Renderer.h"

#include "Sample.h"

#include <time.h>
#include <math.h>

#include "../Math/CollisionFunctions.h"

#include "../Core/Timer.h"

#include "DiffractionCollisionPacket.h"

using namespace DirectX;

namespace Farlor
{
    extern Renderer g_RenderingSystem;
    extern Farlor::Timer g_TimerGame;

    static float zCoord = 10.0f;
    static float speed = 20.0f;
    static float g_amplitudeDropRate = 0.001f;

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

        const int numPoints = 15;
        const float initialParticleRadius = 20;

        for (int i = 0; i < numPoints; i++)
        {
            float x = 3.0f;
            float y = 0;


            Vector3 direction(0.0f, 1.0f, 0.0f);
            direction *= speed;
            WaveParticle newParticle(Vector3(x + (initialParticleRadius * i), y, zCoord), direction, (float)g_TimerGame.TotalTime(), initialParticleRadius);
            // We need to figure out how to measure "angle" for  parallel waves. Important for diffraction.
            newParticle.m_dispersionAngle = 0.0f;
            AddParticle(newParticle);
        }

        // for (int i = 0; i < numPoints; i++)
        // {
        //     float x = 3.0f;
        //     float y = 60;


        //     Vector3 direction(0.0f, -1.0f, 0.0f);
        //     direction *= speed;
        //     WaveParticle newParticle(Vector3(x + (initialParticleRadius * i), y, zCoord), direction, (float)g_TimerGame.TotalTime(), initialParticleRadius);
        //     // We need to figure out how to measure "angle" for  parallel waves. Important for diffraction.
        //     newParticle.m_dispersionAngle = 0.0f;
        //     AddParticle(newParticle);
        // }

        // const int numPoints = 16;

        // auto angle = 0.0f;
        // auto deltaTheta = TWO_PI / numPoints;
        // // auto deltaTheta = TWO_PI / 16;

        // angle += 2*deltaTheta;
        
        // auto genIntInRange = [](int min, int max)
        // {
        //     return rand() % max + min;
        // };
        
        // Vector3 startPosition = Vector3(0.0f, 0.0f, zCoord);
        
        // for (int i = 0; i < numPoints; i++)
        // {
        //     Vector3 planeOrigin = Vector3(0.0f, 0.0f, zCoord);
        //     Vector3 direction = Vector3(cos(angle), sin(angle), 0.0f);
        //     direction = direction.Normalized();
        //     direction *= speed;
        //     WaveParticle newParticle(startPosition, direction, (float)g_TimerGame.TotalTime(), 20.f);
        //     newParticle.m_dispersionAngle = deltaTheta;
        //     AddParticle(newParticle);
        
        //     cout << "Particle direction: " << direction.Normalized() << endl;
        //     cout << "Dispersion angle: " << deltaTheta << endl;

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


            // Before Collisions, we do the radius subdivide step
            {
                // Generate triangle representing area that wave particle covers
                
                // We want left and right of the current m_position
                // Then we want left and right of the new position

                Vector2 oldPoint2d = Vector2(oldPoint.x, oldPoint.y);
                Vector2 newPoint2d = Vector2(newPoint.x, newPoint.y);

                Vector3 direction = m_waveParticles[i].m_direction;
                direction = direction.Normalized();
                Vector2 direction2d = Vector2(direction.x, direction.y);
                direction2d = direction2d.Normalized();

                Vector3 birthPoint = m_waveParticles[i].m_birthPosition;
                float dispersionAngle = m_waveParticles[i].m_dispersionAngle;
                float halfAngle = dispersionAngle / 2.0f;

                Vector3 perpendicularDir = Vector3(-direction.y, direction.x, direction.z);
                perpendicularDir = perpendicularDir.Normalized();

                // Distance traveled
                float oldDistTraveled = Vector3(oldPoint - birthPoint).Magnitude();
                float newDistTraveled = Vector3(newPoint - birthPoint).Magnitude();

                // Perpendicular Directions
                // Vector3 oldPerpDir = Vector3(perpendicularDir * oldDistTraveled) * tan(halfAngle);
                // Vector3 newPerpDir = Vector3(perpendicularDir * newDistTraveled) * tan(halfAngle);

                Vector3 oldPerpDir = perpendicularDir * (m_waveParticles[i].m_particleSize / 2.0f);
                Vector3 newPerpDir = perpendicularDir * (m_waveParticles[i].m_particleSize / 2.0f);

                // Calculate left and rights
                Vector3 oldLeft = oldPoint + oldPerpDir;
                Vector3 oldRight = oldPoint - oldPerpDir;

                Vector3 newLeft = newPoint + newPerpDir;
                Vector3 newRight = newPoint - newPerpDir;

                Vector2 B = Vector2(oldLeft.x, oldLeft.y);
                Vector2 C = Vector2(oldRight.x, oldRight.y);

                // Second points of rectangle
                Vector2 E = Vector2(newLeft.x, newLeft.y);
                Vector2 D = Vector2(newRight.x, newRight.y);

                // Check if end points of any obstacles intersect this area. If so, we have a place for diffraction
                vector<DiffractionCollisionPacket> m_diffractionCollisionPackets;

                // Need particle dir
                Vector2 particleDir = Vector2(direction.x, direction.y);
                particleDir = particleDir.Normalized();

                // Make sure that the triangle doesnt have an area of zero
                if (!((TriangleArea(B, C, D) < 0.00000001f) || (TriangleArea(B, D, E) < 0.000000001f)))
                {
                    for (auto& lineSegment : m_collisionSegments)
                    {
                        Vector2 firstEndPoint = Vector2(lineSegment.m_first.x, lineSegment.m_first.y);
                        Vector2 secondEndPoint = Vector2(lineSegment.m_second.x, lineSegment.m_second.y);

                        bool endpointCollision = false;
                        
                        // First end point
                        if (PointInTriangle(firstEndPoint, B, C, D))
                        {
                            cout << "First" << endl;
                            cout << "Point: " << firstEndPoint << endl;
                            cout << "B: " << B << endl;
                            cout << "C: " << C << endl;
                            cout << "D: " << D << endl;
                            endpointCollision = true;
                        }
                        if (PointInTriangle(firstEndPoint, B, D, E))
                        {
                            cout << "First" << endl;
                            cout << "Point: " << firstEndPoint << endl;
                            cout << "B: " << B << endl;
                            cout << "D: " << D << endl;
                            cout << "E: " << E << endl;
                            endpointCollision = true;
                        }

                        if (endpointCollision)
                        {
                            cout << "OldPoint2d: " << oldPoint2d << endl;
                            cout << "NewPoint2d: " << newPoint2d << endl;

                            float distLeft = DistancePointToLine(B, E, firstEndPoint);
                            float distRight = DistancePointToLine(C, D, firstEndPoint);
                            float particleSize = m_waveParticles[i].m_particleSize;

                            cout << "Dist left: " << distLeft << endl;
                            cout << "Dist Right: " << distRight << endl;
                            cout << "Particle Radius: " << m_waveParticles[i].m_particleSize << endl;

                            float amountTowardB = distLeft / (particleSize*2.0f);
                            float amountTowardC = distRight / (particleSize*2.0f);

                            // Vector2 endpointDir = secondEndPoint - Vector2(birthPoint.x, birthPoint.y);
                            // endpointDir = endpointDir.Normalized();
                            // cout << "Endpoint dir: " <<  endpointDir << endl;
                            // cout << "Dir2D: " <<  direction2d << endl;

                            // float angleToParticleDir = acos(endpointDir.Dot(direction2d));
                            // cout << angleToParticleDir << endl;

                            // float angle1 = halfAngle - angleToParticleDir;
                            // float angle2 = halfAngle + angleToParticleDir;

                            // cout << dispersionAngle << endl; 
                            // cout << "Angle1: " << angle1 << endl;
                            // cout << "Angle2: " << angle2 << endl;
                            // cout << "Tot angle: " << angle1+angle2 << endl;

                            // float halfAngle1 = angle1 / 2.0f;
                            // float halfAngle2 = angle2 / 2.0f;

                            // cout << "Half Angle 1: " << halfAngle1 << endl;
                            // cout << "Half Angle 2: " << halfAngle2 << endl;

                            // Vector2 part1Dir(endpointDir.x*cos(halfAngle1) - endpointDir.y*sin(halfAngle1),
                            //     endpointDir.x*sin(halfAngle1) + endpointDir.y*cos(halfAngle1));
                            // part1Dir = part1Dir.Normalized();
                            
                            // Vector2 part2Dir(endpointDir.x*cos(-1.0f*halfAngle2) - endpointDir.y*sin(-1.0f*halfAngle2),
                            //     endpointDir.x*sin(-1.0f*halfAngle2) + endpointDir.y*cos(-1.0f*halfAngle2));
                            // part2Dir = part2Dir.Normalized();

                            // cout << "Part1Dir: " << part1Dir.Normalized() << endl;
                            // cout << "Part2Dir: " << part2Dir.Normalized() << endl;


                            Vector2 part1Dir = Vector2(particleDir.x, particleDir.y);
                            Vector2 part2Dir = Vector2(particleDir.x, particleDir.y);

                            part1Dir = part1Dir.Normalized();
                            part2Dir = part2Dir.Normalized();

                            Vector2 bc = C-B;

                            float part1Size = amountTowardB*m_waveParticles[i].m_particleSize;
                            float part2Size = amountTowardC*m_waveParticles[i].m_particleSize;

                            WaveParticle part1 = m_waveParticles[i];
                            WaveParticle part2 = m_waveParticles[i];
                            
                            part1.m_particleSize = part1Size*2.0f;
                            part2.m_particleSize = part2Size*2.0f;
                            
                            part1.m_direction = Vector3(part1Dir.x, part1Dir.y, 0.0f) * speed;
                            part2.m_direction = Vector3(part2Dir.x, part2Dir.y, 0.0f) * speed;

                            part1.m_currentPosition = Vector3(B.x, B.y, zCoord) + Vector3(bc.x, bc.y, zCoord)*(amountTowardB / 2.0f);
                            part2.m_currentPosition = Vector3(C.x, C.y, zCoord) - Vector3(bc.x, bc.y, zCoord)*(amountTowardC / 2.0f);

                            part1.m_birthPosition = part1.m_currentPosition - part1.m_direction * m_waveParticles[i].m_timeMoved;
                            part2.m_birthPosition = part2.m_currentPosition - part2.m_direction * m_waveParticles[i].m_timeMoved;

                            cout << "Time Moved: " << m_waveParticles[i].m_timeMoved << endl;

                            part1.m_dispersionAngle = m_waveParticles[i].m_dispersionAngle*amountTowardB;
                            part2.m_dispersionAngle = m_waveParticles[i].m_dispersionAngle*amountTowardC;

                            // part1.m_particleSize = part1Size;
                            // part2.m_particleSize = part2Size;

                            m_waveParticles[i].m_amplitude = 0.0f;
                            KillParticles();

                            AddParticle(part1);
                            AddParticle(part2);

                            i = 0;
                            continue;
                        }

                        // Second end point
                        if (PointInTriangle(secondEndPoint, B, C, D))
                        {
                            cout << "Second" << endl;
                            cout << "Point: " << secondEndPoint << endl;
                            cout << "B: " << B << endl;
                            cout << "C: " << C << endl;
                            cout << "D: " << D << endl;
                            endpointCollision = true;
                        }
                        if (PointInTriangle(secondEndPoint, B, D, E))
                        {
                            cout << "Second" << endl;
                            cout << "Point: " << secondEndPoint << endl;
                            cout << "B: " << B << endl;
                            cout << "D: " << D << endl;
                            cout << "E: " << E << endl;
                            endpointCollision = true;
                        }

                        if (endpointCollision)
                        {
                            cout << "OldPoint2d: " << oldPoint2d << endl;
                            cout << "NewPoint2d: " << newPoint2d << endl;

                            float distLeft = DistancePointToLine(B, E, secondEndPoint);
                            float distRight = DistancePointToLine(C, D, secondEndPoint);
                            float particleSize = m_waveParticles[i].m_particleSize;

                            cout << "Dist left: " << distLeft << endl;
                            cout << "Dist Right: " << distRight << endl;
                            cout << "Particle Radius: " << m_waveParticles[i].m_particleSize << endl;

                            float amountTowardB = distLeft / (particleSize*2.0f);
                            float amountTowardC = distRight / (particleSize*2.0f);

                            // Vector2 endpointDir = secondEndPoint - Vector2(birthPoint.x, birthPoint.y);
                            // endpointDir = endpointDir.Normalized();
                            // cout << "Endpoint dir: " <<  endpointDir << endl;
                            // cout << "Dir2D: " <<  direction2d << endl;

                            // float angleToParticleDir = acos(endpointDir.Dot(direction2d));
                            // cout << angleToParticleDir << endl;

                            // float angle1 = halfAngle - angleToParticleDir;
                            // float angle2 = halfAngle + angleToParticleDir;

                            // cout << dispersionAngle << endl; 
                            // cout << "Angle1: " << angle1 << endl;
                            // cout << "Angle2: " << angle2 << endl;
                            // cout << "Tot angle: " << angle1+angle2 << endl;

                            // float halfAngle1 = angle1 / 2.0f;
                            // float halfAngle2 = angle2 / 2.0f;

                            // cout << "Half Angle 1: " << halfAngle1 << endl;
                            // cout << "Half Angle 2: " << halfAngle2 << endl;

                            // Vector2 part1Dir(endpointDir.x*cos(halfAngle1) - endpointDir.y*sin(halfAngle1),
                            //     endpointDir.x*sin(halfAngle1) + endpointDir.y*cos(halfAngle1));
                            // part1Dir = part1Dir.Normalized();
                            
                            // Vector2 part2Dir(endpointDir.x*cos(-1.0f*halfAngle2) - endpointDir.y*sin(-1.0f*halfAngle2),
                            //     endpointDir.x*sin(-1.0f*halfAngle2) + endpointDir.y*cos(-1.0f*halfAngle2));
                            // part2Dir = part2Dir.Normalized();

                            // cout << "Part1Dir: " << part1Dir.Normalized() << endl;
                            // cout << "Part2Dir: " << part2Dir.Normalized() << endl;


                            Vector2 part1Dir = Vector2(particleDir.x, particleDir.y);
                            Vector2 part2Dir = Vector2(particleDir.x, particleDir.y);

                            part1Dir = part1Dir.Normalized();
                            part2Dir = part2Dir.Normalized();

                            Vector2 bc = C-B;

                            float part1Size = amountTowardB*m_waveParticles[i].m_particleSize;
                            float part2Size = amountTowardC*m_waveParticles[i].m_particleSize;

                            WaveParticle part1 = m_waveParticles[i];
                            WaveParticle part2 = m_waveParticles[i];
                            
                            part1.m_particleSize = part1Size*2.0f;
                            part2.m_particleSize = part2Size*2.0f;
                            
                            part1.m_direction = Vector3(part1Dir.x, part1Dir.y, 0.0f) * speed;
                            part2.m_direction = Vector3(part2Dir.x, part2Dir.y, 0.0f) * speed;

                            part1.m_currentPosition = Vector3(B.x, B.y, zCoord) + Vector3(bc.x, bc.y, zCoord)*(amountTowardB / 2.0f);
                            part2.m_currentPosition = Vector3(C.x, C.y, zCoord) - Vector3(bc.x, bc.y, zCoord)*(amountTowardC / 2.0f);

                            part1.m_birthPosition = part1.m_currentPosition - part1.m_direction * m_waveParticles[i].m_timeMoved;
                            part2.m_birthPosition = part2.m_currentPosition - part2.m_direction * m_waveParticles[i].m_timeMoved;

                            cout << "Time Moved: " << m_waveParticles[i].m_timeMoved << endl;

                            part1.m_dispersionAngle = m_waveParticles[i].m_dispersionAngle*amountTowardB;
                            part2.m_dispersionAngle = m_waveParticles[i].m_dispersionAngle*amountTowardC;

                            // part1.m_particleSize = part1Size;
                            // part2.m_particleSize = part2Size;

                            m_waveParticles[i].m_amplitude = 0.0f;
                            KillParticles();

                            AddParticle(part1);
                            AddParticle(part2);

                            i = 0;
                            continue;
                        }
                    }
                }
            }

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
            //             cout << "First" << endl;
            //             cout << "Point: " << firstEndPoint << endl;
            //             cout << "B: " << B << endl;
            //             cout << "C: " << C << endl;
            //             cout << "D: " << D << endl;
            //             Vector2 lineDir = secondEndPoint - firstEndPoint;
            //             lineDir = lineDir.Normalized();
            //             // m_diffractionCollisionPackets.push_back(DiffractionCollisionPacket(firstEndPoint, lineDir, particleDir));

            //             float dist = DistancePointToLine(oldPoint2d, newPoint2d, firstEndPoint);
                        
            //         }
                    
            //         if (PointInTriangle(secondEndPoint, B, D, E))
            //         {
            //             cout << "Second" << endl;
            //             cout << "Point: " << secondEndPoint << endl;
            //             cout << "B: " << B << endl;
            //             cout << "D: " << D << endl;
            //             cout << "E: " << E << endl;
            //             Vector2 lineDir = firstEndPoint - secondEndPoint;
            //             lineDir = lineDir.Normalized();
            //             // m_diffractionCollisionPackets.push_back(DiffractionCollisionPacket(secondEndPoint, lineDir, particleDir));

            //             float dist = DistancePointToLine(oldPoint2d, newPoint2d, firstEndPoint);
            //         }
            //     }

            //     if (m_diffractionCollisionPackets.size() > 0)
            //     {
            //         cout << "We had a collisions" << endl;

            //         for (auto itr = m_diffractionCollisionPackets.begin(); itr != m_diffractionCollisionPackets.end(); ++itr)
            //         {
            //             cout << "Collision with obstacle end point: " << itr->m_endPoint << endl;
            //             cout << "Obstacle direction: " << itr->m_obstacleDir << endl;
            //             cout << "ParticleDir: " << itr->m_particleDir << endl;

            //             // We need to spawn a new particle for this collision
            //             // First, lets identify the dispersion angle
            //             float diffractionDispersionAngle = acos(itr->m_obstacleDir.Dot(itr->m_particleDir));
            //             cout << "New dispersion angle: " << diffractionDispersionAngle << endl;

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
            //             cout << "First" << endl;
            //             cout << "Point: " << firstEndPoint << endl;
            //             cout << "B: " << B << endl;
            //             cout << "C: " << C << endl;
            //             cout << "D: " << D << endl;
            //             Vector2 lineDir = secondEndPoint - firstEndPoint;
            //             lineDir = lineDir.Normalized();
            //             m_diffractionCollisionPackets.push_back(DiffractionCollisionPacket(firstEndPoint, lineDir, particleDir));
            //         }
                    
            //         if (PointInTriangle(secondEndPoint, B, D, E))
            //         {
            //             cout << "Second" << endl;
            //             cout << "Point: " << secondEndPoint << endl;
            //             cout << "B: " << B << endl;
            //             cout << "D: " << D << endl;
            //             cout << "E: " << E << endl;
            //             Vector2 lineDir = firstEndPoint - secondEndPoint;
            //             lineDir = lineDir.Normalized();
            //             m_diffractionCollisionPackets.push_back(DiffractionCollisionPacket(secondEndPoint, lineDir, particleDir));
            //         }
            //     }

            //     if (m_diffractionCollisionPackets.size() > 0)
            //     {
            //         cout << "We had a collisions" << endl;

            //         for (auto itr = m_diffractionCollisionPackets.begin(); itr != m_diffractionCollisionPackets.end(); ++itr)
            //         {
            //             cout << "Collision with obstacle end point: " << itr->m_endPoint << endl;
            //             cout << "Obstacle direction: " << itr->m_obstacleDir << endl;
            //             cout << "ParticleDir: " << itr->m_particleDir << endl;

            //             // We need to spawn a new particle for this collision
            //             // First, lets identify the dispersion angle
            //             float diffractionDispersionAngle = acos(itr->m_obstacleDir.Dot(itr->m_particleDir));
            //             cout << "New dispersion angle: " << diffractionDispersionAngle << endl;

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
                if (distanceBetweenParticles > (m_waveParticles[i].m_particleSize / 2.0f))
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

                    WaveParticle particleLeft = WaveParticle(pos, Vector3(left.x, left.y, 0.0f), m_waveParticles[i].m_birthTime, m_waveParticles[i].m_particleSize);
                    WaveParticle particleRight = WaveParticle(pos, Vector3(right.x, right.y, 0.0f), m_waveParticles[i].m_birthTime, m_waveParticles[i].m_particleSize);
                    WaveParticle particleSame = WaveParticle(pos, Vector3(direction.x, direction.y, 0.0f), m_waveParticles[i].m_birthTime, m_waveParticles[i].m_particleSize);

                    particleLeft.m_currentPosition = particleLeft.m_birthPosition + particleLeft.m_direction * timeMoved;
                    particleRight.m_currentPosition = particleRight.m_birthPosition + particleRight.m_direction * timeMoved;
                    particleSame.m_currentPosition = particleSame.m_birthPosition + particleSame.m_direction * timeMoved;

                    // cout << "Left Current Position: " <<  particleLeft.m_currentPosition << endl;
                    // cout << "Right Current Position: " << particleRight.m_currentPosition << endl;


                    particleLeft.m_dispersionAngle = origionalAngle / 3.0f;
                    particleRight.m_dispersionAngle = origionalAngle / 3.0f;
                    particleSame.m_dispersionAngle = origionalAngle / 3.0f;

                    particleLeft.m_amplitude = m_waveParticles[i].m_amplitude / 1.1f;
                    particleRight.m_amplitude = m_waveParticles[i].m_amplitude / 1.1f;
                    particleSame.m_amplitude = m_waveParticles[i].m_amplitude / 1.1f;

                    m_waveParticles[i].m_amplitude = 0.0f;

                    m_newParticles.push_back(particleSame);
                    m_newParticles.push_back(particleLeft);
                    m_newParticles.push_back(particleRight);
                }
            }

            // timeMoved = (float)m_timer.GetCurrentTime() - m_waveParticles[i].m_birthTime;
            m_waveParticles[i].m_currentPosition = m_waveParticles[i].m_birthPosition + m_waveParticles[i].m_direction * timeMoved;
            m_waveParticles[i].m_amplitude = m_waveParticles[i].m_amplitude - (g_amplitudeDropRate * timeMoved);

            m_waveParticles[i].m_timeMoved = timeMoved;
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
                // cout << "Killing particle" << endl;
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
                    // cout << "Killing particle: " << endl;
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

        m_vertices = new WaveParticleVertex[m_vertexCount];
        assert(m_vertices);

        m_indices = new unsigned int[m_indexCount];
        assert(m_vertices);

        memset(m_vertices, 0, sizeof(WaveParticleVertex)*m_vertexCount);

        for(int i = 0; i < m_indexCount; i++)
        {
            m_indices[i] = i;
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
        result = pDevice->CreateInputLayout(WaveParticleVertex::s_layout, WaveParticleVertex::s_numElements,
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
        WaveParticleVertex* verticesPtr;

        // Reset array of vertices
        memset(m_vertices, 0, sizeof(WaveParticleVertex) * m_vertexCount);

        // Now build vertex array from particle list array
        // Each particle becomes a quad made out of two triangles
        int index = 0;
        for (int i = 0; i < m_numActualParticles; i++)
        {
            float particleSize = m_waveParticles[i].m_particleSize;
            float waveFrontSize = 20.0f;

            // Angle between two 2d vectors
            Vector2 xDir{1.0f, 0.0f};
            Vector2 yDir{0.0f, 1.0f};
            Vector2 particleDir{m_waveParticles[i].m_direction.x, m_waveParticles[i].m_direction.y};
            particleDir = particleDir.Normalized();
            float angle = atan2(xDir.Dot(particleDir), yDir.Dot(particleDir));
            

            // Bottom Left
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x - particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y - waveFrontSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 0.0f;
            m_vertices[index].m_uv.y = 1.0f;
            m_vertices[index].m_amplitude = m_waveParticles[i].m_amplitude;
            m_vertices[index].m_angle = angle;
            m_vertices[index].m_origin = m_waveParticles[i].m_currentPosition;
            index++;
            // Top Left
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x - particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y + waveFrontSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 0.0f;
            m_vertices[index].m_uv.y = 0.0f;
            m_vertices[index].m_amplitude = m_waveParticles[i].m_amplitude;
            m_vertices[index].m_angle = angle;
            m_vertices[index].m_origin = m_waveParticles[i].m_currentPosition;
            index++;
            // Bottom Right
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x + particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y - waveFrontSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 1.0f;
            m_vertices[index].m_uv.y = 1.0f;
            m_vertices[index].m_amplitude = m_waveParticles[i].m_amplitude;
            m_vertices[index].m_angle = angle;
            m_vertices[index].m_origin = m_waveParticles[i].m_currentPosition;
            index++;
            // Bottom Right
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x + particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y - waveFrontSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 1.0f;
            m_vertices[index].m_uv.y = 1.0f;
            m_vertices[index].m_amplitude = m_waveParticles[i].m_amplitude;
            m_vertices[index].m_angle = angle;
            m_vertices[index].m_origin = m_waveParticles[i].m_currentPosition;
            index++;
            // Top Left
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x - particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y + waveFrontSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 0.0f;
            m_vertices[index].m_uv.y = 0.0f;
            m_vertices[index].m_amplitude = m_waveParticles[i].m_amplitude;
            m_vertices[index].m_angle = angle;
            m_vertices[index].m_origin = m_waveParticles[i].m_currentPosition;
            index++;
            // Top Right
            m_vertices[index].m_position.x = m_waveParticles[i].m_currentPosition.x + particleSize;
            m_vertices[index].m_position.y = m_waveParticles[i].m_currentPosition.y + waveFrontSize;
            m_vertices[index].m_position.z = m_waveParticles[i].m_currentPosition.z;
            m_vertices[index].m_uv.x = 1.0f;
            m_vertices[index].m_uv.y = 0.0f;
            m_vertices[index].m_amplitude = m_waveParticles[i].m_amplitude;
            m_vertices[index].m_angle = angle;
            m_vertices[index].m_origin = m_waveParticles[i].m_currentPosition;
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
        verticesPtr = (WaveParticleVertex*)mappedVertices.pData;

        memcpy(verticesPtr, (void*)m_vertices, sizeof(WaveParticleVertex)*m_vertexCount);
        // Unlock buffer
        pDeviceContext->Unmap(m_vertexBuffer, 0);
    }

    void ParticleSystem::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11ShaderResourceView* pParticleTextureSRView, ID3D11SamplerState* pWPSampleState)
    {
        UpdateBuffers(pDevice, pDeviceContext);

        unsigned int stride = sizeof(WaveParticleVertex);
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
