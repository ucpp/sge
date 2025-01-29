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

        float2() noexcept;
        float2(float x, float y) noexcept;
        float* data() noexcept;

        static const float2 Zero;

        float length() const noexcept;
        float dot(const float2& other) const noexcept;
        float2 normalized() const noexcept;
        float2 reflect(const float2& normal) const noexcept;
        float2 project(const float2& onto) const noexcept;
        static float distance(const float2& a, const float2& b) noexcept;
        static float angle(const float2& a, const float2& b) noexcept;
        static float2 lerp(const float2& a, const float2& b, float t) noexcept;

        float2& operator+=(const float2& other) noexcept;
        float2& operator-=(const float2& other) noexcept;
        float2& operator*=(float scalar) noexcept;
        float2& operator/=(float scalar) noexcept;
        bool operator==(const float2& other) const noexcept;
        bool operator!=(const float2& other) const noexcept;
        float2 operator-() const noexcept;
    };

    inline float2 operator+(const float2& lhs, const float2& rhs);
    inline float2 operator-(const float2& lhs, const float2& rhs);
    inline float2 operator*(const float2& vec, float scalar);
    inline float2 operator*(float scalar, const float2& vec);
    inline float2 operator/(const float2& vec, float scalar);

    class float3
    {
    public:
        union
        {
            struct { float x, y, z; };
            struct { float r, g, b; };
            float components[3];
        };

        float3() noexcept;
        float3(float x, float y, float z) noexcept;
        float* data() noexcept;

        float& operator[](size_t index) noexcept;
        const float& operator[](size_t index) const noexcept;

        static const float3 Zero;

        float length() const noexcept;
        float dot(const float3& other) const noexcept;
        float3 normalized() const noexcept;
        float3 cross(const float3& other) const noexcept;
        float3 reflect(const float3& normal) const noexcept;
        float3 project(const float3& onto) const noexcept;
        static float distance(const float3& a, const float3& b) noexcept;
        static float angle(const float3& a, const float3& b) noexcept;
        static float3 lerp(const float3& a, const float3& b, float t) noexcept;
        static float3 slerp(const float3& a, const float3& b, float t) noexcept;

        float3& operator+=(const float3& other) noexcept;
        float3& operator-=(const float3& other) noexcept;
        float3& operator*=(float scalar) noexcept;
        float3& operator/=(float scalar) noexcept;
        bool operator==(const float3& other) const noexcept;
        bool operator!=(const float3& other) const noexcept;
        float3 operator-() const noexcept;
    };

    inline float3 operator+(const float3& lhs, const float3& rhs);
    inline float3 operator-(const float3& lhs, const float3& rhs);
    inline float3 operator*(const float3& vec, float scalar);
    inline float3 operator*(float scalar, const float3& vec);
    inline float3 operator/(const float3& vec, float scalar);
}

#endif // !_SGE_MATH_H_
