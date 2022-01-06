#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <glad/glad.h>

#include <map>
#include <string>

#include "shader.h"
#include "texture.h"

namespace Engine
{
    class ResourceManager
    {
    public:
        static Shader &LoadShader(const char *vertex_file_name, const char *fragment_file_name, std::string name);
        static Shader GetShader(std::string shader);

        static Texture &LoadTexture(const char *file_name, std::string name, std::string type = "diffuse", bool alpha = false);
        static Texture &GetTexture(std::string name);

        static void Clear();

    private:
        static std::map<std::string, Shader> shaders_;
        static std::map<std::string, Texture> textures_;

    private:
        static std::string LoadShaderSource(const std::string &file_name);
    };
}
#endif