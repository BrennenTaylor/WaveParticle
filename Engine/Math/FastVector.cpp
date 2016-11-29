#pragma once

#include "FastVector.h"

#include <cmath>

namespace Farlor
{
    FastVector::FastVector()
    {
    }

    FastVector::FastVector(float value)
    {
#ifdef FARLOR_USE_SIMD
        m_simdData = _mm_set_ps(value, value, value, value);
#else
        m_data[0] = value;
        m_data[1] = value;
        m_data[2] = value;
        m_data[3] = value;
#endif
    }

    FastVector::FastVector(float x, float y, float z, float w)
    {
#ifdef FARLOR_USE_SIMD
        m_simdData = _mm_set_ps(x, y, z, w);
#else
        m_data[0] = x;
        m_data[1] = y;
        m_data[2] = z;
        m_data[3] = w;
#endif
    }

    FastVector::FastVector(Vector2& vector)
    {
#ifdef FARLOR_USE_SIMD
        m_simdData = _mm_set_ps(vector.x, vector.y, 0.0f, 0.0f);
#else
        m_data[0] = vector.x;
        m_data[1] = vector.y;
        m_data[2] = 0.0f;
        m_data[3] = 0.0f;
#endif
    }

    FastVector::FastVector(Vector3& vector)
    {
#ifdef FARLOR_USE_SIMD
        m_simdData = _mm_set_ps(vector.x, vector.y, vector.z, 0.0f);
#else
        m_data[0] = vector.x;
        m_data[1] = vector.y;
        m_data[2] = vector.z;
        m_data[3] = 0.0f;
#endif
    }

    FastVector::FastVector(Vector4& vector)
    {
#ifdef FARLOR_USE_SIMD
        m_simdData = _mm_set_ps(vector.x, vector.y, vector.z, vector.z);
#else
        m_data[0] = vector.x;
        m_data[1] = vector.y;
        m_data[2] = vector.z;
        m_data[3] = vector.z;
#endif
    }

    FastVector& FastVector::operator+=(const FastVector &rhs)
    {
#ifdef FARLOR_USE_SIMD
        m_simdData = _mm_add_ps(m_simdData, rhs.m_simdData);
#else
        m_data[0] += rhs.m_data[0];
        m_data[1] += rhs.m_data[1];
        m_data[2] += rhs.m_data[2];
        m_data[3] += rhs.m_data[3];
#endif
        return (*this);
    }

    const FastVector FastVector::operator+(const FastVector &vec)
    {
        return FastVector(*this) += vec;
    }
    // Subtract
    FastVector& FastVector::operator-=(const FastVector &rhs)
    {
#ifdef FARLOR_USE_SIMD
        m_simdData = _mm_sub_ps(m_simdData, rhs.m_simdData);
#else
        m_data[0] -= rhs.m_data[0];
        m_data[1] -= rhs.m_data[1];
        m_data[2] -= rhs.m_data[2];
        m_data[3] -= rhs.m_data[3];
#endif
        return (*this);
    }
    const FastVector FastVector::operator-(const FastVector &vec)
    {
        return FastVector(*this) -= vec;
    }
    // Multiply
    FastVector& FastVector::operator*=(const FastVector &rhs)
    {
#ifdef FARLOR_USE_SIMD
        m_simdData = _mm_mul_ps(m_simdData, rhs.m_simdData);
#else
        m_data[0] *= rhs.m_data[0];
        m_data[1] *= rhs.m_data[1];
        m_data[2] *= rhs.m_data[2];
        m_data[3] *= rhs.m_data[3];
#endif
        return (*this);
    }
    const FastVector FastVector::operator*(const FastVector &vec)
    {
        return FastVector(*this) *= vec;
    }
    // Divide
    FastVector& FastVector::operator/=(const FastVector &rhs)
    {
#ifdef FARLOR_USE_SIMD
        m_simdData = _mm_div_ps(m_simdData, rhs.m_simdData);
#else
        m_data[0] /= rhs.m_data[0];
        m_data[1] /= rhs.m_data[1];
        m_data[2] /= rhs.m_data[2];
        m_data[3] /= rhs.m_data[3];
#endif
        return (*this);
    }
    const FastVector FastVector::operator/(const FastVector &vec)
    {
        return FastVector(*this) /= vec;
    }
    // Use % as dot product
    const FastFloat FastVector::operator%(const FastVector &vec)
    {
        return this->Dot(vec);
    }
    // Equality

    // Vector & T
    // Multiply
    FastVector& FastVector::operator*=(const float rhs)
    {
        FastVector scaler(rhs);
        (*this) *= scaler;
        return (*this);
    }

    const FastVector FastVector::operator*(const float rhs)
    {
        return FastVector(*this) *= rhs;
    }
    // Divide
    FastVector& FastVector::operator/=(const float rhs)
    {
        FastVector scaler(rhs);
        (*this) /= scaler;
        return (*this);
    }
    const FastVector FastVector::operator/(const float rhs)
    {
        return FastVector(*this) /= rhs;
    }

    // Standalone
    const FastVector operator*(float lhs, const FastVector& rhs)
    {
        FastVector result = rhs;
        result *= lhs;
        return result;
    }

    // Local Math Operations
    FastFloat FastVector::Magnitude()
    {
#ifdef FARLOR_USE_SIMD
#else
        return sqrt(
            m_data[0] * m_data[0] +
            m_data[1] * m_data[1] +
            m_data[2] * m_data[2] +
            m_data[3] * m_data[3]);
#endif
    }

    FastFloat FastVector::SqrMagnitude()
    {
        return m_data[0] * m_data[0] +
            m_data[1] * m_data[1] +
            m_data[2] * m_data[2] +
            m_data[3] * m_data[3];
    }

    FastVector FastVector::Normalized()
    {
        FastVector result(*this);
        result.Normalize();
        return std::move(result);
    }

    void FastVector::Normalize()
    {
        FastFloat length = Magnitude();
        m_data[0] /= length;
        m_data[1] /= length;
        m_data[2] /= length;
        m_data[3] /= length;
    }

    FastFloat FastVector::Dot(const FastVector& other) const
    {
        M_CHECK_ALIGNMENT(other, sizeof(FastFloat));
#ifdef FARLOR_USE_SIMD
        FastFloat result1 = _mm_mul_ps(m_simdData, other.m_simdData);
        FastFloat result2 = _mm_hadd_ps(result1, result1);
        FastFloat result3 = _mm_hadd_ps(result2, result2);
        return result3;
#else
        float total = m_data[0] * other.m_data[0];
        total += m_data[1] * other.m_data[1];
        total += m_data[2] * other.m_data[2];
        total += m_data[3] * other.m_data[3];
        return total;
#endif
    }

    FastVector FastVector::Cross(FastVector& other)
    {
        FastVector result{
            m_data[1] * other.m_data[2] - m_data[2] * other.m_data[1],
            m_data[2] * other.m_data[0] - m_data[0] * other.m_data[2],
            m_data[0] * other.m_data[1] - m_data[1] * other.m_data[0],
            0.0f};
        return std::move(result);
    }

    FastVector FastVector::Negated()
    {
        FastVector result(*this);
#ifdef FARLOR_USE_SIMD
        result.m_simdData = _mm_sub_ps(_mm_set1_ps(0.0f), m_simdData);
#else
        result *= FastVector(-1.0);
#endif
        return std::move(result);
    }

    FastFloat FastVector::Dot(const FastVector& first, const FastVector& second)
    {
        M_CHECK_ALIGNMENT(first, sizeof(FastVector));
        M_CHECK_ALIGNMENT(second, sizeof(FastVector));
#ifdef FARLOR_USE_SIMD
        FastFloat result1 = _mm_mul_ps(first.m_simdData, second.m_simdData);
        FastFloat result2 = _mm_hadd_ps(result1, result1);
        FastFloat result3 = _mm_hadd_ps(result2, result2);
        return result3;
#else
        float total = first.m_data[0] * second.m_data[0];
        total += first.m_data[1] * second.m_data[1];
        total += first.m_data[2] * second.m_data[2];
        total += first.m_data[3] * second.m_data[3];
        return total;
#endif
    }
}
