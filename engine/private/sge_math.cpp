#include "sge_math.h"

namespace SGE
{
    const float2 float2::Zero = float2(0.0f, 0.0f);
    const float3 float3::Zero = float3(0.0f, 0.0f, 0.0f);

    float2::float2() noexcept : x(0), y(0) {}
    float2::float2(float x, float y) noexcept : x(x), y(y) {}
    float* float2::data() noexcept { return &x; }

    float float2::length() const noexcept
    {
        return std::sqrt(x * x + y * y);
    }

    float2 float2::normalized() const noexcept
    {
        float len = length();
        if (len < EPSILON) return Zero;
        return *this / len;
    }

    void float2::normalize() noexcept
    {
        float len = length();
        if (len >= EPSILON)
        {
            *this /= len;
        }
        else
        { 
            *this = Zero;
        }
    }

    float float2::dot(const float2& other) const noexcept
    {
        return x * other.x + y * other.y;
    }

    float2 float2::lerp(const float2& a, const float2& b, float t) noexcept
    {
        return a + (b - a) * t;
    }

    float float2::distance(const float2& a, const float2& b) noexcept
    {
        return (a - b).length();
    }

    float2 float2::reflect(const float2& normal) const noexcept
    {
        return *this - 2.0f * normal.dot(*this) * normal;
    }

    float2 float2::project(const float2& onto) const noexcept
    {
        return onto * (this->dot(onto) / onto.dot(onto));
    }

    float float2::angle(const float2& a, const float2& b) noexcept
    {
        float dot = a.dot(b);
        dot = std::clamp(dot, -1.0f, 1.0f);
        return std::acos(dot);
    }

    float2& float2::operator+=(const float2& other) noexcept
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    float2& float2::operator-=(const float2& other) noexcept
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    float2& float2::operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    float2& float2::operator/=(float scalar) noexcept
    {
        *this *= (1.0f / scalar);
        return *this;
    }

    bool float2::operator==(const float2& other) const noexcept
    {
        return std::fabs(x - other.x) < EPSILON && std::fabs(y - other.y) < EPSILON;
    }

    bool float2::operator!=(const float2& other) const noexcept
    {
        return !(*this == other);
    }

    float2 float2::operator-() const noexcept
    {
        return float2(-x, -y);
    }

    float3::float3() noexcept : x(0), y(0), z(0) {}
    float3::float3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
    float* float3::data() noexcept { return &x; }

    float& float3::operator[](size_t index) noexcept
    {
        return components[index];
    }

    const float& float3::operator[](size_t index) const noexcept
    {
        return components[index];
    }

    float float3::length() const noexcept
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    float3 float3::normalized() const noexcept
    {
        float len = length();
        if (len < EPSILON) return Zero;
        return *this / len;
    }

    void float3::normalize() noexcept
    {
        float len = length();
        if (len >= EPSILON) 
        {
            *this /= len;
        }
        else
        {
            *this = Zero;
        }
    }

    float float3::dot(const float3& other) const noexcept
    {
        return x * other.x + y * other.y + z * other.z;
    }

    float3 float3::cross(const float3& other) const noexcept
    {
        return float3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float3 float3::lerp(const float3& a, const float3& b, float t) noexcept
    {
        return a + (b - a) * t;
    }

    float3 float3::slerp(const float3& a, const float3& b, float t) noexcept
    {
        float dot = a.dot(b);
        dot = std::clamp(dot, -1.0f, 1.0f);
        float theta = std::acos(dot) * t;
        float3 relativeVec = b - a * dot;
        relativeVec = relativeVec.normalized();
        return a * std::cos(theta) + relativeVec * std::sin(theta);
    }

    float3 float3::reflect(const float3& normal) const noexcept
    {
        return *this - 2.0f * normal.dot(*this) * normal;
    }

    float float3::distance(const float3& a, const float3& b) noexcept
    {
        return (a - b).length();
    }

    float3 float3::project(const float3& onto) const noexcept
    {
        return onto * (this->dot(onto) / onto.dot(onto));
    }

    float float3::angle(const float3& a, const float3& b) noexcept
    {
        float dot = a.dot(b);
        dot = std::clamp(dot, -1.0f, 1.0f);
        return std::acos(dot);
    }

    float3& float3::operator+=(const float3& other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    float3& float3::operator-=(const float3& other) noexcept
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    float3& float3::operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    float3& float3::operator/=(float scalar) noexcept
    {
        *this *= (1.0f / scalar);
        return *this;
    }

    bool float3::operator==(const float3& other) const noexcept
    {
        return std::fabs(x - other.x) < EPSILON &&
               std::fabs(y - other.y) < EPSILON &&
               std::fabs(z - other.z) < EPSILON;
    }

    bool float3::operator!=(const float3& other) const noexcept
    {
        return !(*this == other);
    }

    float3 float3::operator-() const noexcept
    {
        return float3(-x, -y, -z);
    }

    float2 operator+(const float2& lhs, const float2& rhs)
    {
        return float2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    float2 operator-(const float2& lhs, const float2& rhs)
    {
        return float2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    float2 operator*(const float2& vec, float scalar)
    {
        return float2(vec.x * scalar, vec.y * scalar);
    }

    float2 operator*(float scalar, const float2& vec)
    {
        return vec * scalar;
    }

    float2 operator/(const float2& vec, float scalar)
    {
        return vec * (1.0f / scalar);
    }

    float3 operator+(const float3& lhs, const float3& rhs)
    {
        return float3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    float3 operator-(const float3& lhs, const float3& rhs)
    {
        return float3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    float3 operator*(const float3& vec, float scalar)
    {
        return float3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
    }

    float3 operator*(float scalar, const float3& vec)
    {
        return vec * scalar;
    }

    float3 operator/(const float3& vec, float scalar)
    {
        return vec * (1.0f / scalar);
    }
}
