#ifndef _SGE_MATH_H_
#define _SGE_MATH_H_

#include <cmath>
#include <limits>
#include <algorithm>

namespace SGE
{
    constexpr float EPSILON = 1e-5f;

    class float2
    {
    public:
        float x, y;

        float2() noexcept : x(0), y(0) {}
        float2(float x, float y) noexcept : x(x), y(y) {}

        static const float2 Zero;

        float length() const noexcept
        {
            return std::sqrt(x * x + y * y);
        }

        float2 normalized() const noexcept
        {
            float len = length();
            if (len < EPSILON) return Zero;
            return *this / len;
        }

        float dot(const float2& other) const noexcept
        {
            return x * other.x + y * other.y;
        }

        static float2 lerp(const float2& a, const float2& b, float t) noexcept
        {
            return a + (b - a) * t;
        }

        static float distance(const float2& a, const float2& b) noexcept
        {
            return (a - b).length();
        }

        float2 reflect(const float2& normal) const noexcept
        {
            return *this - 2.0f * normal.dot(*this) * normal;
        }

        float2 project(const float2& onto) const noexcept
        {
            return onto * (this->dot(onto) / onto.dot(onto));
        }

        static float angle(const float2& a, const float2& b) noexcept
        {
            float dot = a.dot(b);
            dot = std::clamp(dot, -1.0f, 1.0f);
            return std::acos(dot);
        }

        float2& operator+=(const float2& other) noexcept
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        float2& operator-=(const float2& other) noexcept
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        float2& operator*=(float scalar) noexcept
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        float2& operator/=(float scalar) noexcept
        {
            *this *= (1.0f / scalar);
            return *this;
        }

        bool operator==(const float2& other) const noexcept
        {
            return std::fabs(x - other.x) < EPSILON && std::fabs(y - other.y) < EPSILON;
        }

        bool operator!=(const float2& other) const noexcept
        {
            return !(*this == other);
        }
    };

    inline float2 operator+(const float2& lhs, const float2& rhs)
    {
        return float2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    inline float2 operator-(const float2& lhs, const float2& rhs)
    {
        return float2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    inline float2 operator*(const float2& vec, float scalar)
    {
        return float2(vec.x * scalar, vec.y * scalar);
    }

    inline float2 operator*(float scalar, const float2& vec)
    {
        return vec * scalar;
    }

    inline float2 operator/(const float2& vec, float scalar)
    {
        return vec * (1.0f / scalar);
    }

    const float2 float2::Zero = float2(0.0f, 0.0f);

    class float3
    {
    public:
        union
        {
            struct { float x, y, z; };
            struct { float r, g, b; };
            float data[3];
        };

        constexpr float3() noexcept : x(0), y(0), z(0) {}
        constexpr float3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

        static const float3 Zero;

        float length() const noexcept
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        float3 normalized() const noexcept
        {
            float len = length();
            if (len < EPSILON) return Zero;
            return *this / len;
        }

        float dot(const float3& other) const noexcept
        {
            return x * other.x + y * other.y + z * other.z;
        }

        float3 cross(const float3& other) const noexcept
        {
            return float3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        static float3 lerp(const float3& a, const float3& b, float t) noexcept
        {
            return a + (b - a) * t;
        }

        static float3 slerp(const float3& a, const float3& b, float t) noexcept
        {
            float dot = a.dot(b);
            dot = std::clamp(dot, -1.0f, 1.0f);
            float theta = std::acos(dot) * t;
            float3 relativeVec = b - a * dot;
            relativeVec = relativeVec.normalized();
            return a * std::cos(theta) + relativeVec * std::sin(theta);
        }

        float3 reflect(const float3& normal) const noexcept
        {
            return *this - 2.0f * normal.dot(*this) * normal;
        }

        static float distance(const float3& a, const float3& b) noexcept
        {
            return (a - b).length();
        }

        float3 project(const float3& onto) const noexcept
        {
            return onto * (this->dot(onto) / onto.dot(onto));
        }

        static float angle(const float3& a, const float3& b) noexcept
        {
            float dot = a.dot(b);
            dot = std::clamp(dot, -1.0f, 1.0f);
            return std::acos(dot);
        }

        float3& operator+=(const float3& other) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        float3& operator-=(const float3& other) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        float3& operator*=(float scalar) noexcept
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        float3& operator/=(float scalar) noexcept
        {
            *this *= (1.0f / scalar);
            return *this;
        }

        bool operator==(const float3& other) const noexcept
        {
            return std::fabs(x - other.x) < EPSILON &&
                std::fabs(y - other.y) < EPSILON &&
                std::fabs(z - other.z) < EPSILON;
        }

        bool operator!=(const float3& other) const noexcept
        {
            return !(*this == other);
        }
    };

    inline float3 operator+(const float3& lhs, const float3& rhs)
    {
        return float3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    inline float3 operator-(const float3& lhs, const float3& rhs)
    {
        return float3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    inline float3 operator*(const float3& vec, float scalar)
    {
        return float3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
    }

    inline float3 operator*(float scalar, const float3& vec)
    {
        return vec * scalar;
    }

    inline float3 operator/(const float3& vec, float scalar)
    {
        return vec * (1.0f / scalar);
    }

    const float3 float3::Zero = float3(0.0f, 0.0f, 0.0f);
}

#endif // !_SGE_MATH_H_
