#pragma once

#ifdef FARLOR_USE_SIMD
#include <xmmintrin.h>
#endif

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <cassert>

#define M_CHECK_ALIGNMENT(lObject, luAlignment) \
{ \
    assert( (luAlignment) ); \
    assert( ( (luAlignment) & ( (luAlignment) - 1 ) ) == 0 ); \
    assert( (reinterpret_cast<intptr_t>( &(lObject) ) & ( (luAlignment) - 1 )) == 0 ); \
}

namespace Farlor
{
#ifdef FARLOR_USE_SIMD
    using FastFloat = __m128;
#else
    using FastFloat = float;
#endif

    // Only use this for operations. Switch back for storage
    __declspec(align(16)) class FastVector
    {
    public:
        union
        {
            FastFloat m_simdData;
            struct
            {
                float m_data[4];
            };
        };
        FastVector::FastVector();
        FastVector::FastVector(float value);
        FastVector::FastVector(float x, float y, float z, float w);
        FastVector::FastVector(Vector2& vector);
        FastVector::FastVector(Vector3& vector);
        FastVector::FastVector(Vector4& vector);

        FastVector& FastVector::operator+=(const FastVector &rhs);
        const FastVector operator+(const FastVector &vec);
        // Subtract
        FastVector& FastVector::operator-=(const FastVector &rhs);
        const FastVector operator-(const FastVector &vec);
        // Multiply
        FastVector& FastVector::operator*=(const FastVector &rhs);
        const FastVector operator*(const FastVector &vec);
        // Divide
        FastVector& FastVector::operator/=(const FastVector &rhs);
        const FastVector operator/(const FastVector &vec);
        // Use % as dot product
        FastVector& FastVector::operator%=(const FastVector &rhs) = delete;
        const FastFloat operator%(const FastVector &vec);

        // Vector & T
        // Multiply
        FastVector& operator*=(const float rhs);
        const FastVector operator*(const float rhs);
        // Divide
        FastVector& operator/=(const float rhs);
        const FastVector operator/(const float rhs);

        // Standalone
        friend const FastVector operator*(float lhs, const FastVector& rhs);

        // Out Streaming
        friend std::ostream& operator<<(std::ostream& os, FastVector& vec);

        // Local Math Operations
        FastFloat Magnitude();
        FastFloat SqrMagnitude();
        FastVector Normalized();
        void Normalize();

        FastVector Cross(FastVector& other);

        FastVector Negated();

        FastFloat Dot(const FastVector& other) const;

        static FastFloat Dot(const FastVector& first, const FastVector& second);
    };
}
