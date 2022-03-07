#pragma once

#include "camera.h"
#include "config.h"
#include "input_system.h"
#include "model.h"
#include "cascaded_shadow_map_fbo.h"
#include "shadow_map_fbo.h"
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
		Model model;
		Material material;
		glm::vec3 position;
		glm::vec3 rotation;
		double scale;
		bool enabled;
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

	struct PersProjInfo
	{
		float z_near;
		float z_far;
	};

	struct OrthoProjInfo
	{
		float r;        // right
		float l;        // left
		float b;        // bottom
		float t;        // top
		float n;        // z near
		float f;        // z far
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
		int& getShadowMapSize();
		void resetShadowMap();
		uint32_t getShadowMapTexture(int index);

	public:
		std::vector<Object> objects;
		bool skybox_enabled = false;

	private:
		glm::mat4 getModelMatrix(const Object& obj);
		void setPointLight(Shader shader, PointLightData data, int index);
		void setDirectionalLight(Shader shader);
		glm::mat4 getLightSpaceMatrix(glm::vec3 ligth_direction, OrthoProjInfo orto_info);
		void calculateOrtoProjs(int width, int height);

		std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
		void Test(uint32_t width, uint32_t height);

	private:
		Camera* camera{ nullptr };
		SceneData data;
		Shader skybox_shader;

		std::vector<PointLight> point_lights;
		DirectionalLight directional_light;
		CubemapRenderer skybox_renderer;

		Shader depth_shader;
		CascadedShadowMapFBO cascaded_shadow_map;
		PersProjInfo pers_proj_info;
		OrthoProjInfo shadow_ortho_proj_info[CascadedShadowMapFBO::count_maps];
		glm::mat4 ortho_container[CascadedShadowMapFBO::count_maps];
		float cascade_end[CascadedShadowMapFBO::count_maps + 1];
		static const int num_frustrum_corners = 8;

		bool inited;
	};
}