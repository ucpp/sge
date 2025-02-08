#ifndef _SGE_MATH_H_
#define _SGE_MATH_H_

#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>

namespace SGE
{
    constexpr float EPSILON = 1e-5f;
    constexpr float PI = 3.14159265358979323846f;

    constexpr float ConvertToRadians(float degrees) noexcept { return degrees * (PI / 180.0f); }
    constexpr float ConvertToDegrees(float radians) noexcept { return radians * (180.0f / PI); }

    // --------------------------------------------------------------------------
    // float1
    // --------------------------------------------------------------------------
    struct float1
    {
        float value;

        float1() = default;
        float1(float v) : value(v) {}

        operator float() const { return value; }
    };

    // --------------------------------------------------------------------------
    // float2
    // --------------------------------------------------------------------------

    class float2
    {
    public:
        float x, y;

        float2() noexcept;
        float2(float x, float y) noexcept;
        float* data() noexcept;
        const float* data() const noexcept;

        static const float2 Zero;

        float length() const noexcept;
        float2 normalized() const noexcept;
        void normalize() noexcept;

        float2& operator+=(const float2& other) noexcept;
        float2& operator-=(const float2& other) noexcept;
        float2& operator*=(float scalar) noexcept;
        float2& operator/=(float scalar) noexcept;
        bool operator==(const float2& other) const noexcept;
        bool operator!=(const float2& other) const noexcept;
        float2 operator-() const noexcept;
    };

    float dot(const float2& a, const float2& b) noexcept;
    float2 reflect(const float2& vec, const float2& normal) noexcept;
    float2 project(const float2& vec, const float2& onto) noexcept;
    float distance(const float2& a, const float2& b) noexcept;
    float angle(const float2& a, const float2& b) noexcept;
    float2 lerp(const float2& a, const float2& b, float t) noexcept;
    float2 operator+(const float2& lhs, const float2& rhs) noexcept;
    float2 operator-(const float2& lhs, const float2& rhs) noexcept;
    float2 operator*(const float2& vec, float scalar) noexcept;
    float2 operator*(float scalar, const float2& vec) noexcept;
    float2 operator/(const float2& vec, float scalar) noexcept;

    // --------------------------------------------------------------------------
    // float3
    // --------------------------------------------------------------------------

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
        const float* data() const noexcept;

        float& operator[](size_t index) noexcept;
        const float& operator[](size_t index) const noexcept;

        static const float3 Zero;

        float length() const noexcept;
        float3 normalized() const noexcept;
        void normalize() noexcept;

        float3& operator+=(const float3& other) noexcept;
        float3& operator-=(const float3& other) noexcept;
        float3& operator*=(float scalar) noexcept;
        float3& operator/=(float scalar) noexcept;
        bool operator==(const float3& other) const noexcept;
        bool operator!=(const float3& other) const noexcept;
        float3 operator-() const noexcept;
    };

    float3 cross(const float3& lhs, const float3& rhs) noexcept;
    float3 slerp(const float3& a, const float3& b, float t) noexcept;
    float dot(const float3& a, const float3& b) noexcept;
    float3 reflect(const float3& vec, const float3& normal) noexcept;
    float3 project(const float3& vec, const float3& onto) noexcept;
    float distance(const float3& a, const float3& b) noexcept;
    float angle(const float3& a, const float3& b) noexcept;
    float3 lerp(const float3& a, const float3& b, float t) noexcept;
    float3 operator+(const float3& lhs, const float3& rhs) noexcept;
    float3 operator-(const float3& lhs, const float3& rhs) noexcept;
    float3 operator*(const float3& vec, float scalar) noexcept;
    float3 operator*(float scalar, const float3& vec) noexcept;
    float3 operator/(const float3& vec, float scalar) noexcept;

    // --------------------------------------------------------------------------
    // float4
    // --------------------------------------------------------------------------

    class float4
    {
    public:
        union
        {
            struct { float x, y, z, w; };
            struct { float r, g, b, a; };
            float components[4];
        };

        float4() noexcept;
        float4(float x, float y, float z, float w) noexcept;
        float* data() noexcept;
        const float* data() const noexcept;

        float& operator[](size_t index) noexcept;
        const float& operator[](size_t index) const noexcept;

        float4& operator+=(const float4& other) noexcept;
        float4& operator-=(const float4& other) noexcept;
        float4& operator*=(float scalar) noexcept;
        float4& operator/=(float scalar) noexcept;
        bool operator==(const float4& other) noexcept;
        bool operator!=(const float4& other) noexcept;
        float4 operator-() noexcept;

        static const float4 Zero;
    };

    float4 operator+(const float4& lhs, const float4& rhs) noexcept;
    float4 operator-(const float4& lhs, const float4& rhs) noexcept;
    float4 operator*(const float4& vec, float scalar) noexcept;
    float4 operator/(const float4& vec, float scalar) noexcept;

    // --------------------------------------------------------------------------
    // float2x2
    // --------------------------------------------------------------------------

    class float2x2
    {
    public:
        // row-major order: m[0] = m00, m[1] = m01, m[2] = m10, m[3] = m11
        union
        {
            struct
            {
                float m00, m01;
                float m10, m11;
            };
            float m[4]; // Row-major: [m00, m01, m10, m11]
        };

        float2x2() noexcept;
        float2x2(float m00, float m01, float m10, float m11) noexcept;
        float2x2(const float2& row0, const float2& row1) noexcept;

        float* data() noexcept;
        const float* data() const noexcept;

        static const float2x2 Identity;
        static const float2x2 Zero;

        float2x2 transposed() const noexcept;
        float determinant() const noexcept;
        float2x2 inverse() const noexcept;

        float2 operator*(const float2& vec) const noexcept;
        float2x2 operator*(const float2x2& other) const noexcept;
        float2x2& operator*=(const float2x2& other) noexcept;
        float2x2 operator*(float scalar) const noexcept;
        float2x2& operator*=(float scalar) noexcept;
        float2x2 operator+(const float2x2& other) const noexcept;
        float2x2& operator+=(const float2x2& other) noexcept;
        float2x2 operator-(const float2x2& other) const noexcept;
        float2x2& operator-=(const float2x2& other) noexcept;
        bool operator==(const float2x2& other) const noexcept;
        bool operator!=(const float2x2& other) const noexcept;
    };

    float2x2 operator*(float scalar, const float2x2& mat) noexcept;

    // --------------------------------------------------------------------------
    // float3x3
    // --------------------------------------------------------------------------

    class float3x3
    {
    public:
        // row-major order: m[0] = m00, m[1] = m01, m[2] = m02,
        //                  m[3] = m10, m[4] = m11, m[5] = m12,
        //                  m[6] = m20, m[7] = m21, m[8] = m22
        union
        {
            struct
            {
                float m00, m01, m02;
                float m10, m11, m12;
                float m20, m21, m22;
            };
            float m[9]; // Row-major: [m00, m01, m02, m10, m11, m12, m20, m21, m22]
        };

        float3x3() noexcept;
        float3x3(float m00, float m01, float m02,
                 float m10, float m11, float m12,
                 float m20, float m21, float m22) noexcept;
        float3x3(const float3& row0, const float3& row1, const float3& row2) noexcept;

        float* data() noexcept;
        const float* data() const noexcept;

        static const float3x3 Identity;
        static const float3x3 Zero;

        float3x3 transposed() const noexcept;
        float determinant() const noexcept;
        float3x3 inverse() const noexcept;

        float3 operator*(const float3& vec) const noexcept;
        float3x3 operator*(const float3x3& other) const noexcept;
        float3x3& operator*=(const float3x3& other) noexcept;
        float3x3 operator*(float scalar) const noexcept;
        float3x3& operator*=(float scalar) noexcept;
        float3x3 operator+(const float3x3& other) const noexcept;
        float3x3& operator+=(const float3x3& other) noexcept;
        float3x3 operator-(const float3x3& other) const noexcept;
        float3x3& operator-=(const float3x3& other) noexcept;
        bool operator==(const float3x3& other) const noexcept;
        bool operator!=(const float3x3& other) const noexcept;
    };

    float3x3 operator*(float scalar, const float3x3& mat) noexcept;

    // --------------------------------------------------------------------------
    // float4x4
    // --------------------------------------------------------------------------

    class alignas(16) float4x4
    {
    public:
        // row-major order: m[0]  = m00, m[1]  = m01, m[2]  = m02, m[3]  = m03,
        //                  m[4]  = m10, m[5]  = m11, m[6]  = m12, m[7]  = m13,
        //                  m[8]  = m20, m[9]  = m21, m[10] = m22, m[11] = m23,
        //                  m[12] = m30, m[13] = m31, m[14] = m32, m[15] = m33
        union
        {
            struct
            {
                float m00, m01, m02, m03;
                float m10, m11, m12, m13;
                float m20, m21, m22, m23;
                float m30, m31, m32, m33;
            };
            float m[16]; // Row-major: [m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33]
        };

        float4x4() noexcept;
        float4x4(float m00, float m01, float m02, float m03, 
                 float m10, float m11, float m12, float m13, 
                 float m20, float m21, float m22, float m23, 
                 float m30, float m31, float m32, float m33) noexcept;
        float4x4(const float4& row0, const float4& row1, const float4& row2, const float4& row3) noexcept;

        float* data() noexcept;
        const float* data() const noexcept;

        static const float4x4 Identity;
        static const float4x4 Zero;

        float4x4 transposed() const noexcept;
        float determinant() const noexcept;
        float4x4 inverse() const noexcept;

        float4 operator*(const float4& vec) const noexcept;
        float4x4 operator*(const float4x4& other) const noexcept;
        float4x4& operator*=(const float4x4& other) noexcept;
        float4x4 operator*(float scalar) const noexcept;
        float4x4& operator*=(float scalar) noexcept;
        float4x4 operator+(const float4x4& other) const noexcept;
        float4x4& operator+=(const float4x4& other) noexcept;
        float4x4 operator-(const float4x4& other) const noexcept;
        float4x4& operator-=(const float4x4& other) noexcept;
        bool operator==(const float4x4& other) const noexcept;
        bool operator!=(const float4x4& other) const noexcept;
    };

    float4x4 operator*(float scalar, const float4x4& mat) noexcept;
    std::ostream& operator<<(std::ostream& os, const float4x4& mat);

    // --------------------------------------------------------------------------
    // camera and transformations
    // --------------------------------------------------------------------------

    float4x4 CreateViewMatrix(const float3& eye, const float3& target, const float3& up) noexcept;
    float4x4 CreatePerspectiveProjectionMatrix(float fov, float aspectRatio, float nearZ, float farZ) noexcept;
    float4x4 CreateTranslationMatrix(const float3& translation) noexcept;
    float4x4 CreateRotationMatrixYawPitchRoll(float yaw, float pitch, float roll) noexcept;
    float4x4 CreateScaleMatrix(const float3& scale) noexcept;
    float4x4 CreateOrthographicProjectionMatrix(float width, float height, float nearZ, float farZ) noexcept;
}

#endif // !_SGE_MATH_H_
