#include "Vector4.h"
#include "../Farlor.h"

#include <cmath>

namespace Farlor
{
    Vector4::Vector4()
        : x {0}
        , y {0}
        , z {0}
        , w {0}
    {
    }

    Vector4::Vector4(float value)
        : x {value}
        , y {value}
        , z {value}
        , w {value}
    {
    }

    Vector4::Vector4(const float xNew, const float yNew, const float zNew, const float wNew)
        : x {xNew}
        , y {yNew}
        , z {zNew}
        , w {wNew}
    {
    }

///////////////////////////////////////////////////////////////
////////////// Vector4 & Vector4
///////////////////////////////////////////////////////////////
    Vector4& Vector4::operator+=(const Vector4 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    const Vector4 Vector4::operator+(const Vector4 &other)
    {
        return Vector4(*this) += other;
    }
///////////////////////////////////////////////////////////////
    Vector4& Vector4::operator-=(const Vector4 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    const Vector4 Vector4::operator-(const Vector4 &other)
    {
        return Vector4(*this) -= other;
    }
///////////////////////////////////////////////////////////////
    Vector4& Vector4::operator*=(const Vector4 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        z *= rhs.w;
        return *this;
    }

    const Vector4 Vector4::operator*(const Vector4 &other)
    {
        return Vector4(*this) *= other;
    }
///////////////////////////////////////////////////////////////
    Vector4& Vector4::operator/=(const Vector4 &rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }

    const Vector4 Vector4::operator/(const Vector4 &other)
    {
        return Vector4(*this) /= other;
    }
///////////////////////////////////////////////////////////////
    // Lets override the % to be the dot product
    const float Vector4::operator%(const Vector4 &other)
    {
        return (*this).Dot(other);
    }
///////////////////////////////////////////////////////////////
    const bool Vector4::operator==(const Vector4 &other)
    {
        return (x == other.x && y == other.y && z == other.z && w == other.w);
    }
///////////////////////////////////////////////////////////////
    const bool Vector4::operator!=(const Vector4 &other)
    {
        return !(*this == other);
    }
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Vector4 & float
///////////////////////////////////////////////////////////////
    Vector4& Vector4::operator*=(const float& rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        w *= rhs;
        return *this;
    }

    const Vector4 Vector4::operator*(const float& other)
    {
        return Vector4(*this) *= other;
    }

    Vector4& Vector4::operator/=(const float &rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        w /= rhs;
        return *this;
    }

    const Vector4 Vector4::operator/(const float &other)
    {
        return Vector4(*this) /= other;
    }

///////////////////////////////////////////////////////////////
////////////// Standalone operators
///////////////////////////////////////////////////////////////
    const Vector4 operator*(float lhs, const Vector4& rhs)
    {
        return Vector4{lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w};
    }

///////////////////////////////////////////////////////////////
////////////// Outstreaming operators
///////////////////////////////////////////////////////////////
    ostream& operator<<(ostream& os, Vector4& vec)
    {
        os << "<" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ">";
        return os;
    }

///////////////////////////////////////////////////////////////
////////////// Local functions
//////////////////////////////////////////////////////////////
    float Vector4::Magnitude()
    {
        return sqrt(x*x + y*y + z*z + w*w);
    }

    float Vector4::SqrMagnitude()
    {
        return x*x + y*y + z*z + w*w;
    }

    Vector4 Vector4::Normalized()
    {
        float mag = Magnitude();
        ASSERT(mag != 0);

        return Vector4 {x / mag, y / mag, z / mag, w / mag};
    }

    void Vector4::Normalize()
    {
        float mag = Magnitude();
        ASSERT(mag != 0);
        x /= mag;
        y /= mag;
        z /= mag;
        w /= mag;
    }

    float Vector4::Dot(const Vector4& other) const
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

///////////////////////////////////////////////////////////////
////////////// Static functions
//////////////////////////////////////////////////////////////

    float Vector4::Magnitude(Vector4& vec)
    {
        return vec.Magnitude();
    }

    float Vector4::SqrMagnitude(Vector4& vec)
    {
        return vec.SqrMagnitude();
    }

    Vector4 Vector4::Normalized(Vector4& vec)
    {
        return vec.Normalized();
    }

    void Vector4::Normalize(Vector4& vec)
    {
        vec.Normalize();
    }

    float Vector4::Dot(const Vector4& first, const Vector4& second)
    {
        return first.Dot(second);
    }
}
