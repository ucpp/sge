#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "model.h"
#include "log.h"

namespace sge
{
	std::map<std::string, Shader> ResourceManager::shaders;
	std::map<std::string, Texture> ResourceManager::textures;
	std::map<std::string, Model> ResourceManager::models;
	std::map<std::string, Cubemap> ResourceManager::cubemaps;

	std::set<std::string> ResourceManager::user_shaders;
	unsigned char* ResourceManager::default_texture;

	void ResourceManager::loadResources(const ResourcesData& config)
	{
		Log::info("loading and compiling shaders:\n");
		auto shaders_data = config.shaders;
		for (auto& data : shaders_data)
		{
			loadShader(data);
		}
		Log::info("loading models with textures:\n");
		auto models_data = config.models;
		for (auto& data : models_data)
		{
			loadModel(data.path, data.name);
		}
		Log::info("loading cubemaps:\n");
		auto cubemaps = config.cubemaps;
		for (auto& data : cubemaps)
		{
			loadCubemap(data.face_textures, data.name);
		}
	}

	Shader ResourceManager::loadShader(const ShaderData& data)
	{
		Shader shader;
		shader.data = data;
		Log::info("%s\n", shader.data.name.c_str());
		shader.compile(loadShaderSource(shader.data.path_to_vertex), loadShaderSource(shader.data.path_to_fragment));
		shaders[shader.data.name] = shader;
		
		if (!shader.data.hide_in_editor)
		{
			user_shaders.emplace(shader.data.name);
		}

		return shaders[shader.data.name];
	}

	Texture ResourceManager::loadTexture(const std::string& file_name, const std::string& name, const std::string& type, bool alpha)
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

		const char* path = file_name.c_str();
		unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

		texture.setFormat(channels, sizeof(stbi_uc));
		printTextureSize(file_name, width, height, channels);
		if (data == nullptr)
		{
			texture.generate(getDefaultEmptyTexture(), width, height);
		}
		else
		{
			texture.generate(data, width, height);
			stbi_image_free(data);
		}

		textures[name] = texture;

		return textures[name];
	}

	Model ResourceManager::loadModel(const std::string& file_name, const std::string& name)
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

	Model ResourceManager::getModel(const std::string& name)
	{
		return models[name];
	}

	Shader ResourceManager::getShader(const std::string& name)
	{
		return shaders[name];
	}

	Texture ResourceManager::getTexture(const std::string& name)
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
		user_shaders.clear();

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

		if (default_texture != nullptr)
		{
			delete[] default_texture;
			default_texture = nullptr;
		}
	}

	std::string ResourceManager::loadShaderSource(const std::string& file_name)
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
		catch (const std::exception& e)
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
			faces_data.push_back(stbi_load(path, &width, &height, &channels, 3));
			printTextureSize(path, width, height, channels);
			if (faces_data[i] == nullptr)
			{
				faces_data[i] = getDefaultEmptyTexture();
				sizes.push_back(glm::ivec2(4, 4));
			}
			else
			{
				sizes.push_back(glm::ivec2(width, height));
			}
		}
		cubemap.generate(faces_data, sizes);
		for (uint32_t i = 0; i < faces_data.size(); ++i)
		{
			stbi_image_free(faces_data[i]);
		}

		cubemaps[name] = cubemap;
		faces_data.clear();
		return cubemaps[name];
	}

	Cubemap ResourceManager::getCubemap(const std::string& name)
	{
		return cubemaps[name];
	}

	std::set<std::string>& ResourceManager::getUserShaderNames()
	{
		return user_shaders;
	}

	void ResourceManager::printTextureSize(const std::string& name, uint32_t width, uint32_t height, uint32_t channels)
	{
		Log::info("Texture %s loaded with size: %d mb\n", name.c_str(), width * height * channels * sizeof(stbi_uc) / 1024 / 1024);
	}

	unsigned char* ResourceManager::getDefaultEmptyTexture()
	{
		if (default_texture == nullptr)
		{
			uint32_t width = 4;
			uint32_t height = 4;

			default_texture = new unsigned char[64];

			for (int i = 0; i < 64; ++i)
			{
				default_texture[i] = 255;
			}
		}

		return default_texture;
	}
}