cmake_minimum_required(VERSION 3.10)

project(ChessForge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(raylib REQUIRED)

include_directories(src)

file(GLOB_RECURSE ENGINE_SOURCES src/engine/*.cpp)
file(GLOB_RECURSE GAME_SOURCES src/game/*.cpp)
file(GLOB_RECURSE UI_SOURCES src/ui/*.cpp)
set(CORE_SOURCES ${ENGINE_SOURCES} ${GAME_SOURCES} ${UI_SOURCES})

# Main Executable
add_executable(${PROJECT_NAME} src/main.cpp ${CORE_SOURCES})
target_link_libraries(${PROJECT_NAME} raylib)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Test Executable (perft)
add_executable(perft src/test/perft.cpp ${CORE_SOURCES})
target_link_libraries(perft raylib)
set_target_properties(perft PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)