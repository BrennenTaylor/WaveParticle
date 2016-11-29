#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace Farlor
{
    class Camera
    {
    public:
        Camera(bool isMovable);
        void Update(float dt);

        XMVECTOR m_defaultForward;
        XMVECTOR m_defaultRight;
        XMVECTOR m_camForward;
        XMVECTOR m_camRight;

        XMMATRIX m_camRotationMatrix;

        float m_moveLeftRight;
        float m_moveBackForward;
        float m_moveUpDown;

        float m_camYaw;
        float m_camPitch;

        XMMATRIX m_camView;
        XMVECTOR m_camPosition;
        XMVECTOR m_camTarget;
        XMVECTOR m_camUp;

        float m_speed;
        float m_rotateSpeed;

        bool m_IsMovable;
    };
}
