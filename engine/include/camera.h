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

        void setPosition(double x, double y, double z);
        void setSpeed(double speed);
        void update(float delta_time);

        glm::mat4 getViewMatrix() const;
        glm::vec3 getPosition() const;
        float getZoom() const;

    private:
        void refresh();
        void updateMouseMovement();
        void processMouseScroll(float offset);
        void processInput(float delta_time);

    private:
        const float clamp_pitch_angle = 89.0f;
        const float zoom_max = 45.0f;
        const float zoom_min = 1.0f;

        InputSystem *input{nullptr};

        double last_x;
        double last_y;

        float pitch;
        float yaw;
        float zoom;

        float mouse_speed;
        float speed;

        glm::vec3 position;
        glm::vec3 up;
        glm::vec3 world_up;
        glm::vec3 front;
        glm::vec3 right;

        std::shared_ptr<void> scroll_callback;
    };
}