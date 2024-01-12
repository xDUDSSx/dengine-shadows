cmake_minimum_required(VERSION 3.26)
project(Dengine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create project target
add_executable(Dengine)

# Make default build type Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Organize files into groups in Visual Studio based on directories (ideal goal)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    # Enable multi-processor compilation for MSVC.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    message(STATUS "MSVC: CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
    message(STATUS "MSVC: CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")

    set(CMAKE_SHARED_LINKER_FLAGS /MANIFEST:NO)
endif ()

# Set VS_DEBUGGER_WORKING_DIRECTORY
set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
# NOTE: In other IDE's, working directory has to be set manually to the project root dir

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
# Set compiler warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 -D_CRT_SECURE_NO_WARNINGS)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif ()

#add_executable(Dengine src/main.cpp ${DENGINE_SOURCE})
target_sources(Dengine PUBLIC src/main.cpp ${DENGINE_SOURCE})
target_link_libraries(Dengine
        glfw
        glm
        assimp
        ImGuizmo
)

# Compiler flags overview
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "CXX debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CXX release with deb info flags: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "CXX release flags: ${CMAKE_CXX_FLAGS_RELEASE}")
get_target_property(MAIN_CFLAGS ${PROJECT_NAME} COMPILE_OPTIONS)
message(STATUS "Target compiler flags are: ${MAIN_CFLAGS}\n")
