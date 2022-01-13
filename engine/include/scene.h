#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "config.h"
#include "input_system.h"
#include "model.h"
#include "render_texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
        glm::vec3 rotation;
        Model model;
        Material material;
        double scale;
    };

    class DirectionalLight
    {
    public:
        DirectionalLightData data;
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
        void Update(float delta_time, int width, int height);
        void Shutdown();

        void EnableNormalMaps(bool is_enable);
        Camera* GetMainCamera() const;

    private:
        glm::mat4 GetModelMatrix(const Object& obj);
        void SetPointLight(Shader shader, PointLightData data, int index);
        void SetDirectionalLight(Shader shader);
        glm::mat4 GetLightSpaceMatrix(glm::vec3 ligth_direction);
    
    private:
        SceneData data_;
        Camera* camera_;
        std::vector<Object> objects_;
        std::vector<PointLight> point_lights_;
        DirectionalLight directional_light_;
        RenderTexture shadow_buffer_;
        bool inited_;

        const int kShadowMapSize = 1024;
    };
}

#endif