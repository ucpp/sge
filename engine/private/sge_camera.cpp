#include "sge_camera.h"
#include "sge_logger.h"

namespace SGE
{
    Camera::Camera() 
    : m_position(0.0f, 0.0f, -10.0f)
    , m_pitch(0.0f)
    , m_yaw(0.0f)
    , m_fov(DEFAULT_FOV)
    , m_nearPlane(DEFAULT_NEAR_PLANE)
    , m_farPlane(DEFAULT_FAR_PLANE)
    {
        SetRotation(m_pitch, m_yaw);
    }

    void Camera::Initialize(const float3& position, float pitch, float yaw, float fov, float nearPlane, float farPlane)
    {
        m_position = position;
        m_pitch = pitch;
        m_yaw = yaw;
        m_fov = fov;
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;

        SetRotation(pitch, yaw);
    }

    void Camera::SetPosition(const float3& position)
    {
        m_position = position;
    }

    void Camera::SetRotation(float pitch, float yaw)
    {
        m_pitch = pitch;
        m_yaw = yaw;

        float pitchInRadians = m_pitch * DEG_TO_RAD;
        float yawInRadians = m_yaw * DEG_TO_RAD;

        m_direction = float3(
            cosf(pitchInRadians) * cosf(yawInRadians), // X
            sinf(pitchInRadians),                      // Y
            cosf(pitchInRadians) * sinf(yawInRadians)  // Z
        );

        m_direction.normalize();
        Recalculate();
    }

    void Camera::SetDirection(const float3& direction)
    {
        m_direction = direction;
        m_direction = m_direction / m_direction.length();
        Recalculate();
    }

    void Camera::SetTarget(const float3& target)
    {
        m_direction = (target - m_position);
        m_direction.normalize();

        m_yaw = atan2f(m_direction.z, m_direction.x);
        m_pitch = asinf(m_direction.y);

        m_yaw *= RAD_TO_DEG;
        m_pitch *= RAD_TO_DEG;

        Recalculate();
    }

    void Camera::SetFov(float fov)
    {
        m_fov = fov;
    }

    void Camera::SetNearPlane(float nearPlane)
    {
        m_nearPlane = nearPlane;
    }

    void Camera::SetFarPlane(float farPlane)
    {
        m_farPlane = farPlane;
    }

    float3 Camera::GetPosition() const { return m_position; }
    float3 Camera::GetForward() const { return m_direction; }
    float3 Camera::GetRight() const { return m_right; }
    float3 Camera::GetLeft() const { return -m_right; }
    float3 Camera::GetUp() const { return m_up; }
    float3 Camera::GetBackward() const { return -m_direction; }

    float Camera::GetFov() const { return m_fov; }
    float Camera::GetPitch() const { return m_pitch; }
    float Camera::GetYaw() const { return m_yaw; }

    void Camera::Recalculate()
    {
        m_right = m_worldUp.cross(m_direction);
        m_right = m_right / m_right.length();

        m_up = m_direction.cross(m_right);
        m_up = m_up / m_up.length();
    }

    Matrix Camera::GetViewMatrix() const
    {
        float3 pos = m_position + m_direction;
        Vector3 eyePos = { m_position.x, m_position.y, m_position.z };
        Vector3 focusPos = { pos.x, pos.y, pos.z };
        Vector3 upDir = { m_up.x, m_up.y, m_up.z };
        return XMMatrixLookAtLH(eyePos, focusPos, upDir);
    }

    Matrix Camera::GetProjMatrix(int width, int height) const
    {
        float aspectRatio = static_cast<float>(width) / height;
        float fovAngleY = m_fov * DEG_TO_RAD;

        if (aspectRatio < 1.0f)
        {
            fovAngleY /= aspectRatio;
        }

        return XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, m_nearPlane, m_farPlane);
    }

    Matrix Camera::GetViewProjMatrix(int width, int height) const
    {
        return GetProjMatrix(width, height) * GetViewMatrix();
    }

    Matrix Camera::GetInvViewProjMatrix(int width, int height) const
    {
        Matrix viewProj = GetViewProjMatrix(width, height);
        return viewProj.Invert();
    }

    Matrix Camera::GetOrthoProjMatrix(float width, float height) const
    {
        return XMMatrixOrthographicLH(width, height, m_nearPlane, m_farPlane);
    }

    std::array<Matrix, 6> Camera::GetCubeViewMatrices() const
    {
        static const float3 directions[6] = {
            { 1.0f,  0.0f,  0.0f },  // +X
            {-1.0f,  0.0f,  0.0f },  // -X
            { 0.0f,  1.0f,  0.0f },  // +Y
            { 0.0f, -1.0f,  0.0f },  // -Y
            { 0.0f,  0.0f,  1.0f },  // +Z
            { 0.0f,  0.0f, -1.0f }   // -Z
        };

        static const float3 ups[6] = {
            { 0.0f,  1.0f,  0.0f },  // +X
            { 0.0f,  1.0f,  0.0f },  // -X
            { 0.0f,  0.0f, -1.0f },  // +Y
            { 0.0f,  0.0f,  1.0f },  // -Y
            { 0.0f,  1.0f,  0.0f },  // +Z
            { 0.0f,  1.0f,  0.0f }   // -Z
        };

        std::array<Matrix, 6> matrices;
        for (int32 i = 0; i < 6; ++i)
        {
            float3 pos = m_position + directions[i];
            Vector3 eyePos = { m_position.x, m_position.y, m_position.z };
            Vector3 focusPos = { pos.x, pos.y, pos.z };
            Vector3 upDir = { ups[i].x, ups[i].y, ups[i].z };
            matrices[i] = XMMatrixLookAtLH(eyePos, focusPos, upDir);
        }
        return matrices;
    }
}
