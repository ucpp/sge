#include "sge_scene.h"

#include "sge_render_context.h"
#include "sge_model.h"
#include "sge_model_loader.h"
#include "sge_input.h"
#include "sge_data_adapters.h"

namespace SGE
{
    void Scene::Initialize(RenderContext* context)
    {
        m_context = context;

        InitializeCamera();
        InitializeSceneData();
        InitializeRenderableObjects();
    }
    
    void Scene::Update(double deltaTime)
    {
        UpdateCamera(deltaTime);
        UpdateSceneData(deltaTime);
        UpdateRenderableObjects(deltaTime);
    }
    
    void Scene::Shutdown()
    {
        m_sceneData = {};
        m_renderableObjects.clear();
    }
    
    void Scene::InitializeCamera()
    {
        CameraData* cameraData = m_context->GetSceneSettings().GetCameraData();
        m_cameraController.SetCamera(cameraData);
        m_cameraController.SetMoveSpeed(10.0f);
        m_cameraController.SetSensitivity(0.1f);
        DataToCamera(cameraData, &m_mainCamera);
    }

    void Scene::InitializeSceneData()
    {
        m_sceneDataBuffer = std::make_unique<ConstantBuffer>();
        m_sceneDataBuffer->Initialize(m_context->GetD12Device().Get(), m_context->GetCbvSrvUavHeap(), sizeof(SceneData), 0);

        m_sceneData.directionalLight.direction = { 0.2f, 0.2f, 1.0f };
        m_sceneData.directionalLight.color = { 1.0f, 1.0f, 1.0f };
        m_sceneData.directionalLight.intensity = 1.0f;

        for (uint32 i = 0; i < MAX_POINT_LIGHTS; ++i)
        {
            m_sceneData.pointLights[i].position = { 
                -10.0f + static_cast<float>(rand() % 200) / 10.0f, 
                0.0f + static_cast<float>(rand() % 60) / 10.0f,
                -10.0f + static_cast<float>(rand() % 200) / 10.0f 
            };
            m_sceneData.pointLights[i].color = { 
                static_cast<float>(rand() % 100) / 100.0f,
                static_cast<float>(rand() % 100) / 100.0f,
                static_cast<float>(rand() % 100) / 100.0f
            };
            m_sceneData.pointLights[i].intensity = 1.0f;
        }

        for (uint32 i = 0; i < MAX_SPOT_LIGHTS; ++i)
        {
            m_sceneData.spotLights[i].position = { 0.0f, 0.0f, -1.0f };
            m_sceneData.spotLights[i].direction = { 0.0f, 1.0f, 0.0f };

            m_sceneData.spotLights[i].innerConeCos = cos(XMConvertToRadians(15.0f));
            m_sceneData.spotLights[i].outerConeCos = cos(XMConvertToRadians(30.0f));
            m_sceneData.spotLights[i].color = { 0.0f, 1.0f, 0.0f };
            m_sceneData.spotLights[i].intensity = 2.0f;
        }

        m_sceneData.cameraPosition = m_mainCamera.GetPosition();

        m_sceneData.fogStart = 3.0f;
        m_sceneData.fogEnd = 30.0f;
        m_sceneData.fogColor = {0.314f, 0.314f, 0.314f};
        m_sceneData.fogStrength = m_context->GetRenderSettings().isFogEnabled ? 1.0f : 0.0f;
        m_sceneData.fogDensity = 0.1f;

        m_sceneData.zNear = m_mainCamera.GetNear();
        m_sceneData.zFar = m_mainCamera.GetFar();

        m_sceneData.invViewProj = m_mainCamera.GetInvViewProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());
    }

    void Scene::InitializeRenderableObjects()
    {
        auto model = std::make_unique<Model>();
        const std::string path = "resources/backpack/backpack.obj";
        *model = ModelLoader::LoadModel(path, m_context->GetDevice(), m_context->GetCbvSrvUavHeap(), 1);
        model->SetRotation({ 0.0f, 180.0f, 0.0f });
        model->SetPosition({ 0.0f, 0.0f, 10.0f });
 
        m_renderableObjects.push_back(std::move(model));
    }
    
    void Scene::UpdateCamera(double deltaTime)
    {
        m_cameraController.Update(deltaTime);

        CameraData* cameraData = m_context->GetSceneSettings().GetCameraData();
        DataToCamera(cameraData, &m_mainCamera);
    }
    
    void Scene::UpdateSceneData(double deltaTime)
    {
        m_sceneData.cameraPosition = m_mainCamera.GetPosition();
        m_sceneData.fogStrength = m_context->GetRenderSettings().isFogEnabled ? 1.0f : 0.0f;
        m_sceneData.invViewProj = m_mainCamera.GetInvViewProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());
        m_sceneDataBuffer->Update(&m_sceneData, sizeof(SceneData));

        static const float fadeSpeeds[MAX_POINT_LIGHTS] = { 0.5f, 1.0f };
        static float accumulatedTime = 0.0f;
        accumulatedTime += static_cast<float>(deltaTime);

        for (uint32 i = 0; i < MAX_POINT_LIGHTS; ++i)
        {
            float intensityMin = 0.0f;
            float intensityMax = 3.0f;
            m_sceneData.pointLights[i].intensity = intensityMin + (intensityMax - intensityMin) * (sin(accumulatedTime * fadeSpeeds[i]) * 0.5f + 0.5f);
        }
    }
    
    void Scene::UpdateRenderableObjects(double deltaTime)
    {
        static bool playAnimation = false;
        if(Input::Get().GetKeyDown(VK_SPACE))
        {
            playAnimation = !playAnimation;
        }

        float rotationSpeed = 45.0f;
        float rotationDelta = rotationSpeed * static_cast<float>(deltaTime);

        const Matrix& view = m_mainCamera.GetViewMatrix();
        const Matrix& proj = m_mainCamera.GetProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());
        for(auto& object : m_renderableObjects)
        {
            if(playAnimation)
            {
                Vector3 currentRotation = object->GetRotation();
                currentRotation += Vector3(0, rotationDelta, 0);
                object->SetRotation(currentRotation);
            }
            object->Update(view, proj);
        }
    }
}