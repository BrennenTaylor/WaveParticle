file (GLOB_RECURSE GAME_SOURCES *.cpp)
file (GLOB_RECURSE GAME_INCLUDES *.h)

# Set include directorys
include_directories(${CMAKE_SOURCE_DIR}/Engine)

add_library(Game SHARED ${GAME_SOURCES} ${GAME_INCLUDES})
