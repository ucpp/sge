#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
    class Camera
    {
    public:
        void Init(
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
            float spped = 100,
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            float yaw = -90.0f,
            float pitch = 0.0f);

        glm::mat4 GetViewMatrix();
        void ProcessInput(int key_code, float delta_time);
        void ProcessMouseMovement(float x, float y);

    public:
        glm::vec3 position;
        glm::vec3 up;
        glm::vec3 world_up;
        glm::vec3 front;
        glm::vec3 right;
        float speed;
        float mouse_speed;
        float yaw;
        float pitch;

    private:
        void Update();
    };
}

#endif