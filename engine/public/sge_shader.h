#ifndef _SGE_SHADER_H_
#define _SGE_SHADER_H_

#include <d3d12.h>
#include <wrl.h>
#include <string>

using namespace Microsoft::WRL;

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

        void Initialize(const std::wstring& filePath, ShaderType type);
        D3D12_SHADER_BYTECODE GetShaderBytecode() const;
        ID3DBlob* GetBlob() const { return m_blob.Get(); }

        static constexpr const char* EntryPoint = "main";

    private:
        ComPtr<ID3DBlob> m_blob;
        std::string ShaderTypeToTarget(ShaderType type) const;
    };
}

#endif // !_SGE_SHADER_H_
