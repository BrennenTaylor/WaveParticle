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

    # NewRenderer/imgui_demo.cpp
    # NewRenderer/imgui_draw.cpp
    # NewRenderer/imgui_tables.cpp
    # NewRenderer/imgui_widgets.cpp
    # NewRenderer/imgui.cpp
    # NewRenderer/imgui_impl_win32.cpp
    # NewRenderer/imgui_impl_dx11.cpp

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

    # NewRenderer/imconfig.h
    # NewRenderer/imgui_internal.h
    # NewRenderer/imgui.h
    # NewRenderer/imstb_rectpack.h
    # NewRenderer/imstb_textedit.h
    # NewRenderer/imstb_truetype.h
    # NewRenderer/imgui_impl_win32.h
    # NewRenderer/imgui_impl_dx11.h

    PARENT_SCOPE
)
