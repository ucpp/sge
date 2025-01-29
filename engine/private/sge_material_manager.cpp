#include "sge_material_manager.h"

namespace SGE
{
    std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::m_materials;

    Material* MaterialManager::LoadMaterial(const MaterialAssetData& materialAssetData, RenderContext* context)
    {
        if(!HasMaterial(materialAssetData.name))
        {
            std::unique_ptr<Material> material = std::make_unique<Material>();
            material->Initialize(materialAssetData, context);

            m_materials[materialAssetData.name] = std::move(material);
        }

        return m_materials[materialAssetData.name].get();
    }
    
    bool MaterialManager::HasMaterial(const std::string& materialName)
    {
        auto it = m_materials.find(materialName);
        return it != m_materials.end() && it->second;
    }
}