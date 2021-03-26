set(CMAKE_CXX_STANDARD 17)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_path(TCLAP_INCLUDE_DIR tclap/CmdLine.h)
find_library(TCLAP_LIBRARY tclap)

find_package(tinyobjloader CONFIG REQUIRED)

add_executable(Engine

    Main.cpp

    # Source Files
    Core/Events/ButtonEvent.cpp
    Core/Events/ControllerConnectionEvent.cpp
    Core/Events/ControllerVibrationEvent.cpp
    Core/Events/EventManager.cpp
    Core/Events/KeyboardEvent.cpp
    Core/Events/ThumbstickEvent.cpp

    Core/Memory/BaseAllocator.cpp
    Core/Memory/LinearAllocator.cpp
    Core/Memory/PoolAllocator.cpp
    Core/Memory/ProxyAllocator.cpp
    Core/Memory/StackAllocator.cpp

    Core/Engine.cpp
    Core/FixedUpdate.cpp
    Core/Timer.cpp
    Core/Win32Window.cpp

    ECS/Component.cpp
    ECS/EntityManager.cpp
    ECS/TransformManager.cpp

    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/backends/imgui_impl_win32.cpp
    imgui/backends/imgui_impl_dx11.cpp

    Input/ControllerManager.cpp
    Input/InputHandler.cpp

    JsonCpp/jsoncpp.cpp

    Networking/NetworkManager.cpp
    Networking/Packet.cpp
    Networking/Socket.cpp

    NewRenderer/D3D11/DXGIModeDesc.cpp
    NewRenderer/D3D11/DXGISwapChainDesc.cpp
    NewRenderer/D3D11/RasterizerStateDesc.cpp

    NewRenderer/Camera.cpp
    NewRenderer/HeightField.cpp
    NewRenderer/Mesh.cpp
    NewRenderer/Renderer.cpp
    NewRenderer/Shader.cpp
    NewRenderer/TexturedQuad.cpp
    NewRenderer/Vertex.cpp
    NewRenderer/WICTextureLoader.cpp

    Physics/CollisionFunctions.cpp
    Physics/PhysicsComponent.cpp
    Physics/PhysicsSystem.cpp
    Physics/ParticleSystem.cpp
    Physics/LineSegment.cpp
    Physics/Particle.cpp
    Physics/ParticleSystemTimer.cpp

    ResourceManager/ResourceDesc.cpp
    ResourceManager/ResourceHandle.cpp
    ResourceManager/ResourceManager.cpp

    tinyxml2/tinyxml2.cpp

    Util/HashedString.cpp
    Util/StringUtil.cpp
    Util/SystemCheck.cpp
    Util/UIDGenerator.cpp

    # Includes
    Farlor.h

    Core/Events/ButtonEvent.h
    Core/Events/ControllerConnectionEvent.h
    Core/Events/ControllerVibrationEvent.h
    Core/Events/EventData.h
    Core/Events/EventListener.h
    Core/Events/EventManager.h
    Core/Events/KeyboardEvent.h
    Core/Events/ThumbstickEvent.h

    Core/Memory/BaseAllocator.h
    Core/Memory/LinearAllocator.h
    Core/Memory/MemoryAlignmentUtils.h
    Core/Memory/PoolAllocator.h
    Core/Memory/ProxyAllocator.h
    Core/Memory/StackAllocator.h

    Core/Engine.h
    Core/FixedUpdate.h
    Core/EngineConfig.h
    Core/Timer.h
    Core/Win32Window.h
    Core/Window.h

    ECS/Component.h
    ECS/EntityManager.h
    ECS/Entity.h
    ECS/TransformComponent.h
    ECS/TransformManager.h

    imgui/imconfig.h
    imgui/imgui_internal.h
    imgui/imgui.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/backends/imgui_impl_win32.h
    imgui/backends/imgui_impl_dx11.h

    Input/Buttons.h
    Input/ControllerManager.h
    Input/InputHandler.h
    Input/Keys.h

    JsonCpp/json.h
    JsonCpp/json-forwards.h

    Networking/NetworkManager.h
    Networking/Packet.h
    Networking/Socket.h

    NewRenderer/D3D11/DXGIModeDesc.h
    NewRenderer/D3D11/DXGISwapChainDesc.h
    NewRenderer/D3D11/RasterizerStateDesc.h
    NewRenderer/D3D11/WrappedD3D11.h

    NewRenderer/Camera.h
    NewRenderer/CBs.h
    NewRenderer/HeightField.h
    NewRenderer/IRenderer.h
    NewRenderer/LightComponents.h
    NewRenderer/Mesh.h
    NewRenderer/Renderer.h
    NewRenderer/RenderingComponent.h
    NewRenderer/Shader.h
    NewRenderer/TexturedQuad.h
    NewRenderer/Vertex.h
    NewRenderer/WICTextureLoader.h

    Physics/CollisionFunctions.h
    Physics/PhysicsComponent.h
    Physics/PhysicsSystem.h
    Physics/Particle.h
    Physics/ParticleSystem.h
    Physics/Plane.h
    Physics/ParticleSystemTimer.h
    Physics/Sample.h

    Platform/Platform.h

    ResourceManager/ResourceDesc.h
    ResourceManager/ResourceHandle.h
    ResourceManager/ResourceManager.h
    ResourceManager/ResourceTypes.h

    tinyxml2/tinyxml2.h

    Util/HashedString.h
    Util/StringUtil.h
    Util/SystemCheck.h
    Util/UIDGenerator.h
)


# target_link_libraries(Engine Game d3d11 dxgi winmm d3dcompiler xinput)
target_link_libraries(Engine
    PUBLIC FMath::FMath    
    PUBLIC fmt::fmt
    PUBLIC fmt::fmt-header-only
    PUBLIC tinyobjloader::tinyobjloader
    PUBLIC d3d11 
    PUBLIC dxgi
    PUBLIC winmm
    PUBLIC d3dcompiler
    PUBLIC xinput
    PUBLIC Ws2_32
    PUBLIC dxguid
)

target_include_directories(Engine
    PUBLIC ${TCLAP_INCLUDE_DIR}
    PUBLIC imgui/
)


target_compile_definitions(Engine
    PUBLIC _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

set_target_properties(Engine PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
