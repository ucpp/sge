#include <iostream>
#include <iomanip>
#include <gtest/gtest.h>
#include "core/sge_math.h"
using namespace SGE;

TEST(sge_math, ConvertToRadians) 
{
    EXPECT_NEAR(ConvertToRadians(0.0f), 0.0f, EPSILON);
    EXPECT_NEAR(ConvertToRadians(90.0f), PI / 2, EPSILON);
    EXPECT_NEAR(ConvertToRadians(180.0f), PI, EPSILON);
    EXPECT_NEAR(ConvertToRadians(360.0f), 2 * PI, EPSILON);
}

TEST(sge_math, ConvertToDegrees)
{
    EXPECT_NEAR(ConvertToDegrees(0.0f), 0.0f, EPSILON);
    EXPECT_NEAR(ConvertToDegrees(PI / 2), 90.0f, EPSILON);
    EXPECT_NEAR(ConvertToDegrees(PI), 180.0f, EPSILON);
    EXPECT_NEAR(ConvertToDegrees(2 * PI), 360.0f, EPSILON);
}

// --------------------------------------------------------------------------
// float2
// --------------------------------------------------------------------------

TEST(sge_math_float2, DefaultConstructor) 
{
    float2 vec;
    EXPECT_EQ(vec.x, 0.0f);
    EXPECT_EQ(vec.y, 0.0f);
}

TEST(sge_math_float2, ParameterizedConstructor) 
{
    float2 vec(1.0f, 2.0f);
    EXPECT_EQ(vec.x, 1.0f);
    EXPECT_EQ(vec.y, 2.0f);
}

TEST(sge_math_float2, DataPointer) 
{
    float2 vec(3.0f, 4.0f);
    float* data = vec.data();
    EXPECT_EQ(data[0], 3.0f);
    EXPECT_EQ(data[1], 4.0f);
}

TEST(sge_math_float2, ConstDataPointer) 
{
    const float2 vec(5.0f, 6.0f);
    const float* data = vec.data();
    EXPECT_EQ(data[0], 5.0f);
    EXPECT_EQ(data[1], 6.0f);
}

TEST(sge_math_float2, ZeroConstant) 
{
    EXPECT_EQ(float2::Zero.x, 0.0f);
    EXPECT_EQ(float2::Zero.y, 0.0f);
}

TEST(sge_math_float2, Length) 
{
    float2 vec(3.0f, 4.0f);
    EXPECT_EQ(vec.length(), 5.0f);
}

TEST(sge_math_float2, Normalized) 
{
    float2 vec(3.0f, 4.0f);
    float2 norm = vec.normalized();
    EXPECT_NEAR(norm.x, 0.6f, EPSILON);
    EXPECT_NEAR(norm.y, 0.8f, EPSILON);
}

TEST(sge_math_float2, Normalize) 
{
    float2 vec(3.0f, 4.0f);
    vec.normalize();
    EXPECT_NEAR(vec.x, 0.6f, EPSILON);
    EXPECT_NEAR(vec.y, 0.8f, EPSILON);
}

TEST(sge_math_float2, AdditionAssignment) 
{
    float2 vec1(1.0f, 2.0f);
    float2 vec2(3.0f, 4.0f);
    vec1 += vec2;
    EXPECT_EQ(vec1.x, 4.0f);
    EXPECT_EQ(vec1.y, 6.0f);
}

TEST(sge_math_float2, SubtractionAssignment) 
{
    float2 vec1(5.0f, 6.0f);
    float2 vec2(2.0f, 3.0f);
    vec1 -= vec2;
    EXPECT_EQ(vec1.x, 3.0f);
    EXPECT_EQ(vec1.y, 3.0f);
}

TEST(sge_math_float2, MultiplicationAssignment) 
{
    float2 vec(2.0f, 3.0f);
    vec *= 2.0f;
    EXPECT_EQ(vec.x, 4.0f);
    EXPECT_EQ(vec.y, 6.0f);
}

TEST(sge_math_float2, DivisionAssignment) 
{
    float2 vec(4.0f, 6.0f);
    vec /= 2.0f;
    EXPECT_EQ(vec.x, 2.0f);
    EXPECT_EQ(vec.y, 3.0f);
}

TEST(sge_math_float2, EqualityOperator) 
{
    float2 vec1(1.0f, 2.0f);
    float2 vec2(1.0f, 2.0f);
    EXPECT_TRUE(vec1 == vec2);
}

TEST(sge_math_float2, InequalityOperator) 
{
    float2 vec1(1.0f, 2.0f);
    float2 vec2(3.0f, 4.0f);
    EXPECT_TRUE(vec1 != vec2);
}

TEST(sge_math_float2, NegationOperator) 
{
    float2 vec(1.0f, 2.0f);
    float2 neg = -vec;
    EXPECT_EQ(neg.x, -1.0f);
    EXPECT_EQ(neg.y, -2.0f);
}

TEST(sge_math_float2, DotProduct) 
{
    float2 vec1(1.0f, 2.0f);
    float2 vec2(3.0f, 4.0f);
    EXPECT_EQ(dot(vec1, vec2), 11.0f);
}

TEST(sge_math_float2, Reflect) 
{
    float2 vec(1.0f, -1.0f);
    float2 normal(0.0f, 1.0f);
    float2 reflected = reflect(vec, normal);
    EXPECT_NEAR(reflected.x, 1.0f, EPSILON);
    EXPECT_NEAR(reflected.y, 1.0f, EPSILON);
}

TEST(sge_math_float2, Project) 
{
    float2 vec(3.0f, 4.0f);
    float2 onto(1.0f, 0.0f);
    float2 projected = project(vec, onto);
    EXPECT_NEAR(projected.x, 3.0f, EPSILON);
    EXPECT_NEAR(projected.y, 0.0f, EPSILON);
}

TEST(sge_math_float2, Distance) 
{
    float2 vec1(1.0f, 2.0f);
    float2 vec2(4.0f, 6.0f);
    EXPECT_NEAR(distance(vec1, vec2), 5.0f, EPSILON);
}

TEST(sge_math_float2, Angle) 
{
    float2 vec1(1.0f, 0.0f);
    float2 vec2(0.0f, 1.0f);
    EXPECT_NEAR(angle(vec1, vec2), PI / 2, EPSILON);
}

TEST(sge_math_float2, Lerp) 
{
    float2 vec1(1.0f, 2.0f);
    float2 vec2(3.0f, 4.0f);
    float2 result = lerp(vec1, vec2, 0.5f);
    EXPECT_NEAR(result.x, 2.0f, EPSILON);
    EXPECT_NEAR(result.y, 3.0f, EPSILON);
}

TEST(sge_math_float2, AdditionOperator) 
{
    float2 vec1(1.0f, 2.0f);
    float2 vec2(3.0f, 4.0f);
    float2 result = vec1 + vec2;
    EXPECT_EQ(result.x, 4.0f);
    EXPECT_EQ(result.y, 6.0f);
}

TEST(sge_math_float2, SubtractionOperator) 
{
    float2 vec1(5.0f, 6.0f);
    float2 vec2(2.0f, 3.0f);
    float2 result = vec1 - vec2;
    EXPECT_EQ(result.x, 3.0f);
    EXPECT_EQ(result.y, 3.0f);
}

TEST(sge_math_float2, MultiplicationOperator) 
{
    float2 vec(2.0f, 3.0f);
    float2 result = vec * 2.0f;
    EXPECT_EQ(result.x, 4.0f);
    EXPECT_EQ(result.y, 6.0f);
}

TEST(sge_math_float2, ScalarMultiplicationOperator) 
{
    float2 vec(2.0f, 3.0f);
    float2 result = 2.0f * vec;
    EXPECT_EQ(result.x, 4.0f);
    EXPECT_EQ(result.y, 6.0f);
}

TEST(sge_math_float2, DivisionOperator) 
{
    float2 vec(4.0f, 6.0f);
    float2 result = vec / 2.0f;
    EXPECT_EQ(result.x, 2.0f);
    EXPECT_EQ(result.y, 3.0f);
}

// --------------------------------------------------------------------------
// float3
// --------------------------------------------------------------------------

TEST(sge_math_float3, DefaultConstructor) 
{
    float3 vec;
    EXPECT_EQ(vec.x, 0.0f);
    EXPECT_EQ(vec.y, 0.0f);
    EXPECT_EQ(vec.z, 0.0f);
}

TEST(sge_math_float3, ParameterizedConstructor) 
{
    float3 vec(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(vec.x, 1.0f);
    EXPECT_EQ(vec.y, 2.0f);
    EXPECT_EQ(vec.z, 3.0f);
}

TEST(sge_math_float3, DataPointer) 
{
    float3 vec(3.0f, 4.0f, 5.0f);
    float* data = vec.data();
    EXPECT_EQ(data[0], 3.0f);
    EXPECT_EQ(data[1], 4.0f);
    EXPECT_EQ(data[2], 5.0f);
}

TEST(sge_math_float3, ConstDataPointer) 
{
    const float3 vec(5.0f, 6.0f, 7.0f);
    const float* data = vec.data();
    EXPECT_EQ(data[0], 5.0f);
    EXPECT_EQ(data[1], 6.0f);
    EXPECT_EQ(data[2], 7.0f);
}

TEST(sge_math_float3, IndexOperator) 
{
    float3 vec(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(vec[0], 1.0f);
    EXPECT_EQ(vec[1], 2.0f);
    EXPECT_EQ(vec[2], 3.0f);
}

TEST(sge_math_float3, ConstIndexOperator) 
{
    const float3 vec(4.0f, 5.0f, 6.0f);
    EXPECT_EQ(vec[0], 4.0f);
    EXPECT_EQ(vec[1], 5.0f);
    EXPECT_EQ(vec[2], 6.0f);
}

TEST(sge_math_float3, ZeroConstant) 
{
    EXPECT_EQ(float3::Zero.x, 0.0f);
    EXPECT_EQ(float3::Zero.y, 0.0f);
    EXPECT_EQ(float3::Zero.z, 0.0f);
}

TEST(sge_math_float3, Length) 
{
    float3 vec(1.0f, 2.0f, 2.0f);
    EXPECT_NEAR(vec.length(), 3.0f, EPSILON);
}

TEST(sge_math_float3, Normalized) 
{
    float3 vec(1.0f, 2.0f, 2.0f);
    float3 norm = vec.normalized();
    EXPECT_NEAR(norm.x, 1.0f / 3.0f, EPSILON);
    EXPECT_NEAR(norm.y, 2.0f / 3.0f, EPSILON);
    EXPECT_NEAR(norm.z, 2.0f / 3.0f, EPSILON);
}

TEST(sge_math_float3, Normalize) 
{
    float3 vec(1.0f, 2.0f, 2.0f);
    vec.normalize();
    EXPECT_NEAR(vec.x, 1.0f / 3.0f, EPSILON);
    EXPECT_NEAR(vec.y, 2.0f / 3.0f, EPSILON);
    EXPECT_NEAR(vec.z, 2.0f / 3.0f, EPSILON);
}

TEST(sge_math_float3, AdditionAssignment) 
{
    float3 vec1(1.0f, 2.0f, 3.0f);
    float3 vec2(4.0f, 5.0f, 6.0f);
    vec1 += vec2;
    EXPECT_EQ(vec1.x, 5.0f);
    EXPECT_EQ(vec1.y, 7.0f);
    EXPECT_EQ(vec1.z, 9.0f);
}

TEST(sge_math_float3, SubtractionAssignment) 
{
    float3 vec1(5.0f, 6.0f, 7.0f);
    float3 vec2(2.0f, 3.0f, 4.0f);
    vec1 -= vec2;
    EXPECT_EQ(vec1.x, 3.0f);
    EXPECT_EQ(vec1.y, 3.0f);
    EXPECT_EQ(vec1.z, 3.0f);
}

TEST(sge_math_float3, MultiplicationAssignment) 
{
    float3 vec(2.0f, 3.0f, 4.0f);
    vec *= 2.0f;
    EXPECT_EQ(vec.x, 4.0f);
    EXPECT_EQ(vec.y, 6.0f);
    EXPECT_EQ(vec.z, 8.0f);
}

TEST(sge_math_float3, DivisionAssignment) 
{
    float3 vec(4.0f, 6.0f, 8.0f);
    vec /= 2.0f;
    EXPECT_EQ(vec.x, 2.0f);
    EXPECT_EQ(vec.y, 3.0f);
    EXPECT_EQ(vec.z, 4.0f);
}

TEST(sge_math_float3, EqualityOperator) 
{
    float3 vec1(1.0f, 2.0f, 3.0f);
    float3 vec2(1.0f, 2.0f, 3.0f);
    EXPECT_TRUE(vec1 == vec2);
}

TEST(sge_math_float3, InequalityOperator) 
{
    float3 vec1(1.0f, 2.0f, 3.0f);
    float3 vec2(4.0f, 5.0f, 6.0f);
    EXPECT_TRUE(vec1 != vec2);
}

TEST(sge_math_float3, NegationOperator) 
{
    float3 vec(1.0f, 2.0f, 3.0f);
    float3 neg = -vec;
    EXPECT_EQ(neg.x, -1.0f);
    EXPECT_EQ(neg.y, -2.0f);
    EXPECT_EQ(neg.z, -3.0f);
}

TEST(sge_math_float3, CrossProduct) 
{
    float3 vec1(1.0f, 0.0f, 0.0f);
    float3 vec2(0.0f, 1.0f, 0.0f);
    float3 result = cross(vec1, vec2);
    EXPECT_EQ(result.x, 0.0f);
    EXPECT_EQ(result.y, 0.0f);
    EXPECT_EQ(result.z, 1.0f);
}

TEST(sge_math_float3, DotProduct) 
{
    float3 vec1(1.0f, 2.0f, 3.0f);
    float3 vec2(4.0f, 5.0f, 6.0f);
    EXPECT_EQ(dot(vec1, vec2), 32.0f);
}

TEST(sge_math_float3, Reflect) 
{
    float3 vec(1.0f, -1.0f, 0.0f);
    float3 normal(0.0f, 1.0f, 0.0f);
    float3 reflected = reflect(vec, normal);
    EXPECT_NEAR(reflected.x, 1.0f, EPSILON);
    EXPECT_NEAR(reflected.y, 1.0f, EPSILON);
    EXPECT_NEAR(reflected.z, 0.0f, EPSILON);
}

TEST(sge_math_float3, Project) 
{
    float3 vec(3.0f, 4.0f, 0.0f);
    float3 onto(1.0f, 0.0f, 0.0f);
    float3 projected = project(vec, onto);
    EXPECT_NEAR(projected.x, 3.0f, EPSILON);
    EXPECT_NEAR(projected.y, 0.0f, EPSILON);
    EXPECT_NEAR(projected.z, 0.0f, EPSILON);
}

TEST(sge_math_float3, Distance) 
{
    float3 vec1(1.0f, 2.0f, 3.0f);
    float3 vec2(4.0f, 6.0f, 8.0f);
    EXPECT_NEAR(distance(vec1, vec2), sqrtf(50.0f), EPSILON);
}

TEST(sge_math_float3, Angle) 
{
    float3 vec1(1.0f, 0.0f, 0.0f);
    float3 vec2(0.0f, 1.0f, 0.0f);
    EXPECT_NEAR(angle(vec1, vec2), PI / 2, EPSILON);
}

TEST(sge_math_float3, Lerp) 
{
    float3 vec1(1.0f, 2.0f, 3.0f);
    float3 vec2(3.0f, 4.0f, 5.0f);
    float3 result = lerp(vec1, vec2, 0.5f);
    EXPECT_NEAR(result.x, 2.0f, EPSILON);
    EXPECT_NEAR(result.y, 3.0f, EPSILON);
    EXPECT_NEAR(result.z, 4.0f, EPSILON);
}

TEST(sge_math_float3, AdditionOperator) 
{
    float3 vec1(1.0f, 2.0f, 3.0f);
    float3 vec2(4.0f, 5.0f, 6.0f);
    float3 result = vec1 + vec2;
    EXPECT_EQ(result.x, 5.0f);
    EXPECT_EQ(result.y, 7.0f);
    EXPECT_EQ(result.z, 9.0f);
}

TEST(sge_math_float3, SubtractionOperator) 
{
    float3 vec1(5.0f, 6.0f, 7.0f);
    float3 vec2(2.0f, 3.0f, 4.0f);
    float3 result = vec1 - vec2;
    EXPECT_EQ(result.x, 3.0f);
    EXPECT_EQ(result.y, 3.0f);
    EXPECT_EQ(result.z, 3.0f);
}

TEST(sge_math_float3, MultiplicationOperator) 
{
    float3 vec(2.0f, 3.0f, 4.0f);
    float3 result = vec * 2.0f;
    EXPECT_EQ(result.x, 4.0f);
    EXPECT_EQ(result.y, 6.0f);
    EXPECT_EQ(result.z, 8.0f);
}

TEST(sge_math_float3, ScalarMultiplicationOperator) 
{
    float3 vec(2.0f, 3.0f, 4.0f);
    float3 result = 2.0f * vec;
    EXPECT_EQ(result.x, 4.0f);
    EXPECT_EQ(result.y, 6.0f);
    EXPECT_EQ(result.z, 8.0f);
}

TEST(sge_math_float3, DivisionOperator) 
{
    float3 vec(4.0f, 6.0f, 8.0f);
    float3 result = vec / 2.0f;
    EXPECT_EQ(result.x, 2.0f);
    EXPECT_EQ(result.y, 3.0f);
    EXPECT_EQ(result.z, 4.0f);
}

// --------------------------------------------------------------------------
// float4
// --------------------------------------------------------------------------

TEST(sge_math_float4, DefaultConstructor) 
{
    float4 vec;
    EXPECT_EQ(vec.x, 0.0f);
    EXPECT_EQ(vec.y, 0.0f);
    EXPECT_EQ(vec.z, 0.0f);
    EXPECT_EQ(vec.w, 0.0f);
}

TEST(sge_math_float4, ParameterizedConstructor) 
{
    float4 vec(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec.x, 1.0f);
    EXPECT_EQ(vec.y, 2.0f);
    EXPECT_EQ(vec.z, 3.0f);
    EXPECT_EQ(vec.w, 4.0f);
}

TEST(sge_math_float4, DataPointer) 
{
    float4 vec(3.0f, 4.0f, 5.0f, 6.0f);
    float* data = vec.data();
    EXPECT_EQ(data[0], 3.0f);
    EXPECT_EQ(data[1], 4.0f);
    EXPECT_EQ(data[2], 5.0f);
    EXPECT_EQ(data[3], 6.0f);
}

TEST(sge_math_float4, ConstDataPointer) 
{
    const float4 vec(5.0f, 6.0f, 7.0f, 8.0f);
    const float* data = vec.data();
    EXPECT_EQ(data[0], 5.0f);
    EXPECT_EQ(data[1], 6.0f);
    EXPECT_EQ(data[2], 7.0f);
    EXPECT_EQ(data[3], 8.0f);
}

TEST(sge_math_float4, IndexOperator) 
{
    float4 vec(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(vec[0], 1.0f);
    EXPECT_EQ(vec[1], 2.0f);
    EXPECT_EQ(vec[2], 3.0f);
    EXPECT_EQ(vec[3], 4.0f);
}

TEST(sge_math_float4, ConstIndexOperator) 
{
    const float4 vec(4.0f, 5.0f, 6.0f, 7.0f);
    EXPECT_EQ(vec[0], 4.0f);
    EXPECT_EQ(vec[1], 5.0f);
    EXPECT_EQ(vec[2], 6.0f);
    EXPECT_EQ(vec[3], 7.0f);
}

TEST(sge_math_float4, ZeroConstant) 
{
    EXPECT_EQ(float4::Zero.x, 0.0f);
    EXPECT_EQ(float4::Zero.y, 0.0f);
    EXPECT_EQ(float4::Zero.z, 0.0f);
    EXPECT_EQ(float4::Zero.w, 0.0f);
}

TEST(sge_math_float4, AdditionAssignment) 
{
    float4 vec1(1.0f, 2.0f, 3.0f, 4.0f);
    float4 vec2(4.0f, 5.0f, 6.0f, 7.0f);
    vec1 += vec2;
    EXPECT_EQ(vec1.x, 5.0f);
    EXPECT_EQ(vec1.y, 7.0f);
    EXPECT_EQ(vec1.z, 9.0f);
    EXPECT_EQ(vec1.w, 11.0f);
}

TEST(sge_math_float4, SubtractionAssignment) 
{
    float4 vec1(5.0f, 6.0f, 7.0f, 8.0f);
    float4 vec2(2.0f, 3.0f, 4.0f, 5.0f);
    vec1 -= vec2;
    EXPECT_EQ(vec1.x, 3.0f);
    EXPECT_EQ(vec1.y, 3.0f);
    EXPECT_EQ(vec1.z, 3.0f);
    EXPECT_EQ(vec1.w, 3.0f);
}

TEST(sge_math_float4, MultiplicationAssignment) 
{
    float4 vec(2.0f, 3.0f, 4.0f, 5.0f);
    vec *= 2.0f;
    EXPECT_EQ(vec.x, 4.0f);
    EXPECT_EQ(vec.y, 6.0f);
    EXPECT_EQ(vec.z, 8.0f);
    EXPECT_EQ(vec.w, 10.0f);
}

TEST(sge_math_float4, DivisionAssignment) 
{
    float4 vec(4.0f, 6.0f, 8.0f, 10.0f);
    vec /= 2.0f;
    EXPECT_EQ(vec.x, 2.0f);
    EXPECT_EQ(vec.y, 3.0f);
    EXPECT_EQ(vec.z, 4.0f);
    EXPECT_EQ(vec.w, 5.0f);
}

TEST(sge_math_float4, EqualityOperator) 
{
    float4 vec1(1.0f, 2.0f, 3.0f, 4.0f);
    float4 vec2(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_TRUE(vec1 == vec2);
}

TEST(sge_math_float4, InequalityOperator) 
{
    float4 vec1(1.0f, 2.0f, 3.0f, 4.0f);
    float4 vec2(4.0f, 5.0f, 6.0f, 7.0f);
    EXPECT_TRUE(vec1 != vec2);
}

TEST(sge_math_float4, NegationOperator) 
{
    float4 vec(1.0f, 2.0f, 3.0f, 4.0f);
    float4 neg = -vec;
    EXPECT_EQ(neg.x, -1.0f);
    EXPECT_EQ(neg.y, -2.0f);
    EXPECT_EQ(neg.z, -3.0f);
    EXPECT_EQ(neg.w, -4.0f);
}

TEST(sge_math_float4, AdditionOperator) 
{
    float4 vec1(1.0f, 2.0f, 3.0f, 4.0f);
    float4 vec2(4.0f, 5.0f, 6.0f, 7.0f);
    float4 result = vec1 + vec2;
    EXPECT_EQ(result.x, 5.0f);
    EXPECT_EQ(result.y, 7.0f);
    EXPECT_EQ(result.z, 9.0f);
    EXPECT_EQ(result.w, 11.0f);
}

TEST(sge_math_float4, SubtractionOperator) 
{
    float4 vec1(5.0f, 6.0f, 7.0f, 8.0f);
    float4 vec2(2.0f, 3.0f, 4.0f, 5.0f);
    float4 result = vec1 - vec2;
    EXPECT_EQ(result.x, 3.0f);
    EXPECT_EQ(result.y, 3.0f);
    EXPECT_EQ(result.z, 3.0f);
    EXPECT_EQ(result.w, 3.0f);
}

TEST(sge_math_float4, MultiplicationOperator) 
{
    float4 vec(2.0f, 3.0f, 4.0f, 5.0f);
    float4 result = vec * 2.0f;
    EXPECT_EQ(result.x, 4.0f);
    EXPECT_EQ(result.y, 6.0f);
    EXPECT_EQ(result.z, 8.0f);
    EXPECT_EQ(result.w, 10.0f);
}

TEST(sge_math_float4, ScalarMultiplicationOperator) 
{
    float4 vec(2.0f, 3.0f, 4.0f, 5.0f);
    float4 result = vec * 2.0f;
    EXPECT_EQ(result.x, 4.0f);
    EXPECT_EQ(result.y, 6.0f);
    EXPECT_EQ(result.z, 8.0f);
    EXPECT_EQ(result.w, 10.0f);
}

TEST(sge_math_float4, DivisionOperator) 
{
    float4 vec(4.0f, 6.0f, 8.0f, 10.0f);
    float4 result = vec / 2.0f;
    EXPECT_EQ(result.x, 2.0f);
    EXPECT_EQ(result.y, 3.0f);
    EXPECT_EQ(result.z, 4.0f);
    EXPECT_EQ(result.w, 5.0f);
}

// --------------------------------------------------------------------------
// float2x2
// --------------------------------------------------------------------------

TEST(sge_math_float2x2, DefaultConstructor)
{
    float2x2 mat;
    EXPECT_EQ(mat.m00, 0.0f);
    EXPECT_EQ(mat.m01, 0.0f);
    EXPECT_EQ(mat.m10, 0.0f);
    EXPECT_EQ(mat.m11, 0.0f);
}

TEST(sge_math_float2x2, ParameterizedConstructor)
{
    float2x2 mat(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(mat.m00, 1.0f);
    EXPECT_EQ(mat.m01, 2.0f);
    EXPECT_EQ(mat.m10, 3.0f);
    EXPECT_EQ(mat.m11, 4.0f);
}

TEST(sge_math_float2x2, ConstructorFromVectors)
{
    float2 row0(1.0f, 2.0f);
    float2 row1(3.0f, 4.0f);
    
    float2x2 mat(row0, row1);

    EXPECT_EQ(mat.m00, 1.0f);
    EXPECT_EQ(mat.m01, 2.0f);
    EXPECT_EQ(mat.m10, 3.0f);
    EXPECT_EQ(mat.m11, 4.0f);
}

TEST(sge_math_float2x2, IdentityMatrix)
{
    EXPECT_EQ(float2x2::Identity.m00, 1.0f);
    EXPECT_EQ(float2x2::Identity.m01, 0.0f);
    EXPECT_EQ(float2x2::Identity.m10, 0.0f);
    EXPECT_EQ(float2x2::Identity.m11, 1.0f);
}

TEST(sge_math_float2x2, ZeroMatrix)
{
    EXPECT_EQ(float2x2::Zero.m00, 0.0f);
    EXPECT_EQ(float2x2::Zero.m01, 0.0f);
    EXPECT_EQ(float2x2::Zero.m10, 0.0f);
    EXPECT_EQ(float2x2::Zero.m11, 0.0f);
}

TEST(sge_math_float2x2, Transpose)
{
    float2x2 mat(1.0f, 2.0f, 3.0f, 4.0f);
    float2x2 transposed = mat.transposed();
    EXPECT_EQ(transposed.m00, 1.0f);
    EXPECT_EQ(transposed.m01, 3.0f);
    EXPECT_EQ(transposed.m10, 2.0f);
    EXPECT_EQ(transposed.m11, 4.0f);
}

TEST(sge_math_float2x2, Determinant)
{
    float2x2 mat(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(mat.determinant(), -2.0f);
}

TEST(sge_math_float2x2, Inverse)
{
    float2x2 mat(4.0f, 7.0f, 2.0f, 6.0f);
    float2x2 inv = mat.inverse();
    EXPECT_NEAR(inv.m00, 0.6f, EPSILON);
    EXPECT_NEAR(inv.m01, -0.7f, EPSILON);
    EXPECT_NEAR(inv.m10, -0.2f, EPSILON);
    EXPECT_NEAR(inv.m11, 0.4f, EPSILON);
}

TEST(sge_math_float2x2, MatrixVectorMultiplication)
{
    float2x2 mat(1.0f, 2.0f, 3.0f, 4.0f);
    float2 vec(1.0f, 1.0f);
    float2 result = mat * vec;
    EXPECT_EQ(result.x, 3.0f);
    EXPECT_EQ(result.y, 7.0f);
}

TEST(sge_math_float2x2, MatrixMultiplication)
{
    float2x2 mat1(1.0f, 2.0f, 3.0f, 4.0f);
    float2x2 mat2(2.0f, 0.0f, 1.0f, 2.0f);
    float2x2 result = mat1 * mat2;
    EXPECT_EQ(result.m00, 4.0f);
    EXPECT_EQ(result.m01, 4.0f);
    EXPECT_EQ(result.m10, 10.0f);
    EXPECT_EQ(result.m11, 8.0f);
}

TEST(sge_math_float2x2, ScalarMultiplication)
{
    float2x2 mat(1.0f, 2.0f, 3.0f, 4.0f);
    float2x2 result = mat * 2.0f;
    EXPECT_EQ(result.m00, 2.0f);
    EXPECT_EQ(result.m01, 4.0f);
    EXPECT_EQ(result.m10, 6.0f);
    EXPECT_EQ(result.m11, 8.0f);
}

TEST(sge_math_float2x2, EqualityOperator)
{
    float2x2 mat1(1.0f, 2.0f, 3.0f, 4.0f);
    float2x2 mat2(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_TRUE(mat1 == mat2);
}

TEST(sge_math_float2x2, InequalityOperator)
{
    float2x2 mat1(1.0f, 2.0f, 3.0f, 4.0f);
    float2x2 mat2(2.0f, 3.0f, 4.0f, 5.0f);
    EXPECT_TRUE(mat1 != mat2);
}

// --------------------------------------------------------------------------
// float3x3
// --------------------------------------------------------------------------

TEST(sge_math_float3x3, DefaultConstructor)
{
    float3x3 mat;
    EXPECT_EQ(mat.m00, 0.0f);
    EXPECT_EQ(mat.m01, 0.0f);
    EXPECT_EQ(mat.m02, 0.0f);
    EXPECT_EQ(mat.m10, 0.0f);
    EXPECT_EQ(mat.m11, 0.0f);
    EXPECT_EQ(mat.m12, 0.0f);
    EXPECT_EQ(mat.m20, 0.0f);
    EXPECT_EQ(mat.m21, 0.0f);
    EXPECT_EQ(mat.m22, 0.0f);
}

TEST(sge_math_float3x3, ParameterizedConstructor)
{
    float3x3 mat(1.0f, 2.0f, 3.0f,
                 4.0f, 5.0f, 6.0f,
                 7.0f, 8.0f, 9.0f);
    EXPECT_EQ(mat.m00, 1.0f);
    EXPECT_EQ(mat.m01, 2.0f);
    EXPECT_EQ(mat.m02, 3.0f);
    EXPECT_EQ(mat.m10, 4.0f);
    EXPECT_EQ(mat.m11, 5.0f);
    EXPECT_EQ(mat.m12, 6.0f);
    EXPECT_EQ(mat.m20, 7.0f);
    EXPECT_EQ(mat.m21, 8.0f);
    EXPECT_EQ(mat.m22, 9.0f);
}

TEST(sge_math_float3x3, ConstructorFromVectors)
{
    float3 row0(1.0f, 2.0f, 3.0f);
    float3 row1(4.0f, 5.0f, 6.0f);
    float3 row2(7.0f, 8.0f, 9.0f);
    
    float3x3 mat(row0, row1, row2);

    EXPECT_EQ(mat.m00, 1.0f);
    EXPECT_EQ(mat.m01, 2.0f);
    EXPECT_EQ(mat.m02, 3.0f);
    EXPECT_EQ(mat.m10, 4.0f);
    EXPECT_EQ(mat.m11, 5.0f);
    EXPECT_EQ(mat.m12, 6.0f);
    EXPECT_EQ(mat.m20, 7.0f);
    EXPECT_EQ(mat.m21, 8.0f);
    EXPECT_EQ(mat.m22, 9.0f);
}

TEST(sge_math_float3x3, IdentityMatrix)
{
    EXPECT_EQ(float3x3::Identity.m00, 1.0f);
    EXPECT_EQ(float3x3::Identity.m01, 0.0f);
    EXPECT_EQ(float3x3::Identity.m02, 0.0f);
    EXPECT_EQ(float3x3::Identity.m10, 0.0f);
    EXPECT_EQ(float3x3::Identity.m11, 1.0f);
    EXPECT_EQ(float3x3::Identity.m12, 0.0f);
    EXPECT_EQ(float3x3::Identity.m20, 0.0f);
    EXPECT_EQ(float3x3::Identity.m21, 0.0f);
    EXPECT_EQ(float3x3::Identity.m22, 1.0f);
}

TEST(sge_math_float3x3, ZeroMatrix)
{
    EXPECT_EQ(float3x3::Zero.m00, 0.0f);
    EXPECT_EQ(float3x3::Zero.m01, 0.0f);
    EXPECT_EQ(float3x3::Zero.m02, 0.0f);
    EXPECT_EQ(float3x3::Zero.m10, 0.0f);
    EXPECT_EQ(float3x3::Zero.m11, 0.0f);
    EXPECT_EQ(float3x3::Zero.m12, 0.0f);
    EXPECT_EQ(float3x3::Zero.m20, 0.0f);
    EXPECT_EQ(float3x3::Zero.m21, 0.0f);
    EXPECT_EQ(float3x3::Zero.m22, 0.0f);
}

TEST(sge_math_float3x3, Transpose)
{
    float3x3 mat(1.0f, 2.0f, 3.0f,
                 4.0f, 5.0f, 6.0f,
                 7.0f, 8.0f, 9.0f);
    float3x3 transposed = mat.transposed();
    EXPECT_EQ(transposed.m00, 1.0f);
    EXPECT_EQ(transposed.m01, 4.0f);
    EXPECT_EQ(transposed.m02, 7.0f);
    EXPECT_EQ(transposed.m10, 2.0f);
    EXPECT_EQ(transposed.m11, 5.0f);
    EXPECT_EQ(transposed.m12, 8.0f);
    EXPECT_EQ(transposed.m20, 3.0f);
    EXPECT_EQ(transposed.m21, 6.0f);
    EXPECT_EQ(transposed.m22, 9.0f);
}

TEST(sge_math_float3x3, Determinant)
{
    float3x3 mat(1.0f, 2.0f, 3.0f,
                 4.0f, 5.0f, 6.0f,
                 7.0f, 8.0f, 9.0f);
    EXPECT_EQ(mat.determinant(), 0.0f); // This matrix is singular
}

TEST(sge_math_float3x3, Inverse)
{
    float3x3 mat(1.0f, 0.0f, 0.0f,
                 0.0f, 2.0f, 0.0f,
                 0.0f, 0.0f, 3.0f);
    float3x3 inv = mat.inverse();

    EXPECT_NEAR(inv.m00, 1.0f, EPSILON);
    EXPECT_NEAR(inv.m01, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m02, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m10, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m11, 0.5f, EPSILON);
    EXPECT_NEAR(inv.m12, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m20, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m21, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m22, 0.333333f, EPSILON);
}

TEST(sge_math_float3x3, Inverse2)
{
    float3x3 mat(2.0f, 1.0f, 3.0f,
                 4.0f, 0.0f, 5.0f,
                 1.0f, 2.0f, 1.0f);
    float3x3 inv = mat.inverse();

    EXPECT_NEAR(inv.m00, -2.0f, EPSILON);
    EXPECT_NEAR(inv.m01, 1.0f, EPSILON);
    EXPECT_NEAR(inv.m02, 1.0f, EPSILON);
    EXPECT_NEAR(inv.m10, 0.2f, EPSILON);
    EXPECT_NEAR(inv.m11, -0.2f, EPSILON);
    EXPECT_NEAR(inv.m12, 0.4f, EPSILON);
    EXPECT_NEAR(inv.m20, 1.6f, EPSILON);
    EXPECT_NEAR(inv.m21, -0.6f, EPSILON);
    EXPECT_NEAR(inv.m22, -0.8f, EPSILON);
}

TEST(sge_math_float3x3, InverseOrthogonal)
{
    float angle = PI / 4.0f; 
    float3x3 mat = {
        cos(angle), -sin(angle), 0.0f,
        sin(angle),  cos(angle), 0.0f,
        0.0f,       0.0f,       1.0f
    };

    float3x3 inv = mat.inverse();

    // For orthogonal matrices, inverse is equal to transpose
    EXPECT_NEAR(inv.m00, mat.m00, EPSILON);
    EXPECT_NEAR(inv.m01, mat.m10, EPSILON);
    EXPECT_NEAR(inv.m02, mat.m20, EPSILON);
    EXPECT_NEAR(inv.m10, mat.m01, EPSILON);
    EXPECT_NEAR(inv.m11, mat.m11, EPSILON);
    EXPECT_NEAR(inv.m12, mat.m21, EPSILON);
    EXPECT_NEAR(inv.m20, mat.m02, EPSILON);
    EXPECT_NEAR(inv.m21, mat.m12, EPSILON);
    EXPECT_NEAR(inv.m22, mat.m22, EPSILON);
}

TEST(sge_math_float3x3, Inverse3)
{
    float3x3 mat(1.0f, 2.0f, 3.0f,
                 4.0f, 5.0f, 6.0f,
                 7.0f, 8.0f, 10.0f);
    float3x3 inv = mat.inverse();

    EXPECT_NEAR(inv.m00, -0.666667f, EPSILON);
    EXPECT_NEAR(inv.m01, -1.333333f, EPSILON);
    EXPECT_NEAR(inv.m02, 1.0f, EPSILON);
    EXPECT_NEAR(inv.m10, -0.666667f, EPSILON);
    EXPECT_NEAR(inv.m11, 3.666667f, EPSILON);
    EXPECT_NEAR(inv.m12, -2.0f, EPSILON);
    EXPECT_NEAR(inv.m20, 1.0f, EPSILON);
    EXPECT_NEAR(inv.m21, -2.0f, EPSILON);
    EXPECT_NEAR(inv.m22, 1.0f, EPSILON);
}

TEST(sge_math_float3x3, MatrixVectorMultiplication)
{
    float3x3 mat(1.0f, 2.0f, 3.0f,
                 4.0f, 5.0f, 6.0f,
                 7.0f, 8.0f, 9.0f);
    float3 vec(1.0f, 1.0f, 1.0f);
    float3 result = mat * vec;
    EXPECT_EQ(result.x, 6.0f);
    EXPECT_EQ(result.y, 15.0f);
    EXPECT_EQ(result.z, 24.0f);
}

TEST(sge_math_float3x3, MatrixMultiplication)
{
    float3x3 mat1(1.0f, 2.0f, 3.0f,
                  4.0f, 5.0f, 6.0f,
                  7.0f, 8.0f, 9.0f);
    float3x3 mat2(2.0f, 0.0f, 1.0f,
                  1.0f, 2.0f, 0.0f,
                  0.0f, 1.0f, 2.0f);
    float3x3 result = mat1 * mat2;
    EXPECT_EQ(result.m00, 4.0f);
    EXPECT_EQ(result.m01, 7.0f);
    EXPECT_EQ(result.m02, 7.0f);
    EXPECT_EQ(result.m10, 13.0f);
    EXPECT_EQ(result.m11, 16.0f);
    EXPECT_EQ(result.m12, 16.0f);
    EXPECT_EQ(result.m20, 22.0f);
    EXPECT_EQ(result.m21, 25.0f);
    EXPECT_EQ(result.m22, 25.0f);
}

TEST(sge_math_float3x3, ScalarMultiplication)
{
    float3x3 mat(1.0f, 2.0f, 3.0f,
                 4.0f, 5.0f, 6.0f,
                 7.0f, 8.0f, 9.0f);
    float3x3 result = mat * 2.0f;
    EXPECT_EQ(result.m00, 2.0f);
    EXPECT_EQ(result.m01, 4.0f);
    EXPECT_EQ(result.m02, 6.0f);
    EXPECT_EQ(result.m10, 8.0f);
    EXPECT_EQ(result.m11, 10.0f);
    EXPECT_EQ(result.m12, 12.0f);
    EXPECT_EQ(result.m20, 14.0f);
    EXPECT_EQ(result.m21, 16.0f);
    EXPECT_EQ(result.m22, 18.0f);
}

TEST(sge_math_float3x3, EqualityOperator)
{
    float3x3 mat1(1.0f, 2.0f, 3.0f,
                  4.0f, 5.0f, 6.0f,
                  7.0f, 8.0f, 9.0f);
    float3x3 mat2(1.0f, 2.0f, 3.0f,
                  4.0f, 5.0f, 6.0f,
                  7.0f, 8.0f, 9.0f);
    EXPECT_TRUE(mat1 == mat2);
}

TEST(sge_math_float3x3, InequalityOperator)
{
    float3x3 mat1(1.0f, 2.0f, 3.0f,
                  4.0f, 5.0f, 6.0f,
                  7.0f, 8.0f, 9.0f);
    float3x3 mat2(2.0f, 3.0f, 4.0f,
                  5.0f, 6.0f, 7.0f,
                  8.0f, 9.0f, 10.0f);
    EXPECT_TRUE(mat1 != mat2);
}

// --------------------------------------------------------------------------
// float4x4
// --------------------------------------------------------------------------

TEST(sge_math_float4x4, DefaultConstructor)
{
    float4x4 mat;
    EXPECT_EQ(mat.m00, 0.0f);
    EXPECT_EQ(mat.m01, 0.0f);
    EXPECT_EQ(mat.m02, 0.0f);
    EXPECT_EQ(mat.m03, 0.0f);
    EXPECT_EQ(mat.m10, 0.0f);
    EXPECT_EQ(mat.m11, 0.0f);
    EXPECT_EQ(mat.m12, 0.0f);
    EXPECT_EQ(mat.m13, 0.0f);
    EXPECT_EQ(mat.m20, 0.0f);
    EXPECT_EQ(mat.m21, 0.0f);
    EXPECT_EQ(mat.m22, 0.0f);
    EXPECT_EQ(mat.m23, 0.0f);
    EXPECT_EQ(mat.m30, 0.0f);
    EXPECT_EQ(mat.m31, 0.0f);
    EXPECT_EQ(mat.m32, 0.0f);
    EXPECT_EQ(mat.m33, 0.0f);
}

TEST(sge_math_float4x4, ParameterizedConstructor)
{
    float4x4 mat(1.0f, 2.0f, 3.0f, 4.0f,
                 5.0f, 6.0f, 7.0f, 8.0f,
                 9.0f, 10.0f, 11.0f, 12.0f,
                 13.0f, 14.0f, 15.0f, 16.0f);

    EXPECT_EQ(mat.m00, 1.0f);
    EXPECT_EQ(mat.m01, 2.0f);
    EXPECT_EQ(mat.m02, 3.0f);
    EXPECT_EQ(mat.m03, 4.0f);
    EXPECT_EQ(mat.m10, 5.0f);
    EXPECT_EQ(mat.m11, 6.0f);
    EXPECT_EQ(mat.m12, 7.0f);
    EXPECT_EQ(mat.m13, 8.0f);
    EXPECT_EQ(mat.m20, 9.0f);
    EXPECT_EQ(mat.m21, 10.0f);
    EXPECT_EQ(mat.m22, 11.0f);
    EXPECT_EQ(mat.m23, 12.0f);
    EXPECT_EQ(mat.m30, 13.0f);
    EXPECT_EQ(mat.m31, 14.0f);
    EXPECT_EQ(mat.m32, 15.0f);
    EXPECT_EQ(mat.m33, 16.0f);
}

TEST(sge_math_float4x4, ConstructorFromVectors)
{
    float4 row0(1.0f, 2.0f, 3.0f, 4.0f);
    float4 row1(5.0f, 6.0f, 7.0f, 8.0f);
    float4 row2(9.0f, 10.0f, 11.0f, 12.0f);
    float4 row3(13.0f, 14.0f, 15.0f, 16.0f);

    float4x4 mat(row0, row1, row2, row3);

    EXPECT_EQ(mat.m00, 1.0f);
    EXPECT_EQ(mat.m01, 2.0f);
    EXPECT_EQ(mat.m02, 3.0f);
    EXPECT_EQ(mat.m03, 4.0f);
    EXPECT_EQ(mat.m10, 5.0f);
    EXPECT_EQ(mat.m11, 6.0f);
    EXPECT_EQ(mat.m12, 7.0f);
    EXPECT_EQ(mat.m13, 8.0f);
    EXPECT_EQ(mat.m20, 9.0f);
    EXPECT_EQ(mat.m21, 10.0f);
    EXPECT_EQ(mat.m22, 11.0f);
    EXPECT_EQ(mat.m23, 12.0f);
    EXPECT_EQ(mat.m30, 13.0f);
    EXPECT_EQ(mat.m31, 14.0f);
    EXPECT_EQ(mat.m32, 15.0f);
    EXPECT_EQ(mat.m33, 16.0f);
}

TEST(sge_math_float4x4, IdentityMatrix)
{
    EXPECT_EQ(float4x4::Identity.m00, 1.0f);
    EXPECT_EQ(float4x4::Identity.m01, 0.0f);
    EXPECT_EQ(float4x4::Identity.m02, 0.0f);
    EXPECT_EQ(float4x4::Identity.m03, 0.0f);
    EXPECT_EQ(float4x4::Identity.m10, 0.0f);
    EXPECT_EQ(float4x4::Identity.m11, 1.0f);
    EXPECT_EQ(float4x4::Identity.m12, 0.0f);
    EXPECT_EQ(float4x4::Identity.m13, 0.0f);
    EXPECT_EQ(float4x4::Identity.m20, 0.0f);
    EXPECT_EQ(float4x4::Identity.m21, 0.0f);
    EXPECT_EQ(float4x4::Identity.m22, 1.0f);
    EXPECT_EQ(float4x4::Identity.m23, 0.0f);
    EXPECT_EQ(float4x4::Identity.m30, 0.0f);
    EXPECT_EQ(float4x4::Identity.m31, 0.0f);
    EXPECT_EQ(float4x4::Identity.m32, 0.0f);
    EXPECT_EQ(float4x4::Identity.m33, 1.0f);
}

TEST(sge_math_float4x4, ZeroMatrix)
{
    EXPECT_EQ(float4x4::Zero.m00, 0.0f);
    EXPECT_EQ(float4x4::Zero.m01, 0.0f);
    EXPECT_EQ(float4x4::Zero.m02, 0.0f);
    EXPECT_EQ(float4x4::Zero.m03, 0.0f);
    EXPECT_EQ(float4x4::Zero.m10, 0.0f);
    EXPECT_EQ(float4x4::Zero.m11, 0.0f);
    EXPECT_EQ(float4x4::Zero.m12, 0.0f);
    EXPECT_EQ(float4x4::Zero.m13, 0.0f);
    EXPECT_EQ(float4x4::Zero.m20, 0.0f);
    EXPECT_EQ(float4x4::Zero.m21, 0.0f);
    EXPECT_EQ(float4x4::Zero.m22, 0.0f);
    EXPECT_EQ(float4x4::Zero.m23, 0.0f);
    EXPECT_EQ(float4x4::Zero.m30, 0.0f);
    EXPECT_EQ(float4x4::Zero.m31, 0.0f);
    EXPECT_EQ(float4x4::Zero.m32, 0.0f);
    EXPECT_EQ(float4x4::Zero.m33, 0.0f);
}

TEST(sge_math_float4x4, Transpose)
{
    float4x4 mat(1.0f, 2.0f, 3.0f, 4.0f,
                 5.0f, 6.0f, 7.0f, 8.0f,
                 9.0f, 10.0f, 11.0f, 12.0f,
                 13.0f, 14.0f, 15.0f, 16.0f);

    float4x4 transposed = mat.transposed();

    EXPECT_EQ(transposed.m00, 1.0f);
    EXPECT_EQ(transposed.m01, 5.0f);
    EXPECT_EQ(transposed.m02, 9.0f);
    EXPECT_EQ(transposed.m03, 13.0f);
    EXPECT_EQ(transposed.m10, 2.0f);
    EXPECT_EQ(transposed.m11, 6.0f);
    EXPECT_EQ(transposed.m12, 10.0f);
    EXPECT_EQ(transposed.m13, 14.0f);
    EXPECT_EQ(transposed.m20, 3.0f);
    EXPECT_EQ(transposed.m21, 7.0f);
    EXPECT_EQ(transposed.m22, 11.0f);
    EXPECT_EQ(transposed.m23, 15.0f);
    EXPECT_EQ(transposed.m30, 4.0f);
    EXPECT_EQ(transposed.m31, 8.0f);
    EXPECT_EQ(transposed.m32, 12.0f);
    EXPECT_EQ(transposed.m33, 16.0f);
}

TEST(sge_math_float4x4, Determinant)
{
    float4x4 mat(1.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 2.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 3.0f, 0.0f,
                 0.0f, 0.0f, 0.0f, 4.0f);

    EXPECT_EQ(mat.determinant(), 24.0f); // 1 * 2 * 3 * 4 = 24
}

TEST(sge_math_float4x4, Inverse)
{
    float4x4 mat(1.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 2.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 3.0f, 0.0f,
                 0.0f, 0.0f, 0.0f, 4.0f);

    float4x4 inv = mat.inverse();

    EXPECT_NEAR(inv.m00, 1.0f, EPSILON);
    EXPECT_NEAR(inv.m01, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m02, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m03, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m10, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m11, 0.5f, EPSILON);
    EXPECT_NEAR(inv.m12, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m13, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m20, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m21, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m22, 0.333333f, EPSILON);
    EXPECT_NEAR(inv.m23, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m30, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m31, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m32, 0.0f, EPSILON);
    EXPECT_NEAR(inv.m33, 0.25f, EPSILON);
}

TEST(sge_math_float4x4, MatrixVectorMultiplication)
{
    float4x4 mat(1.0f, 2.0f, 3.0f, 4.0f,
                 5.0f, 6.0f, 7.0f, 8.0f,
                 9.0f, 10.0f, 11.0f, 12.0f,
                 13.0f, 14.0f, 15.0f, 16.0f);

    float4 vec(1.0f, 1.0f, 1.0f, 1.0f);
    float4 result = mat * vec;

    EXPECT_EQ(result.x, 10.0f);
    EXPECT_EQ(result.y, 26.0f);
    EXPECT_EQ(result.z, 42.0f);
    EXPECT_EQ(result.w, 58.0f);
}

TEST(sge_math_float4x4, MatrixMultiplication)
{
    float4x4 mat1(1.0f, 2.0f, 3.0f, 4.0f,
                  5.0f, 6.0f, 7.0f, 8.0f,
                  9.0f, 10.0f, 11.0f, 12.0f,
                  13.0f, 14.0f, 15.0f, 16.0f);

    float4x4 mat2(2.0f, 0.0f, 1.0f, 0.0f,
                  1.0f, 2.0f, 0.0f, 0.0f,
                  0.0f, 1.0f, 2.0f, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f);

    float4x4 result = mat1 * mat2;

    EXPECT_EQ(result.m00, 4.0f);
    EXPECT_EQ(result.m01, 7.0f);
    EXPECT_EQ(result.m02, 7.0f);
    EXPECT_EQ(result.m03, 4.0f);
    EXPECT_EQ(result.m10, 16.0f);
    EXPECT_EQ(result.m11, 19.0f);
    EXPECT_EQ(result.m12, 19.0f);
    EXPECT_EQ(result.m13, 8.0f);
    EXPECT_EQ(result.m20, 28.0f);
    EXPECT_EQ(result.m21, 31.0f);
    EXPECT_EQ(result.m22, 31.0f);
    EXPECT_EQ(result.m23, 12.0f);
    EXPECT_EQ(result.m30, 40.0f);
    EXPECT_EQ(result.m31, 43.0f);
    EXPECT_EQ(result.m32, 43.0f);
    EXPECT_EQ(result.m33, 16.0f);
}

TEST(sge_math_float4x4, ScalarMultiplication)
{
    float4x4 mat(1.0f, 2.0f, 3.0f, 4.0f,
                 5.0f, 6.0f, 7.0f, 8.0f,
                 9.0f, 10.0f, 11.0f, 12.0f,
                 13.0f, 14.0f, 15.0f, 16.0f);

    float4x4 result = mat * 2.0f;

    EXPECT_EQ(result.m00, 2.0f);
    EXPECT_EQ(result.m01, 4.0f);
    EXPECT_EQ(result.m02, 6.0f);
    EXPECT_EQ(result.m03, 8.0f);
    EXPECT_EQ(result.m10, 10.0f);
    EXPECT_EQ(result.m11, 12.0f);
    EXPECT_EQ(result.m12, 14.0f);
    EXPECT_EQ(result.m13, 16.0f);
    EXPECT_EQ(result.m20, 18.0f);
    EXPECT_EQ(result.m21, 20.0f);
    EXPECT_EQ(result.m22, 22.0f);
    EXPECT_EQ(result.m23, 24.0f);
    EXPECT_EQ(result.m30, 26.0f);
    EXPECT_EQ(result.m31, 28.0f);
    EXPECT_EQ(result.m32, 30.0f);
    EXPECT_EQ(result.m33, 32.0f);
}

TEST(sge_math_float4x4, EqualityOperator)
{
    float4x4 mat1(1.0f, 2.0f, 3.0f, 4.0f,
                  5.0f, 6.0f, 7.0f, 8.0f,
                  9.0f, 10.0f, 11.0f, 12.0f,
                  13.0f, 14.0f, 15.0f, 16.0f);

    float4x4 mat2(1.0f, 2.0f, 3.0f, 4.0f,
                  5.0f, 6.0f, 7.0f, 8.0f,
                  9.0f, 10.0f, 11.0f, 12.0f,
                  13.0f, 14.0f, 15.0f, 16.0f);

    EXPECT_TRUE(mat1 == mat2);
}

TEST(sge_math_float4x4, InequalityOperator)
{
    float4x4 mat1(1.0f, 2.0f, 3.0f, 4.0f,
                  5.0f, 6.0f, 7.0f, 8.0f,
                  9.0f, 10.0f, 11.0f, 12.0f,
                  13.0f, 14.0f, 15.0f, 16.0f);

    float4x4 mat2(2.0f, 3.0f, 4.0f, 5.0f,
                  6.0f, 7.0f, 8.0f, 9.0f,
                  10.0f, 11.0f, 12.0f, 13.0f,
                  14.0f, 15.0f, 16.0f, 17.0f);

    EXPECT_TRUE(mat1 != mat2);
}

// --------------------------------------------------------------------------
// Camera and Transformations
// --------------------------------------------------------------------------

TEST(sge_math_float4x4, CreateViewMatrix)
{
    float3 eye(0.0f, 0.0f, 5.0f);
    float3 target(0.0f, 0.0f, 0.0f);
    float3 up(0.0f, 1.0f, 0.0f);

    float4x4 viewMatrix = CreateViewMatrix(eye, target, up);

    // (X axis)
    EXPECT_NEAR(viewMatrix.m00, -1.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m01, 0.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m02, 0.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m03, 0.0f, EPSILON);

    // (Y axis)
    EXPECT_NEAR(viewMatrix.m10, 0.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m11, 1.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m12, 0.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m13, 0.0f, EPSILON);

    // (Z axis)
    EXPECT_NEAR(viewMatrix.m20, 0.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m21, 0.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m22, -1.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m23, 5.0f, EPSILON);

    // (Homogeneous coordinate)
    EXPECT_NEAR(viewMatrix.m30, 0.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m31, 0.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m32, 0.0f, EPSILON);
    EXPECT_NEAR(viewMatrix.m33, 1.0f, EPSILON);
}

TEST(sge_math_float4x4, CreatePerspectiveProjectionMatrix)
{
    float fov = PI / 4.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearZ = 0.1f;
    float farZ = 100.0f;

    float4x4 projMatrix = CreatePerspectiveProjectionMatrix(fov, aspectRatio, nearZ, farZ);

    float tanHalfFov = tan(fov / 2.0f);
    float height = 1.0f / tanHalfFov;
    float width = height / aspectRatio;
    float range = farZ - nearZ;

    EXPECT_NEAR(projMatrix.m00, width, EPSILON);
    EXPECT_NEAR(projMatrix.m11, height, EPSILON);
    EXPECT_NEAR(projMatrix.m22, farZ / range, EPSILON);
    EXPECT_NEAR(projMatrix.m23, -farZ * nearZ / range, EPSILON);
    EXPECT_NEAR(projMatrix.m32, 1.0f, EPSILON);
}

TEST(sge_math_float4x4, CreateTranslationMatrix)
{
    float3 translation(1.0f, 2.0f, 3.0f);
    float4x4 transMatrix = CreateTranslationMatrix(translation);

    EXPECT_EQ(transMatrix.m00, 1.0f);
    EXPECT_EQ(transMatrix.m01, 0.0f);
    EXPECT_EQ(transMatrix.m02, 0.0f);
    EXPECT_EQ(transMatrix.m03, 1.0f);
    EXPECT_EQ(transMatrix.m10, 0.0f);
    EXPECT_EQ(transMatrix.m11, 1.0f);
    EXPECT_EQ(transMatrix.m12, 0.0f);
    EXPECT_EQ(transMatrix.m13, 2.0f);
    EXPECT_EQ(transMatrix.m20, 0.0f);
    EXPECT_EQ(transMatrix.m21, 0.0f);
    EXPECT_EQ(transMatrix.m22, 1.0f);
    EXPECT_EQ(transMatrix.m23, 3.0f);
    EXPECT_EQ(transMatrix.m30, 0.0f);
    EXPECT_EQ(transMatrix.m31, 0.0f);
    EXPECT_EQ(transMatrix.m32, 0.0f);
    EXPECT_EQ(transMatrix.m33, 1.0f);
}

TEST(sge_math_float4x4, CreateRotationMatrixYawPitchRoll)
{
    float yaw = PI / 4.0f;
    float pitch = PI / 6.0f;
    float roll = PI / 3.0f;

    float4x4 rotMatrix = CreateRotationMatrixYawPitchRoll(yaw, pitch, roll);

    EXPECT_NEAR(rotMatrix.m00, 0.433013f, EPSILON);
    EXPECT_NEAR(rotMatrix.m01, -0.75f, EPSILON);
    EXPECT_NEAR(rotMatrix.m02, 0.5f, EPSILON);
    EXPECT_NEAR(rotMatrix.m10, 0.866025f, EPSILON);
    EXPECT_NEAR(rotMatrix.m11, 0.433013f, EPSILON);
    EXPECT_NEAR(rotMatrix.m12, -0.25f, EPSILON);
    EXPECT_NEAR(rotMatrix.m20, 0.25f, EPSILON);
    EXPECT_NEAR(rotMatrix.m21, 0.5f, EPSILON);
    EXPECT_NEAR(rotMatrix.m22, 0.866025f, EPSILON);
    EXPECT_NEAR(rotMatrix.m30, 0.0f, EPSILON);
    EXPECT_NEAR(rotMatrix.m31, 0.0f, EPSILON);
    EXPECT_NEAR(rotMatrix.m32, 0.0f, EPSILON);
    EXPECT_NEAR(rotMatrix.m33, 1.0f, EPSILON);
}

TEST(sge_math_float4x4, CreateScaleMatrix)
{
    float3 scale(2.0f, 3.0f, 4.0f);
    float4x4 scaleMatrix = CreateScaleMatrix(scale);

    EXPECT_EQ(scaleMatrix.m00, 2.0f);
    EXPECT_EQ(scaleMatrix.m01, 0.0f);
    EXPECT_EQ(scaleMatrix.m02, 0.0f);
    EXPECT_EQ(scaleMatrix.m03, 0.0f);
    EXPECT_EQ(scaleMatrix.m10, 0.0f);
    EXPECT_EQ(scaleMatrix.m11, 3.0f);
    EXPECT_EQ(scaleMatrix.m12, 0.0f);
    EXPECT_EQ(scaleMatrix.m13, 0.0f);
    EXPECT_EQ(scaleMatrix.m20, 0.0f);
    EXPECT_EQ(scaleMatrix.m21, 0.0f);
    EXPECT_EQ(scaleMatrix.m22, 4.0f);
    EXPECT_EQ(scaleMatrix.m23, 0.0f);
    EXPECT_EQ(scaleMatrix.m30, 0.0f);
    EXPECT_EQ(scaleMatrix.m31, 0.0f);
    EXPECT_EQ(scaleMatrix.m32, 0.0f);
    EXPECT_EQ(scaleMatrix.m33, 1.0f);
}

TEST(sge_math_float4x4, CreateOrthographicProjectionMatrix)
{
    float width = 10.0f;
    float height = 10.0f;
    float nearZ = 1.0f;
    float farZ = 100.0f;

    float4x4 expectedMatrix =
    {
        0.2f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.2f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.010101f, -0.010101f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    float4x4 resultMatrix = CreateOrthographicProjectionMatrix(width, height, nearZ, farZ);

    for (int i = 0; i < 16; ++i) 
    {
        EXPECT_NEAR(resultMatrix.m[i], expectedMatrix.m[i], EPSILON)
            << "Element at index " << i << " is incorrect!";
    }
}