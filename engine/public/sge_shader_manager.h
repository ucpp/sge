#ifndef _SGE_SHADER_MANAGER_H_
#define _SGE_SHADER_MANAGER_H_

#include "pch.h"
#include "sge_shader.h"

namespace SGE
{
    class ShaderManager
    {
    public:
        static const Shader& GetShader(const std::string& shaderPath, ShaderType type, bool reload = false);

    private:
        static std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaderCache;
    };
}

#endif // !_SGE_SHADER_MANAGER_H_
