#ifndef _SGE_MATERIAL_DATA_H_
#define _SGE_MATERIAL_DATA_H_

#include <string>

namespace SGE
{
    struct MaterialData
    {
        std::string diffuseTexturePath;
        std::string normalTexturePath;
        std::string specularTexturePath;
    };
}

#endif // !_SGE_MATERIAL_DATA_H_