#include "scene.h"
#include "resource_manager.h"

namespace Engine
{
    Scene::Scene(SceneData data) : data_(data), inited_(false) {}

    void Scene::Init(InputSystem &input)
    {
        CameraData cam = data_.camera;
        camera_ = new Camera(input, glm::vec3(cam.x, cam.y, cam.z), cam.speed);

        for(const ObjectData& object_data : data_.objects)
        {
            Object* object = new Object();
            object->model = ResourceManager::GetModel(object_data.model);
            object->material.shader = ResourceManager::GetShader(object_data.material.shader);
            object->position = glm::vec3(object_data.x, object_data.y, object_data.z);
            object->scale = static_cast<float>(object_data.scale);
            objects_.push_back(object);
        }

        for(const auto& light : data_.point_lights)
        {
            PointLight point_light;
            point_light.data = light;
            point_lights_.push_back(point_light);
        }

        // add directional light

        inited_ = true;
    }

    void Scene::Update(float delta_time, float aspect_ratio)
    {
        if(inited_)
        {
            camera_->Update(delta_time);

            glm::mat4 view = camera_->GetViewMatrix();
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(camera_->GetZoom()), aspect_ratio, 0.1f, 1000.0f);

            for(auto obj : objects_)
            {
                //TODO: fix it (change to real data)
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(obj->scale));

                auto shader = obj->material.shader;
                shader.Use();
                shader.SetMatrix4("view", view);
                shader.SetMatrix4("projection", projection);
                shader.SetMatrix4("model", model);

                shader.SetVec3("viewPosition", camera_->GetPosition());

                if(shader.is_lit)
                {
                    shader.SetFloat("material.shininess", 128.0f);
                    shader.SetInt("normals_enabled", obj->material.normal_maps);

                    int i = 0;
                    for(auto light : point_lights_)
                    {
                        std::string light_name = "pointLights[" + std::to_string(i) + "]";
                        auto lit = light.data;
                        shader.SetVec3((light_name + ".color").c_str(), lit.color.r, lit.color.g, lit.color.b);
                        shader.SetVec3((light_name + ".position").c_str(), lit.x, lit.y, lit.z);

                        shader.SetFloat((light_name + ".linear").c_str(), lit.linear);
                        shader.SetFloat((light_name + ".quadratic").c_str(), lit.quadratic);

                        shader.SetFloat((light_name + ".ambient").c_str(), lit.ambient);
                        shader.SetFloat((light_name + ".diffuse").c_str(), lit.diffuse);
                        shader.SetFloat((light_name + ".specular").c_str(), lit.specular);
                        i++;
                    }

                    //TODO:
                    shader.SetVec3("directionalLight.direction", -20.0f, -20.0f, 0.0f);
                    shader.SetFloat("directionalLight.ambient", 0.05f);
                    shader.SetFloat("directionalLight.diffuse", 0.4f);
                    shader.SetFloat("directionalLight.specular", 0.6f);
                }
                obj->model.Draw(shader);
            }
        }
    }

    void Scene::Shutdown()
    {
        inited_ = false;

        delete camera_;
        camera_ = nullptr;

        //TODO: clear memory
    }

    void Scene::EnableNormalMaps(bool is_enable)
    {
        for(auto obj : objects_)
        {
            obj->material.normal_maps = is_enable;
        }
    }

    Camera *Scene::GetMainCamera() const
    {
        return camera_;
    }
}