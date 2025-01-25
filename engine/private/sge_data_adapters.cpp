#include "sge_data_adapters.h"

#include "sge_scene_settings.h"
#include "sge_scene_data.h"
#include "sge_camera.h"
#include "sge_logger.h"
#include "sge_model.h"

namespace SGE
{
    void SyncData(const CameraData* data, Camera* camera)
    {
        if (camera == nullptr)
        {
            LOG_ERROR("SyncData: Camera pointer is null.");
            return;
        }

        if (data == nullptr)
        {
            LOG_ERROR("SyncData: CameraData pointer is null.");
            return;
        }

        camera->SetPosition(Vector3(data->position[0], data->position[1], data->position[2]));
        camera->SetRotation(data->rotation[0], data->rotation[1]);

        camera->SetFov(data->fov);
        camera->SetNearPlane(data->nearPlane);
        camera->SetFarPlane(data->farPlane);
    }
    
    void SyncData(const DirectionalLightData* data, DirectionalLight* directionalLight)
    {
        if (directionalLight == nullptr)
        {
            LOG_ERROR("SyncData: DirectionalLight pointer is null.");
            return;
        }

        if (data == nullptr)
        {
            LOG_ERROR("SyncData: DirectionalLightData pointer is null.");
            return;
        }

        directionalLight->intensity = data->intensity;
        directionalLight->direction = { data->direction[0], data->direction[1], data->direction[2] };
        directionalLight->color = { data->color[0], data->color[1], data->color[2] };
    }
    
    void SyncData(const ModelData* data, ModelInstance* model)
    {
        if (model == nullptr)
        {
            LOG_ERROR("SyncData: ModelInstance pointer is null.");
            return;
        }

        if (data == nullptr)
        {
            LOG_ERROR("SyncData: ModelData pointer is null.");
            return;
        }

        model->SetPosition(Vector3(data->position[0], data->position[1], data->position[2]));
        model->SetRotation(Vector3(data->rotation[0], data->rotation[1], data->rotation[2]));
        model->SetScale(Vector3(data->scale[0], data->scale[1], data->scale[2]));
    }
}