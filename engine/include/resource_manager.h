#pragma once

#include <glad/glad.h>

#include <map>
#include <string>

#include "shader.h"
#include "texture.h"
#include "config.h"

namespace sge
{
    class Model;

    class ResourceManager
    {
    public:
        static void LoadResources(const ResourcesData &config);

        static Shader LoadShader(const std::string &vertex_file_name, const std::string &fragment_file_name, const std::string &name, bool lit = false);
        static Shader GetShader(const std::string &shader);

        static Texture LoadTexture(const std::string &file_name, const std::string &name, const std::string &type = "diffuse", bool alpha = false);
        static Texture GetTexture(const std::string &name);

        static Model LoadModel(const std::string &file_name, const std::string &name);
        static Model GetModel(const std::string &name);

        static void Clear();

    private:
        static std::map<std::string, Shader> shaders_;
        static std::map<std::string, Texture> textures_;
        static std::map<std::string, Model> models_;

    private:
        static std::string LoadShaderSource(const std::string &file_name);
    };
}