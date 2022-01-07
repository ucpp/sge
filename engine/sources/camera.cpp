#include "camera.h"

#include <GLFW/glfw3.h>
#include <math.h>

#include "input_system.h"

namespace Engine
{
    void Camera::SetInputSystem(InputSystem& input)
    {
        input_ = &input;
    }

    void Camera::Init(glm::vec3 position, float speed, glm::vec3 up, float yaw, float pitch)
    {
        this->position = position;
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        this->up = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->speed = speed;
        this->world_up = up;
        this->mouse_speed = 0.1f;
        Refresh();
    }

    void Camera::Update(float delta_time)
    {
        UpdateMouseMovement();
        ProcessInput(delta_time);
    }

    glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(position, position - front, up);
    }

    void Camera::UpdateMouseMovement()
    {
        if(input_ == nullptr)
        {
            return;
        }

        if(!input_->IsPressedMouseRight())
        {
            last_x_ = input_->GetX();
            last_y_ = input_->GetY();
            return;
        }

        double x = input_->GetX() - last_x_;
        double y = last_y_ - input_->GetY();

        last_x_ = input_->GetX();
        last_y_ = input_->GetY();

        yaw += (x * mouse_speed);
        pitch -= (y * mouse_speed);
        pitch = std::min(std::max(pitch, -89.0f), 89.0f);

        Refresh();
    }

    void Camera::ProcessInput(float delta_time)
    {
        float velocity = speed * delta_time;

        if (input_->IsKeyPressed(GLFW_KEY_W))
        {
            position -= front * velocity;
        }
        else if (input_->IsKeyPressed(GLFW_KEY_S))
        {
            position += front * velocity;
        }
        else if (input_->IsKeyPressed(GLFW_KEY_A))
        {
            position += right * velocity;
        }
        else if (input_->IsKeyPressed(GLFW_KEY_D))
        {
            position -= right * velocity;
        }
    }

    void Camera::Refresh()
    {
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);

        right = glm::normalize(glm::cross(front, world_up));
        up = glm::normalize(glm::cross(right, front));
    }
}