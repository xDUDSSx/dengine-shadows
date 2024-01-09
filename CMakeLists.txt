cmake_minimum_required(VERSION 3.26)
project(Dengine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create project target
add_executable(Dengine)

# Setup output directories
set(DENGINE_OUT_DIR "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
        "${DENGINE_OUT_DIR}"
        CACHE PATH
        "Single Directory for all Libraries")

# Setup the Executable output Directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        "${DENGINE_OUT_DIR}"
        CACHE PATH
        "Single Directory for all Executables.")

# Setup the Executable output Directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        "${DENGINE_OUT_DIR}"
        CACHE PATH
        "Single Directory for all static libraries.")

# Project sources
file(GLOB_RECURSE DENGINE_SOURCE
        "src/*.h"
        "src/*.hpp"
        "src/*.cpp"
)
include_directories(src/)

# GLAD
file(GLOB_RECURSE GLAD_SOURCE
        "${CMAKE_SOURCE_DIR}/libs/glad/include/*"
        "libs/glad/src/*"
)
list(APPEND DENGINE_SOURCE "${GLAD_SOURCE}")
include_directories(libs/glad/include)

# GLM
add_subdirectory(libs/glm)

# spglog
add_subdirectory(libs/spdlog)
include_directories(libs/spdlog/include)

# Imgui
set(IMGUI_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_demo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_tables.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/misc/cpp/imgui_stdlib.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_opengl3.cpp
)
list(APPEND DENGINE_SOURCE "${IMGUI_SOURCE}")
include_directories(Dengine libs/imgui)

# Imguizmo
add_subdirectory(libs/imguizmo)
include_directories(libs/imguizmo/imguizmo)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw)

# Assimp
set(ASSIMP_BUILD_TESTS False CACHE BOOL "Assimp should not build tests.")
set(ASSIMP_BUILD_ASSIMP_TOOLS CACHE INTERNAL FALSE)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT CACHE INTERNAL FALSE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT CACHE INTERNAL FALSE)
#set(ASSIMP_BUILD_3DS_IMPORTER OFF)
set(ASSIMP_BUILD_COLLADA_IMPORTER ON)
set(ASSIMP_BUILD_FBX_IMPORTER ON)
set(ASSIMP_BUILD_GLTF_IMPORTER ON)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
#set(ASSIMP_BUILD_PLY_IMPORTER OFF)

add_subdirectory(libs/assimp)

set(ASSIMP_BUILD_ZLIB ON)

# magic_enum
include_directories(libs/magic_enum/include/magic_enum)

# Compile and link
#add_executable(Dengine src/main.cpp ${DENGINE_SOURCE})
target_sources(Dengine PUBLIC src/main.cpp ${DENGINE_SOURCE})
target_link_libraries(Dengine
        glfw
        glm
        assimp
        ImGuizmo
)

# Debug stuff
#message("Debug list:")
#foreach(file_path ${DENGINE_SOURCE})
#    message(${file_path})
#endforeach()