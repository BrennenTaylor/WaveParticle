#include "ResourceManager.h"

#include "../tinyxml2/tinyxml2.h"

#include <iostream>

using namespace std;

namespace Farlor
{
    ResourceManager::ResourceManager()
        : m_scopedResourceDescs()
    {
    }

    void ResourceManager::LoadResourceDesc(string& filename)
    {
        tinyxml2::XMLDocument resourceDoc;
        resourceDoc.LoadFile(filename.c_str());

        // Get Resources Node
        auto* pResourcesNode = resourceDoc.FirstChildElement("resources");
        if (!pResourcesNode)
        {
            cout << "Failed to load resources node" << endl;
            return;
        }

        // Get all child resource nodes
        for(auto* pResourceNode = pResourcesNode->FirstChildElement("resource");
            pResourceNode != nullptr;
            pResourceNode = pResourceNode->NextSiblingElement())
        {
            auto uidElement = pResourceNode->FirstChildElement("UID");
            string resourceUIDStr = uidElement->GetText();
            auto typeElement = pResourceNode->FirstChildElement("Type");
            string resourceTypeStr = typeElement->GetText();
            auto filenameElement = pResourceNode->FirstChildElement("Filename");
            string resourceFilenameStr = filenameElement->GetText();
            auto scopeElement = pResourceNode->FirstChildElement("Scope");
            string resourceScopeStr = scopeElement->GetText();
            auto binaryElement = pResourceNode->FirstChildElement("Binary");
            string resourceBinaryStr = binaryElement->GetText();

            u32 resourceUID = stoi(resourceUIDStr);
            u32 resourceBinary = stoi(resourceBinaryStr);
            ResourceType resourceType = ParseType(resourceTypeStr);

            AddResourceDesc(resourceUID, resourceType,
                resourceFilenameStr, resourceScopeStr, resourceBinary);
        }
    }

    void ResourceManager::LoadScope(std::string& scope)
    {
        auto scopeListItr = m_scopedResourceDescs.find(scope);
        if(scopeListItr != m_scopedResourceDescs.end())
        {
            for (auto& resource : m_scopedResourceDescs[scope])
            {
                resource.Load();
            }
        }
    }

    void ResourceManager::UnloadScope(std::string& scope)
    {
        auto scopeListItr = m_scopedResourceDescs.find(scope);
        if(scopeListItr != m_scopedResourceDescs.end())
        {
            for (auto& resource : m_scopedResourceDescs[scope])
            {
                resource.Unload();
            }
        }
    }

    void ResourceManager::AddResourceDesc(u32 uid, ResourceType type, std::string& filename, std::string& scope, u32 isBinary)
    {
        ResourceDesc resourceDesc(uid, type, filename, scope, isBinary);
        m_scopedResourceDescs[scope].push_back(resourceDesc);
    }

    ResourceType ResourceManager::ParseType(std::string typeStr)
    {
        if (typeStr == "Model")
        {
            return ResourceType::Model;
        }

        return ResourceType::Unknown;
    }
}
