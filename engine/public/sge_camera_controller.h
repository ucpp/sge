#ifndef _SGE_CAMERA_CONTROLLER_H_
#define _SGE_CAMERA_CONTROLLER_H_

#include "pch.h"

namespace SGE
{
    class CameraController final
    {
    public:
        void Update(double deltaTime);

        void SetCamera(class Camera* camera) { m_camera = camera; }
        void SetMoveSpeed(float speed) { m_moveSpeed = speed; }
        void SetSensitivity(float sensitivity) { m_sensitivity = sensitivity; }

    private:
        class Camera* m_camera = nullptr;
        float m_moveSpeed = 10.0f;
        float m_sensitivity = 0.1f;
        Vector2 m_lastMousePosition = {0.0f, 0.0f};

        static constexpr float MAX_PITCH = 89.0f; 
        static constexpr float MIN_FOV = 30.0f;
        static constexpr float MAX_FOV = 120.0f;
    };
}

#endif // !_SGE_CAMERA_CONTROLLER_H_