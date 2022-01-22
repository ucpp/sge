#pragma once

#include <glad/glad.h>

#include <map>
#include <string>
#include <set>

#include "shader.h"
#include "texture.h"
#include "config.h"
#include "cubemap.h"

namespace sge
{
    class Model;

    class ResourceManager
    {
    public:
        static void loadResources(const ResourcesData &config);

        static Shader loadShader(const std::string &vertex_file_name, const std::string &fragment_file_name, const std::string &name, bool lit = false);
        static Shader getShader(const std::string &shader);

        static Texture loadTexture(const std::string &file_name, const std::string &name, const std::string &type = "diffuse", bool alpha = false);
        static Texture getTexture(const std::string &name);

        static Model loadModel(const std::string &file_name, const std::string &name);
        static Model getModel(const std::string &name);

        static Cubemap loadCubemap(const std::vector<std::string>& faces, const std::string &name);
        static Cubemap getCubemap(const std::string& name);

        static std::set<std::string>& getAllShaderNames();

        static void clear();

    private:
        static std::map<std::string, Shader> shaders;
        static std::map<std::string, Texture> textures;
        static std::map<std::string, Model> models;
        static std::map<std::string, Cubemap> cubemaps;

        static std::set<std::string> shader_names;

    private:
        static std::string loadShaderSource(const std::string &file_name);
    };
}