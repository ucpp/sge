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

    Shader &ResourceManager::LoadShader(const char *vertex_file_name, const char *fragment_file_name, std::string name)
    {
        std::string vertex_shader_source = LoadShaderSource(vertex_file_name);
        const char *vertex_source = vertex_shader_source.c_str();

        std::string fragment_shader_source = LoadShaderSource(fragment_file_name);
        const char *fragment_source = fragment_shader_source.c_str();

        Shader shader;
        shader.Compile(vertex_source, fragment_source);
        shaders_[name] = shader;

        return shaders_[name];
    }

    Texture &ResourceManager::LoadTexture(const char *file_name, std::string name, std::string type, bool alpha)
    {
        //std::cout <<"tex: " << name << std::endl;
        if (textures_.count(name) == 1)
        {
            return GetTexture(name);
        }

        Texture texture;
        texture.type = type;

        if (alpha)
        {
            texture.EnableAlpha();
        }

        //stbi_set_flip_vertically_on_load(true);

        int width = 0;
        int height = 0;
        int channels = 0;
        //size_t pixel_size = sizeof(float);
        size_t pixel_size = sizeof(stbi_uc);
        unsigned char *data = stbi_load(file_name, &width, &height, &channels, 0);
        uint32_t mip_levels = static_cast<int>(std::floor(std::log2(std::max(width, height))) + 1);
        texture.SetFormat(channels, pixel_size);
        texture.Generate(data, width, height);
        stbi_image_free(data);
        textures_[name] = texture;

        return textures_[name];
    }
    
    Model &ResourceManager::LoadModel(const char *file_name, std::string name)
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

    Model &ResourceManager::GetModel(std::string name)
    {
        return models_[name];
    }

    Shader &ResourceManager::GetShader(std::string name)
    {
        return shaders_[name];
    }

    Texture &ResourceManager::GetTexture(std::string name)
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

        for(auto model : models_)
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