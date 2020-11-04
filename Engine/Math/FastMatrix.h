#pragma once

#include "FastVector.h"

#include <xmmintrin.h>

namespace Farlor
{
    class Matrix2x2;
    class Matrix3x3;
    class Matrix4x4;

    // Only use this for operations. Switch back for storage
    __declspec(align(16)) class FastMatrix
    {
    public:

        union
        {
            struct
            {
                __m128 m_fast0, m_fast1, m_fast2, m_fast3;
            };
            struct
            {
                FastVector m_rows[4];
            };
            struct
            {
                float m_data[16];
            };
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

        FastMatrix();

        FastMatrix(Matrix2x2& matrix);
        FastMatrix(Matrix3x3& matrix);
        FastMatrix(Matrix4x4& matrix);

        FastMatrix(FastVector& row1, FastVector& row2,
            FastVector& row3, FastVector& row4);

        FastMatrix(
            float r1c1, float r1c2, float r1c3, float r1c4,
            float r2c1, float r2c2, float r2c3, float r2c4,
            float r3c1, float r3c2, float r3c3, float r3c4,
            float r4c1, float r4c2, float r4c3, float r4c4);

        FastMatrix& FastMatrix::operator+=(const FastMatrix &rhs);
        const FastMatrix operator+(const FastMatrix &mat);
        // Subtract
        FastMatrix& FastMatrix::operator-=(const FastMatrix &rhs);
        const FastMatrix operator-(const FastMatrix &mat);
        // Multiply
        FastMatrix& FastMatrix::operator*=(const FastMatrix &rhs);
        FastMatrix operator*(const FastMatrix &mat);

        // Vector & T
        // Multiply
        FastMatrix& operator*=(const float rhs);

        // Operation on matrix
        void Transpose();
        FastMatrix Transposed();

        // Standalone
        friend const FastMatrix operator*(float lhs, const FastMatrix& rhs);

        // Static support methods
        static FastMatrix LookAtLH(FastVector& eye,
            FastVector& focus, FastVector& up);
        static FastMatrix Identity();
        static FastMatrix PerspectiveFovLH(float fov, float aspect, float near, float far);
    };
}
