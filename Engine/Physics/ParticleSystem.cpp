#include  "ParticleSystem.h"

#include <DirectXMath.h>

#include "../NewRenderer/Renderer.h"

#include "Sample.h"

#include <time.h>
#include <math.h>

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
            newParticle.m_dispersionAngle = 0.0f;
            AddParticle(newParticle);
        }
        //
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
        // for (int i = 0; i < numPoints; i++)
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
        // cout << "Failed to add particle, all particle slots active" << endl;
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

            // vector<Sample> particleSamples;
            //
            // // float startAngle = -1.0f * m_waveParticles[i].m_sampleAngle / 2.0f;
            // // float sampleAngleStep = m_waveParticles[i].m_sampleAngle / sampleDensity;
            //
            // int numCollided = 0;
            // // for (int j = 0; j < sampleDensity; j++)
            // // {
            //     Sample leftSample;
            //     Sample rightSample;
            //
            //     float angle = m_waveParticles[i].m_dispersionAngle;
            //
            //     Vector3 direction = m_waveParticles[i].m_direction;
            //
            //     Vector3 leftSampleDirection;
            //     leftSampleDirection.x = direction.x * cos(-angle) - direction.y * sin(-angle);
            //     leftSampleDirection.y = direction.x * sin(-angle) + direction.y * cos(-angle);
            //     leftSampleDirection = leftSampleDirection.Normalized();
            //
            //     Vector3 rightSampleDirection;
            //     rightSampleDirection.x = direction.x * cos(angle) - direction.y * sin(angle);
            //     rightSampleDirection.y = direction.x * sin(angle) + direction.y * cos(angle);
            //     rightSampleDirection = leftSampleDirection.Normalized();
            //
            //
            //     leftSample.m_isCollision = false;
            //     rightSample.m_isCollision = false;
            //
            //     // This needs to change as we are representing wave particles instead of samples
            //     // Parallel wave
            //     if (angle == 0.0f)
            //     {
            //         leftSample.m_oldPosition = oldPoint - Vector3(-direction.y, direction.x, 0.0f) * 10.0f;
            //         leftSample.m_newPosition = newPoint - Vector3(-direction.y, direction.x, 0.0f) * 10.0f;
            //
            //         rightSample.m_oldPosition = oldPoint + Vector3(-direction.y, direction.x, 0.0f) * 10.0f;
            //         rightSample.m_newPosition = newPoint + Vector3(-direction.y, direction.x, 0.0f) * 10.0f;
            //
            //
            //     }
            //     // Non parallel wave
            //     else
            //     {
            //     }
            //
            //     Vector3 sampleOldPoint = leftSample.m_oldPosition;
            //     Vector3 sampleNewPoint = leftSample.m_newPosition;
            //
            //     // Do collisions
            //     for (auto& lineSegment : m_collisionSegments)
            //     {
            //         Vector3 first = lineSegment.m_first;
            //         Vector3 second = lineSegment.m_second;
            //
            //         Vector3 lineDir = second - first;
            //         lineDir = lineDir.Normalized();
            //         Vector3 lineDirNorm = Vector3(-1.0f * lineDir.y, lineDir.x, 0.0f);
            //
            //         float distanceOld = (sampleOldPoint - first).Dot(lineDirNorm);
            //         float distanceNew = (sampleNewPoint - first).Dot(lineDirNorm);
            //
            //         if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
            //         {
            //             float s1_x, s1_y, s2_x, s2_y;
            //             s1_x = sampleNewPoint.x - sampleOldPoint.x;
            //             s1_y = sampleNewPoint.y - sampleOldPoint.y;
            //
            //             s2_x = second.x - first.x;
            //             s2_y = second.y - first.y;
            //
            //             float s, t;
            //             s = (-s1_y * (sampleOldPoint.x - first.x) + s1_x * (sampleOldPoint.y - first.y)) / (-s2_x * s1_y + s1_x * s2_y);
            //             t = ( s2_x * (sampleOldPoint.y - first.y) - s2_y * (sampleOldPoint.x - first.x)) / (-s2_x * s1_y + s1_x * s2_y);
            //
            //             if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
            //             {
            //                 leftSample.m_isCollision = true;
            //                 numCollided++;
            //                 break;
            //             }
            //         }
            //     }
            //
            //     for (auto& plane : m_collisionPlanes)
            //     {
            //         float distanceOld = (sampleOldPoint - plane.m_point).Dot(plane.m_normal);
            //         float distanceNew = (sampleNewPoint - plane.m_point).Dot(plane.m_normal);
            //         if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
            //         {
            //             // Collision with plane happened
            //             leftSample.m_isCollision = true;
            //             numCollided++;
            //             break;
            //         }
            //     }
            //     particleSamples.push_back(leftSample);
            // // }
            //
            //
            //
            // // Handle right sample
            // sampleOldPoint = rightSample.m_oldPosition;
            // sampleNewPoint = rightSample.m_newPosition;
            //
            // // Do collisions
            // for (auto& lineSegment : m_collisionSegments)
            // {
            //     Vector3 first = lineSegment.m_first;
            //     Vector3 second = lineSegment.m_second;
            //
            //     Vector3 lineDir = second - first;
            //     lineDir = lineDir.Normalized();
            //     Vector3 lineDirNorm = Vector3(-1.0f * lineDir.y, lineDir.x, 0.0f);
            //
            //     float distanceOld = (sampleOldPoint - first).Dot(lineDirNorm);
            //     float distanceNew = (sampleNewPoint - first).Dot(lineDirNorm);
            //
            //     if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
            //     {
            //         float s1_x, s1_y, s2_x, s2_y;
            //         s1_x = sampleNewPoint.x - sampleOldPoint.x;
            //         s1_y = sampleNewPoint.y - sampleOldPoint.y;
            //
            //         s2_x = second.x - first.x;
            //         s2_y = second.y - first.y;
            //
            //         float s, t;
            //         s = (-s1_y * (sampleOldPoint.x - first.x) + s1_x * (sampleOldPoint.y - first.y)) / (-s2_x * s1_y + s1_x * s2_y);
            //         t = ( s2_x * (sampleOldPoint.y - first.y) - s2_y * (sampleOldPoint.x - first.x)) / (-s2_x * s1_y + s1_x * s2_y);
            //
            //         if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
            //         {
            //             rightSample.m_isCollision = true;
            //             numCollided++;
            //             break;
            //         }
            //     }
            // }
            //
            // for (auto& plane : m_collisionPlanes)
            // {
            //     float distanceOld = (sampleOldPoint - plane.m_point).Dot(plane.m_normal);
            //     float distanceNew = (sampleNewPoint - plane.m_point).Dot(plane.m_normal);
            //     if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
            //     {
            //         // Collision with plane happened
            //         rightSample.m_isCollision = true;
            //         numCollided++;
            //         break;
            //     }
            // }
            //
            // particleSamples.push_back(rightSample);
            //
            // if (numCollided > 0)
            //     cout << "Num collided" << numCollided << " for " << i << endl;

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

            if (!collision)
            {
                vector<Sample> particleSamples;

                // float startAngle = -1.0f * m_waveParticles[i].m_sampleAngle / 2.0f;
                // float sampleAngleStep = m_waveParticles[i].m_sampleAngle / sampleDensity;

                int numCollided = 0;
                // for (int j = 0; j < sampleDensity; j++)
                // {
                    Sample leftSample;
                    Sample rightSample;

                    Vector3 leftCollisionNormal;
                    Vector3 rightCollisionNormal;

                    float leftCollisionAngle;
                    float rightCollisionAngle;

                    float angle = m_waveParticles[i].m_dispersionAngle;

                    Vector3 direction = m_waveParticles[i].m_direction;
                    direction = direction.Normalized();

                    Vector3 leftSampleDirection;
                    leftSampleDirection.x = direction.x * cos(-angle) - direction.y * sin(-angle);
                    leftSampleDirection.y = direction.x * sin(-angle) + direction.y * cos(-angle);
                    leftSampleDirection = leftSampleDirection.Normalized();

                    Vector3 rightSampleDirection;
                    rightSampleDirection.x = direction.x * cos(angle) - direction.y * sin(angle);
                    rightSampleDirection.y = direction.x * sin(angle) + direction.y * cos(angle);
                    rightSampleDirection = leftSampleDirection.Normalized();


                    leftSample.m_isCollision = false;
                    rightSample.m_isCollision = false;

                    // This needs to change as we are representing wave particles instead of samples
                    // Parallel wave
                    if (angle == 0.0f)
                    {
                        leftSample.m_oldPosition = oldPoint - Vector3(-direction.y, direction.x, 0.0f) * 10.0f;
                        leftSample.m_newPosition = newPoint - Vector3(-direction.y, direction.x, 0.0f) * 10.0f;

                        rightSample.m_oldPosition = oldPoint + Vector3(-direction.y, direction.x, 0.0f) * 10.0f;
                        rightSample.m_newPosition = newPoint + Vector3(-direction.y, direction.x, 0.0f) * 10.0f;
                    }
                    // Non parallel wave
                    else
                    {
                    }

                    Vector3 sampleOldPoint = leftSample.m_oldPosition;
                    Vector3 sampleNewPoint = leftSample.m_newPosition;

                    // Do collisions
                    for (auto& lineSegment : m_collisionSegments)
                    {
                        Vector3 first = lineSegment.m_first;
                        Vector3 second = lineSegment.m_second;

                        Vector3 lineDir = second - first;
                        lineDir = lineDir.Normalized();
                        Vector3 lineDirNorm = Vector3(-1.0f * lineDir.y, lineDir.x, 0.0f);

                        float distanceOld = (sampleOldPoint - first).Dot(lineDirNorm);
                        float distanceNew = (sampleNewPoint - first).Dot(lineDirNorm);

                        if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
                        {
                            float s1_x, s1_y, s2_x, s2_y;
                            s1_x = sampleNewPoint.x - sampleOldPoint.x;
                            s1_y = sampleNewPoint.y - sampleOldPoint.y;

                            s2_x = second.x - first.x;
                            s2_y = second.y - first.y;

                            float s, t;
                            s = (-s1_y * (sampleOldPoint.x - first.x) + s1_x * (sampleOldPoint.y - first.y)) / (-s2_x * s1_y + s1_x * s2_y);
                            t = ( s2_x * (sampleOldPoint.y - first.y) - s2_y * (sampleOldPoint.x - first.x)) / (-s2_x * s1_y + s1_x * s2_y);

                            if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
                            {
                                leftSample.m_isCollision = true;
                                leftCollisionNormal =  direction.Normalized() - lineDir.Normalized();
                                leftCollisionAngle = acos(direction.Normalized().Dot(lineDir.Normalized()));
                                numCollided++;
                                break;
                            }
                        }
                    }

                    for (auto& plane : m_collisionPlanes)
                    {
                        float distanceOld = (sampleOldPoint - plane.m_point).Dot(plane.m_normal);
                        float distanceNew = (sampleNewPoint - plane.m_point).Dot(plane.m_normal);
                        if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
                        {
                            // Collision with plane happened
                            leftSample.m_isCollision = true;
                            numCollided++;
                            break;
                        }
                    }
                    particleSamples.push_back(leftSample);
                // }



                // Handle right sample
                sampleOldPoint = rightSample.m_oldPosition;
                sampleNewPoint = rightSample.m_newPosition;

                // Do collisions
                for (auto& lineSegment : m_collisionSegments)
                {
                    Vector3 first = lineSegment.m_first;
                    Vector3 second = lineSegment.m_second;

                    Vector3 lineDir = second - first;
                    lineDir = lineDir.Normalized();
                    Vector3 lineDirNorm = Vector3(-1.0f * lineDir.y, lineDir.x, 0.0f);

                    float distanceOld = (sampleOldPoint - first).Dot(lineDirNorm);
                    float distanceNew = (sampleNewPoint - first).Dot(lineDirNorm);

                    if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
                    {
                        float s1_x, s1_y, s2_x, s2_y;
                        s1_x = sampleNewPoint.x - sampleOldPoint.x;
                        s1_y = sampleNewPoint.y - sampleOldPoint.y;

                        s2_x = second.x - first.x;
                        s2_y = second.y - first.y;

                        float s, t;
                        s = (-s1_y * (sampleOldPoint.x - first.x) + s1_x * (sampleOldPoint.y - first.y)) / (-s2_x * s1_y + s1_x * s2_y);
                        t = ( s2_x * (sampleOldPoint.y - first.y) - s2_y * (sampleOldPoint.x - first.x)) / (-s2_x * s1_y + s1_x * s2_y);

                        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
                        {
                            rightSample.m_isCollision = true;
                            rightCollisionNormal =  direction.Normalized() - lineDir.Normalized();
                            rightCollisionAngle = acos(direction.Normalized().Dot(lineDir.Normalized()));
                            numCollided++;
                            break;
                        }
                    }
                }

                for (auto& plane : m_collisionPlanes)
                {
                    float distanceOld = (sampleOldPoint - plane.m_point).Dot(plane.m_normal);
                    float distanceNew = (sampleNewPoint - plane.m_point).Dot(plane.m_normal);
                    if ((distanceNew >= 0.0f && distanceOld < 0.0f) || (distanceNew <= 0.0f && distanceOld > 0.0f))
                    {
                        // Collision with plane happened
                        rightSample.m_isCollision = true;
                        numCollided++;
                        break;
                    }
                }

                particleSamples.push_back(rightSample);

                if (numCollided > 0)
                    cout << "Num collided" << numCollided << " for " << i << endl;


                // If we have a left collision, handle that.
                if (leftSample.m_isCollision)
                {
                    cout << "Handling left collision" << endl;

                    // Vector3 newParticleDirection{1.0f, 1.0f, 0.0f};
                    // newParticleDirection = newParticleDirection.Normalized();
                    Vector3 newParticleDirection = leftCollisionNormal.Normalized();
                    newParticleDirection *= speed;
                    WaveParticle particleLeft = WaveParticle(leftSample.m_newPosition,
                        newParticleDirection, g_TimerGame.TotalTime());
                    particleLeft.m_dispersionAngle = leftCollisionAngle;
                    particleLeft.m_birthPosition = leftSample.m_newPosition;

                    AddParticle(particleLeft);
                }

                // If we have a right one, handle that
                if (rightSample.m_isCollision)
                {
                    cout << "Handling right collision" << endl;

                    // Vector3 newParticleDirection{-1.0f, 1.0f, 0.0f};
                    // newParticleDirection = newParticleDirection.Normalized();
                    Vector3 newParticleDirection = rightCollisionNormal.Normalized();
                    newParticleDirection *= speed;
                    WaveParticle particleRight = WaveParticle(rightSample.m_newPosition,
                        newParticleDirection, g_TimerGame.TotalTime());
                    particleRight.m_dispersionAngle = rightCollisionAngle;
                    particleRight.m_birthPosition = rightSample.m_newPosition;

                    AddParticle(particleRight);
                }

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
                    // cout << "AngleOwned: " << angleOwned << endl;
                    // cout << "i" << i << endl;
                    // cout << "Performing Subdivide" << endl;
                    // cout << "Subdivide" << endl;
                    numSubdividing++;

                    m_waveParticles[i].m_active = false;

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
                    // WaveParticle particleSame = m_waveParticles[i];

                    particleLeft.m_currentPosition = particleLeft.m_birthPosition + particleLeft.m_direction * timeMoved;
                    particleRight.m_currentPosition = particleRight.m_birthPosition + particleRight.m_direction * timeMoved;
                    particleSame.m_currentPosition = particleSame.m_birthPosition + particleSame.m_direction * timeMoved;

                    // cout << "Left Current Position: " <<  particleLeft.m_currentPosition << endl;
                    // cout << "Right Current Position: " << particleRight.m_currentPosition << endl;

                    // particleSame.m_amplitude = 1.0f;

                    particleLeft.m_dispersionAngle = origionalAngle/3.0f;
                    particleRight.m_dispersionAngle = origionalAngle/3.0f;
                    particleSame.m_dispersionAngle = origionalAngle/3.0f;

                    m_newParticles.push_back(particleLeft);
                    m_newParticles.push_back(particleRight);
                    m_newParticles.push_back(particleSame);

                    // cout << "Generated particles"
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
    }

    void ParticleSystem::KillParticles()
    {
        int numBefore = m_numActualParticles;
        for (int i = 0; i < m_maxParticles; i++)
        {
            if (m_waveParticles[i].m_active && m_waveParticles[i].m_amplitude <= 0.05f)
            {
                // cout << "Killing particle" << endl;
                m_waveParticles[i].m_active = false;

                // Shift all particles over
                for (int j = i; j < (m_maxParticles-1); j++)
                {
                    m_waveParticles[j] = m_waveParticles[j+1];
                    m_waveParticles[j+1].m_active = false;
                }
                m_numActualParticles--;
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
