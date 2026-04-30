# sync_shaders.cmake
# Usage: cmake -P sync_shaders.cmake
# Copies source shaders into the build folder without triggering a C++ rebuild.

cmake_minimum_required(VERSION 3.20)

# Resolve paths relative to this script's location (repository root)
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
set(SOURCE_SHADERS "${REPO_ROOT}/shaders")
set(BUILD_DIR      "${REPO_ROOT}/build")
set(DEST_SHADERS   "${BUILD_DIR}/shaders")

if(NOT EXISTS "${SOURCE_SHADERS}")
    message(FATAL_ERROR "Shaders source directory not found: ${SOURCE_SHADERS}")
endif()

if(NOT EXISTS "${BUILD_DIR}")
    message(FATAL_ERROR "Build directory not found: ${BUILD_DIR} — run cmake first.")
endif()

message(STATUS "Syncing shaders...")
message(STATUS "  From: ${SOURCE_SHADERS}")
message(STATUS "  To:   ${DEST_SHADERS}")

if(EXISTS "${DEST_SHADERS}")
    message(STATUS "  Removing existing destination folder...")
    file(REMOVE_RECURSE "${DEST_SHADERS}")
endif()

file(COPY "${SOURCE_SHADERS}/." DESTINATION "${DEST_SHADERS}")

message(STATUS "Syncing complete. Shaders are up to date in build folder.")