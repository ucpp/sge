#include "sge_camera_controller.h"

#include "sge_input.h"
#include "sge_camera.h"

namespace SGE
{
    void CameraController::Update(double deltaTime)
    {
        if (m_camera == nullptr)
        {
            return;
        }

        float velocity = m_moveSpeed * static_cast<float>(deltaTime);

        if (Input::Get().GetKey('S')) 
        {
            m_camera->SetPosition(m_camera->GetPosition() - m_camera->GetForward() * velocity);
        }
        if (Input::Get().GetKey('W')) 
        {
            m_camera->SetPosition(m_camera->GetPosition() - m_camera->GetBackward() * velocity);
        }
        if (Input::Get().GetKey('A')) 
        {
            m_camera->SetPosition(m_camera->GetPosition() - m_camera->GetLeft() * velocity);
        }
        if (Input::Get().GetKey('D')) 
        {
            m_camera->SetPosition(m_camera->GetPosition() - m_camera->GetRight() * velocity);
        }

        const int32 delta = Input::Get().GetMouseWheelDelta();
        float currentFov = m_camera->GetFov();
        currentFov -= delta * m_sensitivity;
        currentFov = std::clamp(currentFov, MIN_FOV, MAX_FOV);
        m_camera->SetFov(currentFov);

        // Rotation
        if (Input::Get().GetMouseButton(static_cast<unsigned int>(MouseButton::Right)))
        {
            float mouseX = static_cast<float>(Input::Get().GetMouseX());
            float mouseY = static_cast<float>(Input::Get().GetMouseY());
            float deltaX = mouseX - m_lastMousePosition.x;
            float deltaY = mouseY - m_lastMousePosition.y;

            m_lastMousePosition = Vector2(mouseX, mouseY);

            float yaw = m_camera->GetYaw() + deltaX * m_sensitivity;
            float pitch = m_camera->GetPitch() - deltaY * m_sensitivity;

            pitch = std::clamp(pitch, -MAX_PITCH, MAX_PITCH);
            yaw = fmodf(yaw, 360.0f);
            if (yaw > 180.0f)
                yaw -= 360.0f;
            else if (yaw < -180.0f)
                yaw += 360.0f;

            m_camera->SetRotation(pitch, yaw);
        }
        else
        {
            float mouseX = static_cast<float>(Input::Get().GetMouseX());
            float mouseY = static_cast<float>(Input::Get().GetMouseY());
            m_lastMousePosition = Vector2(mouseX, mouseY);
        }
    }
}