#ifndef _SGE_SHADER_INCLUDE_HANDLER_H_
#define _SGE_SHADER_INCLUDE_HANDLER_H_

#include "pch.h"

namespace SGE
{
    class ShaderIncludeHandler : public ID3DInclude
    {
    public:
        ShaderIncludeHandler(const std::string& includeDirectory);
        HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;
        HRESULT __stdcall Close(LPCVOID pData) override;

    private:
        std::string m_includeDirectory;
    };
}

#endif // !_SGE_SHADER_INCLUDE_HANDLER_H_