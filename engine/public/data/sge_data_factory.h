#ifndef _SGE_DATA_FACTORY_H_
#define _SGE_DATA_FACTORY_H_

#include "pch.h"
#include "data/sge_data_structures.h"
#include "core/sge_singleton.h"

namespace SGE
{
    class ObjectDataFactory : public Singleton<ObjectDataFactory>
    {
        friend class Singleton<ObjectDataFactory>;
    public:
        using Creator = std::function<std::unique_ptr<ObjectDataBase>()>;

        void Register(ObjectType type, Creator creator);
        std::unique_ptr<ObjectDataBase> Create(ObjectType type);

    private:
        std::unordered_map<ObjectType, Creator> creators;
    };

    class AssetDataFactory : public Singleton<AssetDataFactory>
    {
        friend class Singleton<AssetDataFactory>;
    public:
        using Creator = std::function<std::unique_ptr<AssetDataBase>()>;

        void Register(AssetType type, Creator creator);
        std::unique_ptr<AssetDataBase> Create(AssetType type);

    private:
        std::unordered_map<AssetType, Creator> creators;
    };

    void RegisterObjectDataTypes();
    void RegisterAssetDataTypes();
}

#endif // !_SGE_DATA_FACTORY_H_