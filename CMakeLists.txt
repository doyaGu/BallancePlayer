cmake_minimum_required(VERSION 3.12)

if (DEFINED PLAYER_VERSION_OVERRIDE)
    set(BALLANCE_PLAYER_VERSION "${PLAYER_VERSION_OVERRIDE}")
else ()
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" BALLANCE_PLAYER_VERSION)
    string(STRIP "${BALLANCE_PLAYER_VERSION}" BALLANCE_PLAYER_VERSION)
endif ()
if (NOT BALLANCE_PLAYER_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$")
    message(FATAL_ERROR "VERSION must contain a numeric four-part version, got '${BALLANCE_PLAYER_VERSION}'")
endif ()

project(BallancePlayer
        VERSION ${BALLANCE_PLAYER_VERSION}
        DESCRIPTION "The brand-new player for Ballance"
        HOMEPAGE_URL https://github.com/doyaGu/BallancePlayer
        LANGUAGES C CXX
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

# When built as part of Ballanced, inherit the top-level C++ standard.
# When built standalone, default to C++17.
if (PLAYER_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED YES)
endif ()

include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BALLANCE_BUILD_STATIC "Build runtime modules statically into Player" OFF)

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

# Dependencies (CK2/VxMath)
#
# When BallancePlayer is built as part of the Ballanced superproject, CK2 and VxMath
# are provided as regular CMake targets, so no external "Virtools SDK" is required.
#
# When built standalone, prefer the bundled sibling directories (../CK2, ../VxMath)
# if present (this workspace layout), and fall back to the legacy external SDK flow.

if (NOT TARGET CK2 OR NOT TARGET VxMath)
    if (NOT TARGET VxMath AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../VxMath/CMakeLists.txt")
        if (BALLANCE_BUILD_STATIC)
            set(VXMATH_BUILD_SHARED OFF CACHE BOOL "" FORCE)
            set(VXMATH_BUILD_STATIC ON CACHE BOOL "" FORCE)
        endif ()
        set(VXMATH_INSTALL ON CACHE BOOL "Generate install target" FORCE)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../VxMath" "${CMAKE_CURRENT_BINARY_DIR}/VxMath")
    endif ()

    if (NOT TARGET CK2 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../CK2/CMakeLists.txt")
        if (BALLANCE_BUILD_STATIC)
            set(CK2_BUILD_SHARED OFF CACHE BOOL "" FORCE)
            set(CK2_BUILD_STATIC ON CACHE BOOL "" FORCE)
        endif ()
        set(CK2_INSTALL ON CACHE BOOL "Generate install target" FORCE)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../CK2" "${CMAKE_CURRENT_BINARY_DIR}/CK2")
    endif ()
endif ()

if (NOT TARGET CK2 OR NOT TARGET VxMath)
    # Legacy external SDK flow (kept for compatibility with the standalone BallancePlayer repo)
    set(VIRTOOLS_SDK_PATH "" CACHE PATH "Path to the Virtools SDK")
    option(VIRTOOLS_SDK_FETCH_FROM_GIT "Fetch Virtools SDK from git if not found" OFF)
    set(VIRTOOLS_SDK_FETCH_FROM_GIT_PATH "" CACHE FILEPATH "Location to download SDK")

    if (NOT VIRTOOLS_SDK_PATH)
        if (DEFINED ENV{VIRTOOLS_SDK_PATH})
            set(VIRTOOLS_SDK_PATH "$ENV{VIRTOOLS_SDK_PATH}" CACHE PATH "Path to the Virtools SDK" FORCE)
        elseif (DEFINED ENV{VIRTOOLS_SDK})
            set(VIRTOOLS_SDK_PATH "$ENV{VIRTOOLS_SDK}" CACHE PATH "Path to the Virtools SDK" FORCE)
        endif ()
    endif ()

    if (NOT VIRTOOLS_SDK_PATH)
        if (VIRTOOLS_SDK_FETCH_FROM_GIT)
            include(FetchContent)
            set(FETCHCONTENT_BASE_DIR_SAVE ${FETCHCONTENT_BASE_DIR})
            if (VIRTOOLS_SDK_FETCH_FROM_GIT_PATH)
                get_filename_component(FETCHCONTENT_BASE_DIR "${VIRTOOLS_SDK_FETCH_FROM_GIT_PATH}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
            endif ()
            FetchContent_Declare(
                    virtools_sdk
                    GIT_REPOSITORY https://github.com/doyaGu/Virtools-SDK-2.1.git
                    GIT_TAG main
            )
            message(STATUS "[BallancePlayer] Downloading Virtools SDK")
            FetchContent_MakeAvailable(virtools_sdk)
            set(VIRTOOLS_SDK_PATH "${virtools_sdk_SOURCE_DIR}" CACHE PATH "Path to the Virtools SDK" FORCE)
            set(FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR_SAVE})
        else ()
            message(FATAL_ERROR "CK2/VxMath not found. Provide the bundled sibling directories or set VIRTOOLS_SDK_PATH / enable VIRTOOLS_SDK_FETCH_FROM_GIT.")
        endif ()
    endif ()

    find_path(VIRTOOLS_SDK_INCLUDE_DIR CKAll.h
            HINTS $ENV{VIRTOOLS_SDK_PATH} ${VIRTOOLS_SDK_PATH}
            PATH_SUFFIXES Include Includes include includes
    )
    if (NOT VIRTOOLS_SDK_INCLUDE_DIR)
        message(FATAL_ERROR "Virtools SDK headers not found. Expected CKAll.h under ${VIRTOOLS_SDK_PATH}.")
    endif ()

    foreach (_lib IN ITEMS CK2 VxMath)
        find_library(VIRTOOLS_SDK_${_lib}_LIB NAMES ${_lib}
                HINTS $ENV{VIRTOOLS_SDK_PATH} ${VIRTOOLS_SDK_PATH}
                PATH_SUFFIXES Lib lib
        )
        if (NOT VIRTOOLS_SDK_${_lib}_LIB)
            message(FATAL_ERROR "Virtools SDK library '${_lib}' not found under ${VIRTOOLS_SDK_PATH}.")
        endif ()

        if (NOT TARGET ${_lib})
            add_library(${_lib} UNKNOWN IMPORTED)
            set_target_properties(${_lib} PROPERTIES
                    IMPORTED_LOCATION "${VIRTOOLS_SDK_${_lib}_LIB}"
                    INTERFACE_INCLUDE_DIRECTORIES "${VIRTOOLS_SDK_INCLUDE_DIR}"
            )
        endif ()
    endforeach ()
endif ()

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

if (BALLANCE_BUILD_STATIC)
    if (NOT TARGET CK2_3DStatic AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../RenderEngine/CMakeLists.txt")
        set(CKRE_BUILD_SHARED OFF CACHE BOOL "" FORCE)
        set(CKRE_BUILD_STATIC ON CACHE BOOL "" FORCE)
        set(CKRE_INSTALL ON CACHE BOOL "Generate install target" FORCE)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../RenderEngine" "${CMAKE_CURRENT_BINARY_DIR}/RenderEngine")
    endif ()

    if (NOT TARGET Dx8InputManagerStatic AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Managers/CMakeLists.txt")
        set(DX8INPUT_BUILD_SHARED OFF CACHE BOOL "" FORCE)
        set(DX8INPUT_BUILD_STATIC ON CACHE BOOL "" FORCE)
        set(DX8INPUT_INSTALL ON CACHE BOOL "Generate install target" FORCE)
        set(DX8SOUND_BUILD_SHARED OFF CACHE BOOL "" FORCE)
        set(DX8SOUND_BUILD_STATIC ON CACHE BOOL "" FORCE)
        set(DX8SOUND_INSTALL ON CACHE BOOL "Generate install target" FORCE)
        set(CKPARAMOP_BUILD_SHARED OFF CACHE BOOL "" FORCE)
        set(CKPARAMOP_BUILD_STATIC ON CACHE BOOL "" FORCE)
        set(CKPARAMOP_INSTALL ON CACHE BOOL "Generate install target" FORCE)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Managers" "${CMAKE_CURRENT_BINARY_DIR}/Managers")
    endif ()

    if (NOT TARGET AVIReaderStatic AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Plugins/CMakeLists.txt")
        set(CKPLUGINS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
        set(CKPLUGINS_BUILD_STATIC ON CACHE BOOL "" FORCE)
        set(CKPLUGINS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(CKPLUGINS_INSTALL ON CACHE BOOL "Generate install target" FORCE)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Plugins" "${CMAKE_CURRENT_BINARY_DIR}/Plugins")
    endif ()

    if (NOT TARGET physics_RTStatic AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../BuildingBlocks/CMakeLists.txt")
        set(CKBB_BUILD_SHARED OFF CACHE BOOL "" FORCE)
        set(CKBB_BUILD_STATIC ON CACHE BOOL "" FORCE)
        set(CKBB_INSTALL ON CACHE BOOL "Generate install target" FORCE)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../BuildingBlocks" "${CMAKE_CURRENT_BINARY_DIR}/BuildingBlocks")
    endif ()
endif ()

set(PLAYER_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include" CACHE INTERNAL "")
set(PLAYER_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src" CACHE INTERNAL "")

add_subdirectory(src)

if (PLAYER_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(tests)
endif ()
