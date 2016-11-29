add_subdirectory(Core)
add_subdirectory(ECS)
add_subdirectory(Input)
add_subdirectory(JsonCpp)
add_subdirectory(Math)
add_subdirectory(Networking)
add_subdirectory(NewRenderer)
add_subdirectory(Platform)
add_subdirectory(Physics)
# add_subdirectory(Renderer)
add_subdirectory(ResourceManager)
add_subdirectory(tinyxml2)
add_subdirectory(Util)

set (ENGINE_SOURCES
    ${ENGINE_SOURCES}
    Main.cpp
)

set (ENGINE_INCLUDES
    ${ENGINE_INCLUDES}
    Farlor.h
)

include_directories(${EXTERNAL_INCLUDES})

link_directories(${EXTERNAL_LIB_64})

add_executable(Engine ${ENGINE_SOURCES} ${ENGINE_INCLUDES})

# target_link_libraries(Engine Game d3d11 dxgi winmm d3dcompiler xinput)
target_link_libraries(Engine d3d11 dxgi winmm d3dcompiler xinput Ws2_32 dxguid)
