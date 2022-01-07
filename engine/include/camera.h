#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
    class InputSystem;

    class Camera
    {
    public:
        Camera(InputSystem& input, glm::vec3 position, float speed);

        void SetPosition(double x, double y, double z);
        void SetSpeed(double speed);
        void Update(float delta_time);

        glm::mat4 GetViewMatrix() const;
        glm::vec3 GetPosition() const;

    private:
        void Refresh();
        void UpdateMouseMovement();
        void ProcessInput(float delta_time);
    
    private:
        const float kClampPitchAngle = 89.0f;

        InputSystem* input_ {nullptr};

        double last_x_;
        double last_y_;

        float pitch_;
        float yaw_;

        float mouse_speed_;
        float speed_;

        glm::vec3 position_;
        glm::vec3 up_;
        glm::vec3 world_up_;
        glm::vec3 front_;
        glm::vec3 right_;
    };
}

#endif