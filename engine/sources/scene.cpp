#include "scene.h"
#include "resource_manager.h"
#include "log.h"

namespace sge
{
    Scene::Scene(SceneData data) : data(data), inited(false) {}
    
    void Scene::initialize(InputSystem &input)
    {
        CameraData cam = data.camera;
        this->camera = new Camera(input, glm::vec3(cam.position.x, cam.position.y, cam.position.z), cam.speed);

        for (const ObjectData &object_data : data.objects)
        {
            Object object;
            object.enabled = true;
            object.name = object_data.name;
            object.model = ResourceManager::getModel(object_data.model);
            object.material.shader = ResourceManager::getShader(object_data.material.shader);
            object.material.color = glm::vec3(object_data.material.color.r, object_data.material.color.g, object_data.material.color.b);
            object.position = glm::vec3(object_data.position.x, object_data.position.y, object_data.position.z);
            object.rotation = glm::vec3(object_data.rotation.x, object_data.rotation.y, object_data.rotation.z);
            object.scale = static_cast<float>(object_data.scale);
            object.enabled = object_data.enabled;
            this->objects.push_back(object);
        }

        for (const auto &light : data.point_lights)
        {
            PointLight point_light;
            point_light.data = light;
            point_lights.push_back(point_light);
        }

        directional_light.data = data.directional_light;

        shadow_buffer.initialize(shadow_map_size);
        depth_shader = ResourceManager::getShader("depth");

        if (!data.skybox.empty())
        {
            skybox_renderer.initialize(data.skybox, data.skybox_shader);
        }
        inited = true;
    }

    void Scene::update(float delta_time, int width, int height)
    {
        if (inited)
        {
            shadow_buffer.clear();

            auto light_pos = directional_light.data.position;
            glm::mat4 light_space = getLightSpaceMatrix(glm::vec3(light_pos.x, light_pos.y, light_pos.z));

            depth_shader.use();
            depth_shader.setMatrix4("lightSpace", light_space);

            shadow_buffer.bind();
            shadow_buffer.clear();

            for (auto &obj : objects)
            {
                if (!obj.enabled) continue;

                glm::mat4 model = getModelMatrix(obj);
                depth_shader.setMatrix4("model", model);
                obj.model.draw(depth_shader);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            camera->update(delta_time);

            glm::mat4 view = camera->getViewMatrix();
            glm::mat4 projection = glm::mat4(1.0f);
            float aspect = static_cast<float>(width) / static_cast<float>(height);
            projection = glm::perspective(glm::radians(camera->getZoom()), aspect, 0.1f, 1000.0f);

            for (auto &obj : objects)
            {
                if (!obj.enabled) continue;

                glm::mat4 model = getModelMatrix(obj);

                auto shader = obj.material.shader;
                shader.use();
                shader.setVec3("color", obj.material.color);
                shader.setInt("skybox", 6);
                shader.setMatrix4("view", view);
                shader.setMatrix4("projection", projection);
                shader.setMatrix4("model", model);
                shader.setVec3("viewPosition", camera->getPosition());
                shader.setMatrix4("lightSpace", light_space);

                if (shader.is_lit)
                {
                    shader.setFloat("material.shininess", 128.0f);
                    shader.setBool("normals_enabled", obj.material.normal_maps);

                    int i = 0;
                    for (auto &light : point_lights)
                    {
                        setPointLight(shader, light.data, i);
                        i++;
                    }
                    if (data.directional_light.enabled)
                    {
                        setDirectionalLight(shader);
                    }
                }
                shadow_buffer.bindTexture(5);
                shader.setInt("shadowMap", 5);
                obj.model.draw(shader, skybox_renderer.cubemap_texture);
            }
            if (!data.skybox.empty()) 
            {
                skybox_renderer.render(view, projection);
            }
        }
    }

    glm::mat4 Scene::getModelMatrix(const Object &obj)
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
        for (auto &obj : objects)
        {
            obj.material.normal_maps = is_enable;
        }
    }

    Camera *Scene::getMainCamera() const
    {
        return camera;
    }

    // TODO: move to render
    glm::mat4 Scene::getLightSpaceMatrix(glm::vec3 light_direction)
    {
        float distance = 8.0f;
        float near = 1.0f;
        float far = distance * 2;

        glm::mat4 light_projection = glm::ortho(-distance, distance, -distance, distance, near, far);

        glm::vec3 eye = camera->getPosition() - normalize(light_direction) * distance;
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

    DirectionalLight *Scene::getDirectionalLight()
    {
        return &directional_light;
    }
}