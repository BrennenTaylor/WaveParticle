#pragma once

#include <DirectXMath.h>

namespace Farlor
{
    class Camera
    {
    public:
        Camera(bool isMovable);
        void Update(float dt);

        DirectX::XMVECTOR m_defaultForward;
        DirectX::XMVECTOR m_defaultRight;
        DirectX::XMVECTOR m_camForward;
        DirectX::XMVECTOR m_camRight;

        DirectX::XMMATRIX m_camRotationMatrix;

        float m_moveLeftRight;
        float m_moveBackForward;
        float m_moveUpDown;

        float m_camYaw;
        float m_camPitch;

        DirectX::XMMATRIX m_camView;
        DirectX::XMVECTOR m_camPosition;
        DirectX::XMVECTOR m_camTarget;
        DirectX::XMVECTOR m_camUp;

        float m_speed;
        float m_rotateSpeed;

        bool m_IsMovable;
    };
}
