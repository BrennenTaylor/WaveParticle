# This is a build file for Farlor

# Format

# 1. Required CMake Version
# Specify minimum version for cmake
cmake_minimum_required(VERSION 2.8)

# set(CMAKE_CXX_COMPILER "cl.exe")
# set(CMAKE_C_COMPILER "cl.exe")
# set(CMAKE_CXX_LINK_EXECUTABLE "link.exe")
# set(CMAKE_C_LINK_EXECUTABLE "link.exe")

# 2. Package Name
# Projects name
project(Engine)

message (STATUS "*******Build System Information*******")
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    add_definitions(-DSYSTEM_LINUX)
endif()
if("${CMAKE_SYSTEM_NAME}" STREQUAL "WINDOWS")
    add_definitions(-DSYSTEM_WINDOWS)
endif()
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# Set CMAKE version
set(ENGINE_MAJOR_VERSION 0)
set(ENGINE_MINOR_VERSION 1)
set(ENGINE_PATCH_VERSION 0)
set(ENGINE_VERSION ${ENGINE_MAJOR_VERSION}.${ENGINE_MINOR_VERSION}.${ENGINE_PATCH_VERSION})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(UNIX)
    message(status "Setting GCC flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    message(status "Setting MSVC flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX /nologo")
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
endif()

message(status "** CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# Add subdirectories for project

# Set the output folder
message ("CMAKE_SOURCE_DIR" ${CMAKE_SOURCE_DIR})
message ("CMAKE_BINARY_DIR" ${CMAKE_BINARY_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

message ("CMAKE_BINARY_DIR" ${CMAKE_BINARY_DIR})
message ("CMAKE_RUNTIME_OUTPUT_DIRECTORY" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message ("CMAKE_LIBRARY_OUTPUT_DIRECTORY" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

SET(EXECUTABLE_NAME app)

SET (EXTERNAL_INCLUDES "$ENV{EXTERNAL_INCLUDES}/")
SET (EXTERNAL_LIB_64 "$ENV{EXTERNAL_LIBS}/x64/")

message (${EXTERNAL_INCLUDES})
message (${EXTERNAL_LIB_64})

add_definitions( -DGAME_SHARED )

add_subdirectory(Engine)
add_subdirectory(Game)
