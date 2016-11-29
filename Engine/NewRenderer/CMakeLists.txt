add_subdirectory(D3D11)

set (ENGINE_SOURCES

    ${ENGINE_SOURCES}

    NewRenderer/Camera.cpp
    NewRenderer/Mesh.cpp
    NewRenderer/Renderer.cpp
    NewRenderer/Shader.cpp
    NewRenderer/Vertex.cpp
    NewRenderer/WICTextureLoader.cpp

    NewRenderer/Terrain.cpp


    PARENT_SCOPE
)

set (ENGINE_INCLUDES

    ${ENGINE_INCLUDES}

    NewRenderer/Camera.h
    NewRenderer/IRenderer.h
    NewRenderer/Mesh.h
    NewRenderer/Renderer.h
    NewRenderer/RenderingComponent.h
    NewRenderer/Shader.h
    NewRenderer/Vertex.h
    NewRenderer/WICTextureLoader.h

    NewRenderer/Terrain.h

    PARENT_SCOPE
)
