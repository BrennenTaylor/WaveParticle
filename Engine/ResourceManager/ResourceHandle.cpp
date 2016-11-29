#include "ResourceHandle.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace Farlor
{
    string ResourceHandle::s_ResourcePath{"resources/Models/"};

    ResourceHandle::ResourceHandle(string& filename, u32 isBinary)
        : m_buffer(nullptr)
        , m_bufferSize(0)
    {
        string pathFilename = s_ResourcePath + filename;

        ifstream resourceFile;


        if(isBinary)
        {
            resourceFile.open(pathFilename, ios::in | ios::binary);
        }
        else
        {
            resourceFile.open(pathFilename);
        }

        if (resourceFile.is_open())
        {
            resourceFile.seekg(0, ios::end);
            auto bufSize = resourceFile.tellg();
            // Leave room for null byte
            m_bufferSize = (u32)bufSize + 1;
            resourceFile.seekg(0, ios::beg);

            m_buffer = new u8[m_bufferSize];
            memset(m_buffer, 0, m_bufferSize);

            resourceFile.read(reinterpret_cast<char*>(m_buffer), m_bufferSize);

            resourceFile.close();
        }
    }
}
