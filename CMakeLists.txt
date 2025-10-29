cmake_minimum_required(VERSION 3.16)
project(PathfindingVisualizer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
set(SFML_BUILD_SYSTEM ON CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_USE_STATIC_STD_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(sfml)

add_executable(pathfinding_visualizer
    src/main.cpp
    src/Grid.cpp
    src/Pathfinder.cpp
)

target_include_directories(pathfinding_visualizer PRIVATE include)
target_link_libraries(pathfinding_visualizer PRIVATE sfml-graphics sfml-window sfml-system)

if(MSVC)
    target_compile_options(pathfinding_visualizer PRIVATE /W4 /permissive-)
else()
    target_compile_options(pathfinding_visualizer PRIVATE -Wall -Wextra -Wpedantic)
endif()
