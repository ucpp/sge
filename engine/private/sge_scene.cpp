#include "sge_scene.h"

#include "sge_render_context.h"
#include "sge_model.h"
#include "sge_model_loader.h"
#include "sge_material.h"
#include "sge_input.h"
#include "sge_data_adapters.h"
#include "sge_material_manager.h"

namespace SGE
{
    void Scene::Initialize(RenderContext* context)
    {
        m_context = context;

        InitializeCamera();
        InitializeSceneData();
        InstantiateModels();
    }
    
    void Scene::Update(double deltaTime)
    {
        UpdateCamera(deltaTime);
        UpdateSceneData(deltaTime);
        UpdateModels(deltaTime);
    }
    
    void Scene::Shutdown()
    {
        m_sceneData = {};
        m_modelInstances.clear();
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

        InitializeFog();
        
        m_sceneData.cameraPosition = m_mainCamera.GetPosition();
        m_sceneData.zNear = m_mainCamera.GetNear();
        m_sceneData.zFar = m_mainCamera.GetFar();
        m_sceneData.invViewProj = m_mainCamera.GetInvViewProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());
    }

    void Scene::InstantiateModels()
    {
        const ProjectAssets& projectAssets = m_context->GetAssetsSettings();

        for (const auto& obj : m_context->GetSceneSettings().objects)
        {
            if (obj->type != ObjectType::Model)
            {
                continue;
            }

            if (const auto* modelData = dynamic_cast<const ModelData*>(obj.get()))
            {
                const std::string& assetName = modelData->assetId; 
                const std::string& materialName = modelData->materialId;

                const ModelAssetSettings& modelAsset = projectAssets.GetModel(assetName);
                const MaterialAssetSettings& materialAsset = projectAssets.GetMaterial(materialName);

                bool isModelLoaded = ModelLoader::LoadModel(modelAsset);
                ModelInstance* instance = ModelLoader::Instantiate(modelAsset, m_context);
                Material* material = MaterialManager::LoadMaterial(materialAsset, m_context); 
                instance->SetMaterial(material);
                SyncData(modelData, instance); 

                m_modelInstances[modelData] = instance;
            }
        }
    }

    void Scene::InitializeFog()
    {
        m_sceneData.fogStart = 3.0f;
        m_sceneData.fogEnd = 30.0f;
        m_sceneData.fogColor = {0.314f, 0.314f, 0.314f};
        m_sceneData.fogStrength = m_context->GetRenderSettings().isFogEnabled ? 1.0f : 0.0f;
        m_sceneData.fogDensity = 0.1f;
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
    
    void Scene::UpdateModels(double deltaTime)
    {
        const Matrix& view = m_mainCamera.GetViewMatrix();
        const Matrix& proj = m_mainCamera.GetProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());

        for (const auto& pair : m_modelInstances)
        {
            SyncData(pair.first, pair.second);
            pair.second->Update(view, proj); 
        }
    }
}