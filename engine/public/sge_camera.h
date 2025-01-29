#ifndef _SGE_CAMERA_H_
#define _SGE_CAMERA_H_

#include "pch.h"

namespace SGE
{
    class Camera final
    {
    public:
        Camera();

        void Initialize(const float3& position = { 0.0f, 0.0f, 10.0f }, 
                        float pitch = 0.0f, 
                        float yaw = 90.0f, 
                        float fov = DEFAULT_FOV,
                        float nearPlane = DEFAULT_NEAR_PLANE, 
                        float farPlane = DEFAULT_FAR_PLANE);

        void SetPosition(const float3& position);
        void SetRotation(float pitch, float yaw);
        void SetDirection(const float3& direction);
        void SetTarget(const float3& target);
        void SetFov(float fov);
        void SetNearPlane(float nearPlane);
        void SetFarPlane(float farPlane);

        float3 GetPosition() const;
        float3 GetForward() const;
        float3 GetRight() const;
        float3 GetLeft() const;
        float3 GetUp() const;
        float3 GetBackward() const;
        
        float GetFov() const;
        float GetPitch() const;
        float GetYaw() const;
        float GetNear() const { return m_nearPlane; }
        float GetFar() const { return m_farPlane; }

        Matrix GetViewMatrix() const;
        Matrix GetProjMatrix(int width, int height) const;
        Matrix GetOrthoProjMatrix(float width, float height) const;
        Matrix Camera::GetViewProjMatrix(int width, int height) const;
        Matrix Camera::GetInvViewProjMatrix(int width, int height) const;

        std::array<Matrix, 6> GetCubeViewMatrices() const;

    private:
        void Recalculate();

        float3 m_position;
        float3 m_direction;
        float3 m_up;
        float3 m_right;
        const float3 m_worldUp = { 0.0f, 1.0f, 0.0f };

        float m_fov;
        float m_pitch;
        float m_yaw;
        float m_nearPlane;
        float m_farPlane;

        static constexpr float DEFAULT_FOV = 45.0f;
        static constexpr float DEFAULT_NEAR_PLANE = 0.01f;
        static constexpr float DEFAULT_FAR_PLANE = 125.0f;
        static constexpr float DEG_TO_RAD = XM_PI / 180.0f;
        static constexpr float RAD_TO_DEG = 180.0f / XM_PI;
    };
}

#endif // !_SGE_CAMERA_H_
