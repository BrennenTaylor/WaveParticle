#pragma once

#include "../Farlor.h"
#include "ResourceTypes.h"

#include <string>

namespace Farlor
{
    class ResourceHandle;

    class ResourceDesc
    {
    public:
        ResourceDesc(u32 uid, ResourceType type, std::string& filename, std::string& scope, u32 isBinary);
        void Load();
        void Unload();

        ResourceHandle* GetResourceHandle();

    private:
        u32 m_uid;
        ResourceType m_type;
        std::string m_filename;
        std::string m_scope;
        u32 m_isBinary;
        bool m_loaded;
        u32 m_numScopesLoaded;

        ResourceHandle* m_resourceHandle;
    };
}
