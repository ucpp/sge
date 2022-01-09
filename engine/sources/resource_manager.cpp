#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "model.h"

namespace Engine
{
    std::map<std::string, Shader> ResourceManager::shaders_;
    std::map<std::string, Texture> ResourceManager::textures_;
    std::map<std::string, Model> ResourceManager::models_;

    Shader &ResourceManager::LoadShader(const std::string& vertex_file_name, const std::string& fragment_file_name, const std::string& name)
    {
        std::string vertex_shader_source = LoadShaderSource(vertex_file_name);
        std::string fragment_shader_source = LoadShaderSource(fragment_file_name);

        Shader shader;
        shader.Compile(vertex_shader_source, fragment_shader_source);
        shaders_[name] = shader;

        return shaders_[name];
    }

    Texture &ResourceManager::LoadTexture(const std::string& file_name, const std::string& name, const std::string& type, bool alpha)
    {
        if (textures_.count(name) == 1)
        {
            return GetTexture(name);
        }

        Texture texture;
        texture.type = type;

        // stbi_set_flip_vertically_on_load(true);

        int width = 0;
        int height = 0;
        int channels = 0;
        const char* path = file_name.c_str();
        unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

        texture.SetFormat(channels, sizeof(stbi_uc));
        texture.Generate(data, width, height);

        stbi_image_free(data);
        textures_[name] = texture;

        return textures_[name];
    }

    Model &ResourceManager::LoadModel(const std::string& file_name, const std::string& name)
    {
        if (models_.count(name) == 1)
        {
            return GetModel(name);
        }
        Model model;
        model.Load(file_name);
        models_[name] = model;

        return models_[name];
    }

    Model &ResourceManager::GetModel(const std::string& name)
    {
        return models_[name];
    }

    Shader &ResourceManager::GetShader(const std::string& name)
    {
        return shaders_[name];
    }

    Texture &ResourceManager::GetTexture(const std::string& name)
    {
        return textures_[name];
    }

    void ResourceManager::Clear()
    {
        for (auto shader : shaders_)
        {
            glDeleteProgram(shader.second.GetId());
        }

        shaders_.clear();

        for (auto texture : textures_)
        {
            glDeleteTextures(1, &texture.second.id);
        }

        textures_.clear();

        for (auto model : models_)
        {
            model.second.Clear();
        }

        models_.clear();
    }

    std::string ResourceManager::LoadShaderSource(const std::string &file_name)
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
        catch (const std::exception &e)
        {
            std::cout << e.what() << '\n';
        }

        return file_source;
    }
}