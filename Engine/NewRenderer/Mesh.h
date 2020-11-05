#pragma once

#include "Vertex.h"

#include <string>
#include <vector>

namespace Farlor
{
    class Mesh
    {
    public:
        Mesh();

        static bool LoadObjTinyObj(const std::string& filename, Mesh& mesh);

        static bool CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, Mesh& mesh);

        void InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
        void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);


    private:
        std::vector<VertexPositionUVNormal> m_vertices;
        std::vector<unsigned int> m_indices;

        ID3D11Buffer* m_pVertexBuffer;
        ID3D11Buffer* m_pIndexBuffer;
    };
}
