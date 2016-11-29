#include "ResourceDesc.h"
#include "ResourceHandle.h"

namespace Farlor
{
    ResourceDesc::ResourceDesc(u32 uid, ResourceType type, std::string& filename, std::string& scope, u32 isBinary)
        : m_uid(uid)
        , m_type(type)
        , m_filename(filename)
        , m_scope(scope)
        , m_isBinary(isBinary)
        , m_loaded(false)
        , m_numScopesLoaded(0)
        , m_resourceHandle(nullptr)
    {
    }

    void ResourceDesc::Load()
    {
        // Actually load the data for the resource

        m_resourceHandle = new ResourceHandle(m_filename, m_isBinary);
    }

    void ResourceDesc::Unload()
    {
        // Actually unload the data for the resource
    }

    ResourceHandle* ResourceDesc::GetResourceHandle()
    {
        return m_resourceHandle;
    }
}
