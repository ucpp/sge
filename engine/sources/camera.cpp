#include "camera.h"

#include <GLFW/glfw3.h>
#include <math.h>

#include "input_system.h"

#include <iostream>
#include <memory>

namespace Engine
{
    Camera::Camera(InputSystem &input, glm::vec3 position, float speed) : position_(position), speed_(speed)
    {
        input_ = &input;
        scroll_callback_ = input_->on_mouse_scroll.AddListener(std::bind(&Camera::ProcessMouseScroll, this, std::placeholders::_1));

        front_ = glm::vec3(0.0f, 0.0f, -1.0f);
        up_ = glm::vec3(0.0f, 1.0f, 0.0f);
        world_up_ = up_;

        mouse_speed_ = 0.1f;

        pitch_ = 0.0f;
        yaw_ = -90.0f;
        zoom_ = 45.0f;

        last_x_ = 0.0;
        last_y_ = 0.0;

        Refresh();
    }

    Camera::~Camera()
    {
        if (input_ == nullptr)
        {
            return;
        }

        input_->on_mouse_scroll.RemoveListener(scroll_callback_);
    }

    void Camera::SetPosition(double x, double y, double z)
    {
        position_ = glm::vec3(x, y, z);
    }

    void Camera::SetSpeed(double speed)
    {
        speed_ = speed;
    }

    glm::vec3 Camera::GetPosition() const
    {
        return position_;
    }

    float Camera::GetZoom() const
    {
        return zoom_;
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(position_, position_ - front_, up_);
    }

    void Camera::Update(float delta_time)
    {
        UpdateMouseMovement();
        ProcessInput(delta_time);
    }

    void Camera::UpdateMouseMovement()
    {
        if (input_ == nullptr)
        {
            return;
        }

        if (!input_->IsPressedMouseRight())
        {
            last_x_ = input_->GetX();
            last_y_ = input_->GetY();
            return;
        }

        double x = input_->GetX() - last_x_;
        double y = last_y_ - input_->GetY();

        last_x_ = input_->GetX();
        last_y_ = input_->GetY();

        yaw_ += (x * mouse_speed_);
        pitch_ -= (y * mouse_speed_);
        pitch_ = std::min(std::max(pitch_, -kClampPitchAngle), kClampPitchAngle);

        Refresh();
    }

    void Camera::ProcessInput(float delta_time)
    {
        float velocity = speed_ * delta_time;

        if (input_->IsKeyPressed(GLFW_KEY_W))
        {
            position_ -= front_ * velocity;
        }
        else if (input_->IsKeyPressed(GLFW_KEY_S))
        {
            position_ += front_ * velocity;
        }
        else if (input_->IsKeyPressed(GLFW_KEY_A))
        {
            position_ += right_ * velocity;
        }
        else if (input_->IsKeyPressed(GLFW_KEY_D))
        {
            position_ -= right_ * velocity;
        }
    }

    void Camera::ProcessMouseScroll(float offset)
    {
        zoom_ -= offset * 2;
        zoom_ = std::min(std::max(zoom_, kZoomMin), kZoomMax);
    }

    void Camera::Refresh()
    {
        glm::vec3 front_direction;
        front_direction.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_direction.y = sin(glm::radians(pitch_));
        front_direction.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front_direction);

        right_ = glm::normalize(glm::cross(front_, world_up_));
        up_ = glm::normalize(glm::cross(right_, front_));
    }
}