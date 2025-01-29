#include "sge_data_adapters.h"

#include "sge_data_structures.h"
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

        camera->SetPosition(data->position);
        camera->SetRotation(data->rotation.x, data->rotation.y);

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

        model->SetPosition(data->position);
        model->SetRotation(data->rotation);
        model->SetScale(data->scale);
    }
}