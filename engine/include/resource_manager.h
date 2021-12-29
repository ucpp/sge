#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <glad/glad.h>

#include <map>
#include <string>

#include "shader.h"

class ResourceManager
{
public:
    static Shader& LoadShader(const char* vertex_file_name, const char* fragment_file_name, std::string name);
    static Shader& GetShader(std::string shader);
    static void Clear();

private:
    static std::map<std::string, Shader> shaders_;

private:
    static std::string LoadShaderSource(const std::string& file_name);
};

#endif