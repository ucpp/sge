#include "sge_shader_manager.h"

namespace SGE
{
    std::unordered_map<std::string, std::unique_ptr<Shader>> ShaderManager::m_shaderCache;

    const Shader& ShaderManager::GetShader(const std::string& shaderPath, ShaderType type, bool reload)
    {
        std::string cacheKey = shaderPath + "_" + std::to_string(static_cast<int>(type));
        auto it = m_shaderCache.find(cacheKey);

        if (it != m_shaderCache.end())
        {
            if (reload)
            {
                it->second->Initialize(shaderPath, type);
                return *it->second;
            }
            return *it->second;
        }

        auto shader = std::make_unique<Shader>();
        shader->Initialize(shaderPath, type);

        m_shaderCache[cacheKey] = std::move(shader);

        return *m_shaderCache[cacheKey];
    }
}
