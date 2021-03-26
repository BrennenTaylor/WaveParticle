cmake_minimum_required(VERSION 3.12)

project(WaveParticleSandbox)

find_package(spdlog CONFIG REQUIRED)

find_path(TCLAP_INCLUDE_DIR tclap/CmdLine.h)
find_library(TCLAP_LIBRARY tclap)

add_subdirectory(FMath)
add_subdirectory(Engine)