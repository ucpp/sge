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
        m_cameraController.Initialize(cameraData);
        SyncData(cameraData, &m_mainCamera);
    }

    void Scene::InitializeSceneData()
    {
        m_sceneDataBuffer = std::make_unique<ConstantBuffer>();
        m_sceneDataBuffer->Initialize(m_context->GetD12Device().Get(), m_context->GetCbvSrvUavHeap(), sizeof(SceneData), 0);

        DirectionalLightData* directionalLightData = m_context->GetSceneSettings().GetDirectionalLight();
        SyncData(directionalLightData, &m_sceneData.directionalLight);

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
        SyncData(cameraData, &m_mainCamera);
    }
    
    void Scene::UpdateSceneData(double deltaTime)
    {
        DirectionalLightData* directionalLightData = m_context->GetSceneSettings().GetDirectionalLight();
        SyncData(directionalLightData, &m_sceneData.directionalLight);

        m_sceneData.cameraPosition = m_mainCamera.GetPosition();
        m_sceneData.fogStrength = m_context->GetRenderSettings().isFogEnabled ? 1.0f : 0.0f;
        m_sceneData.invViewProj = m_mainCamera.GetInvViewProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());
        m_sceneDataBuffer->Update(&m_sceneData, sizeof(SceneData));
    }
    
    void Scene::UpdateRenderableObjects(double deltaTime)
    {
        const Matrix& view = m_mainCamera.GetViewMatrix();
        const Matrix& proj = m_mainCamera.GetProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());
        for(auto& object : m_renderableObjects)
        {
            object->Update(view, proj);
        }
    }
}