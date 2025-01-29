#ifndef _SGE_CAMERA_CONTROLLER_H_
#define _SGE_CAMERA_CONTROLLER_H_

#include "pch.h"
#include "sge_math.h"

namespace SGE
{
    class CameraController final
    {
    public:
        void Initialize(class CameraData* cameraData);
        void Update(double deltaTime);

    private:
        void HandleMovement(double deltaTime);
        void HandleRotation();
        void HandleFovAdjustment();

        float3 CalculateForwardVector(float pitch, float yaw) const;
        void UpdatePosition(float3 direction, float velocity);

        class CameraData* m_cameraData = nullptr;
        float m_moveSpeed = 10.0f;
        float m_sensitivity = 0.1f;
        float2 m_lastMousePosition = {0.0f, 0.0f};

        static constexpr float MAX_PITCH = 89.0f; 
        static constexpr float MIN_FOV = 30.0f;
        static constexpr float MAX_FOV = 120.0f;
    };
}

#endif // !_SGE_CAMERA_CONTROLLER_H_