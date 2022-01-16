#include "camera.h"

#include <GLFW/glfw3.h>
#include <math.h>

#include "input_system.h"

#include <iostream>
#include <memory>

namespace sge
{
    Camera::Camera(InputSystem &input, glm::vec3 position, float speed) : position(position), speed(speed)
    {
        this->input = &input;
        scroll_callback = this->input->on_mouse_scroll.addListener(std::bind(&Camera::processMouseScroll, this, std::placeholders::_1));

        front = glm::vec3(0.0f, 0.0f, -1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        world_up = up;

        mouse_speed = 0.1f;

        pitch = 0.0f;
        yaw = -90.0f;
        zoom = 45.0f;

        last_x = 0.0;
        last_y = 0.0;

        refresh();
    }

    Camera::~Camera()
    {
        if (input == nullptr)
        {
            return;
        }

        input->on_mouse_scroll.removeListener(scroll_callback);
    }

    void Camera::setPosition(double x, double y, double z)
    {
        position = glm::vec3(x, y, z);
    }

    void Camera::setSpeed(double speed)
    {
        speed = speed;
    }

    glm::vec3 Camera::getPosition() const
    {
        return position;
    }

    float Camera::getZoom() const
    {
        return zoom;
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(position, position - front, up);
    }

    void Camera::update(float delta_time)
    {
        updateMouseMovement();
        processInput(delta_time);
    }

    void Camera::updateMouseMovement()
    {
        if (input == nullptr)
        {
            return;
        }

        if (!input->isPressedMouseRight())
        {
            last_x = input->getX();
            last_y = input->getY();
            return;
        }

        double x = input->getX() - last_x;
        double y = last_y - input->getY();

        last_x = input->getX();
        last_y = input->getY();

        yaw += (x * mouse_speed);
        pitch -= (y * mouse_speed);
        pitch = std::min(std::max(pitch, -clamp_pitch_angle), clamp_pitch_angle);

        refresh();
    }

    void Camera::processInput(float delta_time)
    {
        float velocity = speed * delta_time;

        if (input->isKeyPressed(GLFW_KEY_W))
        {
            position -= front * velocity;
        }
        else if (input->isKeyPressed(GLFW_KEY_S))
        {
            position += front * velocity;
        }
        else if (input->isKeyPressed(GLFW_KEY_A))
        {
            position += right * velocity;
        }
        else if (input->isKeyPressed(GLFW_KEY_D))
        {
            position -= right * velocity;
        }
    }

    void Camera::processMouseScroll(float offset)
    {
        zoom -= offset * 2;
        zoom = std::min(std::max(zoom, zoom_min), zoom_max);
    }

    void Camera::refresh()
    {
        glm::vec3 front_direction;
        front_direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front_direction.y = sin(glm::radians(pitch));
        front_direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front_direction);

        right = glm::normalize(glm::cross(front, world_up));
        up = glm::normalize(glm::cross(right, front));
    }
}