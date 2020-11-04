#pragma once

#include "Vector4.h"

#include "FastMatrix.h"

namespace Farlor
{
    class Matrix4x4
    {
    public:
        union
        {
            float m_data[16];
            Vector4 m_rows[4];
            struct
            {
                float m_r1c1;
                float m_r1c2;
                float m_r1c3;
                float m_r1c4;
                float m_r2c1;
                float m_r2c2;
                float m_r2c3;
                float m_r2c4;
                float m_r3c1;
                float m_r3c2;
                float m_r3c3;
                float m_r3c4;
                float m_r4c1;
                float m_r4c2;
                float m_r4c3;
                float m_r4c4;
            };
        };

        Matrix4x4();
        Matrix4x4(FastMatrix& fastMatrix);

        // Out Streaming
        friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat);
    };
}
