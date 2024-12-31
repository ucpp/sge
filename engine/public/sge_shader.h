#ifndef _SGE_SHADER_H_
#define _SGE_SHADER_H_

#include "pch.h"

namespace SGE
{
    enum class ShaderType
    {
        Vertex,
        Pixel,
        Compute,
        Geometry
    };

    class Shader final
    {
    public:
        Shader() = default;
        ~Shader();

        void Initialize(const std::string& filePath, ShaderType type);
        D3D12_SHADER_BYTECODE GetShaderBytecode() const;
        ID3DBlob* GetBlob() const { return m_blob.Get(); }

    private:
        std::string ShaderTypeToTarget(ShaderType type) const;

    private:
        ComPtr<ID3DBlob> m_blob;
        static constexpr const char* EntryPoint = "main";
    };
}

#endif // !_SGE_SHADER_H_
