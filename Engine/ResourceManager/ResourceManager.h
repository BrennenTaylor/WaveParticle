#pragma once

#include <string>

#include "../Farlor.h"

#include "ResourceDesc.h"
#include "ResourceTypes.h"

namespace Farlor
{
    class ResourceManager
    {
    public:
        ResourceManager();
        void LoadResourceDesc(std::string& filename);

        void LoadScope(std::string& scope);
        void UnloadScope(std::string& scope);

    private:
        void AddResourceDesc(u32 uid, ResourceType type, std::string& filename, std::string& scope, u32 isBinary);
        ResourceType ParseType(std::string typeStr);

    private:
        std::map<std::string, std::vector<ResourceDesc>> m_scopedResourceDescs;
    };
}
