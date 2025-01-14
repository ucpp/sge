#include "sge_camera.h"

namespace SGE
{
    Camera::Camera() 
        : m_position(0.0f, 0.0f, -10.0f),
          m_pitch(0.0f),
          m_yaw(0.0f),
          m_fov(DEFAULT_FOV),
          m_nearPlane(DEFAULT_NEAR_PLANE),
          m_farPlane(DEFAULT_FAR_PLANE)
    {
        SetRotation(m_pitch, m_yaw);
    }

    void Camera::Initialize(const Vector3& position, float pitch, float yaw, float fov, float nearPlane, float farPlane)
    {
        m_position = position;
        m_pitch = pitch;
        m_yaw = yaw;
        m_fov = fov;
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;

        SetRotation(pitch, yaw);
    }

    void Camera::SetPosition(const Vector3& position)
    {
        m_position = position;
    }

    void Camera::SetRotation(float pitch, float yaw)
    {
        m_pitch = pitch;
        m_yaw = yaw;

        float pitchInRadians = m_pitch * DEG_TO_RAD;
        float yawInRadians = m_yaw * DEG_TO_RAD;

        m_direction = Vector3(
            cosf(pitchInRadians) * cosf(yawInRadians), // X
            sinf(pitchInRadians),                      // Y
            cosf(pitchInRadians) * sinf(yawInRadians)  // Z
        );

        m_direction.Normalize();
        Recalculate();
    }

    void Camera::SetDirection(const Vector3& direction)
    {
        m_direction = direction;
        m_direction = m_direction / m_direction.Length();
        Recalculate();
    }

    void Camera::SetTarget(const Vector3& target)
    {
        m_direction = (target - m_position);
        m_direction.Normalize();

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

    Vector3 Camera::GetPosition() const { return m_position; }

    Vector3 Camera::GetForward() const { return m_direction; }

    Vector3 Camera::GetRight() const { return m_right; }

    Vector3 Camera::GetLeft() const { return -m_right; }

    Vector3 Camera::GetUp() const { return m_up; }

    Vector3 Camera::GetBackward() const { return -m_direction; }

    float Camera::GetFov() const { return m_fov; }

    float Camera::GetPitch() const { return m_pitch; }

    float Camera::GetYaw() const { return m_yaw; }

    void Camera::Recalculate()
    {
        m_right = m_worldUp.Cross(m_direction);
        m_right = m_right / m_right.Length();

        m_up = m_direction.Cross(m_right);
        m_up = m_up / m_up.Length();
    }

    Matrix Camera::GetViewMatrix() const
    {
        return XMMatrixTranspose(Matrix::CreateLookAt(m_position, m_position + m_direction, m_up));
    }

    Matrix Camera::GetProjMatrix(int width, int height) const
    {
        float aspectRatio = static_cast<float>(width) / height;
        float fovAngleY = m_fov * DEG_TO_RAD;

        if (aspectRatio < 1.0f)
        {
            fovAngleY /= aspectRatio;
        }

        return XMMatrixTranspose(XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, m_nearPlane, m_farPlane));
    }

    Matrix Camera::GetViewProjMatrix(int width, int height) const
    {
        return GetViewMatrix() * GetProjMatrix(width, height);
    }

    Matrix Camera::GetInvViewProjMatrix(int width, int height) const
    {
        Matrix viewProj = GetViewProjMatrix(width, height);
        return viewProj.Invert();
    }

    Matrix Camera::GetOrthoProjMatrix(float width, float height) const
    {
        return XMMatrixTranspose(XMMatrixOrthographicRH(width, height, m_nearPlane, m_farPlane));
    }

    std::array<Matrix, 6> Camera::GetCubeViewMatrices() const
    {
        static const Vector3 directions[6] = {
            { 1.0f,  0.0f,  0.0f },  // +X
            {-1.0f,  0.0f,  0.0f },  // -X
            { 0.0f,  1.0f,  0.0f },  // +Y
            { 0.0f, -1.0f,  0.0f },  // -Y
            { 0.0f,  0.0f,  1.0f },  // +Z
            { 0.0f,  0.0f, -1.0f }   // -Z
        };

        static const Vector3 ups[6] = {
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
            matrices[i] = XMMatrixLookAtRH(m_position, m_position + directions[i], ups[i]);
        }
        return matrices;
    }
}
