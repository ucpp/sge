#ifndef _SGE_MATERIAL_MANAGER_H_
#define _SGE_MATERIAL_MANAGER_H_

#include "pch.h"
#include "sge_application_settings.h"
#include "sge_material.h"

namespace SGE
{
    class MaterialManager
    {
    public:
        static Material* LoadMaterial(const MaterialAssetSettings& materialSettings, RenderContext* context);

    private:
        static bool HasMaterial(const std::string& materialName);

    private:
        static std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
    };
}

#endif // !_SGE_MATERIAL_MANAGER_H_
