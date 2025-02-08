#include "core/sge_shader_include_handler.h"

namespace SGE
{
    ShaderIncludeHandler::ShaderIncludeHandler(const std::string& includeDirectory) : m_includeDirectory(includeDirectory){}
    
    HRESULT __stdcall ShaderIncludeHandler::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
    {
        std::string fullPath = m_includeDirectory + "/" + pFileName;

        std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            return E_FAIL;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        char* data = new char[size];
        if (!file.read(data, size))
        {
            delete[] data;
            return E_FAIL;
        }

        *ppData = data;
        *pBytes = static_cast<UINT>(size);
        return S_OK;
    }
    
    HRESULT __stdcall ShaderIncludeHandler::Close(LPCVOID pData)
    {
        delete[] static_cast<const char*>(pData);
        return S_OK;
    }
}