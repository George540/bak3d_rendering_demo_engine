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

# Define all third-party libraries here with their subdirectories
# This allows easy identification and management of external dependencies

# Example structure (modify based on your actual third_party contents):
# If you have libraries like glfw, glew, glm, etc., add them like this:

if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/assimp")
	add_library(assimp SHARED IMPORTED)

	set_target_properties(assimp PROPERTIES
        IMPORTED_IMPLIB "${BAK3D_THIRD_PARTY_DIR}/assimp/lib/assimp-vc143-mt.lib"
        IMPORTED_LOCATION "${BAK3D_THIRD_PARTY_DIR}/assimp/lib/assimp-vc143-mt.dll"
        INTERFACE_INCLUDE_DIRECTORIES "${BAK3D_THIRD_PARTY_DIR}/assimp/include"
    )

    set_target_properties(assimp PROPERTIES FOLDER "ThirdParty")
endif()

if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/glad")
    add_library(glad STATIC)
	target_sources(glad PRIVATE
        ${BAK3D_THIRD_PARTY_DIR}/glad/glad.c
    )
    target_include_directories(glad PUBLIC
        ${BAK3D_THIRD_PARTY_DIR}/glad
    )
    set_target_properties(glad PROPERTIES FOLDER "ThirdParty")
endif()

if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/glfw/CMakeLists.txt")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    add_subdirectory(${BAK3D_THIRD_PARTY_DIR}/glfw)

    set_target_properties(glfw PROPERTIES FOLDER "ThirdParty")

endif()

if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/glm")
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${BAK3D_THIRD_PARTY_DIR}/glm)
endif()

# ImGui (if present)
if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/imgui")
    add_library(imgui STATIC)
    target_sources(imgui PRIVATE
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_demo.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_draw.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_tables.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_widgets.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_impl_glfw.cpp
        ${BAK3D_THIRD_PARTY_DIR}/imgui/src/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PUBLIC
        ${BAK3D_THIRD_PARTY_DIR}/imgui/include
    )
    set_target_properties(imgui PROPERTIES FOLDER "ThirdParty")
endif()

if(EXISTS "${BAK3D_THIRD_PARTY_DIR}/KHR")
    add_library(KHR INTERFACE)
	target_include_directories(KHR INTERFACE
		${BAK3D_THIRD_PARTY_DIR}
	)
endif()

# Add more third-party libraries as needed...
# For each library, follow the pattern:
# 1. Check if it exists
# 2. Add it as a subdirectory or create an interface library
# 3. Set FOLDER property for organization in Visual Studio

# ============================================================================
# Main Project
# ============================================================================

# List of source files from bak3d subdirectories
set(BAK3D_SOURCES
    ${BAK3D_SOURCE_DIR}/main.cpp
)

# Include subdirectories
set(BAK3D_INCLUDE_DIRS
    ${BAK3D_SOURCE_DIR}
    ${BAK3D_SOURCE_DIR}/engine
    ${BAK3D_SOURCE_DIR}/editor
)

# Glob sources from engine subdirectory
file(GLOB_RECURSE ENGINE_SOURCES "${BAK3D_SOURCE_DIR}/engine/*.cpp" "${BAK3D_SOURCE_DIR}/engine/*.h" "${BAK3D_SOURCE_DIR}/engine/*.hpp")
list(APPEND BAK3D_SOURCES ${ENGINE_SOURCES})

# Glob sources from editor subdirectory
file(GLOB_RECURSE EDITOR_SOURCES "${BAK3D_SOURCE_DIR}/editor/*.cpp" "${BAK3D_SOURCE_DIR}/editor/*.h" "${BAK3D_SOURCE_DIR}/editor/*.hpp")
list(APPEND BAK3D_SOURCES ${EDITOR_SOURCES})

# Create the main executable
add_executable(bak3d ${BAK3D_SOURCES})

# Set target include directories
target_include_directories(bak3d PRIVATE ${BAK3D_INCLUDE_DIRS})

# ============================================================================
# Third-Party Include Directories
# ============================================================================

# Add include directories for all third-party libraries
target_include_directories(bak3d PRIVATE ${BAK3D_THIRD_PARTY_DIR})

# If using specific third-party libraries, link them here
# Example:
target_link_libraries(bak3d PRIVATE
    glm
    glad
    imgui
    assimp
    glfw
)

# Uncomment and modify based on which third-party libraries you're actually using:
if(TARGET assimp)
    target_link_libraries(bak3d PRIVATE assimp)
endif()
if(TARGET glad)
    target_link_libraries(bak3d PRIVATE glad)
endif()
if(TARGET glfw)
    target_link_libraries(bak3d PRIVATE glfw)
endif()
if(TARGET glm)
    target_link_libraries(bak3d PRIVATE glm)
endif()
if(TARGET imgui)
    target_link_libraries(bak3d PRIVATE imgui)
	target_link_libraries(imgui PUBLIC glfw)
endif()
if(TARGET KHR)
    target_link_libraries(bak3d PRIVATE KHR)
	target_link_libraries(glad PUBLIC KHR)
endif()

# ============================================================================
# Assets and Resources Configuration
# ============================================================================

# Define asset paths as preprocessor definitions so source code can access them
# Convert Windows paths to forward slashes for consistency
string(REPLACE "\\" "/" ASSETS_PATH_NORMALIZED "${BAK3D_ASSETS_DIR}")
string(REPLACE "\\" "/" SHADERS_PATH_NORMALIZED "${BAK3D_SHADERS_DIR}")

target_compile_definitions(bak3d PRIVATE
    BAK3D_ASSETS_DIR="${ASSETS_PATH_NORMALIZED}"
    BAK3D_SHADERS_DIR="${SHADERS_PATH_NORMALIZED}"
)

# ============================================================================
# ImGui Configuration File Setup
# ============================================================================

# Look for ImGui configuration file in the bak3d project directory
set(IMGUI_CONFIG_SOURCE "${BAK3D_SOURCE_DIR}/imgui.ini")
set(IMGUI_CONFIG_DEST "${BAK3D_CONFIG_OUTPUT_DIR}/imgui.ini")

# Copy ImGui config file to build folder if it exists
if(EXISTS "${IMGUI_CONFIG_SOURCE}")
    add_custom_command(TARGET bak3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${IMGUI_CONFIG_SOURCE}"
        "${IMGUI_CONFIG_DEST}"
        COMMENT "Copying ImGui configuration to build folder"
    )
endif()

# Also copy it to the output directory
add_custom_command(TARGET bak3d POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${IMGUI_CONFIG_SOURCE}"
    "$<TARGET_FILE_DIR:bak3d>/imgui.ini"
    COMMENT "Copying ImGui configuration to output directory"
)

add_custom_command(TARGET bak3d POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${BAK3D_THIRD_PARTY_DIR}/assimp/lib/assimp-vc143-mt.dll"
	"$<TARGET_FILE_DIR:bak3d>"
	COMMENT "Copying assimp.dll"
)

# ============================================================================
# Shader Files Setup
# ============================================================================

# Copy shader directory to build output for easy access
if(EXISTS "${BAK3D_SHADERS_DIR}")
    add_custom_command(TARGET bak3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${BAK3D_SHADERS_DIR}"
        "$<TARGET_FILE_DIR:bak3d>/shaders"
        COMMENT "Copying shaders to output directory"
    )
endif()

# ============================================================================
# Assets Symbolic Link/Copy
# ============================================================================

# Copy assets directory to build output
if(EXISTS "${BAK3D_ASSETS_DIR}")
    add_custom_command(TARGET bak3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${BAK3D_ASSETS_DIR}"
        "$<TARGET_FILE_DIR:bak3d>/assets"
        COMMENT "Copying assets to output directory"
    )
endif()

# ============================================================================
# Visual Studio Specific Configuration
# ============================================================================

if(MSVC)
    # Set startup project (optional - uncomment if needed)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT bak3d)
    
    # Group source files by folder in Visual Studio
    source_group("Source Files" FILES ${BAK3D_SOURCE_DIR}/main.cpp)
    source_group("Engine" FILES ${ENGINE_SOURCES})
    source_group("Editor" FILES ${EDITOR_SOURCES})
    
    # Set compiler flags for MSVC
    target_compile_options(bak3d PRIVATE /W4)
endif()

# ============================================================================
# Installation (Optional)
# ============================================================================

# You can optionally define installation rules
# install(TARGETS bak3d RUNTIME DESTINATION bin)
# install(DIRECTORY ${BAK3D_ASSETS_DIR} DESTINATION .)
# install(DIRECTORY ${BAK3D_SHADERS_DIR} DESTINATION .)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "=== bak3d Build Configuration ===")
message(STATUS "Project Root: ${BAK3D_ROOT_DIR}")
message(STATUS "Source Dir: ${BAK3D_SOURCE_DIR}")
message(STATUS "Third Party Dir: ${BAK3D_THIRD_PARTY_DIR}")
message(STATUS "Assets Dir: ${BAK3D_ASSETS_DIR}")
message(STATUS "Shaders Dir: ${BAK3D_SHADERS_DIR}")
message(STATUS "Build Dir: ${CMAKE_BINARY_DIR}")
message(STATUS "Config Output Dir: ${BAK3D_CONFIG_OUTPUT_DIR}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "==================================")
message(STATUS "")
