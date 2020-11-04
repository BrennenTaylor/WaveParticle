#pragma once

#include <iostream>

namespace Farlor
{
    class Vector3
    {
    public:
        union
        {
            float m_data[3];
            struct
            {
                float x;
                float y;
                float z;
            };
        };

        Vector3();
        Vector3(float value);
        Vector3(float xNew, float yNew, float zNew);

        // Vector3 & Vector3
        // Add
        Vector3& Vector3::operator+=(const Vector3 &rhs);
        const Vector3 operator+(const Vector3 &vec);
        // Subtract
        Vector3& Vector3::operator-=(const Vector3 &rhs);
        const Vector3 operator-(const Vector3 &vec);
        // Multiply
        Vector3& Vector3::operator*=(const Vector3 &rhs);
        const Vector3 operator*(const Vector3 &vec);
        // Divide
        Vector3& Vector3::operator/=(const Vector3 &rhs);
        const Vector3 operator/(const Vector3 &vec);
        // Use % as dot product
        Vector3& Vector3::operator%=(const Vector3 &rhs) = delete;
        const float operator%(const Vector3 &vec);
        // Equality
        const bool operator==(const Vector3 &other);
        const bool operator!=(const Vector3 &other);

        // Vector & T
        // Multiply
        Vector3& operator*=(const float &rhs);
        const Vector3 operator*(const float &rhs);
        // Divide
        Vector3& operator/=(const float &rhs);
        const Vector3 operator/(const float &rhs);

        // Standalone
        friend const Vector3 operator*(float lhs, const Vector3& rhs);

        // Out Streaming
        friend std::ostream& operator<<(std::ostream& os, Vector3& vec);

        // Local Math Operations
        float Magnitude();
        float SqrMagnitude();
        Vector3 Normalized();
        void Normalize();

        float Dot(const Vector3& other) const;
        Vector3 Cross(const Vector3& other) const;

        // Static Math Operations
        static float Magnitude(Vector3& vec);
        static float SqrMagnitude(Vector3& vec);
        static Vector3 Normalized(Vector3& vec);
        static void Normalize(Vector3& vec);

        static float Dot(const Vector3& first, const Vector3& second);
        static Vector3 Cross(const Vector3& first, const Vector3& second);

        static float Distance(const Vector3& start, const Vector3& end);
        static float DistanceSquared(const Vector3& start, const Vector3& end);

        static void MakeOrthonormalBasisRHS(Vector3& a, Vector3& b, Vector3& c);
    };

    using Point = Vector3;
}

namespace std
{
    template<> struct hash<Farlor::Vector3>
    {
        size_t operator()(Farlor::Vector3 const& vector) const
        {
            return ((hash<float>()(vector.x) ^ (hash<float>()(vector.y) << 1)) >> 1) ^ (hash<float>()(vector.z) << 1);
        }
    };
}
