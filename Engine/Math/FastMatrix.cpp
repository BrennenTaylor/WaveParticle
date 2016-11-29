#include "FastMatrix.h"

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

#include <utility>

#define _USE_MATH_DEFINES
// #define M_PI 3.14159265f
#include <Math.h>

namespace Farlor
{
    FastMatrix::FastMatrix()
    {
#ifdef FARLOR_USE_SIMD
        m_fast0 = _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f);
        m_fast1 = _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f);
        m_fast2 = _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f);
        m_fast3 = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
#else
        m_data[0] = 0.0f;
        m_data[1] = 0.0f;
        m_data[2] = 0.0f;
        m_data[3] = 0.0f;
        m_data[4] = 0.0f;
        m_data[5] = 0.0f;
        m_data[6] = 0.0f;
        m_data[7] = 0.0f;
        m_data[8] = 0.0f;
        m_data[9] = 0.0f;
        m_data[10] = 0.0f;
        m_data[11] = 0.0f;
        m_data[12] = 0.0f;
        m_data[13] = 0.0f;
        m_data[14] = 0.0f;
        m_data[15] = 0.0f;
#endif
    }

    FastMatrix::FastMatrix(FastVector& row1, FastVector& row2,
            FastVector& row3, FastVector& row4)
    {
        m_rows[0] = row1;
        m_rows[1] = row2;
        m_rows[2] = row3;
        m_rows[3] = row4;
    }

    FastMatrix::FastMatrix(
            float r1c1, float r1c2, float r1c3, float r1c4,
            float r2c1, float r2c2, float r2c3, float r2c4,
            float r3c1, float r3c2, float r3c3, float r3c4,
            float r4c1, float r4c2, float r4c3, float r4c4)
    {
#ifdef FARLOR_USE_SIMD
        m_fast0 = _mm_set_ps(r1c1, r1c2, r1c3, r1c4);
        m_fast1 = _mm_set_ps(r2c1, r2c2, r2c3, r2c4);
        m_fast2 = _mm_set_ps(r3c1, r3c2, r3c3, r3c4);
        m_fast3 = _mm_set_ps(r4c1, r4c2, r4c3, r4c4);
#else
        m_data[0] = r1c1;
        m_data[1] = r1c2;
        m_data[2] = r1c3;
        m_data[3] = r1c4;
        m_data[4] = r2c1;
        m_data[5] = r2c2;
        m_data[6] = r2c3;
        m_data[7] = r2c4;
        m_data[8] = r3c1;
        m_data[9] = r3c2;
        m_data[10] = r3c3;
        m_data[11] = r3c4;
        m_data[12] = r4c1;
        m_data[13] = r4c2;
        m_data[14] = r4c3;
        m_data[15] = r4c4;
#endif
    }

    FastMatrix::FastMatrix(Matrix2x2& matrix)
    {
#ifdef FARLOR_USE_SIMD
        m_fast0 = _mm_set_ps(matrix.m_r1c1, matrix.m_r1c2, 0.0f, 0.0f);
        m_fast1 = _mm_set_ps(matrix.m_r2c1, matrix.m_r2c2, 0.0f, 0.0f);
        m_fast2 = _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f);
        m_fast3 = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
#else
        m_data[0] = matrix.m_r1c1;
        m_data[1] = matrix.m_r1c2;
        m_data[2] = 0.0f;
        m_data[3] = 0.0f;
        m_data[4] = matrix.m_r2c1;
        m_data[5] = matrix.m_r2c2;
        m_data[6] = 0.0f;
        m_data[7] = 0.0f;
        m_data[8] = 0.0f;
        m_data[9] = 0.0f;
        m_data[10] = 0.0f;
        m_data[11] = 0.0f;
        m_data[12] = 0.0f;
        m_data[13] = 0.0f;
        m_data[14] = 0.0f;
        m_data[15] = 1.0f;
#endif
    }

    FastMatrix::FastMatrix(Matrix3x3& matrix)
    {
#ifdef FARLOR_USE_SIMD
        m_fast0 = _mm_set_ps(matrix.m_r1c1, matrix.m_r1c2, matrix.m_r1c3, 0.0f);
        m_fast1 = _mm_set_ps(matrix.m_r2c1, matrix.m_r2c2, matrix.m_r2c3, 0.0f);
        m_fast2 = _mm_set_ps(matrix.m_r3c1, matrix.m_r3c2, matrix.m_r3c3, 0.0f);
        m_fast3 = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
#else
        m_data[0] = matrix.m_r1c1;
        m_data[1] = matrix.m_r1c2;
        m_data[2] = matrix.m_r1c3;
        m_data[3] = 0.0f;
        m_data[4] = matrix.m_r2c1;
        m_data[5] = matrix.m_r2c2;
        m_data[6] = matrix.m_r2c3;
        m_data[7] = 0.0f;
        m_data[8] = matrix.m_r3c1;
        m_data[9] = matrix.m_r3c2;
        m_data[10] = matrix.m_r3c3;
        m_data[11] = 0.0f;
        m_data[12] = 0.0f;
        m_data[13] = 0.0f;
        m_data[14] = 0.0f;
        m_data[15] = 1.0f;
#endif
    }

    FastMatrix::FastMatrix(Matrix4x4& matrix)
    {
#ifdef FARLOR_USE_SIMD
        m_fast0 = _mm_set_ps(matrix.m_r1c1, matrix.m_r1c2, matrix.m_r1c3, matrix.m_r1c4);
        m_fast1 = _mm_set_ps(matrix.m_r2c1, matrix.m_r2c2, matrix.m_r2c3, matrix.m_r2c4);
        m_fast2 = _mm_set_ps(matrix.m_r3c1, matrix.m_r3c2, matrix.m_r3c3, matrix.m_r3c4);
        m_fast3 = _mm_set_ps(matrix.m_r4c1, matrix.m_r4c2, matrix.m_r4c3, matrix.m_r4c4);
#else
        m_data[0] = matrix.m_r1c1;
        m_data[1] = matrix.m_r1c2;
        m_data[2] = matrix.m_r1c3;
        m_data[3] = matrix.m_r1c4;
        m_data[4] = matrix.m_r2c1;
        m_data[5] = matrix.m_r2c2;
        m_data[6] = matrix.m_r2c3;
        m_data[7] = matrix.m_r2c4;
        m_data[8] = matrix.m_r3c1;
        m_data[9] = matrix.m_r3c2;
        m_data[10] = matrix.m_r3c3;
        m_data[11] = matrix.m_r3c4;
        m_data[12] = matrix.m_r4c1;
        m_data[13] = matrix.m_r4c2;
        m_data[14] = matrix.m_r4c3;
        m_data[15] = matrix.m_r4c4;
#endif
    }

    FastMatrix& FastMatrix::operator+=(const FastMatrix &rhs)
    {
#ifdef FARLOR_USE_SIMD
        m_fast0 = _mm_add_ps(m_fast0, rhs.m_fast0);
        m_fast1 = _mm_add_ps(m_fast1, rhs.m_fast1);
        m_fast2 = _mm_add_ps(m_fast2, rhs.m_fast2);
        m_fast3 = _mm_add_ps(m_fast3, rhs.m_fast3);
#else
        for(unsigned int i = 0; i < 16; i++)
        {
            m_data[i] = m_data[i] + rhs.m_data[i];
        }
#endif
        return (*this);
    }

    const FastMatrix FastMatrix::operator+(const FastMatrix &mat)
    {
        return FastMatrix(*this) += mat;
    }

    // Subtract
    FastMatrix& FastMatrix::operator-=(const FastMatrix &rhs)
    {
#ifdef FARLOR_USE_SIMD
        m_fast0 = _mm_sub_ps(m_fast0, rhs.m_fast0);
        m_fast1 = _mm_sub_ps(m_fast1, rhs.m_fast1);
        m_fast2 = _mm_sub_ps(m_fast2, rhs.m_fast2);
        m_fast3 = _mm_sub_ps(m_fast3, rhs.m_fast3);
#else
        for(unsigned int i = 0; i < 16; i++)
        {
            m_data[i] = m_data[i] - rhs.m_data[i];
        }
#endif
        return (*this);
    }

    const FastMatrix FastMatrix::operator-(const FastMatrix &mat)
    {
        return FastMatrix(*this) -= mat;
    }

    // Multiply
    FastMatrix& FastMatrix::operator*=(const FastMatrix &rhs)
    {
#ifdef FARLOR_USE_SIMD

        const __m128 a = rhs.m_fast0;
        const __m128 b = rhs.m_fast1;
        const __m128 c = rhs.m_fast2;
        const __m128 d = rhs.m_fast3;

        __m128 t1, t2;

        t1 = _mm_set1_ps(m_r1c1);
        t2 = _mm_mul_ps(a, t1);
        t1 =_mm_set1_ps(m_r1c2);
        t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
        t1 =_mm_set1_ps(m_r1c3);
        t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
        t1 =_mm_set1_ps(m_r1c4);
        t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

        _mm_store_ps(&m_data[0], t2);

        t1 = _mm_set1_ps(m_r2c1);
        t2 = _mm_mul_ps(a, t1);
        t1 =_mm_set1_ps(m_r2c2);
        t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
        t1 =_mm_set1_ps(m_r2c3);
        t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
        t1 =_mm_set1_ps(m_r2c4);
        t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

        _mm_store_ps(&m_data[4], t2);

        t1 = _mm_set1_ps(m_r3c1);
        t2 = _mm_mul_ps(a, t1);
        t1 =_mm_set1_ps(m_r3c2);
        t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
        t1 =_mm_set1_ps(m_r3c3);
        t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
        t1 =_mm_set1_ps(m_r3c4);
        t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

        _mm_store_ps(&m_data[8], t2);

        t1 = _mm_set1_ps(m_r4c1);
        t2 = _mm_mul_ps(a, t1);
        t1 =_mm_set1_ps(m_r4c2);
        t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
        t1 =_mm_set1_ps(m_r4c3);
        t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
        t1 =_mm_set1_ps(m_r4c4);
        t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

        _mm_store_ps(&m_data[12], t2);

#else

        float i11, i12, i13, i14;
        float i21, i22, i23, i24;
        float i31, i32, i33, i34;
        float i41, i42, i43, i44;

        i11 = m_r1c1 * rhs.m_r1c1 + m_r1c2 * rhs.m_r2c1 + m_r1c3 * rhs.m_r3c1 + m_r1c4 * rhs.m_r4c1;
        i12 = m_r1c1 * rhs.m_r1c2 + m_r1c2 * rhs.m_r2c2 + m_r1c3 * rhs.m_r3c2 + m_r1c4 * rhs.m_r4c2;
        i13 = m_r1c1 * rhs.m_r1c3 + m_r1c2 * rhs.m_r2c3 + m_r1c3 * rhs.m_r3c3 + m_r1c4 * rhs.m_r4c3;
        i14 = m_r1c1 * rhs.m_r1c4 + m_r1c2 * rhs.m_r2c4 + m_r1c3 * rhs.m_r3c4 + m_r1c4 * rhs.m_r4c4;
        i21 = m_r2c1 * rhs.m_r1c1 + m_r2c2 * rhs.m_r2c1 + m_r2c3 * rhs.m_r3c1 + m_r2c4 * rhs.m_r4c1;
        i22 = m_r2c1 * rhs.m_r1c2 + m_r2c2 * rhs.m_r2c2 + m_r2c3 * rhs.m_r3c2 + m_r2c4 * rhs.m_r4c2;
        i23 = m_r2c1 * rhs.m_r1c3 + m_r2c2 * rhs.m_r2c3 + m_r2c3 * rhs.m_r3c3 + m_r2c4 * rhs.m_r4c3;
        i24 = m_r2c1 * rhs.m_r1c4 + m_r2c2 * rhs.m_r2c4 + m_r2c3 * rhs.m_r3c4 + m_r2c4 * rhs.m_r4c4;
        i31 = m_r3c1 * rhs.m_r1c1 + m_r3c2 * rhs.m_r2c1 + m_r3c3 * rhs.m_r3c1 + m_r3c4 * rhs.m_r4c1;
        i32 = m_r3c1 * rhs.m_r1c2 + m_r3c2 * rhs.m_r2c2 + m_r3c3 * rhs.m_r3c2 + m_r3c4 * rhs.m_r4c2;
        i33 = m_r3c1 * rhs.m_r1c3 + m_r3c2 * rhs.m_r2c3 + m_r3c3 * rhs.m_r3c3 + m_r3c4 * rhs.m_r4c3;
        i34 = m_r3c1 * rhs.m_r1c4 + m_r3c2 * rhs.m_r2c4 + m_r3c3 * rhs.m_r3c4 + m_r3c4 * rhs.m_r4c4;
        i41 = m_r4c1 * rhs.m_r1c1 + m_r4c2 * rhs.m_r2c1 + m_r4c3 * rhs.m_r3c1 + m_r4c4 * rhs.m_r4c1;
        i42 = m_r4c1 * rhs.m_r1c2 + m_r4c2 * rhs.m_r2c2 + m_r4c3 * rhs.m_r3c2 + m_r4c4 * rhs.m_r4c2;
        i43 = m_r4c1 * rhs.m_r1c3 + m_r4c2 * rhs.m_r2c3 + m_r4c3 * rhs.m_r3c3 + m_r4c4 * rhs.m_r4c3;
        i44 = m_r4c1 * rhs.m_r1c4 + m_r4c2 * rhs.m_r2c4 + m_r4c3 * rhs.m_r3c4 + m_r4c4 * rhs.m_r4c4;

        m_r1c1 = i11; m_r1c2 = i12; m_r1c3 = i13; m_r1c4 = i14;
        m_r2c1 = i21; m_r2c2 = i22; m_r2c3 = i23; m_r2c4 = i24;
        m_r3c1 = i31; m_r3c2 = i32; m_r3c3 = i33; m_r3c4 = i34;
        m_r4c1 = i41; m_r4c2 = i42; m_r4c3 = i43; m_r4c4 = i44;
#endif
        return *this;
    }

    FastMatrix FastMatrix::operator*(const FastMatrix &mat)
    {
        return FastMatrix(*this) *= mat;
    }

    void FastMatrix::Transpose()
    {
#ifdef FARLOR_USE_SIMD
#else
        std::swap(m_data[1], m_data[4]);
        std::swap(m_data[2], m_data[8]);
        std::swap(m_data[3], m_data[12]);
        std::swap(m_data[6], m_data[9]);
        std::swap(m_data[7], m_data[13]);
        std::swap(m_data[11], m_data[14]);
#endif
    }

    FastMatrix FastMatrix::Transposed()
    {
        FastMatrix result(*this);
        result.Transpose();
        return result;
    }

    FastMatrix FastMatrix::LookAtLH(FastVector& eye,
        FastVector& focus, FastVector& up)
    {
        // Z-axis
        FastVector eyeDir = focus - eye;
        FastVector zAxis = eyeDir.Normalized();

        FastVector upNormalized = up.Normalized();
        FastVector xAxis = upNormalized.Cross(zAxis);
        FastVector xAxisNorm = xAxis.Normalized();

        FastVector yAxis = zAxis.Cross(xAxis);

        FastVector negativeEye = eye.Negated();

        float xDotEye = xAxis.Dot(negativeEye);
        float yDotEye = yAxis.Dot(negativeEye);
        float zDotEye = zAxis.Dot(negativeEye);

        FastMatrix result
        (
            xAxis.m_data[0], yAxis.m_data[1], zAxis.m_data[2], 0.0f,
            xAxis.m_data[0], yAxis.m_data[1], zAxis.m_data[2], 0.0f,
            xAxis.m_data[0], yAxis.m_data[1], zAxis.m_data[2], 0.0f,
            xDotEye, yDotEye, zDotEye, 1.0f
        );

        return result;
    }

    FastMatrix FastMatrix::Identity()
    {
        FastMatrix result
        (
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        return result;
    }

    FastMatrix FastMatrix::PerspectiveFovLH(float fov, float aspect,
        float near, float far)
    {
        float degreesToRadians = (float)M_PI / 180.0f;
        float yScale = 1.0f / tan(degreesToRadians * fov / 2.0f);
        float xScale = (yScale / aspect);
        float nearMinusFar = near-far;

        FastMatrix result
        {
            xScale, 0.0f, 0.0f, 0.0f,
            0.0f, yScale, 0.0f, 0.0f,
            0.0f, 0.0f, ((far + near) / nearMinusFar), -1.0f,
            0.0f, 0.0f, (2.0f * far * near / nearMinusFar), 0.0f
        };

        return result;
    }

}
