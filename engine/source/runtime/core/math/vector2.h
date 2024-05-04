#pragma once
#include "core/meta/reflection/reflection.h"
#include <assert.h>
namespace QYHS
{
    REFLECTION_TYPE(Vector2)
        CLASS(Vector2, Fields)
    {
        REFLECTION_BODY(Vector2);

    public:
        float x{ 0.f };
        float y{ 0.f };

    public:
        Vector2() = default;
        Vector2(float x_, float y_) : x{ x_ }, y{ y_ } {}

        explicit Vector2(const float coords[3]) : x{ coords[0] }, y{ coords[1] } {}

        float operator[](size_t i) const
        {
            assert(i < 2);
            return *(&x + i);
        }

        float& operator[](size_t i)
        {
            assert(i < 2);
            return *(&x + i);
        }
        /// Pointer accessor for direct copying
        float* ptr() { return &x; }
        /// Pointer accessor for direct copying
        const float* ptr() const { return &x; }

        bool operator==(const Vector2 & rhs) const { return (x == rhs.x && y == rhs.y); }

        bool operator!=(const Vector2 & rhs) const { return x != rhs.x || y != rhs.y; }

        // arithmetic operations
        Vector2 operator+(const Vector2 & rhs) const { return Vector2(x + rhs.x, y + rhs.y); }

        Vector2 operator-(const Vector2 & rhs) const { return Vector2(x - rhs.x, y - rhs.y); }

        Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }

        Vector2 operator*(const Vector2 & rhs) const { return Vector2(x * rhs.x, y * rhs.y); }

        Vector2 operator/(float scalar) const
        {
            assert(scalar != 0.0);
            return Vector2(x / scalar, y / scalar);
        }

        Vector2 operator/(const Vector2 & rhs) const
        {
            assert((rhs.x != 0 && rhs.y != 0));
            return Vector2(x / rhs.x, y / rhs.y);
        }

        const Vector2& operator+() const { return *this; }

        Vector2 operator-() const { return Vector2(-x, -y); }

        // overloaded operators to help Vector2
        friend Vector2 operator*(float scalar, const Vector2 & rhs)
        {
            return Vector2(scalar * rhs.x, scalar * rhs.y);
        }

        friend Vector2 operator/(float scalar, const Vector2 & rhs)
        {
            assert(rhs.x != 0 && rhs.y != 0);
            return Vector2(scalar / rhs.x, scalar / rhs.y);
        }

        friend Vector2 operator+(const Vector2 & lhs, float rhs)
        {
            return Vector2(lhs.x + rhs, lhs.y + rhs);
        }

        friend Vector2 operator+(float lhs, const Vector2 & rhs)
        {
            return Vector2(lhs + rhs.x, lhs + rhs.y);
        }

        friend Vector2 operator-(float lhs, const Vector2 & rhs) { return Vector2(lhs - rhs.x, lhs - rhs.y); }


        // arithmetic updates
        Vector2& operator+=(const Vector2 & rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        Vector2& operator+=(float scalar)
        {
            x += scalar;
            y += scalar;
            return *this;
        }

        Vector2& operator-=(const Vector2 & rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        Vector2& operator-=(float scalar)
        {
            x -= scalar;
            y -= scalar;
            return *this;
        }

        Vector2& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Vector2& operator*=(const Vector2 & rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            return *this;
        }

        Vector2& operator/=(float scalar)
        {
            assert(scalar != 0.0);
            x /= scalar;
            y /= scalar;
            return *this;
        }

        Vector2& operator/=(const Vector2 & rhs)
        {
            assert(rhs.x != 0 && rhs.y != 0);
            x /= rhs.x;
            y /= rhs.y;
            return *this;
        }




        // special points
        static const Vector2 ZERO;
        static const Vector2 UNIT_X;
        static const Vector2 UNIT_Y;
        static const Vector2 UNIT_Z;
        static const Vector2 NEGATIVE_UNIT_X;
        static const Vector2 NEGATIVE_UNIT_Y;
        static const Vector2 NEGATIVE_UNIT_Z;
        static const Vector2 UNIT_SCALE;
    };
}