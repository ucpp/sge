#include "data/sge_data_adapters.h"

#include "data/sge_data_structures.h"
#include "data/sge_camera.h"
#include "core/sge_logger.h"
#include "data/sge_model_instance.h"

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

        camera->SetPosition(data->transform.position);
        camera->SetRotation(data->transform.rotation.x, data->transform.rotation.y);

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
        directionalLight->direction = data->direction;
        directionalLight->color = data->color;
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

        model->SetName(data->name);
        model->SetPosition(data->transform.position);
        model->SetRotation(data->transform.rotation);
        model->SetScale(data->transform.scale);
        model->SetActive(data->enabled);
    }

    void SyncData(const PointLightData* data, PointLight* pointLight)
    {
        if (pointLight == nullptr)
        {
            LOG_ERROR("SyncData: PointLight pointer is null.");
            return;
        }

        if (data == nullptr)
        {
            LOG_ERROR("SyncData: PointLightData pointer is null.");
            return;
        }

        pointLight->position = data->position;
        pointLight->color = data->color;
        pointLight->intensity = data->intensity;
        pointLight->radius = data->radius;
    }
}