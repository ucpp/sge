#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "model.h"

namespace sge
{
    std::map<std::string, Shader> ResourceManager::shaders;
    std::map<std::string, Texture> ResourceManager::textures;
    std::map<std::string, Model> ResourceManager::models;
    std::map<std::string, Cubemap> ResourceManager::cubemaps;

    void ResourceManager::loadResources(const ResourcesData &config)
    {
        auto shaders_data = config.shaders;
        for (auto& data : shaders_data)
        {
            loadShader(data.path_to_vertex, data.path_to_fragment, data.name, data.lit);
        }

        auto models_data = config.models;
        for (auto& data : models_data)
        {
            loadModel(data.path, data.name);
        }

        auto cubemaps = config.cubemaps;
        for (auto& data : cubemaps)
        {
            loadCubemap(data.face_textures, data.name);
        }
    }

    Shader ResourceManager::loadShader(const std::string &vertex_file_name, const std::string &fragment_file_name, const std::string &name, bool lit)
    {
        std::string vertex_shader_source = loadShaderSource(vertex_file_name);
        std::string fragment_shader_source = loadShaderSource(fragment_file_name);

        Shader shader;
        shader.is_lit = lit;
        shader.compile(vertex_shader_source, fragment_shader_source);
        shaders[name] = shader;

        return shaders[name];
    }

    Texture ResourceManager::loadTexture(const std::string &file_name, const std::string &name, const std::string &type, bool alpha)
    {
        if (textures.count(name) == 1)
        {
            return getTexture(name);
        }

        Texture texture;
        texture.type = type;

        // stbi_set_flip_vertically_on_load(true);

        int width = 0;
        int height = 0;
        int channels = 0;
        const char *path = file_name.c_str();
        unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

        if (data == nullptr)
        {
            width = 4;
            height = 4;

            data = new unsigned char[64];

            for (int i = 0; i < 64; ++i)
            {
                data[i] = 255;
            }
        }

        texture.setFormat(channels, sizeof(stbi_uc));
        texture.generate(data, width, height);

        stbi_image_free(data);
        textures[name] = texture;

        return textures[name];
    }

    Model ResourceManager::loadModel(const std::string &file_name, const std::string &name)
    {
        if (models.count(name) == 1)
        {
            return getModel(name);
        }
        Model model;
        model.load(file_name);
        models[name] = model;

        return models[name];
    }

    Model ResourceManager::getModel(const std::string &name)
    {
        return models[name];
    }

    Shader ResourceManager::getShader(const std::string &name)
    {
        return shaders[name];
    }

    Texture ResourceManager::getTexture(const std::string &name)
    {
        return textures[name];
    }

    void ResourceManager::clear()
    {
        for (auto shader : shaders)
        {
            glDeleteProgram(shader.second.getId());
        }

        shaders.clear();

        for (auto texture : textures)
        {
            glDeleteTextures(1, &texture.second.id);
        }

        textures.clear();

        for (auto model : models)
        {
            model.second.clear();
        }

        models.clear();

        for (auto cubemap : cubemaps)
        {
            glDeleteTextures(1, &cubemap.second.id);
        }

        cubemaps.clear();
    }

    std::string ResourceManager::loadShaderSource(const std::string &file_name)
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

    Cubemap ResourceManager::loadCubemap(const std::vector<std::string>& faces, const std::string& name)
    {
        if (cubemaps.count(name) == 1)
        {
            return getCubemap(name);
        }

        Cubemap cubemap;
        std::vector<unsigned char*> faces_data;
        std::vector<glm::ivec2> sizes;

        for (uint32_t i = 0; i < faces.size(); ++i)
        {
            int width = 0;
            int height = 0;
            int channels = 0;
            const char* path = faces[i].c_str();
            unsigned char* data = stbi_load(path, &width, &height, &channels, 3);

            if (data == nullptr)
            {
                width = 4;
                height = 4;

                data = new unsigned char[64];

                for (int i = 0; i < 64; ++i)
                {
                    data[i] = 255;
                }
            }

            faces_data.push_back(data);
            sizes.push_back(glm::ivec2(width, height));
        }
        cubemap.generate(faces_data, sizes);

        cubemaps[name] = cubemap;

        for (uint32_t i = 0; i < faces.size(); ++i)
        {
            stbi_image_free(faces_data[i]);
        }

        return cubemaps[name];
    }

    Cubemap ResourceManager::getCubemap(const std::string& name)
    {
        return cubemaps[name];
    }
}