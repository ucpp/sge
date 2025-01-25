#include "sge_material_manager.h"

namespace SGE
{
    std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::m_materials;

    Material* MaterialManager::LoadMaterial(const MaterialAssetSettings& materialSettings, RenderContext* context)
    {
        if(!HasMaterial(materialSettings.name))
        {
            std::unique_ptr<Material> material = std::make_unique<Material>();
            material->Initialize(materialSettings, context);

            m_materials[materialSettings.name] = std::move(material);
        }

        return m_materials[materialSettings.name].get();
    }
    
    bool MaterialManager::HasMaterial(const std::string& materialName)
    {
        auto it = m_materials.find(materialName);
        return it != m_materials.end() && it->second;
    }
}