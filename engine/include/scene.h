#pragma once

#include "camera.h"
#include "config.h"
#include "input_system.h"
#include "model.h"
#include "render_texture.h"
#include "cubemap_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace sge
{
	class Material
	{
	public:
		Material() : normal_maps(true) {}
		Shader shader;
		bool normal_maps;
		ColorData color;
	};

	class Object
	{
	public:
		std::string name;
		bool enabled;
		glm::vec3 position;
		glm::vec3 rotation;
		Model model;
		Material material;
		double scale;
	};

	class DirectionalLight
	{
	public:
		DirectionalLightData data;
	};

	class PointLight
	{
	public:
		PointLightData data;
	};

	class Scene
	{
	public:
		Scene(SceneData data);
		void initialize(InputSystem& input);
		void update(float delta_time, int width, int height);
		void shutdown();

		void enableNormalMaps(bool is_enable);
		Camera* getMainCamera() const;
		uint32_t getCountVertices() const;
		DirectionalLight* getDirectionalLight();
		std::vector<PointLight>& getPointLights();
		bool skyboxEnabled();

	public:
		std::vector<Object> objects;
		bool skybox_enabled = false;

	private:
		glm::mat4 getModelMatrix(const Object& obj);
		void setPointLight(Shader shader, PointLightData data, int index);
		void setDirectionalLight(Shader shader);
		glm::mat4 getLightSpaceMatrix(glm::vec3 ligth_direction);

	private:
		Camera* camera{ nullptr };
		SceneData data;
		Shader depth_shader;
		std::vector<PointLight> point_lights;
		DirectionalLight directional_light;
		RenderTexture shadow_buffer;
		CubemapRenderer skybox_renderer;
		Shader skybox_shader;
		bool inited;

		const int shadow_map_size = 1024;
	};
}