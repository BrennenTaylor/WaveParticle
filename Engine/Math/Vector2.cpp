#include "Vector2.h"
#include "../Farlor.h"

#include <cmath>

namespace Farlor
{
    Vector2::Vector2()
        : x {0}
        , y {0}
    {
    }

    Vector2::Vector2(float value)
        : x {value}
        , y {value}
    {
    }

    Vector2::Vector2(const float xNew, const float yNew)
        : x {xNew}
        , y {yNew}
    {
    }

///////////////////////////////////////////////////////////////
////////////// Vector2 & Vector
///////////////////////////////////////////////////////////////
    Vector2& Vector2::operator+=(const Vector2 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    const Vector2 Vector2::operator+(const Vector2 &other)
    {
        return Vector2(*this) += other;
    }
///////////////////////////////////////////////////////////////
    Vector2& Vector2::operator-=(const Vector2 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    const Vector2 Vector2::operator-(const Vector2 &other)
    {
        return Vector2(*this) -= other;
    }
///////////////////////////////////////////////////////////////
    Vector2& Vector2::operator*=(const Vector2 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    const Vector2 Vector2::operator*(const Vector2 &other)
    {
        return Vector2(*this) *= other;
    }
///////////////////////////////////////////////////////////////
    Vector2& Vector2::operator/=(const Vector2 &rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    const Vector2 Vector2::operator/(const Vector2 &other)
    {
        return Vector2(*this) /= other;
    }
///////////////////////////////////////////////////////////////
    // Lets override the % to be the dot product
    const float Vector2::operator%(const Vector2 &other)
    {
        return (*this).Dot(other);
    }
///////////////////////////////////////////////////////////////
    const bool Vector2::operator==(const Vector2 &other)
    {
        return (x == other.x && y == other.y);
    }
///////////////////////////////////////////////////////////////
    const bool Vector2::operator!=(const Vector2 &other)
    {
        return !(*this == other);
    }
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Vector2 & float
///////////////////////////////////////////////////////////////
    Vector2& Vector2::operator*=(const float& rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    const Vector2 Vector2::operator*(const float& other)
    {
        return Vector2(*this) *= other;
    }

    Vector2& Vector2::operator/=(const float &rhs)
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    const Vector2 Vector2::operator/(const float &other)
    {
        return Vector2(*this) /= other;
    }

///////////////////////////////////////////////////////////////
////////////// Standalone operators
///////////////////////////////////////////////////////////////
    const Vector2 operator*(float lhs, const Vector2& rhs)
    {
        return Vector2(lhs * rhs.x, lhs * rhs.y);
    }

///////////////////////////////////////////////////////////////
////////////// Outstreaming operators
///////////////////////////////////////////////////////////////
    std::ostream& operator<<(std::ostream& os, const Vector2& vec)
    {
        os << "<" << vec.x << ", " << vec.y << ">";
        return os;
    }

///////////////////////////////////////////////////////////////
////////////// Local functions
//////////////////////////////////////////////////////////////
    float Vector2::Magnitude()
    {
        return sqrt(x*x + y*y);
    }

    float Vector2::SqrMagnitude()
    {
        return x*x + y*y;
    }

    Vector2 Vector2::Normalized()
    {
        float mag = Magnitude();
        ASSERT(mag != 0);

        return Vector2 {x / mag, y / mag};
    }

    void Vector2::Normalize()
    {
        float mag = Magnitude();
        ASSERT(mag != 0);
        x /= mag;
        y /= mag;
    }

    float Vector2::Dot(const Vector2& other) const
    {
        return x * other.x + y * other.y;
    }

///////////////////////////////////////////////////////////////
////////////// Static functions
//////////////////////////////////////////////////////////////

    float Vector2::Magnitude(Vector2& vec)
    {
        return vec.Magnitude();
    }

    float Vector2::SqrMagnitude(Vector2& vec)
    {
        return vec.SqrMagnitude();
    }

    Vector2 Vector2::Normalized(Vector2& vec)
    {
        return vec.Normalized();
    }

    void Vector2::Normalize(Vector2& vec)
    {
        vec.Normalize();
    }

    float Vector2::Dot(const Vector2& first, const Vector2& second)
    {
        return first.Dot(second);
    }
}
