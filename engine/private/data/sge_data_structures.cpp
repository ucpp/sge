#include "data/sge_data_structures.h"

#include "core/sge_editor_utils.h"
#include "data/sge_data_factory.h"

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
        DragFloat3("Position:", transform.position);
        DragAngle3("Rotation:", transform.rotation);
        DragFloat3("Scale:", transform.scale);
    }

    void TransformData::ToJson(njson& data)
    {
        ObjectDataBase::ToJson(data);
        data["position"] = transform.position;
        data["rotation"] = transform.rotation;
        data["scale"] = transform.scale;
    }

    void TransformData::FromJson(const njson &data)
    {
        ObjectDataBase::FromJson(data);
        data.at("position").get_to(transform.position);
        data.at("rotation").get_to(transform.rotation);
        data.at("scale").get_to(transform.scale);
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
        Checkbox("Enabled:", enabled);
        TransformData::DrawEditor();
        InputText("Asset ID:", assetId);
        InputText("Material ID:", materialId);

        if (ImGui::CollapsingHeader("Tiling UV"))
        {
            ImGui::DragFloat2("Tiling UV", tilingUV.data(), 0.1f, 0.0f, 10.0f);
        }
    }

    void ModelData::ToJson(njson& data)
    {
        TransformData::ToJson(data);
        data["asset_id"] = assetId;
        data["material_id"] = materialId;
        data["enabled"] = enabled;

        njson tilingUVJson;
        tilingUVJson["x"] = float1(tilingUV.x);
        tilingUVJson["y"] = float1(tilingUV.y);
        data["tiling_uv"] = tilingUVJson;
    }

    void ModelData::FromJson(const njson& data)
    {
        TransformData::FromJson(data);
        data.at("asset_id").get_to(assetId);
        data.at("material_id").get_to(materialId);
        data.at("enabled").get_to(enabled);

        if (data.contains("tiling_uv"))
        {
            tilingUV = { data["tiling_uv"]["x"].get<float1>().value,  data["tiling_uv"]["y"].get<float1>().value };
        }
        else
        {
            tilingUV = float2(float1(1.0f), float1(1.0f));
        }
    }

    void AnimatedModelData::ToJson(njson& data)
    {
        ModelData::ToJson(data);
        
        njson boneLayersJson;
        for (const auto& [boneName, weights] : boneLayers)
        {
            njson weightsJson;
            for (size_t i = 0; i < 3; ++i)
            {
                weightsJson.push_back(float1(weights[i]));
            }
            boneLayersJson[boneName] = weightsJson;
        }
        data["bone_layers"] = boneLayersJson;
    }

    void AnimatedModelData::FromJson(const njson& data)
    {
        ModelData::FromJson(data);

        if (data.contains("bone_layers"))
        {
            boneLayers.clear();
            for (const auto& [boneName, weightsJson] : data["bone_layers"].items())
            {
                std::array<float, 3> weights;
                for (size_t i = 0; i < 3; ++i)
                {
                    weights[i] = weightsJson[i].get<float1>().value;
                }
                boneLayers[boneName] = weights;
            }
        }
    }

    void AnimatedModelData::DrawEditor()
    {
        Checkbox("Enabled:", enabled);
        TransformData::DrawEditor();
        InputText("Asset ID:", assetId);
        InputText("Material ID:", materialId);
    }

    void PointLightData::DrawEditor()
    {
        ObjectDataBase::DrawEditor();
        DragFloat3("Position:", position);
        ColorEdit3("Color:", color);
        DragFloat("Intensity:", intensity, 0.0f);
        DragFloat("Radius:", radius, 0.1f);
    }

    void PointLightData::ToJson(njson& data)
    {
        ObjectDataBase::ToJson(data);
        data["position"] = position;
        data["color"] = color;
        data["intensity"] = float1(intensity);
        data["radius"] = float1(radius);
    }

    void PointLightData::FromJson(const njson& data)
    {
        ObjectDataBase::FromJson(data);
        data.at("position").get_to(position);
        data.at("color").get_to(color);
        intensity = data.at("intensity").get<float1>().value;
        radius = data.at("radius").get<float1>().value;
    }

    void DirectionalLightData::DrawEditor()
    {
        ObjectDataBase::DrawEditor();
        DragFloat3("Direction:", direction);
        ColorEdit3("Color:", color);
        DragFloat("Intensity:", intensity, 0.0f);
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

    void SkyboxData::DrawEditor()
    {
        ObjectDataBase::DrawEditor();
        InputText("Cubemap ID:", cubemapId);
    }

    void SkyboxData::ToJson(njson& data)
    {
        ObjectDataBase::ToJson(data);
        data["cubemap_id"] = cubemapId;
    }

    void SkyboxData::FromJson(const njson& data)
    {
        ObjectDataBase::FromJson(data);
        data.at("cubemap_id").get_to(cubemapId);
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

    void CubemapAssetData::ToJson(njson& data)
    {
        AssetDataBase::ToJson(data);
        data["back"] = back;
        data["bottom"] = bottom;
        data["front"] = front;
        data["left"] = left;
        data["right"] = right;
        data["top"] = top;
    }

    void CubemapAssetData::FromJson(const njson& data)
    {
        AssetDataBase::FromJson(data);
        data.at("back").get_to(back);
        data.at("bottom").get_to(bottom);
        data.at("front").get_to(front);
        data.at("left").get_to(left);
        data.at("right").get_to(right);
        data.at("top").get_to(top);
    }

    std::array<std::string, 6> CubemapAssetData::GetPaths() const
    {
        return { right, left, top, bottom, front, back };
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

    const AnimatedModelAssetData& AssetsData::GetAnimModel(const std::string& id) const
    {
        auto typeIt = assets.find(AssetType::AnimatedModel);
        if (typeIt != assets.end())
        {
            const auto& modelMap = typeIt->second;
            auto it = modelMap.find(id);
            if (it != modelMap.end())
            {
                return static_cast<const AnimatedModelAssetData&>(*it->second);
            }
        }

        throw std::runtime_error("Animated Model with ID " + id + " not found.");
    }

    const CubemapAssetData& AssetsData::GetCubemap(const std::string& id) const
    {
        auto typeIt = assets.find(AssetType::Cubemap);
        if (typeIt != assets.end())
        {
            const auto& modelMap = typeIt->second;
            auto it = modelMap.find(id);
            if (it != modelMap.end())
            {
                return static_cast<const CubemapAssetData&>(*it->second);
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

    SkyboxData* SceneData::GetSkyboxData()
    {
        if (!m_skyboxData)
        {
            auto it = objects.find(ObjectType::Skybox);
            if (it != objects.end() && !it->second.empty())
            {
                m_skyboxData = dynamic_cast<SkyboxData*>(it->second.front().get());
            }
        }

        return m_skyboxData;
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

    std::unordered_set<std::string> RenderData::GetAllPassNames() const
    {
        std::unordered_set<std::string> passNames;

        for (const auto& pass : forwardPasses)
        {
            passNames.insert(pass.name);
        }

        for (const auto& pass : deferredPasses)
        {
            passNames.insert(pass.name);
        }

        return passNames;
    }
    
    std::unordered_set<std::string> RenderData::GetAllOutputs() const
    {
        std::unordered_set<std::string> outputs;

        for (const auto& pass : forwardPasses)
        {
            outputs.insert(pass.output.begin(), pass.output.end());
        }

        for (const auto& pass : deferredPasses)
        {
            outputs.insert(pass.output.begin(), pass.output.end());
        }

        return outputs;
    }

    void RenderData::BuildForwardOutputsCache() const
    {
        if (!forwardOutputsCached)
        {
            std::unordered_set<std::string> uniqueOutputs;
            for (const auto& pass : forwardPasses)
            {
                uniqueOutputs.insert(pass.output.begin(), pass.output.end());
            }

            cachedForwardOutputs.assign(uniqueOutputs.begin(), uniqueOutputs.end());

            cachedForwardOutputPtrs.clear();
            for (const auto& output : cachedForwardOutputs)
            {
                cachedForwardOutputPtrs.push_back(output.c_str());
            }

            forwardOutputsCached = true;
        }
    }

    void RenderData::BuildDeferredOutputsCache() const
    {
        if (!deferredOutputsCached)
        {
            std::unordered_set<std::string> uniqueOutputs;
            for (const auto& pass : deferredPasses)
            {
                uniqueOutputs.insert(pass.output.begin(), pass.output.end());
            }

            cachedDeferredOutputs.assign(uniqueOutputs.begin(), uniqueOutputs.end());

            cachedDeferredOutputPtrs.clear();
            for (const auto& output : cachedDeferredOutputs)
            {
                cachedDeferredOutputPtrs.push_back(output.c_str());
            }

            deferredOutputsCached = true;
        }
    }

    const std::vector<std::string>& RenderData::GetForwardOutputs() const
    {
        BuildForwardOutputsCache();
        return cachedForwardOutputs;
    }

    const std::vector<std::string>& RenderData::GetDeferredOutputs() const
    {
        BuildDeferredOutputsCache();
        return cachedDeferredOutputs;
    }

    const std::vector<const char*>& RenderData::GetForwardOutputsData() const
    {
        BuildForwardOutputsCache();
        return cachedForwardOutputPtrs;
    }

    const std::vector<const char*>& RenderData::GetDeferredOutputsData() const
    {
        BuildDeferredOutputsCache();
        return cachedDeferredOutputPtrs;
    }

    uint32 RenderData::GetForwardOutputsCount() const
    {
        return static_cast<uint32>(GetForwardOutputs().size());
    }

    uint32 RenderData::GetDeferredOutputsCount() const
    {
        return static_cast<uint32>(GetDeferredOutputs().size());
    }

    std::optional<RenderPassData> RenderData::FindRenderPassDataByName(const std::string& name) const
    {
        auto itForward = std::find_if(forwardPasses.begin(), forwardPasses.end(), [&name](const RenderPassData& pass) { return pass.name == name; });
        if (itForward != forwardPasses.end())
        {
            return *itForward;
        }

        auto itDeferred = std::find_if(deferredPasses.begin(), deferredPasses.end(),[&name](const RenderPassData& pass) { return pass.name == name; });
        if (itDeferred != deferredPasses.end())
        {
            return *itDeferred;
        }

        return std::nullopt;
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
                int32 typeInt;
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
        data = njson{
            {"v_sync", render.vSync},
            {"final_render_type", render.finalRender},
            {"render_technique", static_cast<int32>(render.technique)},
            {"forward_render_passes", render.forwardPasses},
            {"deferred_render_passes", render.deferredPasses}
        };
    }

    void from_json(const njson& data, RenderData& render)
    {
        data.at("v_sync").get_to(render.vSync);
        data.at("final_render_type").get_to(render.finalRender);
        
        int32 typeInt;
        data.at("render_technique").get_to(typeInt);
        render.technique = static_cast<RenderTechnique>(typeInt);

        if (data.contains("forward_render_passes"))
        {
            data.at("forward_render_passes").get_to(render.forwardPasses);
        }

        if (data.contains("deferred_render_passes"))
        {
            data.at("deferred_render_passes").get_to(render.deferredPasses);
        }
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

    void to_json(njson& data, const RenderPassData& pass)
    {
        data = njson
        {
            {"name", pass.name},
            {"input", pass.input},
            {"output", pass.output}
        };

        if (!pass.vertexShaderName.empty())
        {
            data["vertex_shader_name"] = pass.vertexShaderName;
        }
        
        if (!pass.pixelShaderName.empty())
        {
            data["pixel_shader_name"] = pass.pixelShaderName;
        }
        
        if (!pass.computeShaderName.empty())
        {
            data["compute_shader_name"] = pass.computeShaderName;
        }
        
        if (!pass.geometryShaderName.empty())
        {
            data["geometry_shader_name"] = pass.geometryShaderName;
        }
    }

    void from_json(const njson& data, RenderPassData& pass)
    {
        data.at("name").get_to(pass.name);
        data.at("input").get_to(pass.input);
        data.at("output").get_to(pass.output);

        if (data.contains("vertex_shader_name"))
        {
            data.at("vertex_shader_name").get_to(pass.vertexShaderName);
        }

        if (data.contains("pixel_shader_name"))
        {
            data.at("pixel_shader_name").get_to(pass.pixelShaderName);
        }

        if (data.contains("compute_shader_name"))
        {
            data.at("compute_shader_name").get_to(pass.computeShaderName);
        }

        if (data.contains("geometry_shader_name"))
        {
            data.at("geometry_shader_name").get_to(pass.geometryShaderName);
        }
    }
}