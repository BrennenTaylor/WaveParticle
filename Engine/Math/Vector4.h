#pragma once

#include <iostream>

namespace Farlor
{
    class Vector4
    {
    public:
        union
        {
            float m_data[4];
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };
        };

        Vector4();
        Vector4(float value);
        Vector4(float xNew, float yNew, float zNew, float wNew);

        // Vector4 & Vector4
        // Add
        Vector4& Vector4::operator+=(const Vector4 &rhs);
        const Vector4 operator+(const Vector4 &vec);
        // Subtract
        Vector4& Vector4::operator-=(const Vector4 &rhs);
        const Vector4 operator-(const Vector4 &vec);
        // Multiply
        Vector4& Vector4::operator*=(const Vector4 &rhs);
        const Vector4 operator*(const Vector4 &vec);
        // Divide
        Vector4& Vector4::operator/=(const Vector4 &rhs);
        const Vector4 operator/(const Vector4 &vec);
        // Use % as dot product
        Vector4& Vector4::operator%=(const Vector4 &rhs) = delete;
        const float operator%(const Vector4 &vec);
        // Equality
        const bool operator==(const Vector4 &other);
        const bool operator!=(const Vector4 &other);

        // Vector & T
        // Multiply
        Vector4& operator*=(const float &rhs);
        const Vector4 operator*(const float &rhs);
        // Divide
        Vector4& operator/=(const float &rhs);
        const Vector4 operator/(const float &rhs);

        // Standalone
        friend const Vector4 operator*(float lhs, const Vector4& rhs);

        // Out Streaming
        friend std::ostream& operator<<(std::ostream& os, Vector4& vec);

        // Local Math Operations
        float Magnitude();
        float SqrMagnitude();
        Vector4 Normalized();
        void Normalize();

        float Dot(const Vector4& other) const;

        // Static Math Operations
        static float Magnitude(Vector4& vec);
        static float SqrMagnitude(Vector4& vec);
        static Vector4 Normalized(Vector4& vec);
        static void Normalize(Vector4& vec);

        static float Dot(const Vector4& first, const Vector4& second);
    };
}
