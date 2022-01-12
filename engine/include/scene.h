#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "config.h"
#include "input_system.h"
#include "model.h"

#include <glm/glm.hpp>
#include <vector>

namespace Engine
{
    class Material
    {
    public:
        Material() : normal_maps(true){}
        Shader shader;
        bool normal_maps;
    };

    class Object
    {
    public:
        glm::vec3 position;
        Model model;
        Material material;
        double scale;
    };

    class PointLight
    {
    public:
        PointLightData data;
    };

    class Scene
    {
    public:
        Scene(SceneData data);
        void Init(InputSystem& input);
        void Update(float delta_time, float aspect_ratio);
        void Shutdown();

        void EnableNormalMaps(bool is_enable);
        Camera* GetMainCamera() const;

    private:
        SceneData data_;
        Camera* camera_;
        std::vector<Object*> objects_;
        std::vector<PointLight> point_lights_;

        bool inited_;
    };
}

#endif