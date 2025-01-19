#include "sge_shader.h"

#include "sge_helpers.h"
#include "sge_logger.h"
#include "sge_shader_include_handler.h"

namespace SGE
{
    Shader::~Shader()
    {
        m_blob.Reset();
    }
    
    void Shader::Initialize(const std::string& filePath, ShaderType type)
    {
        UINT compileFlags = 0;
#ifdef _DEBUG
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        std::string target = ShaderTypeToTarget(type);
        std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());

        std::string includeDirectory = GetDirectoryFromFilePath(filePath);
        ShaderIncludeHandler includeHandler(includeDirectory);
        ComPtr<ID3DBlob> errorBlob;
    
        HRESULT hr = D3DCompileFromFile(
            wFilePath.c_str(),
            nullptr,
            &includeHandler,
            EntryPoint,
            target.c_str(),
            compileFlags,
            0,
            &m_blob,
            &errorBlob
        );
        
        if (FAILED(hr))
        {
            if (errorBlob)
            {
                std::string errorMessage(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
                LOG_ERROR("Shader compilation error in file {}:\n {}", filePath, errorMessage);
            }
            else
            {
                LOG_ERROR("Shader compilation failed with no error message.");
            }

            Verify(hr, "Failed to compile shader");
        }
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

    std::string Shader::GetDirectoryFromFilePath(const std::string& filePath) const
    {
        size_t lastSlash = filePath.find_last_of("/\\");
        if (lastSlash == std::string::npos)
        {
            return "";
        }
        return filePath.substr(0, lastSlash);
    }
}