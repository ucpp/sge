#include "sge_camera_controller.h"

#include "sge_input.h"
#include "sge_logger.h"

namespace SGE
{
    void CameraController::Initialize(CameraData* cameraData)
    {
        if(cameraData == nullptr)
        {
            LOG_ERROR("CameraController::Initialize camera data is null");
            return;
        }

        m_cameraData = cameraData;
        m_moveSpeed = cameraData->moveSpeed;
        m_sensitivity = cameraData->sensitivity;
    }

    void CameraController::Update(double deltaTime)
    {
        if (m_cameraData == nullptr)
        {
            return;
        }

        m_moveSpeed = m_cameraData->moveSpeed;
        m_sensitivity = m_cameraData->sensitivity;

        HandleMovement(deltaTime);
        HandleRotation();
        HandleFovAdjustment();
    }

    void CameraController::HandleMovement(double deltaTime)
    {
        const float velocity = m_moveSpeed * static_cast<float>(deltaTime);

        const float3 forward = CalculateForwardVector(m_cameraData->rotation[0], m_cameraData->rotation[1]);
        const float3 up(0, 1, 0);
        float3 right = forward.cross(up);
        right.normalize();

        if (Input::Get().GetKey('S'))
        {
            UpdatePosition(-forward, velocity);
        }
        if (Input::Get().GetKey('W'))
        {
            UpdatePosition(forward, velocity);
        }
        if (Input::Get().GetKey('A'))
        {
            UpdatePosition(right, velocity);
        }
        if (Input::Get().GetKey('D'))
        {
            UpdatePosition(-right, velocity);
        }
    }

    void CameraController::HandleRotation()
    {
        if (Input::Get().GetMouseButton(static_cast<unsigned int>(MouseButton::Right)))
        {
            float mouseX = static_cast<float>(Input::Get().GetMouseX());
            float mouseY = static_cast<float>(Input::Get().GetMouseY());
            float deltaX = mouseX - m_lastMousePosition.x;
            float deltaY = mouseY - m_lastMousePosition.y;

            m_lastMousePosition = float2(mouseX, mouseY);

            m_cameraData->rotation.y -= deltaX * m_sensitivity; // Yaw
            m_cameraData->rotation.x -= deltaY * m_sensitivity; // Pitch

            m_cameraData->rotation.x = std::clamp(m_cameraData->rotation.x, -MAX_PITCH, MAX_PITCH);
            m_cameraData->rotation.y = fmodf(m_cameraData->rotation.y, 360.0f);
            if (m_cameraData->rotation.y > 180.0f)
                m_cameraData->rotation.y -= 360.0f;
            else if (m_cameraData->rotation.y < -180.0f)
                m_cameraData->rotation.y += 360.0f;
        }
        else
        {
            float mouseX = static_cast<float>(Input::Get().GetMouseX());
            float mouseY = static_cast<float>(Input::Get().GetMouseY());
            m_lastMousePosition = float2(mouseX, mouseY);
        }
    }

    void CameraController::HandleFovAdjustment()
    {
        const int32 delta = Input::Get().GetMouseWheelDelta();
        m_cameraData->fov -= delta * m_sensitivity;
        m_cameraData->fov = std::clamp(m_cameraData->fov, MIN_FOV, MAX_FOV);
    }

    float3 CameraController::CalculateForwardVector(float pitch, float yaw) const
    {
        float pitchInRadians = DirectX::XMConvertToRadians(pitch);
        float yawInRadians = DirectX::XMConvertToRadians(yaw);

        return float3(
            cosf(pitchInRadians) * cosf(yawInRadians),
            sinf(pitchInRadians),
            cosf(pitchInRadians) * sinf(yawInRadians)
        );
    }

    void CameraController::UpdatePosition(float3 direction, float velocity)
    {
        if (direction.length() > 0)
        {
            direction.normalize();
            m_cameraData->position += direction * velocity;
        }
    }
}