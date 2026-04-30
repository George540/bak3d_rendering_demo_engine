cmake_minimum_required(VERSION 3.20)

# Project definition
project(bak3d VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)

# Enable folder grouping in Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Define root paths
set(BAK3D_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BAK3D_SOURCE_DIR ${BAK3D_ROOT_DIR}/bak3d)
set(BAK3D_THIRD_PARTY_DIR ${BAK3D_ROOT_DIR}/third_party)
set(BAK3D_ASSETS_DIR ${BAK3D_ROOT_DIR}/assets)
set(BAK3D_SHADERS_DIR ${BAK3D_ROOT_DIR}/shaders)
set(BAK3D_CONFIG_OUTPUT_DIR ${CMAKE_BINARY_DIR}/config)

# Create output directories if they don't exist
file(MAKE_DIRECTORY ${BAK3D_CONFIG_OUTPUT_DIR})

# ============================================================================
# Third-Party Libraries Setup
# ============================================================================

message(STATUS "")
message(STATUS "--- Third-Party Libraries ---")

# assimp
if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/assimp")
    message(STATUS "  [assimp] Found at ${BAK3D_THIRD_PARTY_DIR}/assimp")
    add_library(assimp SHARED IMPORTED)
    set_target_properties(assimp PROPERTIES
        IMPORTED_IMPLIB    "${BAK3D_THIRD_PARTY_DIR}/assimp/lib/assimp-vc143-mt.lib"
        IMPORTED_LOCATION  "${BAK3D_THIRD_PARTY_DIR}/assimp/lib/assimp-vc143-mt.dll"
        INTERFACE_INCLUDE_DIRECTORIES "${BAK3D_THIRD_PARTY_DIR}/assimp/include"
    )
    set_target_properties(assimp PROPERTIES FOLDER "ThirdParty")
    message(STATUS "  [assimp] Import lib : assimp-vc143-mt.lib")
    message(STATUS "  [assimp] Runtime DLL: assimp-vc143-mt.dll")
else()
    message(STATUS "  [assimp] NOT FOUND — skipping")
endif()

# glad
if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/glad")
    message(STATUS "  [glad] Found at ${BAK3D_THIRD_PARTY_DIR}/glad")
    add_library(glad STATIC)
    target_sources(glad PRIVATE
        ${BAK3D_THIRD_PARTY_DIR}/glad/glad.c
    )
    target_include_directories(glad PUBLIC
        ${BAK3D_THIRD_PARTY_DIR}/glad
    )
    set_target_properties(glad PROPERTIES FOLDER "ThirdParty")
    message(STATUS "  [glad] Built as STATIC from glad.c")
else()
    message(STATUS "  [glad] NOT FOUND — skipping")
endif()

# glfw
if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/glfw/CMakeLists.txt")
    message(STATUS "  [glfw] Found CMakeLists.txt at ${BAK3D_THIRD_PARTY_DIR}/glfw")
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)
    add_subdirectory(${BAK3D_THIRD_PARTY_DIR}/glfw)
    set_target_properties(glfw PROPERTIES FOLDER "ThirdParty")
    message(STATUS "  [glfw] Added via add_subdirectory (static, no docs/tests/examples)")
else()
    message(STATUS "  [glfw] NOT FOUND — skipping")
endif()

# glm
if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/glm")
    message(STATUS "  [glm] Found at ${BAK3D_THIRD_PARTY_DIR}/glm")
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${BAK3D_THIRD_PARTY_DIR}/glm)
    message(STATUS "  [glm] Added as INTERFACE (header-only)")
else()
    message(STATUS "  [glm] NOT FOUND — skipping")
endif()

# ImGui + ImPlot
if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/imgui")
    message(STATUS "  [imgui] Found at ${BAK3D_THIRD_PARTY_DIR}/imgui")
    add_library(imgui STATIC)
    target_sources(imgui PRIVATE
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_demo.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_draw.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_tables.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_widgets.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_impl_glfw.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_impl_opengl3.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/implot.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/implot_demo.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/implot_items.cpp
    )
    target_include_directories(imgui PUBLIC
        ${BAK3D_THIRD_PARTY_DIR}/imgui/include
    )
    set_target_properties(imgui PROPERTIES FOLDER "ThirdParty")
    message(STATUS "  [imgui] Built as STATIC (core + demo + draw + tables + widgets)")
    message(STATUS "  [imgui] Backends: imgui_impl_glfw, imgui_impl_opengl3")
    message(STATUS "  [imgui] ImPlot: implot + implot_demo + implot_items")
else()
    message(STATUS "  [imgui] NOT FOUND — skipping")
endif()

# KHR
if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/KHR")
    message(STATUS "  [KHR] Found at ${BAK3D_THIRD_PARTY_DIR}/KHR")
    add_library(KHR INTERFACE)
    target_include_directories(KHR INTERFACE ${BAK3D_THIRD_PARTY_DIR})
    message(STATUS "  [KHR] Added as INTERFACE (header-only, include root: ${BAK3D_THIRD_PARTY_DIR})")
else()
    message(STATUS "  [KHR] NOT FOUND — skipping")
endif()

# ============================================================================
# Source Collection
# ============================================================================

message(STATUS "")
message(STATUS "--- Source Collection ---")

set(BAK3D_SOURCES ${BAK3D_SOURCE_DIR}/main.cpp)
message(STATUS "  Entry point: bak3d/main.cpp")

set(BAK3D_INCLUDE_DIRS
    ${BAK3D_SOURCE_DIR}
    ${BAK3D_SOURCE_DIR}/engine
    ${BAK3D_SOURCE_DIR}/editor
)

file(GLOB_RECURSE ENGINE_SOURCES
    "${BAK3D_SOURCE_DIR}/engine/*.cpp"
    "${BAK3D_SOURCE_DIR}/engine/*.h"
    "${BAK3D_SOURCE_DIR}/engine/*.hpp"
)
list(APPEND BAK3D_SOURCES ${ENGINE_SOURCES})
list(LENGTH ENGINE_SOURCES ENGINE_SOURCE_COUNT)
message(STATUS "  Engine sources  : ${ENGINE_SOURCE_COUNT} file(s) from bak3d/engine/")

file(GLOB_RECURSE EDITOR_SOURCES
    "${BAK3D_SOURCE_DIR}/editor/*.cpp"
    "${BAK3D_SOURCE_DIR}/editor/*.h"
    "${BAK3D_SOURCE_DIR}/editor/*.hpp"
)
list(APPEND BAK3D_SOURCES ${EDITOR_SOURCES})
list(LENGTH EDITOR_SOURCES EDITOR_SOURCE_COUNT)
message(STATUS "  Editor sources  : ${EDITOR_SOURCE_COUNT} file(s) from bak3d/editor/")

file(GLOB_RECURSE SHADER_FILES
    "${BAK3D_SHADERS_DIR}/*.vert"
    "${BAK3D_SHADERS_DIR}/*.frag"
    "${BAK3D_SHADERS_DIR}/*.glsl"
    "${BAK3D_SHADERS_DIR}/*.geom"
    "${BAK3D_SHADERS_DIR}/*.comp"
    "${BAK3D_SHADERS_DIR}/*.tesc"
    "${BAK3D_SHADERS_DIR}/*.tese"
)
list(LENGTH SHADER_FILES SHADER_FILE_COUNT)
message(STATUS "  Shader files    : ${SHADER_FILE_COUNT} file(s) from shaders/")

list(LENGTH BAK3D_SOURCES TOTAL_SOURCE_COUNT)
message(STATUS "  Total sources   : ${TOTAL_SOURCE_COUNT} file(s) (excluding shaders)")

# ============================================================================
# Main Executable
# ============================================================================

message(STATUS "")
message(STATUS "--- Executable Target ---")

add_executable(bak3d ${BAK3D_SOURCES} ${SHADER_FILES})
message(STATUS "  Target 'bak3d' created as executable")

target_include_directories(bak3d PRIVATE ${BAK3D_INCLUDE_DIRS})
message(STATUS "  Include dirs: bak3d/, bak3d/engine/, bak3d/editor/")

target_include_directories(bak3d PRIVATE ${BAK3D_THIRD_PARTY_DIR})
message(STATUS "  Third-party include root: ${BAK3D_THIRD_PARTY_DIR}")

# ============================================================================
# Linking
# ============================================================================

message(STATUS "")
message(STATUS "--- Linking ---")

# Initial unconditional link (keeps parity with original intent)
target_link_libraries(bak3d PRIVATE glm glad imgui assimp glfw)

# Guarded per-target links with logging
if(TARGET assimp)
    target_link_libraries(bak3d PRIVATE assimp)
    message(STATUS "  bak3d <- assimp (SHARED, imported)")
endif()
if(TARGET glad)
    target_link_libraries(bak3d PRIVATE glad)
    message(STATUS "  bak3d <- glad (STATIC)")
endif()
if(TARGET glfw)
    target_link_libraries(bak3d PRIVATE glfw)
    message(STATUS "  bak3d <- glfw (STATIC)")
endif()
if(TARGET glm)
    target_link_libraries(bak3d PRIVATE glm)
    message(STATUS "  bak3d <- glm (INTERFACE/header-only)")
endif()
if(TARGET imgui)
    target_link_libraries(bak3d PRIVATE imgui)
    target_link_libraries(imgui PUBLIC glfw)
    message(STATUS "  bak3d <- imgui (STATIC)")
    message(STATUS "  imgui <- glfw (PUBLIC, for backend headers)")
endif()
if(TARGET KHR)
    target_link_libraries(bak3d PRIVATE KHR)
    target_link_libraries(glad PUBLIC KHR)
    message(STATUS "  bak3d <- KHR (INTERFACE/header-only)")
    message(STATUS "  glad  <- KHR (PUBLIC, header-only)")
endif()

# ============================================================================
# Compile Definitions
# ============================================================================

message(STATUS "")
message(STATUS "--- Compile Definitions ---")

set(BAK3D_RUNTIME_DIR ${CMAKE_BINARY_DIR})
string(REPLACE "\\" "/" ASSETS_PATH_NORMALIZED "${BAK3D_RUNTIME_DIR}/assets")
string(REPLACE "\\" "/" SHADERS_PATH_NORMALIZED "${BAK3D_RUNTIME_DIR}/shaders")

target_compile_definitions(bak3d PRIVATE
    BAK3D_ASSETS_DIR="${ASSETS_PATH_NORMALIZED}"
    BAK3D_SHADERS_DIR="${SHADERS_PATH_NORMALIZED}"
)
message(STATUS "  BAK3D_ASSETS_DIR  = ${ASSETS_PATH_NORMALIZED}")
message(STATUS "  BAK3D_SHADERS_DIR = ${SHADERS_PATH_NORMALIZED}")

# ============================================================================
# Post-Build Steps
# ============================================================================

message(STATUS "")
message(STATUS "--- Post-Build Steps ---")

# ImGui config
set(IMGUI_CONFIG_SOURCE "${BAK3D_SOURCE_DIR}/imgui.ini")
set(IMGUI_CONFIG_DEST   "${BAK3D_CONFIG_OUTPUT_DIR}/imgui.ini")

if(EXISTS "${IMGUI_CONFIG_SOURCE}")
    add_custom_command(TARGET bak3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${IMGUI_CONFIG_SOURCE}" "${IMGUI_CONFIG_DEST}"
        COMMENT "[POST-BUILD] Copying imgui.ini -> config/"
    )
    add_custom_command(TARGET bak3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${IMGUI_CONFIG_SOURCE}" "$<TARGET_FILE_DIR:bak3d>/imgui.ini"
        COMMENT "[POST-BUILD] Copying imgui.ini -> output bin dir"
    )
    message(STATUS "  imgui.ini  : will copy to config/ and output bin dir")
else()
    message(STATUS "  imgui.ini  : NOT FOUND at ${IMGUI_CONFIG_SOURCE} — skipping copy")
endif()

# assimp DLL
add_custom_command(TARGET bak3d POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${BAK3D_THIRD_PARTY_DIR}/assimp/lib/assimp-vc143-mt.dll"
        "$<TARGET_FILE_DIR:bak3d>"
    COMMENT "[POST-BUILD] Copying assimp-vc143-mt.dll -> output bin dir"
)
message(STATUS "  assimp DLL : will copy assimp-vc143-mt.dll -> output bin dir")

# Shaders
if(EXISTS "${BAK3D_SHADERS_DIR}")
    add_custom_command(TARGET bak3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${BAK3D_SHADERS_DIR}" "${CMAKE_BINARY_DIR}/shaders"
        COMMENT "[POST-BUILD] Copying shaders/ -> build/shaders/"
    )
    message(STATUS "  shaders    : will sync shaders/ -> build/shaders/")
endif()

# Assets
if(EXISTS "${BAK3D_ASSETS_DIR}")
    add_custom_command(TARGET bak3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${BAK3D_ASSETS_DIR}" "${CMAKE_BINARY_DIR}/assets"
        COMMENT "[POST-BUILD] Copying assets/ -> build/assets/"
    )
    message(STATUS "  assets     : will sync assets/ -> build/assets/")
endif()

# ============================================================================
# MSVC Configuration
# ============================================================================

if(MSVC)
    message(STATUS "")
    message(STATUS "--- MSVC Configuration ---")

    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT bak3d)
    message(STATUS "  Startup project: bak3d")

    source_group("Source Files" FILES ${BAK3D_SOURCE_DIR}/main.cpp)
    source_group("Engine"       FILES ${ENGINE_SOURCES})
    source_group("Editor"       FILES ${EDITOR_SOURCES})
    source_group("Shaders"      FILES ${SHADER_FILES})

    target_compile_options(bak3d PRIVATE
        /W4           # Warning level 4
        /FS           # Synchronous PDB writes
        /permissive-  # Standards conformance
        /std:c++latest
    )
    message(STATUS "  Flags: /W4 /FS /permissive- /std:c++latest")
endif()

# ============================================================================
# Build Summary
# ============================================================================

message(STATUS "")
message(STATUS "=== bak3d Build Configuration ===")
message(STATUS "  Project root  : ${BAK3D_ROOT_DIR}")
message(STATUS "  Source dir    : ${BAK3D_SOURCE_DIR}")
message(STATUS "  Third-party   : ${BAK3D_THIRD_PARTY_DIR}")
message(STATUS "  Assets dir    : ${BAK3D_ASSETS_DIR}")
message(STATUS "  Shaders dir   : ${BAK3D_SHADERS_DIR}")
message(STATUS "  Build dir     : ${CMAKE_BINARY_DIR}")
message(STATUS "  Config output : ${BAK3D_CONFIG_OUTPUT_DIR}")
message(STATUS "  C++ standard  : C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Sources       : ${TOTAL_SOURCE_COUNT} (engine: ${ENGINE_SOURCE_COUNT}, editor: ${EDITOR_SOURCE_COUNT})")
message(STATUS "  Shaders       : ${SHADER_FILE_COUNT}")
message(STATUS "==================================")
message(STATUS "")
