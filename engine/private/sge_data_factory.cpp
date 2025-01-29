#include "sge_data_factory.h"

namespace SGE
{
    void ObjectDataFactory::Register(ObjectType type, Creator creator)
    {
        creators[type] = std::move(creator);
    }
    
    std::unique_ptr<ObjectDataBase> ObjectDataFactory::Create(ObjectType type)
    {
        auto it = creators.find(type);
        if (it != creators.end())
        {
            return it->second();
        }

        throw std::runtime_error("Unknown object type");
    }
    
    void AssetDataFactory::Register(AssetType type, Creator creator)
    {
        creators[type] = std::move(creator);
    }
    
    std::unique_ptr<AssetDataBase> AssetDataFactory::Create(AssetType type)
    {
        auto it = creators.find(type);
        if (it != creators.end())
        {
            return it->second();
        }
    
        throw std::runtime_error("Unknown asset type");
    }
    
    void RegisterObjectDataTypes()
    {
        auto& factory = ObjectDataFactory::Get();
        factory.Register(ObjectType::Camera, []() { return std::make_unique<CameraData>(); });
        factory.Register(ObjectType::Model, []() { return std::make_unique<ModelData>(); });
        factory.Register(ObjectType::PointLight, []() { return std::make_unique<PointLightData>(); });
        factory.Register(ObjectType::DirectionalLight, []() { return std::make_unique<DirectionalLightData>(); });
    }
    
    void RegisterAssetDataTypes()
    {
        auto& factory = AssetDataFactory::Get();
        factory.Register(AssetType::Model, []() { return std::make_unique<ModelAssetData>(); });
        factory.Register(AssetType::Material, []() { return std::make_unique<MaterialAssetData>(); });
        factory.Register(AssetType::Light, []() { return std::make_unique<LightAssetData>(); });
    }
}