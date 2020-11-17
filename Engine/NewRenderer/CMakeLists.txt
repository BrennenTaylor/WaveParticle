add_subdirectory(D3D11)

set (ENGINE_SOURCES

    ${ENGINE_SOURCES}

    NewRenderer/Camera.cpp
    NewRenderer/HeightField.cpp
    NewRenderer/Mesh.cpp
    NewRenderer/Renderer.cpp
    NewRenderer/Shader.cpp
    NewRenderer/TexturedQuad.cpp
    NewRenderer/Vertex.cpp
    NewRenderer/WICTextureLoader.cpp

    PARENT_SCOPE
)

set (ENGINE_INCLUDES

    ${ENGINE_INCLUDES}

    NewRenderer/Camera.h
    NewRenderer/HeightField.h
    NewRenderer/IRenderer.h
    NewRenderer/Mesh.h
    NewRenderer/Renderer.h
    NewRenderer/RenderingComponent.h
    NewRenderer/Shader.h
    NewRenderer/TexturedQuad.h
    NewRenderer/Vertex.h
    NewRenderer/WICTextureLoader.h

    PARENT_SCOPE
)
