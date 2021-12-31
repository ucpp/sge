#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace Engine
{
    std::map<std::string, Shader> ResourceManager::shaders_;

    Shader& ResourceManager::LoadShader(const char* vertex_file_name, const char* fragment_file_name, std::string name)
    {
        std::string vertex_shader_source = LoadShaderSource(vertex_file_name);
        const char* vertex_source = vertex_shader_source.c_str();

        std::string fragment_shader_source = LoadShaderSource(fragment_file_name);
        const char* fragment_source = fragment_shader_source.c_str();
        
        Shader shader;
        shader.Compile(vertex_source, fragment_source);
        shaders_[name] = shader;

        return shaders_[name];
    }

    Shader& ResourceManager::GetShader(std::string name)
    {
        return shaders_[name];
    }

    void ResourceManager::Clear()
    {
        for(auto shader : shaders_)
        {
            glDeleteProgram(shader.second.GetId());
        }

        shaders_.clear();
    }

    std::string ResourceManager::LoadShaderSource(const std::string& file_name)
    {
        std::string file_source;

        try
        {
            std::ifstream file_stream(file_name);
            std::stringstream stream;
            stream << file_stream.rdbuf();
            file_stream.close();
            file_source = stream.str();
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }
        
        return file_source;
    }
    }