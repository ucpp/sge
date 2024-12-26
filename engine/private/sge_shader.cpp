#include "sge_shader.h"

#include "sge_helpers.h"
#include <d3dcompiler.h>

namespace SGE
{
    Shader::~Shader()
    {
        m_blob.Reset();
    }
    
    void Shader::Initialize(const std::wstring& filePath, ShaderType type)
    {
        UINT compileFlags = 0;
#ifdef _DEBUG
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        std::string target = ShaderTypeToTarget(type);

        HRESULT hr = D3DCompileFromFile(
            filePath.c_str(),
            nullptr,
            nullptr,
            EntryPoint,
            target.c_str(),
            compileFlags,
            0,
            &m_blob,
            nullptr
        );

        Verify(hr, "Failed to compile shader");
    }

    D3D12_SHADER_BYTECODE Shader::GetShaderBytecode() const
    {
        return D3D12_SHADER_BYTECODE{ m_blob->GetBufferPointer(), m_blob->GetBufferSize() };
    }

    std::string Shader::ShaderTypeToTarget(ShaderType type) const
    {
        switch (type)
        {
            case ShaderType::Vertex: return "vs_5_0";
            case ShaderType::Pixel: return "ps_5_0";
            case ShaderType::Compute: return "cs_5_0";
            case ShaderType::Geometry: return "gs_5_0";
            default: throw std::invalid_argument("Unsupported shader type.");
        }
    }
}