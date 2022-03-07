#include "scene.h"
#include "resource_manager.h"
#include "log.h"

namespace sge
{
	Scene::Scene(SceneData data) : data(data), inited(false) {}

	void Scene::initialize(InputSystem& input)
	{
		CameraData cam = data.camera;
		this->camera = new Camera(input, glm::vec3(cam.position.x, cam.position.y, cam.position.z), cam.speed);
		this->camera->setRotation(cam.pitch, cam.yaw);

		for (const ObjectData& object_data : data.objects)
		{
			Object object;
			object.enabled = true;
			object.name = object_data.name;
			object.model = ResourceManager::getModel(object_data.model);
			object.material.shader = ResourceManager::getShader(object_data.material.shader);
			object.material.color = object_data.material.color;
			object.position = glm::vec3(object_data.position.x, object_data.position.y, object_data.position.z);
			object.rotation = glm::vec3(object_data.rotation.x, object_data.rotation.y, object_data.rotation.z);
			object.scale = static_cast<float>(object_data.scale);
			object.enabled = object_data.enabled;
			this->objects.push_back(object);
		}

		for (const auto& light : data.point_lights)
		{
			PointLight point_light;
			point_light.data = light;
			point_lights.push_back(point_light);
		}

		directional_light.data = data.directional_light;

		cascaded_shadow_map.initialize(data.shadow_map_size, data.shadow_map_size);
		depth_shader = ResourceManager::getShader("depth");

		pers_proj_info.z_near = 0.1f;
		pers_proj_info.z_far = 50.0f;

		cascade_end[0] = pers_proj_info.z_near;
		cascade_end[1] = 8.0f,
		cascade_end[2] = 20.0f,
		cascade_end[3] = pers_proj_info.z_far;

		if (!data.skybox.empty())
		{
			skybox_renderer.initialize(data.skybox, data.skybox_shader);
		}
		inited = true;
	}

	void Scene::resetShadowMap()
	{
		cascaded_shadow_map.destroy();
		cascaded_shadow_map.initialize(data.shadow_map_size, data.shadow_map_size);
	}

	uint32_t Scene::getShadowMapTexture(int index)
	{
		return cascaded_shadow_map.getTexId(index);
	}

	void Scene::update(float delta_time, int width, int height)
	{
		if (inited)
		{
			camera->update(delta_time);

			glm::mat4 view = camera->getViewMatrix();
			glm::mat4 projection = glm::mat4(1.0f);
			float aspect = static_cast<float>(width) / static_cast<float>(height);
			projection = glm::perspective(glm::radians(camera->getZoom()), aspect, pers_proj_info.z_near, pers_proj_info.z_far);

			calculateOrtoProjs(width, height);
			//Test(width, height);

			glClear(GL_DEPTH_BUFFER_BIT);

			for (uint32_t i = 0; i < CascadedShadowMapFBO::count_maps; ++i)
			{
				auto light_pos = directional_light.data.position;
				//glm::mat4 light_space = getLightSpaceMatrix(glm::vec3(light_pos.x, light_pos.y, light_pos.z), shadow_ortho_proj_info[i]);

				depth_shader.use();
				depth_shader.setMatrix4("lightSpace", ortho_container[i]);

				cascaded_shadow_map.bindForWriting(i);
				glClear(GL_DEPTH_BUFFER_BIT);

				for (auto& obj : objects)
				{
					if (!obj.enabled) continue;

					glm::mat4 model = getModelMatrix(obj);
					depth_shader.setMatrix4("model", model);
					obj.model.draw(depth_shader);
				}
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (auto& obj : objects)
			{
				if (!obj.enabled) continue;

				glm::mat4 model = getModelMatrix(obj);

				auto shader = obj.material.shader;
				shader.use();

				shader.setVec3("color", glm::vec3(obj.material.color.r, obj.material.color.g, obj.material.color.b));
				shader.setInt("skybox", 6);
				shader.setMatrix4("view", view);
				shader.setMatrix4("projection", projection);
				shader.setMatrix4("model", model);
				shader.setVec3("viewPosition", camera->getPosition());
				for (uint32_t i = 0; i < CascadedShadowMapFBO::count_maps; ++i)
				{
					auto light_pos = directional_light.data.position;
					//glm::mat4 light_space = getLightSpaceMatrix(glm::vec3(light_pos.x, light_pos.y, light_pos.z), shadow_ortho_proj_info[i]);
					shader.setMatrix4(("lightSpace[" + std::to_string(i) + "]").c_str(), ortho_container[i]);
					shader.setFloat(("cascadeEndClipSpace[" + std::to_string(i) + "]").c_str(), cascade_end[i+1]);
				}

				if (shader.data.lit)
				{
					shader.setFloat("material.shininess", 128.0f);
					shader.setBool("normals_enabled", obj.material.normal_maps);
					shader.setBool("skylight_enabled", skybox_enabled);
					int count_point_lights = 0;
					for (auto& light : point_lights)
					{
						if (!light.data.enabled) continue;

						setPointLight(shader, light.data, count_point_lights);
						count_point_lights++;
					}
					shader.setInt("count_point_lights", count_point_lights);
					if (data.directional_light.enabled)
					{
						setDirectionalLight(shader);
					}
				}
				cascaded_shadow_map.bindForReading();
				for (uint32_t i = 0; i < CascadedShadowMapFBO::count_maps; ++i)
				{
					shader.setInt(("shadowMap[" + std::to_string(i) + "]").c_str(), 5 + i);
				}
				obj.model.draw(shader, skybox_renderer.cubemap_texture);
			}
			if (!data.skybox.empty() && skybox_enabled)
			{
				skybox_renderer.render(view, projection);
			}
		}
	}

	glm::mat4 Scene::getModelMatrix(const Object& obj)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(obj.position.x, obj.position.y, obj.position.z));

		model = glm::rotate(model, static_cast<float>(glm::radians(obj.rotation.x)), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, static_cast<float>(glm::radians(obj.rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, static_cast<float>(glm::radians(obj.rotation.z)), glm::vec3(0.0f, 0.0f, 1.0f));

		model = glm::scale(model, glm::vec3(obj.scale));

		return model;
	}

	void Scene::setPointLight(Shader shader, PointLightData data, int index)
	{
		std::string light_name = "pointLights[" + std::to_string(index) + "]";
		shader.setVec3((light_name + ".color").c_str(), data.color.r, data.color.g, data.color.b);
		shader.setVec3((light_name + ".position").c_str(), data.position.x, data.position.y, data.position.z);

		shader.setFloat((light_name + ".linear").c_str(), data.linear);
		shader.setFloat((light_name + ".quadratic").c_str(), data.quadratic);

		shader.setFloat((light_name + ".ambient").c_str(), data.ambient);
		shader.setFloat((light_name + ".diffuse").c_str(), data.diffuse);
		shader.setFloat((light_name + ".specular").c_str(), data.specular);
	}

	void Scene::setDirectionalLight(Shader shader)
	{
		auto& data = directional_light.data;
		auto& position = data.position;
		shader.setBool("directional_enabled", data.enabled);
		shader.setVec3("directionalLight.direction", position.x, position.y, position.z);
		shader.setFloat("directionalLight.ambient", data.ambient);
		shader.setFloat("directionalLight.diffuse", data.diffuse);
		shader.setFloat("directionalLight.specular", data.specular);
	}

	void Scene::shutdown()
	{
		inited = false;
		skybox_renderer.shutdown();

		delete camera;
		camera = nullptr;
	}

	void Scene::enableNormalMaps(bool is_enable)
	{
		for (auto& obj : objects)
		{
			obj.material.normal_maps = is_enable;
		}
	}

	Camera* Scene::getMainCamera() const
	{
		return camera;
	}

	// TODO: move to render
	glm::mat4 Scene::getLightSpaceMatrix(glm::vec3 light_direction, OrthoProjInfo orto_info)
	{
		float distance = 5.0f;
		float near = 0.1f;
		float far = distance * 2;

		glm::mat4 light_projection = glm::ortho(orto_info.l, orto_info.r, orto_info.b, orto_info.t, orto_info.n, orto_info.f);

		glm::vec3 eye = camera->getPosition() - light_direction;
		glm::vec3 center = camera->getPosition();
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 light_view = glm::lookAt(eye, center, up);

		return light_projection * light_view;
	}

	uint32_t Scene::getCountVertices() const
	{
		uint32_t count_vertices = 0;
		for (auto& obj : objects)
		{
			if (obj.enabled)
			{
				count_vertices += obj.model.getCountVertices();
			}
		}

		return count_vertices;
	}

	DirectionalLight* Scene::getDirectionalLight()
	{
		return &directional_light;
	}

	std::vector<PointLight>& Scene::getPointLights()
	{
		return point_lights;
	}

	int& Scene::getShadowMapSize()
	{
		return data.shadow_map_size;
	}

	void Scene::calculateOrtoProjs(int width, int height)
	{
		float FOV, near_plane, far_plane, ratio;
		ratio = static_cast<float>(width) / static_cast<float>(height);
		near_plane = pers_proj_info.z_near;
		far_plane = pers_proj_info.z_far;

		float tan_half_HFOV = glm::tan(glm::radians(camera->getZoom() / 2.0f)) / ratio;
		float tan_half_VFOV = glm::tan(glm::radians(camera->getZoom() / 2.0));

		auto light_pos = directional_light.data.position;
		glm::vec3 light_direction(light_pos.x, light_pos.y, light_pos.z);
		glm::mat4 light_view = glm::lookAt(-glm::normalize(light_direction), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 proj = glm::perspective(glm::radians(camera->getZoom()), ratio, near_plane, far_plane);

		for (uint32_t i = 0; i < CascadedShadowMapFBO::count_maps; ++i)
		{
			float xn = cascade_end[i] * tan_half_HFOV;
			float xf = cascade_end[i + 1] * tan_half_HFOV;
			float yn = cascade_end[i] * tan_half_VFOV;
			float yf = cascade_end[i + 1] * tan_half_VFOV;
			Log::info("xn = %.1f, xf = %.1f, yn = %.1f, yf = %.1f\n",xn, xf, yn, yf);

			glm::vec4 frustum_corners[num_frustrum_corners] = {
				// near face
				glm::vec4( xn,  yn, -cascade_end[i], 1.0),
				glm::vec4(-xn,  yn, -cascade_end[i], 1.0),
				glm::vec4( xn, -yn, -cascade_end[i], 1.0),
				glm::vec4(-xn, -yn, -cascade_end[i], 1.0),

				// far face
				glm::vec4( xf,  yf, -cascade_end[i + 1], 1.0),
				glm::vec4(-xf,  yf, -cascade_end[i + 1], 1.0),
				glm::vec4( xf, -yf, -cascade_end[i + 1], 1.0),
				glm::vec4(-xf, -yf, -cascade_end[i + 1], 1.0)
			};

			glm::vec4 frustum_corners_light[num_frustrum_corners];

			float minX, maxX, minY, maxY, minZ, maxZ;
			minX = minY = minZ = std::numeric_limits<GLfloat>::max();
			maxX = maxY = maxZ = std::numeric_limits<GLfloat>::min();

			glm::mat4 cam = camera->getViewMatrix();
			glm::mat4 camInverse = glm::inverse(cam);

			for (uint32_t j = 0; j < num_frustrum_corners; ++j)
			{
				glm::vec4 vW = camInverse * frustum_corners[j];
				frustum_corners_light[j] = light_view * vW;

				minX = std::min(minX, frustum_corners_light[j].x);
				maxX = std::max(maxX, frustum_corners_light[j].x);
				minY = std::min(minY, frustum_corners_light[j].y);
				maxY = std::max(maxY, frustum_corners_light[j].y);
				minZ = std::min(minZ, -frustum_corners_light[j].z);
				maxZ = std::max(maxZ, -frustum_corners_light[j].z);
			}

			ortho_container[i] = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ) * light_view;
		}
	}

	void Scene::Test(uint32_t width, uint32_t height)
	{
		float FOV, near_plane, far_plane, ratio;
		ratio = static_cast<float>(width) / static_cast<float>(height);
		near_plane = pers_proj_info.z_near;
		far_plane = pers_proj_info.z_far;
		glm::mat4 proj = glm::perspective(glm::radians(camera->getZoom()), ratio, near_plane, far_plane);

		auto corners = getFrustumCornersWorldSpace(proj, camera->getViewMatrix());

		glm::vec3 center = glm::vec3(0, 0, 0);
		for (const auto& v : corners)
		{
			center += glm::vec3(v);
		}
		center /= corners.size();

		auto light_pos = directional_light.data.position;
		glm::vec3 light_direction(light_pos.x, light_pos.y, light_pos.z);
		const auto lightView = glm::lookAt(center + light_direction, center, glm::vec3(0.0f, 1.0f, 0.0f));

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::min();
		for (const auto& v : corners)
		{
			const auto trf = lightView * v;
			minX = std::min(minX, trf.x);
			maxX = std::max(maxX, trf.x);
			minY = std::min(minY, trf.y);
			maxY = std::max(maxY, trf.y);
			minZ = std::min(minZ, trf.z);
			maxZ = std::max(maxZ, trf.z);
		}

		constexpr float zMult = 10.0f;
		if (minZ < 0)
		{
			minZ *= zMult;
		}
		else
		{
			minZ /= zMult;
		}
		if (maxZ < 0)
		{
			maxZ /= zMult;
		}
		else
		{
			maxZ *= zMult;
		}

		const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

		ortho_container[0] = lightProjection * lightView;
		ortho_container[1] = lightProjection * lightView;
		ortho_container[2] = lightProjection * lightView;
	}

	std::vector<glm::vec4> Scene::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
	{
		const auto inv = glm::inverse(proj * view);

		std::vector<glm::vec4> frustumCorners;
		for (unsigned int x = 0; x < 2; ++x)
		{
			for (unsigned int y = 0; y < 2; ++y)
			{
				for (unsigned int z = 0; z < 2; ++z)
				{
					const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
					frustumCorners.push_back(pt / pt.w);
				}
			}
		}

		return frustumCorners;
	}
}