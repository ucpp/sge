#include "sge_application_settings.h"

#include "sge_helpers.h"

namespace SGE
{
    const char **WindowSettings::GetResolutions()
    {
        return resolutionCStrings.data();
    }

    void WindowSettings::UpdateResolutionCStrings()
    {
        resolutionCStrings.clear();
        for (const auto& res : resolutions)
        {
            resolutionCStrings.push_back(res.c_str());
        }
    }
    
    uint32_t WindowSettings::GetWidth() const
    {
        if (selectedResolution >= resolutions.size())
        {
            throw std::out_of_range("Selected resolution index is out of range.");
        }

        const std::string& resolution = resolutions[selectedResolution];
        size_t xPos = resolution.find('x');
        if (xPos == std::string::npos)
        {
            throw std::invalid_argument("Invalid resolution format. Expected 'WidthxHeight'.");
        }

        return std::stoul(resolution.substr(0, xPos));
    }
    
    uint32_t WindowSettings::GetHeight() const
    {
        if (selectedResolution >= resolutions.size())
        {
            throw std::out_of_range("Selected resolution index is out of range.");
        }

        const std::string& resolution = resolutions[selectedResolution];
        size_t xPos = resolution.find('x');
        if (xPos == std::string::npos)
        {
            throw std::invalid_argument("Invalid resolution format. Expected 'WidthxHeight'.");
        }

        return std::stoul(resolution.substr(xPos + 1));
    }
    
    void AssetBase::ToJson(nlohmann::json& j) const
    {
        j = nlohmann::json
        {
            {"name", name},
            {"type", static_cast<int>(type)}
        };
    }
    
    void AssetBase::FromJson(const nlohmann::json& j)
    {
        j.at("name").get_to(name);
        int32 typeInt {};
        j.at("type").get_to(typeInt);
        type = static_cast<AssetType>(typeInt);  
    }

    ModelAssetSettings::ModelAssetSettings()
    {
        type = AssetType::Model;
    }

    void ModelAssetSettings::ToJson(nlohmann::json& j) const
    {
        AssetBase::ToJson(j);
        j["path"] = path;
    }

    void ModelAssetSettings::FromJson(const nlohmann::json& j)
    {
        AssetBase::FromJson(j);
        j.at("path").get_to(path);
    }

    MaterialAssetSettings::MaterialAssetSettings()
    {
        type = AssetType::Material;
    }

    void MaterialAssetSettings::ToJson(nlohmann::json &j) const
    {
        AssetBase::ToJson(j);
        j["albedo_texture_path"] = albedoTexturePath;
        j["metallic_texture_path"] = metallicTexturePath;
        j["normal_texture_path"] = normalTexturePath;
        j["roughness_texture_path"] = roughnessTexturePath;
    }

    void MaterialAssetSettings::FromJson(const nlohmann::json &j)
    {
        AssetBase::FromJson(j);
        j.at("albedo_texture_path").get_to(albedoTexturePath);
        j.at("metallic_texture_path").get_to(metallicTexturePath);
        j.at("normal_texture_path").get_to(normalTexturePath);
        j.at("roughness_texture_path").get_to(roughnessTexturePath);
    }

    LightAssetSettings::LightAssetSettings()
    {
        type = AssetType::Light;
    }

    std::unique_ptr<AssetBase> CreateObject(AssetType type)
    {
        switch (type)
        {
            case AssetType::Model:
                return std::make_unique<ModelAssetSettings>();
            case AssetType::Material:
                return std::make_unique<MaterialAssetSettings>();
            case AssetType::Light:
                return std::make_unique<LightAssetSettings>();
            default:
                throw std::runtime_error("Unknown object type");
        }
    }
    
    const MaterialAssetSettings& ProjectAssets::GetMaterial(const std::string& id) const
    {
        auto it = assets.find(id);
        if (it != assets.end())
        {
            return static_cast<const MaterialAssetSettings&>(*it->second); 
        }
    
        throw std::runtime_error("Material with ID " + id + " not found.");
    }
    
    const ModelAssetSettings& ProjectAssets::GetModel(const std::string& id) const
    {
        auto it = assets.find(id);
        if (it != assets.end())
        {
            return static_cast<const ModelAssetSettings&>(*it->second); 
        }
        throw std::runtime_error("Model with ID " + id + " not found.");
    }
}