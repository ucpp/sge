#include "scene.h"
#include "resource_manager.h"

namespace Engine
{
    Scene::Scene(SceneData data) : data_(data), inited_(false) {}


    Shader depth_shader_;

    void Scene::Init(InputSystem &input)
    {
        CameraData cam = data_.camera;
        camera_ = new Camera(input, glm::vec3(cam.position.x, cam.position.y, cam.position.z), cam.speed);

        for(const ObjectData& object_data : data_.objects)
        {
            Object object;
            object.model = ResourceManager::GetModel(object_data.model);
            object.material.shader = ResourceManager::GetShader(object_data.material.shader);
            object.position = glm::vec3(object_data.position.x, object_data.position.y, object_data.position.z);
            object.rotation = glm::vec3(object_data.rotation.x, object_data.rotation.y, object_data.rotation.z);
            object.scale = static_cast<float>(object_data.scale);
            objects_.push_back(object);
        }

        for(const auto& light : data_.point_lights)
        {
            PointLight point_light;
            point_light.data = light;
            point_lights_.push_back(point_light);
        }

        directional_light_.data = data_.directional_light;

        shadow_buffer_.Init(kShadowMapSize);
        depth_shader_ = ResourceManager::GetShader("depth");
        inited_ = true;
    }

    void Scene::Update(float delta_time, int width, int height)
    {
        if(inited_)
        {
            shadow_buffer_.Clear();

            auto light_pos = directional_light_.data.position;
            glm::mat4 light_space = GetLightSpaceMatrix(glm::vec3(light_pos.x, light_pos.y, light_pos.z));

            depth_shader_.Use();
            depth_shader_.SetMatrix4("lightSpace", light_space);

            shadow_buffer_.Bind();
            shadow_buffer_.Clear();

            for(auto& obj : objects_)
            {
                glm::mat4 model = GetModelMatrix(obj);
                depth_shader_.SetMatrix4("model", model);
                obj.model.Draw(depth_shader_);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            camera_->Update(delta_time);

            glm::mat4 view = camera_->GetViewMatrix();
            glm::mat4 projection = glm::mat4(1.0f);
            float aspect = static_cast<float>(width) / static_cast<float>(height);
            projection = glm::perspective(glm::radians(camera_->GetZoom()), aspect, 0.1f, 1000.0f);

            for(auto& obj : objects_)
            {
                glm::mat4 model = GetModelMatrix(obj);

                auto shader = obj.material.shader;
                shader.Use();
                
                shader.SetMatrix4("view", view);
                shader.SetMatrix4("projection", projection);
                shader.SetMatrix4("model", model);
                shader.SetVec3("viewPosition", camera_->GetPosition());
                shader.SetMatrix4("lightSpace", light_space);

                if(shader.is_lit)
                {
                    shader.SetFloat("material.shininess", 128.0f);
                    shader.SetBool("normals_enabled", obj.material.normal_maps);

                    int i = 0;
                    for(auto& light : point_lights_)
                    {
                        SetPointLight(shader, light.data, i);
                        i++;
                    }

                   SetDirectionalLight(shader);
                }
                shadow_buffer_.BindTexture(5);
                shader.SetInt("shadowMap", 5);
                obj.model.Draw(shader);
            }
        }
    }

    glm::mat4 Scene::GetModelMatrix(const Object& obj)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(obj.position.x, obj.position.y, obj.position.z));
        
        model = glm::rotate(model, static_cast<float>(glm::radians(obj.rotation.x)), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, static_cast<float>(glm::radians(obj.rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, static_cast<float>(glm::radians(obj.rotation.z)), glm::vec3(0.0f, 0.0f, 1.0f));
        
        model = glm::scale(model, glm::vec3(obj.scale));

        return model;
    }

    void Scene::SetPointLight(Shader shader, PointLightData data, int index)
    {
        std::string light_name = "pointLights[" + std::to_string(index) + "]";
        shader.SetVec3((light_name + ".color").c_str(), data.color.r, data.color.g, data.color.b);
        shader.SetVec3((light_name + ".position").c_str(), data.position.x, data.position.y, data.position.z);

        shader.SetFloat((light_name + ".linear").c_str(), data.linear);
        shader.SetFloat((light_name + ".quadratic").c_str(), data.quadratic);

        shader.SetFloat((light_name + ".ambient").c_str(), data.ambient);
        shader.SetFloat((light_name + ".diffuse").c_str(), data.diffuse);
        shader.SetFloat((light_name + ".specular").c_str(), data.specular);
    }

    void Scene::SetDirectionalLight(Shader shader)
    {
        auto data = directional_light_.data;
        auto position = data.position;
        shader.SetVec3("directionalLight.direction", position.x, position.y, position.z);
        shader.SetFloat("directionalLight.ambient", data.ambient);
        shader.SetFloat("directionalLight.diffuse", data.diffuse);
        shader.SetFloat("directionalLight.specular", data.specular);
    }

    void Scene::Shutdown()
    {
        inited_ = false;

        delete camera_;
        camera_ = nullptr;
    }

    void Scene::EnableNormalMaps(bool is_enable)
    {
        for(auto& obj : objects_)
        {
            obj.material.normal_maps = is_enable;
        }
    }

    Camera *Scene::GetMainCamera() const
    {
        return camera_;
    }

    //TODO: move to render
    glm::mat4 Scene::GetLightSpaceMatrix(glm::vec3 light_direction)
    {
        float distance = 8.0f;
        float near = 1.0f;
        float far = distance * 2;

        glm::mat4 light_projection = glm::ortho(-distance, distance, -distance, distance, near, far);
        
        glm::vec3 eye = camera_->GetPosition() - normalize(light_direction) * distance;
        glm::vec3 center = camera_->GetPosition();
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 light_view = glm::lookAt(eye, center, up);

        return light_projection * light_view;
    }
}