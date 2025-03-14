#include "data/sge_scene.h"

#include "rendering/sge_render_context.h"
#include "data/sge_model_instance.h"
#include "data/sge_model_loader.h"
#include "data/sge_material.h"
#include "core/sge_input.h"
#include "data/sge_data_adapters.h"
#include "data/sge_material_manager.h"

namespace SGE
{
    void Scene::Initialize(RenderContext* context)
    {
        m_context = context;

        InitializeCamera();
        InitializeFrameData();
        InstantiateModels();
        InstantiateAnimatedModels();

        SceneData& sceneData = m_context->GetSceneData();
        SkyboxData* data = sceneData.GetSkyboxData();
        if(data)
        {
            const AssetsData& assetsData = m_context->GetAssetsData();
            m_skyboxCubemap = assetsData.GetCubemap(data->cubemapId);
        }
    }
    
    void Scene::Update(double deltaTime)
    {
        UpdateCamera(deltaTime);
        UpdateModels(deltaTime);
        SyncFrameData();
    }
    
    void Scene::Shutdown()
    {
        m_frameData = {};
        m_modelInstances.clear();
        m_animatedModelInstances.clear();
    }

    void Scene::SyncFrameData()
    {
        SceneData& sceneData = m_context->GetSceneData();

        m_frameData.cameraPosition = m_mainCamera.GetPosition();
        m_frameData.zNear = m_mainCamera.GetNear();
        m_frameData.zFar = m_mainCamera.GetFar();
        m_frameData.invProj = m_mainCamera.GetProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight()).inverse();
        m_frameData.invView = m_mainCamera.GetViewMatrix().inverse();
        m_frameData.viewProj = m_mainCamera.GetProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight()) * m_mainCamera.GetViewMatrix();
        m_frameData.viewProjSky =  m_mainCamera.GetProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight()) * m_mainCamera.GetViewSky();
        DirectionalLightData* directionalLightData = sceneData.GetDirectionalLight();
        SyncData(directionalLightData, &m_frameData.directionalLight);
        
        float3 lightDirection = directionalLightData->direction.normalized();
        float3 lightPosition = -lightDirection * 7.0f;
        float3 target = lightPosition + lightDirection;
        float3 up = float3(0.0f, 1.0f, 0.0f);
        if (abs(dot(lightDirection, up)) > 0.99f)
        {
            up = float3(0.0f, 0.0f, 1.0f);
        }

        float aspectRatio = static_cast<float>(m_context->GetScreenWidth() / m_context->GetScreenHeight());
        float4x4 lightView = CreateViewMatrix(lightPosition, target, up);
        float4x4 lightProj = CreateOrthographicProjectionMatrix(20.0f * aspectRatio, 20.0f, 0.01f, 20.0f);
        m_frameData.lightView = lightView;
        m_frameData.lightProj = lightProj;

        auto it = sceneData.objects.find(ObjectType::PointLight);
        if (it != sceneData.objects.end())
        {
            const uint32 count = static_cast<uint32>(it->second.size());
            m_frameData.activePointLightsCount = count;

            for (uint32 i = 0; i < count; ++i)
            {
                if (const auto* pointLightData = dynamic_cast<const PointLightData*>(it->second[i].get()))
                {
                    SyncData(pointLightData, &m_frameData.pointLights[i]);
                }
            }
        }
        else
        {
            m_frameData.activePointLightsCount = 0;
        }

        m_frameData.fogStart = 3.0f;
        m_frameData.fogEnd = 30.0f;
        m_frameData.fogColor = {0.314f, 0.314f, 0.314f};
        m_frameData.fogStrength = 0.0f;
        m_frameData.fogDensity = 0.1f;

        m_frameDataBuffer->Update(&m_frameData, sizeof(FrameData));
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

        SyncFrameData();
    }

    void Scene::InstantiateModels()
    {
        m_modelInstances.clear();
        const AssetsData& assetsData = m_context->GetAssetsData();
        const auto& sceneObjects = m_context->GetSceneData().objects;

        auto it = sceneObjects.find(ObjectType::Model);
        if (it == sceneObjects.end())
        {
            return;
        }

        for (const auto& obj : it->second)
        {
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

    void Scene::InstantiateAnimatedModels()
    {
        m_animatedModelInstances.clear();
        const AssetsData& assetsData = m_context->GetAssetsData();
        const auto& sceneObjects = m_context->GetSceneData().objects;

        auto it = sceneObjects.find(ObjectType::AnimatedModel);
        if (it == sceneObjects.end())
        {
            return;
        }

        for (const auto& obj : it->second)
        {
            if (const auto* animData = dynamic_cast<const AnimatedModelData*>(obj.get()))
            {
                const std::string& assetName = animData->assetId;
                const std::string& materialName = animData->materialId;

                const AnimatedModelAssetData& modelAsset = assetsData.GetAnimModel(assetName);
                const MaterialAssetData& materialAsset = assetsData.GetMaterial(materialName);

                bool isModelLoaded = ModelLoader::LoadAnimatedModel(modelAsset);
                AnimatedModelInstance* instance = ModelLoader::InstantiateAnimated(modelAsset, m_context);
                Material* material = MaterialManager::LoadMaterial(materialAsset, m_context);
                instance->SetMaterial(material);
                SyncData(animData, instance); // todo: change to sync anim

                m_animatedModelInstances[animData] = instance;
            }
        }
    }

    void Scene::UpdateCamera(double deltaTime)
    {
        m_cameraController.Update(deltaTime);

        CameraData* cameraData = m_context->GetSceneData().GetCameraData();
        SyncData(cameraData, &m_mainCamera);
    }
    
    void Scene::UpdateModels(double deltaTime)
    {
        const float4x4& view = m_mainCamera.GetViewMatrix();
        const float4x4& proj = m_mainCamera.GetProjMatrix(m_context->GetScreenWidth(), m_context->GetScreenHeight());

        for (const auto& pair : m_modelInstances)
        {
            SyncData(pair.first, pair.second);
            pair.second->UpdateTransform(view, proj); 
        }

        for (const auto& pair : m_animatedModelInstances)
        {
            SyncData(pair.first, pair.second);
            pair.second->FixedUpdate(static_cast<float>(deltaTime));
            pair.second->UpdateTransform(view, proj); 
        }
    }

    AnimatedModelInstance* Scene::GetAnimModel(const AnimatedModelData* data) const
    {
        if(data)
        {
            auto it = m_animatedModelInstances.find(data);
            if(it != m_animatedModelInstances.end())
            {
                return it->second;
            }
        }
        return nullptr;
    }
}
