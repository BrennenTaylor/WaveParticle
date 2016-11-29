#include "Mesh.h"

#include <fstream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#include "Vertex.h"

namespace Farlor
{
    Mesh::Mesh()
        : m_vertices{}
        , m_indices{}
    {
    }

    void Mesh::InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        HRESULT hr;
        // Fill in a buffer description.
        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth        = sizeof( VertexPositionUVNormal ) * (int)m_vertices.size();
        bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags   = 0;
        bufferDesc.MiscFlags        = 0;

        // Fill in the subresource data.
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = m_vertices.data();
        InitData.SysMemPitch = 0;
        InitData.SysMemSlicePitch = 0;

        // Create the vertex buffer.
        hr = pDevice->CreateBuffer( &bufferDesc, &InitData, &m_pVertexBuffer );
        if (FAILED(hr))
        {
            cout << "Failed to create vertex buffer: " << endl;
        }

        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        ZeroMemory(&InitData, sizeof(InitData));
        bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth        = sizeof( unsigned int ) * (int)m_indices.size();
        bufferDesc.BindFlags        = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags   = 0;
        bufferDesc.MiscFlags        = 0;

        // Fill in the subresource data.
        InitData.pSysMem = m_indices.data();
        InitData.SysMemPitch = 0;
        InitData.SysMemSlicePitch = 0;

        // Create the index buffer.
        hr = pDevice->CreateBuffer( &bufferDesc, &InitData, &m_pIndexBuffer );
        if (FAILED(hr))
        {
            cout << "Failed to create index buffer: " << endl;
        }
    }

    void Mesh::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    {
        unsigned int stride;
        unsigned int offset;
        stride = sizeof(VertexPositionUVNormal);
	    offset = 0;
        pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
        pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pDeviceContext->DrawIndexed((unsigned int)m_indices.size(), 0, 0);
    }

    bool Mesh::CreateCylinder(float bottomRadius, float topRadius, float height, u32 sliceCount, u32 stackCount, Mesh& mesh)
    {
        mesh.m_vertices.clear();
        mesh.m_indices.clear();

        float stackHeight = height / stackCount;
        float radiusStep = (topRadius - bottomRadius) / stackCount;

        u32 ringCount = stackCount + 1;

        // Compute vertices
        for (u32 i = 0; i < ringCount; ++i)
        {
            float y = -0.5f * height + i * stackHeight;
            float r = bottomRadius + i * radiusStep;

            // Vertics of ring
            float dTheta = 2.0f * FARLOR_PI / sliceCount;
            for (u32 j = 0; j < sliceCount; ++j)
            {
                VertexPositionUVNormal vertex;
                float c = cosf(j * dTheta);
                float s = sinf(j * dTheta);

                vertex.m_position = Vector3(r * c, y, r * s);

                vertex.m_uv.x = (float)j / sliceCount;
                vertex.m_uv.y = 1.0f - (float)i / stackCount;

                Vector3 tangent(-1.0f * s, 0.0f, c);
                float dr = bottomRadius - topRadius;
                Vector3 bitangent(dr * c, -1.0f * height, dr * s);

                vertex.m_normal = tangent.Cross(bitangent).Normalized();
            }
        }

        u32 ringVertexCount = sliceCount + 1;

        for (u32 i = 0; i < stackCount; ++i)
        {
            for (u32 j = 0; j < stackCount; ++j)
            {
                mesh.m_indices.push_back(i*ringVertexCount + j);
                mesh.m_indices.push_back((i+1)*ringVertexCount + j);
                mesh.m_indices.push_back((i+1)*ringVertexCount + j+1);
                mesh.m_indices.push_back(i*ringVertexCount + j);
                mesh.m_indices.push_back((i+1)*ringVertexCount + j+1);
                mesh.m_indices.push_back(i*ringVertexCount + j+1);
            }
        }

        //  No end cap geometry yet, need to do!!

        return true;
    }

    bool Mesh::LoadObjTinyObj(const std::string& filename, Mesh& mesh)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str()))
        {
            cout << "Failed to load model:" << filename << endl;
        }

        std::unordered_map<VertexPositionUVNormal, int> uniqueVertices = {};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                VertexPositionUVNormal vertex = {};

                vertex.m_position =
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                // vertex.m_uv =
                // {
                //     attrib.texcoords[2 * index.texcoord_index + 0],
                //     1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                // };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = (int)mesh.m_vertices.size();
                    mesh.m_vertices.push_back(vertex);
                }

                mesh.m_indices.push_back(uniqueVertices[vertex]);
            }
        }

        // Generate normals if the values are not loaded.
        // TODO: Check that the values are not loaded.

        for (u32 i = 0; i < (u32)mesh.m_indices.size(); i += 3)
        {
            u32 index0 = mesh.m_indices[i];
            u32 index1 = mesh.m_indices[i+1];
            u32 index2 = mesh.m_indices[i+2];

            Vector3 pos0 = mesh.m_vertices[index0].m_position;
            Vector3 pos1 = mesh.m_vertices[index1].m_position;
            Vector3 pos2 = mesh.m_vertices[index2].m_position;

            Vector3 a = pos1 - pos0;
            Vector3 b = pos2 - pos0;
            Vector3 faceNormal = a.Cross(b);

            mesh.m_vertices[index0].m_normal += faceNormal;
            mesh.m_vertices[index1].m_normal += faceNormal;
            mesh.m_vertices[index2].m_normal += faceNormal;
        }

        for (u32 i = 0; i < (u32)mesh.m_vertices.size(); ++i)
        {
            mesh.m_vertices[i].m_normal = mesh.m_vertices[i].m_normal.Normalized();
        }

        return true;
    }
}
