#include "sge_data_adapters.h"

#include "sge_scene_settings.h"
#include "sge_camera.h"
#include "sge_logger.h"

namespace SGE
{
    void DataToCamera(CameraData* data, Camera* camera)
    {
        if (camera == nullptr)
        {
            LOG_ERROR("DataToCamera: Camera pointer is null.");
            return;
        }

        if (data == nullptr)
        {
            LOG_ERROR("DataToCamera: CameraData pointer is null.");
            return;
        }

        camera->SetPosition(Vector3(data->position[0], data->position[1], data->position[2]));
        camera->SetRotation(data->rotation[0], data->rotation[1]);

        camera->SetFov(data->fov);
        camera->SetNearPlane(data->nearPlane);
        camera->SetFarPlane(data->farPlane);
    }
}