#pragma once

#include <iostream>

using namespace std;

namespace Farlor
{
    class Vector2
    {
    public:
        union
        {
            float m_data[2];
            struct
            {
                float x;
                float y;
            };
        };

        Vector2();
        Vector2(float value);
        Vector2(float xNew, float yNew);

        // Vector2 & Vector2
        // Add
        Vector2& Vector2::operator+=(const Vector2 &rhs);
        const Vector2 operator+(const Vector2 &vec);
        // Subtract
        Vector2& Vector2::operator-=(const Vector2 &rhs);
        const Vector2 operator-(const Vector2 &vec);
        // Multiply
        Vector2& Vector2::operator*=(const Vector2 &rhs);
        const Vector2 operator*(const Vector2 &vec);
        // Divide
        Vector2& Vector2::operator/=(const Vector2 &rhs);
        const Vector2 operator/(const Vector2 &vec);
        // Use % as dot product
        Vector2& Vector2::operator%=(const Vector2 &rhs) = delete;
        const float operator%(const Vector2 &vec);
        // Equality
        const bool operator==(const Vector2 &other);
        const bool operator!=(const Vector2 &other);

        // Vector & T
        // Multiply
        Vector2& operator*=(const float &rhs);
        const Vector2 operator*(const float &rhs);
        // Divide
        Vector2& operator/=(const float &rhs);
        const Vector2 operator/(const float &rhs);

        // Standalone
        friend const Vector2 operator*(float lhs, const Vector2& rhs);

        // Out Streaming
        friend ostream& operator<<(ostream& os, const Vector2& vec);

        // Local Math Operations
        float Magnitude();
        float SqrMagnitude();
        Vector2 Normalized();
        void Normalize();

        float Dot(const Vector2& other) const;

        // Static Math Operations
        static float Magnitude(Vector2& vec);
        static float SqrMagnitude(Vector2& vec);
        static Vector2 Normalized(Vector2& vec);
        static void Normalize(Vector2& vec);

        static float Dot(const Vector2& first, const Vector2& second);
    };
}

namespace std
{
    template<> struct hash<Farlor::Vector2>
    {
        size_t operator()(Farlor::Vector2 const& vector) const
        {
            return ((hash<float>()(vector.x) ^ (hash<float>()(vector.y) << 1)) >> 1);
        }
    };
}
