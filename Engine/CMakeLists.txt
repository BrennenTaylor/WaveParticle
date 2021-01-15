set(CMAKE_CXX_STANDARD 17)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_path(TCLAP_INCLUDE_DIR tclap/CmdLine.h)
find_library(TCLAP_LIBRARY tclap)

find_package(tinyobjloader CONFIG REQUIRED)

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

    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    imgui_impl_win32.cpp
    imgui_impl_dx11.cpp

    Main.cpp
)

set (ENGINE_INCLUDES
    ${ENGINE_INCLUDES}

    imconfig.h
    imgui_internal.h
    imgui.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    imgui_impl_win32.h
    imgui_impl_dx11.h

    Farlor.h
)

include_directories(
    ${EXTERNAL_INCLUDES}
    ${TCLAP_INCLUDE_DIR}
)

link_directories(${EXTERNAL_LIB_64})

add_executable(Engine
    ${ENGINE_SOURCES}
    ${ENGINE_INCLUDES}
)

target_compile_definitions(Engine
    PUBLIC _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

set_target_properties(Engine PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)



# target_link_libraries(Engine Game d3d11 dxgi winmm d3dcompiler xinput)
target_link_libraries(Engine
    fmt::fmt
    fmt::fmt-header-only
    tinyobjloader::tinyobjloader
    d3d11 
    dxgi
    winmm
    d3dcompiler
    xinput
    Ws2_32
    dxguid
)
