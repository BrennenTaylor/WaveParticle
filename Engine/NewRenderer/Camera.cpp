#include "Camera.h"

#include "../Input/InputHandler.h"

namespace Farlor
{
    extern InputHandler g_InputHandler;

    Camera::Camera(bool isMovable)
    {
        m_IsMovable = isMovable;

        m_defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        m_defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        m_camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        m_camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        m_camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

        m_moveLeftRight = 0.0f;
        m_moveBackForward = 0.0f;
        m_moveUpDown = 0.0f;

        m_camYaw = 0.0f;
        m_camPitch = 0.0f;

        m_camPosition = XMVectorSet(20.0f, 20.0f, 20.0f, 0.0f);
        m_camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        m_speed = 20.0f;
        m_rotateSpeed = 1.1f;

        m_camView = XMMatrixIdentity();
    }

    void Camera::Update(float dt)
    {
        if (m_IsMovable)
        {
            float lx, ly, rx, ry;
            g_InputHandler.PollThumbstick(lx, ly, rx, ry);

            bool controller = false;
            if (lx < 0.0f || lx > 0.0f)
            {
                m_moveLeftRight = dt * m_speed * lx;
                controller = true;
            }
            if (ly < 0.0f || ly > 0.0f)
            {
                m_moveBackForward = dt * m_speed * ly;
                controller = true;
            }

            if (rx < 0.0f || rx > 0.0f)
            {
                m_camYaw +=  dt * m_rotateSpeed * rx;
                controller = true;
            }
            if (ry < 0.0f || ry > 0.0f)
            {
                m_camPitch += dt * m_rotateSpeed * ry * -1.0f;
                controller = true;
            }

            if (!controller)
            // Handle Input
            if (g_InputHandler.KeyPressed(Keys::A))
            {
                m_moveLeftRight -= dt * m_speed;
            }
            if (g_InputHandler.KeyPressed(Keys::W))
            {
                m_moveBackForward += dt * m_speed;
            }
            if (g_InputHandler.KeyPressed(Keys::D))
            {
                m_moveLeftRight += dt * m_speed;
            }
            if (g_InputHandler.KeyPressed(Keys::S))
            {
                m_moveBackForward -= dt * m_speed;
            }
            if (g_InputHandler.KeyPressed(Keys::Left_Shift))
            {
                m_moveUpDown += dt * m_speed;
            }
            if (g_InputHandler.KeyPressed(Keys::Left_Ctrl))
            {
                m_moveUpDown -= dt * m_speed;
            }
        }

        m_camRotationMatrix = XMMatrixRotationRollPitchYaw(m_camPitch, m_camYaw, 0);
        m_camTarget = XMVector3TransformCoord(m_defaultForward, m_camRotationMatrix);
        m_camTarget = XMVector3Normalize(m_camTarget);

        XMMATRIX RotateYTempMatrix;
        RotateYTempMatrix = XMMatrixRotationY(m_camYaw);

        m_camRight = XMVector3TransformCoord(m_defaultRight, RotateYTempMatrix);
        // m_camUp = XMVector3TransformCoord(m_camUp, RotateYTempMatrix);
        m_camForward = XMVector3TransformCoord(m_defaultForward, RotateYTempMatrix);

        m_camPosition += m_moveLeftRight*m_camRight;
        m_camPosition += m_moveBackForward*m_camForward;
        m_camPosition += m_moveUpDown*m_camUp;

        m_moveLeftRight = 0.0f;
        m_moveBackForward = 0.0f;
        m_moveUpDown = 0.0f;

        m_camTarget = m_camPosition + m_camTarget;

        m_camView = XMMatrixLookAtLH( m_camPosition, m_camTarget, m_camUp );
    }
}
