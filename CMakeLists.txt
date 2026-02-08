cmake_minimum_required(VERSION 3.12)

project(BallancePlayer
        VERSION 0.3.8.0
        DESCRIPTION "The brand-new player for Ballance"
        HOMEPAGE_URL https://github.com/doyaGu/BallancePlayer
        )

# Determine if BallancePlayer is built as a subproject (using add_subdirectory) or if it is the main project.
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
    set(PLAYER_IS_TOP_LEVEL ${PROJECT_IS_TOP_LEVEL})
else ()
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(PLAYER_IS_TOP_LEVEL ON)
    else ()
        set(PLAYER_IS_TOP_LEVEL OFF)
    endif ()
endif ()

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED NO)
set(CMAKE_CXX_EXTENSIONS YES)

# Add path for custom modules
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Use folders to organize targets in an IDE (only when top-level)
if (PLAYER_IS_TOP_LEVEL)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
endif ()

if (NOT WIN32)
    message(FATAL_ERROR "Only Windows is supported.")
endif ()

# Use relative paths
if (WIN32 AND PLAYER_IS_TOP_LEVEL)
    set(CMAKE_USE_RELATIVE_PATHS TRUE)
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
endif ()

if (PLAYER_IS_TOP_LEVEL)
    if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Setting build type to 'Release' as no build type was specified")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type (Debug/Release)" FORCE)
    endif ()
endif ()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND PLAYER_IS_TOP_LEVEL)
    # Only set a source-root install prefix when building BallancePlayer as a top-level project.
    # When used as a subdirectory, the parent should control the install prefix.
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH "The root directory of the installation" FORCE)
    message(STATUS "Setting default install directory to ${CMAKE_INSTALL_PREFIX} as no install directory was specified")
endif ()

# Disable msvc unsafe warnings
add_compile_definitions(
        $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        $<$<C_COMPILER_ID:MSVC>:_CRT_NONSTDC_NO_WARNINGS>
)

find_package(VirtoolsSDK REQUIRED)

if (NOT DEFINED PLAYER_NAME)
    set(PLAYER_NAME "Player")
endif ()

set(PLAYER_VERSION ${PROJECT_VERSION})

if (NOT DEFINED PLAYER_SCREEN_WIDTH)
    set(PLAYER_SCREEN_WIDTH 640 CACHE STRING "Player screen width default value")
endif ()
if (NOT DEFINED PLAYER_SCREEN_HEIGHT)
    set(PLAYER_SCREEN_HEIGHT 480 CACHE STRING "Player screen height default value")
endif ()
if (NOT DEFINED PLAYER_SCREEN_BPP)
    set(PLAYER_SCREEN_BPP 32 CACHE STRING "Player screen bpp default value")
endif ()

set(PLAYER_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include" CACHE INTERNAL "")
set(PLAYER_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src" CACHE INTERNAL "")

add_subdirectory(src)