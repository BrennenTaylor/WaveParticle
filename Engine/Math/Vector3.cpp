#include "Vector3.h"
#include "../Farlor.h"

#include <cmath>

namespace Farlor
{
    Vector3::Vector3()
        : x {0}
        , y {0}
        , z {0}
    {
    }

    Vector3::Vector3(float value)
        : x {value}
        , y {value}
        , z {value}
    {
    }

    Vector3::Vector3(const float xNew, const float yNew, const float zNew)
        : x {xNew}
        , y {yNew}
        , z {zNew}
    {
    }

///////////////////////////////////////////////////////////////
////////////// Vector3 & Vector
///////////////////////////////////////////////////////////////
    Vector3& Vector3::operator+=(const Vector3 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    const Vector3 Vector3::operator+(const Vector3 &other)
    {
        return Vector3(*this) += other;
    }
///////////////////////////////////////////////////////////////
    Vector3& Vector3::operator-=(const Vector3 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    const Vector3 Vector3::operator-(const Vector3 &other)
    {
        return Vector3(*this) -= other;
    }
///////////////////////////////////////////////////////////////
    Vector3& Vector3::operator*=(const Vector3 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    const Vector3 Vector3::operator*(const Vector3 &other)
    {
        return Vector3(*this) *= other;
    }
///////////////////////////////////////////////////////////////
    Vector3& Vector3::operator/=(const Vector3 &rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    const Vector3 Vector3::operator/(const Vector3 &other)
    {
        return Vector3(*this) /= other;
    }
///////////////////////////////////////////////////////////////
    // Lets override the % to be the dot product
    const float Vector3::operator%(const Vector3 &other)
    {
        return (*this).Dot(other);
    }
///////////////////////////////////////////////////////////////
    const bool Vector3::operator==(const Vector3 &other)
    {
        return (x == other.x && y == other.y && z == other.z);
    }
///////////////////////////////////////////////////////////////
    const bool Vector3::operator!=(const Vector3 &other)
    {
        return !(*this == other);
    }
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Vector3 & float
///////////////////////////////////////////////////////////////
    Vector3& Vector3::operator*=(const float& rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    const Vector3 Vector3::operator*(const float& other)
    {
        return Vector3(*this) *= other;
    }

    Vector3& Vector3::operator/=(const float &rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    const Vector3 Vector3::operator/(const float &other)
    {
        return Vector3(*this) /= other;
    }

///////////////////////////////////////////////////////////////
////////////// Standalone operators
///////////////////////////////////////////////////////////////
    const Vector3 operator*(float lhs, const Vector3& rhs)
    {
        return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
    }

///////////////////////////////////////////////////////////////
////////////// Outstreaming operators
///////////////////////////////////////////////////////////////
    ostream& operator<<(ostream& os, Vector3& vec)
    {
        os << "<" << vec.x << ", " << vec.y << ", " << vec.z << ">";
        return os;
    }

///////////////////////////////////////////////////////////////
////////////// Local functions
//////////////////////////////////////////////////////////////
    float Vector3::Magnitude()
    {
        return sqrt(x*x + y*y + z*z);
    }

    float Vector3::SqrMagnitude()
    {
        return x*x + y*y + z*z;
    }

    Vector3 Vector3::Normalized()
    {
        float mag = Magnitude();

        if (mag == 0.0f)
        {
            return Vector3{0.0f, 0.0f, 0.0f};
        }

        return Vector3 {x / mag, y / mag, z / mag};
    }

    void Vector3::Normalize()
    {
        float mag = Magnitude();
        ASSERT(mag != 0);
        x /= mag;
        y /= mag;
        z /= mag;
    }

    float Vector3::Dot(const Vector3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 Vector3::Cross(const Vector3& other) const
    {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }

///////////////////////////////////////////////////////////////
////////////// Static functions
//////////////////////////////////////////////////////////////

    float Vector3::Magnitude(Vector3& vec)
    {
        return vec.Magnitude();
    }

    float Vector3::SqrMagnitude(Vector3& vec)
    {
        return vec.SqrMagnitude();
    }

    Vector3 Vector3::Normalized(Vector3& vec)
    {
        return vec.Normalized();
    }

    void Vector3::Normalize(Vector3& vec)
    {
        vec.Normalize();
    }

    float Vector3::Dot(const Vector3& first, const Vector3& second)
    {
        return first.Dot(second);
    }

    Vector3 Vector3::Cross(const Vector3& first, const Vector3& second)
    {
        return first.Cross(second);
    }

    static float Distance(const Vector3& start, const Vector3& end)
    {
        float x = start.x - end.x;
        float y = start.y - end.y;
        float z = start.z - end.z;
        return sqrt(x*x + y*y + z*z);
    }

    static float DistanceSquared(const Vector3& start, const Vector3& end)
    {
        float x = start.x - end.x;
        float y = start.y - end.y;
        float z = start.z - end.z;
        return x*x + y*y + z*z;
    }

    // For RHS. Flip oer
    void MakeOrthonormalBasisRHS(Vector3& a, Vector3& b, Vector3& c)
    {
        a.Normalize();
        c = a.Cross(b);
        if(c.SqrMagnitude() == 0.0)
            return;
        c.Normalize();
        b = c.Cross(a);
    }
}
