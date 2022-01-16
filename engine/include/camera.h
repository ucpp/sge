#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "event.h"

namespace sge
{
    class InputSystem;

    class Camera
    {
    public:
        Camera(InputSystem &input, glm::vec3 position, float speed);
        ~Camera();

        void SetPosition(double x, double y, double z);
        void SetSpeed(double speed);
        void Update(float delta_time);

        glm::mat4 GetViewMatrix() const;
        glm::vec3 GetPosition() const;
        float GetZoom() const;

    private:
        void Refresh();
        void UpdateMouseMovement();
        void ProcessMouseScroll(float offset);
        void ProcessInput(float delta_time);

    private:
        const float kClampPitchAngle = 89.0f;
        const float kZoomMax = 45.0f;
        const float kZoomMin = 1.0f;

        InputSystem *input_{nullptr};

        double last_x_;
        double last_y_;

        float pitch_;
        float yaw_;
        float zoom_;

        float mouse_speed_;
        float speed_;

        glm::vec3 position_;
        glm::vec3 up_;
        glm::vec3 world_up_;
        glm::vec3 front_;
        glm::vec3 right_;

        std::shared_ptr<void> scroll_callback_;
    };
}