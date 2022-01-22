#pragma once

#include "json.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace sge
{
    struct Vector3
    {
        double x;
        double y;
        double z;
    };

    struct ColorData
    {
        double r;
        double g;
        double b;
    };

    struct SettingsData
    {
        std::string application_name;
        int window_width;
        int window_height;
        bool vsync_enabled;
        bool imgui_enabled;
        ColorData clear_color;
        std::string start_scene;
    };

    struct ShaderData
    {
        std::string name;
        std::string path_to_fragment;
        std::string path_to_vertex;
        bool lit;
    };

    struct ModelData
    {
        std::string name;
        std::string path;
    };

    struct CubemapData
    {
        std::string name;
        std::vector<std::string> face_textures;
    };

    struct ResourcesData
    {
        std::vector<ShaderData> shaders;
        std::vector<ModelData> models;
        std::vector<CubemapData> cubemaps;
    };

    struct MaterialData
    {
        std::string shader;
        ColorData color;
    };

    struct ObjectData
    {
        std::string name;
        std::string model;
        bool enabled;
        Vector3 position;
        Vector3 rotation;
        float scale;
        MaterialData material;
    };

    struct CameraData
    {
        std::string name;
        Vector3 position;
        double speed;
    };

    struct DirectionalLightData
    {
        bool enabled;
        std::string gizmo;
        std::string gizmo_shader;
        Vector3 position;
        double ambient;
        double diffuse;
        double specular;
    };

    struct PointLightData
    {
        std::string gizmo;
        std::string gizmo_shader;
        Vector3 position;
        double ambient;
        double diffuse;
        double specular;
        double linear;
        double quadratic;
        ColorData color;
    };

    struct SceneData
    {
        std::string name;
        std::vector<ObjectData> objects;
        CameraData camera;
        std::string skybox;
        std::string skybox_shader;
        DirectionalLightData directional_light;
        std::vector<PointLightData> point_lights;
    };

    struct Data
    {
        SettingsData settings;
        ResourcesData resources;
        std::vector<SceneData> scenes;

        SceneData getStartScene()
        {
            auto find_iter = std::find_if(scenes.begin(), scenes.end(),
                                          [&](SceneData data)
                                          {
                                              return settings.start_scene == data.name;
                                          });
            if (find_iter != scenes.end())
            {
                return *find_iter;
            }

            std::cout << "Could not find scene " << settings.start_scene << "!" << std::endl;
            assert(scenes.size() > 0 && "Scenes array is empty!");

            return scenes.front();
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector3, x, y, z);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ColorData, r, g, b);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SettingsData, application_name, window_width, window_height, vsync_enabled, imgui_enabled, start_scene, clear_color);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ShaderData, name, path_to_fragment, path_to_vertex, lit);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ModelData, name, path);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CubemapData, name, face_textures);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ResourcesData, shaders, models, cubemaps);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MaterialData, shader, color);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ObjectData, name, model, enabled, position, rotation, scale, material);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CameraData, name, position, speed);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DirectionalLightData, enabled, gizmo, gizmo_shader, position, ambient, diffuse, specular);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PointLightData, gizmo, gizmo_shader, position, ambient, diffuse, specular, linear, quadratic, color);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SceneData, name, objects, camera, skybox, skybox_shader, directional_light, point_lights);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Data, settings, resources, scenes);
    
    class Config
    {
    public:
        bool load(const std::string &path);

    public:
        Data data;
    };
}