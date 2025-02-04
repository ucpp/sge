#include "sge_math.h"

namespace SGE
{
    // --------------------------------------------------------------------------
    // float2
    // --------------------------------------------------------------------------

    const float2 float2::Zero = float2(0.0f, 0.0f);

    float2::float2() noexcept : x(0), y(0) {}
    float2::float2(float x, float y) noexcept : x(x), y(y) {}
    float* float2::data() noexcept { return &x; }
    const float* float2::data() const noexcept { return &x; }

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

    float dot(const float2& a, const float2& b) noexcept
    {
        return a.x * b.x + a.y * b.y;
    }

    float2 reflect(const float2& vec, const float2& normal) noexcept
    {
        float dotProduct = dot(vec, normal);
        return vec - 2.0f * dotProduct * normal;
    }

    float2 project(const float2& vec, const float2& onto) noexcept
    {
        float dotProduct = dot(vec, onto);
        float ontoLengthSq = dot(onto, onto);
        return onto * (dotProduct / ontoLengthSq);
    }

    float distance(const float2& a, const float2& b) noexcept
    {
        return (a - b).length();
    }

    float angle(const float2& a, const float2& b) noexcept
    {
        float dotProduct = dot(a, b);
        float lenA = a.length();
        float lenB = b.length();
        return std::acos(dotProduct / (lenA * lenB));
    }

    float2 lerp(const float2& a, const float2& b, float t) noexcept
    {
        return a + (b - a) * t;
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

    // --------------------------------------------------------------------------
    // float3
    // --------------------------------------------------------------------------

    const float3 float3::Zero = float3(0.0f, 0.0f, 0.0f);

    float3::float3() noexcept : x(0), y(0), z(0) {}
    float3::float3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
    float* float3::data() noexcept { return &x; }
    const float* float3::data() const noexcept { return &x; }
    float& float3::operator[](size_t index) noexcept { return components[index]; }
    const float& float3::operator[](size_t index) const noexcept { return components[index]; }

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

    float dot(const float3& lhs, const float3& rhs) noexcept
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    float3 cross(const float3& lhs, const float3& rhs) noexcept
    {
        return float3
        (
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x
        );
    }

    float3 lerp(const float3& a, const float3& b, float t) noexcept
    {
        return a + (b - a) * t;
    }

    float3 slerp(const float3& a, const float3& b, float t) noexcept
    {
        float dotValue = dot(a, b);
        dotValue = std::clamp(dotValue, -1.0f, 1.0f);
        float theta = std::acos(dotValue) * t;
        float3 relativeVec = b - a * dotValue;
        relativeVec = relativeVec.normalized();
        return a * std::cos(theta) + relativeVec * std::sin(theta);
    }

    float3 reflect(const float3& vec, const float3& normal) noexcept
    {
        return vec - 2.0f * dot(normal, vec) * normal;
    }

    float distance(const float3& a, const float3& b) noexcept
    {
        return (a - b).length();
    }

    float3 project(const float3& vec, const float3& onto) noexcept
    {
        return onto * (dot(vec, onto) / dot(onto, onto));
    }

    float angle(const float3& a, const float3& b) noexcept
    {
        float dotValue = dot(a, b);
        dotValue = std::clamp(dotValue, -1.0f, 1.0f);
        return std::acos(dotValue);
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

    float2 operator+(const float2& lhs, const float2& rhs) noexcept
    {
        return float2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    float2 operator-(const float2& lhs, const float2& rhs) noexcept
    {
        return float2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    float2 operator*(const float2& vec, float scalar) noexcept
    {
        return float2(vec.x * scalar, vec.y * scalar);
    }

    float2 operator*(float scalar, const float2& vec) noexcept
    {
        return vec * scalar;
    }

    float2 operator/(const float2& vec, float scalar) noexcept
    {
        return vec * (1.0f / scalar);
    }

    float3 operator+(const float3& lhs, const float3& rhs) noexcept
    {
        return float3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    float3 operator-(const float3& lhs, const float3& rhs) noexcept
    {
        return float3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    float3 operator*(const float3& vec, float scalar) noexcept
    {
        return float3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
    }

    float3 operator*(float scalar, const float3& vec) noexcept
    {
        return vec * scalar;
    }

    float3 operator/(const float3& vec, float scalar) noexcept
    {
        return vec * (1.0f / scalar);
    }

    // --------------------------------------------------------------------------
    // float4
    // --------------------------------------------------------------------------
    
    const float4 float4::Zero = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4::float4() noexcept : x(0), y(0), z(0), w(0) {}
    float4::float4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}
    float* float4::data() noexcept { return components; }
    const float* float4::data() const noexcept { return components; }
    float& float4::operator[](size_t index) noexcept { return components[index]; }
    const float& float4::operator[](size_t index) const noexcept { return components[index]; }

    float4& float4::operator+=(const float4& other) noexcept
    {
        x += other.x; y += other.y; z += other.z; w += other.w;
        return *this;
    }

    float4& float4::operator-=(const float4& other) noexcept
    {
        x -= other.x; y -= other.y; z -= other.z; w -= other.w;
        return *this;
    }

    float4& float4::operator*=(float scalar) noexcept
    {
        x *= scalar; y *= scalar; z *= scalar; w *= scalar;
        return *this;
    }

    float4& float4::operator/=(float scalar) noexcept
    {
        x /= scalar; y /= scalar; z /= scalar; w /= scalar;
        return *this;
    }

    bool float4::operator==(const float4& other) noexcept
    {
        return std::fabs(x - other.x) < EPSILON &&
               std::fabs(y - other.y) < EPSILON &&
               std::fabs(z - other.z) < EPSILON &&
               std::fabs(w - other.w) < EPSILON;
    }

    bool float4::operator!=(const float4& other) noexcept
    {
        return !(*this == other);
    }

    float4 float4::operator-() noexcept
    {
        return float4(-x, -y, -z, -w);
    }

    float4 operator+(const float4& lhs, const float4& rhs) noexcept
    {
        return float4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
    }

    float4 operator-(const float4& lhs, const float4& rhs) noexcept
    {
        return float4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
    }

    float4 operator*(const float4& vec, float scalar) noexcept
    {
        return float4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
    }

    float4 operator/(const float4& vec, float scalar) noexcept
    {
        return float4(vec.x / scalar, vec.y / scalar, vec.z / scalar, vec.w / scalar);
    }

    // --------------------------------------------------------------------------
    // float2x2
    // --------------------------------------------------------------------------

    const float2x2 float2x2::Identity = float2x2(1.0f, 0.0f, 0.0f, 1.0f);
    const float2x2 float2x2::Zero = float2x2(0.0f, 0.0f, 0.0f, 0.0f);


    float2x2::float2x2() noexcept : m00(0), m01(0), m10(0), m11(0) {}

    float2x2::float2x2(float m00, float m01, float m10, float m11) noexcept
    : m00(m00), m01(m01), m10(m10), m11(m11) {}

    float2x2::float2x2(const float2& row0, const float2& row1) noexcept
    : m00(row0.x), m01(row0.y), m10(row1.x), m11(row1.y) {}

    float* float2x2::data() noexcept { return m; }
    const float* float2x2::data() const noexcept { return m; }

    float2x2 float2x2::transposed() const noexcept
    {
        return float2x2(m00, m10, m01, m11);
    }

    float float2x2::determinant() const noexcept
    {
        return m00 * m11 - m01 * m10;
    }

    float2x2 float2x2::inverse() const noexcept
    {
        float det = determinant();
        if (std::fabs(det) < EPSILON)
        {
            return Zero;
        }

        float invDet = 1.0f / det;

        return float2x2
        (
            m11 * invDet, -m01 * invDet,
            -m10 * invDet, m00 * invDet
        );
    }

    float2 float2x2::operator*(const float2& vec) const noexcept
    {
        return float2
        (
            m00 * vec.x + m01 * vec.y,
            m10 * vec.x + m11 * vec.y
        );
    }

    float2x2 float2x2::operator*(const float2x2& other) const noexcept
    {
        return float2x2
        (
            m00 * other.m00 + m01 * other.m10, m00 * other.m01 + m01 * other.m11,
            m10 * other.m00 + m11 * other.m10, m10 * other.m01 + m11 * other.m11
        );
    }

    float2x2 float2x2::operator*(float scalar) const noexcept
    {
        return float2x2
        (
            m00 * scalar, m01 * scalar,
            m10 * scalar, m11 * scalar
        );
    }

    float2x2 float2x2::operator+(const float2x2& other) const noexcept
    {
        return float2x2
        (
            m00 + other.m00, m01 + other.m01,
            m10 + other.m10, m11 + other.m11
        );
    }

    float2x2 float2x2::operator-(const float2x2& other) const noexcept
    {
        return float2x2
        (
            m00 - other.m00, m01 - other.m01,
            m10 - other.m10, m11 - other.m11
        );
    }

    float2x2& float2x2::operator*=(const float2x2& other) noexcept { return *this = *this * other; }
    float2x2& float2x2::operator*=(float scalar) noexcept { return *this = *this * scalar; }
    float2x2& float2x2::operator+=(const float2x2& other) noexcept { return *this = *this + other; }
    float2x2& float2x2::operator-=(const float2x2& other) noexcept { return *this = *this - other; }

    bool float2x2::operator==(const float2x2& other) const noexcept
    {
        return std::fabs(m[0] - other.m[0]) < EPSILON && std::fabs(m[1] - other.m[1]) < EPSILON &&
               std::fabs(m[2] - other.m[2]) < EPSILON && std::fabs(m[3] - other.m[3]) < EPSILON;
    }

    bool float2x2::operator!=(const float2x2& other) const noexcept
    {
        return !(*this == other);
    }

    float2x2 operator*(float scalar, const float2x2& mat) noexcept
    {
        return mat * scalar;
    }

    // --------------------------------------------------------------------------
    // float3x3
    // --------------------------------------------------------------------------

    const float3x3 float3x3::Identity = float3x3(1.0f, 0.0f, 0.0f,
                                                 0.0f, 1.0f, 0.0f,
                                                 0.0f, 0.0f, 1.0f);

    const float3x3 float3x3::Zero = float3x3(0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f);

    float3x3::float3x3() noexcept 
    : m00(0), m01(0), m02(0),
      m10(0), m11(0), m12(0),
      m20(0), m21(0), m22(0) {}

    float3x3::float3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) noexcept
    : m00(m00), m01(m01), m02(m02),
      m10(m10), m11(m11), m12(m12),
      m20(m20), m21(m21), m22(m22) {}

    float3x3::float3x3(const float3& row0, const float3& row1, const float3& row2) noexcept
    : m00(row0.x), m01(row0.y), m02(row0.z),
      m10(row1.x), m11(row1.y), m12(row1.z),
      m20(row2.x), m21(row2.y), m22(row2.z) {}

    float* float3x3::data() noexcept { return m; }
    const float* float3x3::data() const noexcept { return m; }

    float3x3 float3x3::transposed() const noexcept
    {
        return float3x3(m00, m10, m20,
                        m01, m11, m21,
                        m02, m12, m22);
    }

    float float3x3::determinant() const noexcept
    {
        return m00 * (m11 * m22 - m12 * m21)
             - m01 * (m10 * m22 - m12 * m20)
             + m02 * (m10 * m21 - m11 * m20);
    }

    float3x3 float3x3::inverse() const noexcept
    {
        float det = determinant();
        if (std::fabs(det) < EPSILON)
        {
            return Zero;
        }

        float invDet = 1.0f / det;

        return float3x3(
            // Row 0
            (m11 * m22 - m12 * m21) * invDet,
            (m02 * m21 - m01 * m22) * invDet,
            (m01 * m12 - m02 * m11) * invDet,
            // Row 1
            (m12 * m20 - m10 * m22) * invDet,
            (m00 * m22 - m02 * m20) * invDet,
            (m02 * m10 - m00 * m12) * invDet,
            // Row 2
            (m10 * m21 - m11 * m20) * invDet,
            (m01 * m20 - m00 * m21) * invDet,
            (m00 * m11 - m01 * m10) * invDet
        );
    }

    float3 float3x3::operator*(const float3& vec) const noexcept
    {
        return float3
        (
            vec.x * m00 + vec.y * m01 + vec.z * m02,
            vec.x * m10 + vec.y * m11 + vec.z * m12,
            vec.x * m20 + vec.y * m21 + vec.z * m22
        );
    }

    float3x3 float3x3::operator*(const float3x3& other) const noexcept
    {
        return float3x3
        (
            m00 * other.m00 + m01 * other.m10 + m02 * other.m20, m00 * other.m01 + m01 * other.m11 + m02 * other.m21, m00 * other.m02 + m01 * other.m12 + m02 * other.m22,
            m10 * other.m00 + m11 * other.m10 + m12 * other.m20, m10 * other.m01 + m11 * other.m11 + m12 * other.m21, m10 * other.m02 + m11 * other.m12 + m12 * other.m22,
            m20 * other.m00 + m21 * other.m10 + m22 * other.m20, m20 * other.m01 + m21 * other.m11 + m22 * other.m21, m20 * other.m02 + m21 * other.m12 + m22 * other.m22
        );
    }

    float3x3 float3x3::operator*(float scalar) const noexcept
    {
        return float3x3
        (
            m00 * scalar, m01 * scalar, m02 * scalar,
            m10 * scalar, m11 * scalar, m12 * scalar,
            m20 * scalar, m21 * scalar, m22 * scalar
        );
    }

    float3x3 float3x3::operator+(const float3x3& other) const noexcept
    {
        return float3x3
        (
            m00 + other.m00, m01 + other.m01, m02 + other.m02,
            m10 + other.m10, m11 + other.m11, m12 + other.m12,
            m20 + other.m20, m21 + other.m21, m22 + other.m22
        );
    }

    float3x3 float3x3::operator-(const float3x3& other) const noexcept
    {
        return float3x3
        (
            m00 - other.m00, m01 - other.m01, m02 - other.m02,
            m10 - other.m10, m11 - other.m11, m12 - other.m12,
            m20 - other.m20, m21 - other.m21, m22 - other.m22
        );
    }

    float3x3& float3x3::operator*=(const float3x3& other) noexcept { return *this = *this * other; }
    float3x3& float3x3::operator*=(float scalar) noexcept { return *this = *this * scalar; }
    float3x3& float3x3::operator+=(const float3x3& other) noexcept { return *this = *this + other; }
    float3x3& float3x3::operator-=(const float3x3& other) noexcept { return *this = *this - other; }

    bool float3x3::operator==(const float3x3& other) const noexcept
    {
        return std::fabs(m[0] - other.m[0]) < EPSILON && std::fabs(m[1] - other.m[1]) < EPSILON &&
               std::fabs(m[2] - other.m[2]) < EPSILON && std::fabs(m[3] - other.m[3]) < EPSILON &&
               std::fabs(m[4] - other.m[4]) < EPSILON && std::fabs(m[5] - other.m[5]) < EPSILON &&
               std::fabs(m[6] - other.m[6]) < EPSILON && std::fabs(m[7] - other.m[7]) < EPSILON &&
               std::fabs(m[8] - other.m[8]) < EPSILON;
    }

    bool float3x3::operator!=(const float3x3& other) const noexcept
    {
        return !(*this == other);
    }

    float3x3 operator*(float scalar, const float3x3& mat) noexcept
    {
        return mat * scalar;
    }

    // --------------------------------------------------------------------------
    // float4x4
    // --------------------------------------------------------------------------

    const float4x4 float4x4::Identity = float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                                                 0.0f, 1.0f, 0.0f, 0.0f,
                                                 0.0f, 0.0f, 1.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 1.0f);

    const float4x4 float4x4::Zero = float4x4(0.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.0f);

    float4x4::float4x4() noexcept 
    : m00(0), m01(0), m02(0), m03(0),
      m10(0), m11(0), m12(0), m13(0),
      m20(0), m21(0), m22(0), m23(0),
      m30(0), m31(0), m32(0), m33(0) {}

    float4x4::float4x4(float m00, float m01, float m02, float m03,
                       float m10, float m11, float m12, float m13,
                       float m20, float m21, float m22, float m23,
                       float m30, float m31, float m32, float m33) noexcept
    : m00(m00), m01(m01), m02(m02), m03(m03),
      m10(m10), m11(m11), m12(m12), m13(m13),
      m20(m20), m21(m21), m22(m22), m23(m23),
      m30(m30), m31(m31), m32(m32), m33(m33) {}

    float4x4::float4x4(const float4& row0, const float4& row1, const float4& row2, const float4& row3) noexcept
    : m00(row0.x), m01(row0.y), m02(row0.z), m03(row0.w),
      m10(row1.x), m11(row1.y), m12(row1.z), m13(row1.w),
      m20(row2.x), m21(row2.y), m22(row2.z), m23(row2.w),
      m30(row3.x), m31(row3.y), m32(row3.z), m33(row3.w) {}

    float* float4x4::data() noexcept { return m; }
    const float* float4x4::data() const noexcept { return m; }

    float4x4 float4x4::transposed() const noexcept
    {
        return float4x4(m00, m10, m20, m30,
                        m01, m11, m21, m31,
                        m02, m12, m22, m32,
                        m03, m13, m23, m33);
    }

    float float4x4::determinant() const noexcept
    {
        return m00 * (m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31)) -
               m01 * (m10 * (m22 * m33 - m23 * m32) - m12 * (m20 * m33 - m23 * m30) + m13 * (m20 * m32 - m22 * m30)) +
               m02 * (m10 * (m21 * m33 - m23 * m31) - m11 * (m20 * m33 - m23 * m30) + m13 * (m20 * m31 - m21 * m30)) -
               m03 * (m10 * (m21 * m32 - m22 * m31) - m11 * (m20 * m32 - m22 * m30) + m12 * (m20 * m31 - m21 * m30));
    }

    float4x4 float4x4::inverse() const noexcept
    {
        float det = determinant();
        if (std::fabs(det) < EPSILON)
        {
            return Zero;
        }

        float invDet = 1.0f / det;

        return float4x4
        (
            (m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31)) * invDet,
            (m01 * (m22 * m33 - m23 * m32) - m02 * (m21 * m33 - m23 * m31) + m03 * (m21 * m32 - m22 * m31)) * invDet,
            (m01 * (m12 * m33 - m13 * m32) - m02 * (m11 * m33 - m13 * m31) + m03 * (m11 * m32 - m12 * m31)) * invDet,
            (m01 * (m12 * m23 - m13 * m22) - m02 * (m11 * m23 - m13 * m21) + m03 * (m11 * m22 - m12 * m21)) * invDet,

            (m10 * (m22 * m33 - m23 * m32) - m12 * (m20 * m33 - m23 * m30) + m13 * (m20 * m32 - m22 * m30)) * invDet,
            (m00 * (m22 * m33 - m23 * m32) - m02 * (m20 * m33 - m23 * m30) + m03 * (m20 * m32 - m22 * m30)) * invDet,
            (m00 * (m12 * m33 - m13 * m32) - m02 * (m10 * m33 - m13 * m30) + m03 * (m10 * m32 - m12 * m30)) * invDet,
            (m00 * (m12 * m23 - m13 * m22) - m02 * (m10 * m23 - m13 * m20) + m03 * (m10 * m22 - m12 * m20)) * invDet,

            (m10 * (m21 * m33 - m23 * m31) - m11 * (m20 * m33 - m23 * m30) + m13 * (m20 * m31 - m21 * m30)) * invDet,
            (m00 * (m21 * m33 - m23 * m31) - m01 * (m20 * m33 - m23 * m30) + m03 * (m20 * m31 - m21 * m30)) * invDet,
            (m00 * (m11 * m33 - m13 * m31) - m01 * (m10 * m33 - m13 * m30) + m03 * (m10 * m31 - m11 * m30)) * invDet,
            (m00 * (m11 * m23 - m13 * m21) - m01 * (m10 * m23 - m13 * m20) + m03 * (m10 * m21 - m11 * m20)) * invDet,

            (m10 * (m21 * m32 - m22 * m31) - m11 * (m20 * m32 - m22 * m30) + m12 * (m20 * m31 - m21 * m30)) * invDet,
            (m00 * (m21 * m32 - m22 * m31) - m01 * (m20 * m32 - m22 * m30) + m02 * (m20 * m31 - m21 * m30)) * invDet,
            (m00 * (m11 * m32 - m12 * m31) - m01 * (m10 * m32 - m12 * m30) + m02 * (m10 * m31 - m11 * m30)) * invDet,
            (m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) + m02 * (m10 * m21 - m11 * m20)) * invDet
        );
    }

    float4 float4x4::operator*(const float4& vec) const noexcept
    {
        return float4
        (
            vec.x * m00 + vec.y * m01 + vec.z * m02 + vec.w * m03,
            vec.x * m10 + vec.y * m11 + vec.z * m12 + vec.w * m13,
            vec.x * m20 + vec.y * m21 + vec.z * m22 + vec.w * m23,
            vec.x * m30 + vec.y * m31 + vec.z * m32 + vec.w * m33
        );
    }

    float4x4 float4x4::operator*(const float4x4& other) const noexcept
    {
        return float4x4
        (
            m00 * other.m00 + m01 * other.m10 + m02 * other.m20 + m03 * other.m30,
            m00 * other.m01 + m01 * other.m11 + m02 * other.m21 + m03 * other.m31,
            m00 * other.m02 + m01 * other.m12 + m02 * other.m22 + m03 * other.m32,
            m00 * other.m03 + m01 * other.m13 + m02 * other.m23 + m03 * other.m33,

            m10 * other.m00 + m11 * other.m10 + m12 * other.m20 + m13 * other.m30,
            m10 * other.m01 + m11 * other.m11 + m12 * other.m21 + m13 * other.m31,
            m10 * other.m02 + m11 * other.m12 + m12 * other.m22 + m13 * other.m32,
            m10 * other.m03 + m11 * other.m13 + m12 * other.m23 + m13 * other.m33,

            m20 * other.m00 + m21 * other.m10 + m22 * other.m20 + m23 * other.m30,
            m20 * other.m01 + m21 * other.m11 + m22 * other.m21 + m23 * other.m31,
            m20 * other.m02 + m21 * other.m12 + m22 * other.m22 + m23 * other.m32,
            m20 * other.m03 + m21 * other.m13 + m22 * other.m23 + m23 * other.m33,

            m30 * other.m00 + m31 * other.m10 + m32 * other.m20 + m33 * other.m30,
            m30 * other.m01 + m31 * other.m11 + m32 * other.m21 + m33 * other.m31,
            m30 * other.m02 + m31 * other.m12 + m32 * other.m22 + m33 * other.m32,
            m30 * other.m03 + m31 * other.m13 + m32 * other.m23 + m33 * other.m33
        );
    }

    float4x4 float4x4::operator*(float scalar) const noexcept
    {
        return float4x4
        (
            m00 * scalar, m01 * scalar, m02 * scalar, m03 * scalar,
            m10 * scalar, m11 * scalar, m12 * scalar, m13 * scalar,
            m20 * scalar, m21 * scalar, m22 * scalar, m23 * scalar,
            m30 * scalar, m31 * scalar, m32 * scalar, m33 * scalar
        );
    }

    float4x4& float4x4::operator*=(const float4x4& other) noexcept
    {
        *this = *this * other;
        return *this;
    }

    float4x4& float4x4::operator*=(float scalar) noexcept
    {
        m00 *= scalar; m01 *= scalar; m02 *= scalar; m03 *= scalar;
        m10 *= scalar; m11 *= scalar; m12 *= scalar; m13 *= scalar;
        m20 *= scalar; m21 *= scalar; m22 *= scalar; m23 *= scalar;
        m30 *= scalar; m31 *= scalar; m32 *= scalar; m33 *= scalar;
        return *this;
    }

    float4x4 float4x4::operator+(const float4x4& other) const noexcept
    {
        return float4x4
        (
            m00 + other.m00, m01 + other.m01, m02 + other.m02, m03 + other.m03,
            m10 + other.m10, m11 + other.m11, m12 + other.m12, m13 + other.m13,
            m20 + other.m20, m21 + other.m21, m22 + other.m22, m23 + other.m23,
            m30 + other.m30, m31 + other.m31, m32 + other.m32, m33 + other.m33
        );
    }

    float4x4& float4x4::operator+=(const float4x4& other) noexcept
    {
        m00 += other.m00; m01 += other.m01; m02 += other.m02; m03 += other.m03;
        m10 += other.m10; m11 += other.m11; m12 += other.m12; m13 += other.m13;
        m20 += other.m20; m21 += other.m21; m22 += other.m22; m23 += other.m23;
        m30 += other.m30; m31 += other.m31; m32 += other.m32; m33 += other.m33;
        return *this;
    }

    float4x4 float4x4::operator-(const float4x4& other) const noexcept
    {
        return float4x4
        (
            m00 - other.m00, m01 - other.m01, m02 - other.m02, m03 - other.m03,
            m10 - other.m10, m11 - other.m11, m12 - other.m12, m13 - other.m13,
            m20 - other.m20, m21 - other.m21, m22 - other.m22, m23 - other.m23,
            m30 - other.m30, m31 - other.m31, m32 - other.m32, m33 - other.m33
        );
    }

    float4x4& float4x4::operator-=(const float4x4& other) noexcept
    {
        m00 -= other.m00; m01 -= other.m01; m02 -= other.m02; m03 -= other.m03;
        m10 -= other.m10; m11 -= other.m11; m12 -= other.m12; m13 -= other.m13;
        m20 -= other.m20; m21 -= other.m21; m22 -= other.m22; m23 -= other.m23;
        m30 -= other.m30; m31 -= other.m31; m32 -= other.m32; m33 -= other.m33;
        return *this;
    }

    bool float4x4::operator==(const float4x4& other) const noexcept
    {
        return std::fabs(m00 - other.m00) < EPSILON && std::fabs(m01 - other.m01) < EPSILON && 
               std::fabs(m02 - other.m02) < EPSILON && std::fabs(m03 - other.m03) < EPSILON &&
               std::fabs(m10 - other.m10) < EPSILON && std::fabs(m11 - other.m11) < EPSILON && 
               std::fabs(m12 - other.m12) < EPSILON && std::fabs(m13 - other.m13) < EPSILON &&
               std::fabs(m20 - other.m20) < EPSILON && std::fabs(m21 - other.m21) < EPSILON && 
               std::fabs(m22 - other.m22) < EPSILON && std::fabs(m23 - other.m23) < EPSILON &&
               std::fabs(m30 - other.m30) < EPSILON && std::fabs(m31 - other.m31) < EPSILON &&
               std::fabs(m32 - other.m32) < EPSILON && std::fabs(m33 - other.m33) < EPSILON;
    }

    bool float4x4::operator!=(const float4x4& other) const noexcept
    {
        return !(*this == other);
    }

    float4x4 operator*(float scalar, const float4x4& mat) noexcept
    {
        return float4x4
        (
            mat.m00 * scalar, mat.m01 * scalar, mat.m02 * scalar, mat.m03 * scalar,
            mat.m10 * scalar, mat.m11 * scalar, mat.m12 * scalar, mat.m13 * scalar,
            mat.m20 * scalar, mat.m21 * scalar, mat.m22 * scalar, mat.m23 * scalar,
            mat.m30 * scalar, mat.m31 * scalar, mat.m32 * scalar, mat.m33 * scalar
        );
    }
    
    // --------------------------------------------------------------------------
    // camera and transformations
    // --------------------------------------------------------------------------

    float4x4 CreateViewMatrix(const float3& eye, const float3& target, const float3& up) noexcept
    {
        float3 zAxis = (eye - target).normalized();
        float3 xAxis = cross(up, zAxis).normalized();
        float3 yAxis = cross(zAxis, xAxis);

        float4x4 viewMatrix = 
        {
            xAxis.x, xAxis.y, xAxis.z, -dot(xAxis, eye),
            yAxis.x, yAxis.y, yAxis.z, -dot(yAxis, eye),
            zAxis.x, zAxis.y, zAxis.z, -dot(zAxis, eye),
            0.0f,    0.0f,    0.0f,    1.0f
        };

        return viewMatrix;
    }
    
    float4x4 CreatePerspectiveProjectionMatrix(float fov, float aspectRatio, float nearZ, float farZ) noexcept
    {
        float tanHalfFov = tan(fov * 0.5f);
        float height = 1.0f / tanHalfFov;
        float width = height / aspectRatio;
        float range = farZ - nearZ;

        float4x4 projectionMatrix = 
        {
            width, 0.0f,  0.0f,                          0.0f,
            0.0f,  height, 0.0f,                          0.0f,
            0.0f,  0.0f,  -(farZ + nearZ) / range,       -1.0f,
            0.0f,  0.0f,  -2.0f * farZ * nearZ / range,  0.0f
        };

        return projectionMatrix;
    }

    float4x4 CreateTranslationMatrix(const float3& translation) noexcept
    {
        float4x4 translationMatrix =
        {
            1.0f, 0.0f, 0.0f, translation.x,
            0.0f, 1.0f, 0.0f, translation.y,
            0.0f, 0.0f, 1.0f, translation.z,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        return translationMatrix;
    }

    float4x4 CreateRotationMatrixYawPitchRoll(float yaw, float pitch, float roll) noexcept
    {
        float cosYaw = cos(yaw);
        float sinYaw = sin(yaw);
        float cosPitch = cos(pitch);
        float sinPitch = sin(pitch);
        float cosRoll = cos(roll);
        float sinRoll = sin(roll);

        // Y (yaw)
        float4x4 yawMatrix = 
        {
            cosYaw, 0.0f, -sinYaw, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            sinYaw, 0.0f, cosYaw, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        // X (pitch)
        float4x4 pitchMatrix =
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cosPitch, -sinPitch, 0.0f,
            0.0f, sinPitch, cosPitch, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        // Z (roll)
        float4x4 rollMatrix = 
        {
            cosRoll, -sinRoll, 0.0f, 0.0f,
            sinRoll, cosRoll, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        // Yaw -> Pitch -> Roll
        return yawMatrix * pitchMatrix * rollMatrix;
    }

    float4x4 CreateScaleMatrix(const float3& scale) noexcept
    {
        float4x4 scaleMatrix = 
        {
            scale.x, 0.0f, 0.0f, 0.0f,
            0.0f, scale.y, 0.0f, 0.0f,
            0.0f, 0.0f, scale.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        return scaleMatrix;
    }

    float4x4 CreateOrthographicProjectionMatrix(float width, float height, float nearZ, float farZ) noexcept
    {
        float range = farZ - nearZ;

        float4x4 orthoMatrix = 
        {
            2.0f / width, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / height, 0.0f, 0.0f,
            0.0f, 0.0f, -2.0f / range, 0.0f,
            0.0f, 0.0f, -(farZ + nearZ) / range, 1.0f
        };

        return orthoMatrix;
    }
}
