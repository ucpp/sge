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
        InitializeFrameData();
        InstantiateModels();
    }
    
    void Scene::Update(double deltaTime)
    {
        UpdateCamera(deltaTime);
        UpdateFrameData(deltaTime);
        UpdateModels(deltaTime);
    }
    
    void Scene::Shutdown()
    {
        m_frameData = {};
        m_modelInstances.clear();
    }
    
    void Scene::InitializeCamera()
    {
        CameraData* cameraData = m_context->GetSceneData().GetCameraData();
        m_cameraController.Initialize(cameraData);
        SyncData(cameraData, &m_mainCamera);
    }

    void Scene::InitializeFrameData()
    {
        m_frameDataBuffer = std::make_unique<ConstantBuffer>();
        m_frameDataBuffer->Initialize(m_context->GetD12Device().Get(), m_context->GetCbvSrvUavHeap(), sizeof(FrameData), 0);

        DirectionalLightData* directionalLightData = m_context->GetSceneData().GetDirectionalLight();
        SyncData(directionalLightData, &m_frameData.directionalLight);

        InitializeFog();
        
        m_frameData.cameraPosition = m_mainCamera.GetPosition();
        m_frameData.zNear = m_mainCamera.GetNear();
        m_frameData.zFar = m_mainCamera.GetFar();
        m_frameData.invViewProj = m_mainCamera.GetInvViewProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());
    }

    void Scene::InstantiateModels()
    {
        const AssetsData& assetsData = m_context->GetAssetsData();

        for (const auto& obj : m_context->GetSceneData().objects)
        {
            if (obj->type != ObjectType::Model)
            {
                continue;
            }

            if (const auto* modelData = dynamic_cast<const ModelData*>(obj.get()))
            {
                const std::string& assetName = modelData->assetId; 
                const std::string& materialName = modelData->materialId;

                const ModelAssetData& modelAsset = assetsData.GetModel(assetName);
                const MaterialAssetData& materialAsset = assetsData.GetMaterial(materialName);

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
        m_frameData.fogStart = 3.0f;
        m_frameData.fogEnd = 30.0f;
        m_frameData.fogColor = {0.314f, 0.314f, 0.314f};
        m_frameData.fogStrength = m_context->GetRenderData().isFogEnabled ? 1.0f : 0.0f;
        m_frameData.fogDensity = 0.1f;
    }

    void Scene::UpdateCamera(double deltaTime)
    {
        m_cameraController.Update(deltaTime);

        CameraData* cameraData = m_context->GetSceneData().GetCameraData();
        SyncData(cameraData, &m_mainCamera);
    }
    
    void Scene::UpdateFrameData(double deltaTime)
    {
        DirectionalLightData* directionalLightData = m_context->GetSceneData().GetDirectionalLight();
        SyncData(directionalLightData, &m_frameData.directionalLight);

        m_frameData.cameraPosition = m_mainCamera.GetPosition();
        m_frameData.fogStrength = m_context->GetRenderData().isFogEnabled ? 1.0f : 0.0f;
        m_frameData.invViewProj = m_mainCamera.GetInvViewProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());
        m_frameDataBuffer->Update(&m_frameData, sizeof(FrameData));
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