#include "sge_data_structures.h"

#include "sge_editor_utils.h"
#include "sge_data_factory.h"

namespace SGE
{
    void ObjectDataBase::DrawEditor()
    {
        InputText("Name:", name);
    }

    void ObjectDataBase::ToJson(njson& data)
    {
        data = njson
        {
            { "name", name },
            { "type", static_cast<int32>(type) }
        };
    }

    void ObjectDataBase::FromJson(const njson& data)
    {
        data.at("name").get_to(name);
        int32 typeInt {};
        data.at("type").get_to(typeInt);
        type = static_cast<ObjectType>(typeInt);
    }

    void TransformData::DrawEditor()
    {
        ObjectDataBase::DrawEditor();
        DragFloat3("Position:", position);
        DragAngle3("Rotation:", rotation);
        DragFloat3("Scale:", scale);
    }

    void TransformData::ToJson(njson& data)
    {
        ObjectDataBase::ToJson(data);
        data["position"] = position;
        data["rotation"] = rotation;
        data["scale"] = scale;
    }

    void TransformData::FromJson(const njson &data)
    {
        ObjectDataBase::FromJson(data);
        data.at("position").get_to(position);
        data.at("rotation").get_to(rotation);
        data.at("scale").get_to(scale);
    }

    void CameraData::DrawEditor()
    {
        TransformData::DrawEditor();
        DragFloat("FOV:", fov);
        DragFloat("Near Plane:", nearPlane, 0.1f, 10.0f);
        DragFloat("Far Plane:", farPlane, 10.1f, 1000.0f);
        DragFloat("Move speed:", moveSpeed, 0.0f, 1000.0f);
        DragFloat("Sensitivity:", sensitivity, 0.0f, 100.0f);
    }

    void CameraData::ToJson(njson& data)
    {
        TransformData::ToJson(data);
        data["fov"] = float1(fov);
        data["near_plane"] = float1(nearPlane);
        data["far_plane"] = float1(farPlane);
        data["move_speed"] = float1(moveSpeed);
        data["sensitivity"] = float1(sensitivity);
    }

    void CameraData::FromJson(const njson& data)
    {
        TransformData::FromJson(data);
        fov = data.at("fov").get<float1>().value;
        nearPlane = data.at("near_plane").get<float1>().value;
        farPlane = data.at("far_plane").get<float1>().value;
        moveSpeed = data.at("move_speed").get<float1>().value;
        sensitivity = data.at("sensitivity").get<float1>().value;
    }

    void ModelData::DrawEditor()
    {
        TransformData::DrawEditor();
        InputText("Asset ID:", assetId);
        InputText("Material ID:", materialId);
    }

    void ModelData::ToJson(njson& data)
    {
        TransformData::ToJson(data);
        data["asset_id"] = assetId;
        data["material_id"] = materialId;
    }

    void ModelData::FromJson(const njson& data)
    {
        TransformData::FromJson(data);
        data.at("asset_id").get_to(assetId);
        data.at("material_id").get_to(materialId);
    }

    void PointLightData::DrawEditor()
    {
        ObjectDataBase::DrawEditor();
        DragFloat3("Position:", position);
        ColorEdit3("Color:", color);
        DragFloat("Intensity:", intensity);
    }

    void PointLightData::ToJson(njson& data)
    {
        ObjectDataBase::ToJson(data);
        data["position"] = position;
        data["color"] = color;
        data["intensity"] = float1(intensity);
    }

    void PointLightData::FromJson(const njson& data)
    {
        ObjectDataBase::FromJson(data);
        data.at("position").get_to(position);
        data.at("color").get_to(color);
        intensity = data.at("intensity").get<float1>().value;
    }

    void DirectionalLightData::DrawEditor()
    {
        ObjectDataBase::DrawEditor();
        DragFloat3("Direction:", direction);
        ColorEdit3("Color:", color);
        DragFloat("Intensity:", intensity);
    }

    void DirectionalLightData::ToJson(njson& data)
    {
        ObjectDataBase::ToJson(data);
        data["direction"] = direction;
        data["color"] = color;
        data["intensity"] = float1(intensity);
    }

    void DirectionalLightData::FromJson(const njson& data)
    {
        ObjectDataBase::FromJson(data);
        data.at("direction").get_to(direction);
        data.at("color").get_to(color);
        intensity = data.at("intensity").get<float1>().value;
    }

    void AssetDataBase::ToJson(njson& data)
    {
        data = njson
        {
            { "name", name },
            { "type", static_cast<int32>(type) }
        };
    }
    
    void AssetDataBase::FromJson(const njson& data)
    {
        data.at("name").get_to(name);
        int32 typeInt {};
        data.at("type").get_to(typeInt);
        type = static_cast<AssetType>(typeInt);
    }

    void ModelAssetData::ToJson(njson& data)
    {
        AssetDataBase::ToJson(data);
        data["path"] = path;
    }
    
    void ModelAssetData::FromJson(const njson& data)
    {
        AssetDataBase::FromJson(data);
        data.at("path").get_to(path);
    }
    
    void MaterialAssetData::ToJson(njson& data)
    {
        AssetDataBase::ToJson(data);
        data["albedo_texture_path"] = albedoTexturePath;
        data["metallic_texture_path"] = metallicTexturePath;
        data["normal_texture_path"] = normalTexturePath;
        data["roughness_texture_path"] = roughnessTexturePath;
    }

    void MaterialAssetData::FromJson(const njson& data)
    {
        AssetDataBase::FromJson(data);
        data.at("albedo_texture_path").get_to(albedoTexturePath);
        data.at("metallic_texture_path").get_to(metallicTexturePath);
        data.at("normal_texture_path").get_to(normalTexturePath);
        data.at("roughness_texture_path").get_to(roughnessTexturePath);
    }

    const MaterialAssetData& AssetsData::GetMaterial(const std::string& id) const
    {
        auto typeIt = assets.find(AssetType::Material);
        if (typeIt != assets.end())
        {
            const auto& materialMap = typeIt->second;
            auto it = materialMap.find(id);
            if (it != materialMap.end())
            {
                return static_cast<const MaterialAssetData&>(*it->second);
            }
        }

        throw std::runtime_error("Material with ID " + id + " not found.");
    }
    
    const ModelAssetData& AssetsData::GetModel(const std::string& id) const
    {
        auto typeIt = assets.find(AssetType::Model);
        if (typeIt != assets.end())
        {
            const auto& modelMap = typeIt->second;
            auto it = modelMap.find(id);
            if (it != modelMap.end())
            {
                return static_cast<const ModelAssetData&>(*it->second);
            }
        }

        throw std::runtime_error("Model with ID " + id + " not found.");
    }

    CameraData* SceneData::GetCameraData()
    {
        if (!m_mainCamera)
        {
            auto it = objects.find(ObjectType::Camera);
            if (it != objects.end() && !it->second.empty())
            {
                m_mainCamera = dynamic_cast<CameraData*>(it->second.front().get());
            }
        }

        return m_mainCamera;
    }

    DirectionalLightData* SceneData::GetDirectionalLight()
    {
        if (!m_directionalLight)
        {
            auto it = objects.find(ObjectType::DirectionalLight);
            if (it != objects.end() && !it->second.empty())
            {
                m_directionalLight = dynamic_cast<DirectionalLightData*>(it->second.front().get());
            }
        }

        return m_directionalLight;
    }

    const char** WindowData::GetResolutions()
    {
        return resolutionCStrings.data();
    }

    void WindowData::UpdateResolutionCStrings()
    {
        resolutionCStrings.clear();
        for (const auto& res : resolutions)
        {
            resolutionCStrings.push_back(res.c_str());
        }
    }
    
    uint32_t WindowData::GetWidth() const
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
    
    uint32_t WindowData::GetHeight() const
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

    std::string roundToString(float1 value, int32 precision) noexcept
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(precision) << value.value;
        std::string result = stream.str();

        size_t dotPos = result.find('.');
        if (dotPos != std::string::npos)
        {
            size_t lastNonZero = result.find_last_not_of('0');
            if (lastNonZero != std::string::npos && lastNonZero > dotPos)
            {
                result.erase(lastNonZero + 1);
            }
            else
            {
                result.erase(dotPos + 2);
            }
        }

        return result;
    }

    float1 roundFromString(const std::string& str, int32 precision) noexcept
    {
        float value = std::stof(str);
        float factor = static_cast<float>(std::pow(10.0f, precision));
        return {std::round(value * factor) / factor};
    }

    void to_json(njson& data, float1 value)
    {
        data = roundToString(value, 3);
    }

    void from_json(const njson& data, float1& value)
    {
        value = roundFromString(data.get<std::string>(), 3);
    }

    void to_json(njson& data, const float2& vec2)
    {
        data = njson::array({float1(vec2.x), float1(vec2.y)});
    }

    void from_json(const njson& data, float2& vec2)
    {
        vec2.x = data[0].get<float1>().value;
        vec2.y = data[1].get<float1>().value;
    }

    void to_json(njson& data, const float3& vec3)
    {
        data = njson::array({float1(vec3.x), float1(vec3.y), float1(vec3.z)});
    }

    void from_json(const njson& data, float3& vec3)
    {
        vec3.x = data[0].get<float1>().value;
        vec3.y = data[1].get<float1>().value;
        vec3.z = data[2].get<float1>().value;
    }
    
    void to_json(njson& data, const SceneData& scene)
    {
        njson objectsJson = njson::array();

        for (const auto& [type, objectList] : scene.objects)
        {
            for (const auto& obj : objectList)
            {
                njson objJson;
                obj->ToJson(objJson);
                objectsJson.push_back(objJson);
            }
        }

        data["objects"] = objectsJson;
    }

    void from_json(const njson& data, SceneData& scene)
    {
        RegisterObjectDataTypes();
        scene.objects.clear();

        if (data.contains("objects"))
        {
            const auto& objectsJson = data.at("objects");
            for (const auto& objJson : objectsJson)
            {
                int32_t typeInt;
                objJson.at("type").get_to(typeInt);
                auto type = static_cast<ObjectType>(typeInt);
                auto object = ObjectDataFactory::Get().Create(type);
                object->FromJson(objJson);
                scene.objects[type].push_back(std::move(object));
            }
        }
    }
    
    void to_json(njson& data, const WindowData& window)
    {
        data = njson
        {
            { "title", window.title },
            { "selected_resolution", window.selectedResolution },
            { "fullscreen", window.fullscreen },
            { "resolutions", window.resolutions },
            { "target_fps", window.targetFPS },
            { "is_editor_enable", window.isEditorEnable }
        };
    }
    
    void from_json(const njson& data, WindowData& window)
    {
        data.at("title").get_to(window.title);
        data.at("selected_resolution").get_to(window.selectedResolution);
        data.at("fullscreen").get_to(window.fullscreen);
        data.at("resolutions").get_to(window.resolutions);
        data.at("target_fps").get_to(window.targetFPS);
        data.at("is_editor_enable").get_to(window.isEditorEnable);

        window.UpdateResolutionCStrings();
    }
    
    void to_json(njson& data, const RenderData& render)
    {
        data = njson
        {
            { "v_sync", render.vSync },
            { "is_msaa_enabled", render.isMSAAEnabled },
            { "is_fog_enabled", render.isFogEnabled },
            { "is_deferred_rendering", render.isDeferredRendering }
        };
    }
    
    void from_json(const njson& data, RenderData& render)
    {
        data.at("v_sync").get_to(render.vSync);
        data.at("is_msaa_enabled").get_to(render.isMSAAEnabled);
        data.at("is_fog_enabled").get_to(render.isFogEnabled);
        data.at("is_deferred_rendering").get_to(render.isDeferredRendering);
    }
    
    void to_json(njson& data, const AssetsData& assets)
    {
        njson assetsJson = njson::array();

        for (const auto& [type, assetMap] : assets.assets)
        {
            for (const auto& [name, asset] : assetMap)
            {
                njson assetJson;
                asset->ToJson(assetJson);
                assetsJson.push_back(assetJson);
            }
        }

        data["assets"] = assetsJson;
    }
    
    void from_json(const njson& data, AssetsData& assets)
    {
        RegisterAssetDataTypes();
        assets.assets.clear();
        if (data.contains("assets"))
        {
            const auto& assetsJson = data.at("assets");
            for (const auto& assetJson : assetsJson)
            {
                int32_t typeInt;
                assetJson.at("type").get_to(typeInt);
                auto type = static_cast<AssetType>(typeInt);
                auto asset = AssetDataFactory::Get().Create(type);
                asset->FromJson(assetJson);
                assets.assets[type][asset->name] = std::move(asset);
            }
        }
    }
    
    void to_json(njson& data, const ApplicationData& application)
    {
        data = njson
        {
            { "window_data", application.windowData },
            { "render_data", application.renderData },
            { "assets_data", application.assetsData },
            { "scene_data", application.sceneData }
        };
    }

    void from_json(const njson& data, ApplicationData& application)
    {
        data.at("window_data").get_to(application.windowData);
        data.at("render_data").get_to(application.renderData);
        data.at("assets_data").get_to(application.assetsData);
        data.at("scene_data").get_to(application.sceneData);
    }
}