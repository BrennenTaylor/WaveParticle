#pragma once

#include <string>
#include <vector>

#include "../Math/Math.h"

#include "Vertex.h"

namespace Farlor
{
    class Mesh
    {
    public:
        Mesh();

        static bool LoadObjTinyObj(const std::string& filename, Mesh& mesh);

        static bool CreateCylinder(float bottomRadius, float topRadius, float height, u32 sliceCount, u32 stackCount, Mesh& mesh);

        void InitializeBuffers(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
        void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);


    private:
        std::vector<VertexPositionUVNormal> m_vertices;
        std::vector<unsigned int> m_indices;

        ID3D11Buffer* m_pVertexBuffer;
        ID3D11Buffer* m_pIndexBuffer;
    };
}
