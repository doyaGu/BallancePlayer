cmake_minimum_required(VERSION 3.12)

project(BallancePlayer
        VERSION 0.2.1.0
        DESCRIPTION "The brand-new player for Ballance"
        HOMEPAGE_URL https://github.com/doyaGu/BallancePlayer
        )

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED NO)
set(CMAKE_CXX_EXTENSIONS YES)

# Use folders to organize targets in an IDE
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

if (NOT WIN32)
    message(FATAL_ERROR "Only Windows is supported.")
endif ()

# Use relative paths
if (WIN32)
    set(CMAKE_USE_RELATIVE_PATHS TRUE)
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
endif ()

if (MSVC)
    if (NOT CMAKE_GENERATOR_PLATFORM)
        set(CMAKE_GENERATOR_PLATFORM Win32)
    endif ()

    string(TOLOWER ${CMAKE_GENERATOR_PLATFORM} TARGET_PLATFORM)
    if (NOT ("${TARGET_PLATFORM}" STREQUAL "win32"))
        message(FATAL_ERROR "You can only build 32-bit executable since only 32-bit SDK is provided.")
    endif ()
endif ()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as no build type was specified")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type (Debug/Release)" FORCE)
endif ()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH "The root directory of the installation" FORCE)
    message(STATUS "Setting default install directory to ${CMAKE_INSTALL_PREFIX} as no install directory was specified")
endif ()

# Generate a CompilationDatabase (compile_commands.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if (NOT BUILDING_BLOCKS_DIR)
    set(BUILDING_BLOCKS_DIR "BuildingBlocks" CACHE PATH "The directory of the building blocks" FORCE)
    message(STATUS "Setting default building blocks directory to ${CMAKE_INSTALL_PREFIX}/${BUILDING_BLOCKS_DIR}")
endif ()

set(VIRTOOLS_SDK_PATH "${VIRTOOLS_SDK_PATH}" CACHE PATH "Path to the Virtools SDK")
set(VIRTOOLS_SDK_FETCH_FROM_GIT "${VIRTOOLS_SDK_FETCH_FROM_GIT}" CACHE BOOL "Set to ON to fetch copy of SDK from git if not otherwise locatable")
set(VIRTOOLS_SDK_FETCH_FROM_GIT_PATH "${VIRTOOLS_SDK_FETCH_FROM_GIT_PATH}" CACHE FILEPATH "location to download SDK")

if (NOT VIRTOOLS_SDK_PATH)
    if (NOT VIRTOOLS_SDK_FETCH_FROM_GIT)
        message(FATAL_ERROR "Virtools SDK location was not specified. Please set VIRTOOLS_SDK_PATH or set VIRTOOLS_SDK_FETCH_FROM_GIT to on to fetch from git.")
    else ()
        include(FetchContent)
        set(FETCHCONTENT_BASE_DIR_SAVE ${FETCHCONTENT_BASE_DIR})
        if (VIRTOOLS_SDK_FETCH_FROM_GIT_PATH)
            get_filename_component(FETCHCONTENT_BASE_DIR "${VIRTOOLS_SDK_FETCH_FROM_GIT_PATH}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
        endif ()
        FetchContent_Declare(
                Virtools_SDK
                GIT_REPOSITORY https://github.com/doyaGu/Virtools-SDK-2.1.git
                GIT_TAG main
        )
        if (NOT Virtools_SDK)
            message("Downloading Virtools SDK")
            FetchContent_Populate(Virtools_SDK)
            set(VIRTOOLS_SDK_PATH "${virtools_sdk_SOURCE_DIR}" CACHE PATH "Path to the Virtools SDK" FORCE)
        endif ()
        set(FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR_SAVE})
    endif ()
endif ()

find_package(VirtoolsSDK REQUIRED HINTS ${VIRTOOLS_SDK_PATH})

if (NOT DEFINED PLAYER_NAME)
    set(PLAYER_NAME "Player")
endif ()

set(PLAYER_VERSION ${CMAKE_PROJECT_VERSION})

set(PLAYER_SCREEN_WIDTH 640 CACHE STRING "Player screen width default value" FORCE)
set(PLAYER_SCREEN_HEIGHT 480 CACHE STRING "Player screen height default value" FORCE)
set(PLAYER_SCREEN_BPP 32 CACHE STRING "Player screen bpp default value" FORCE)

set(PLAYER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PLAYER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

configure_file(${PLAYER_SOURCE_DIR}/config.h.in ${PLAYER_SOURCE_DIR}/config.h)
configure_file(${PLAYER_SOURCE_DIR}/Player.rc.in ${PLAYER_SOURCE_DIR}/Player.rc)

set(PLAYER_HEADERS
        ${PLAYER_INCLUDE_DIR}/Game.h
        ${PLAYER_INCLUDE_DIR}/GamePlayer.h
        ${PLAYER_INCLUDE_DIR}/GameConfig.h
        ${PLAYER_INCLUDE_DIR}/NeMoContext.h
        ${PLAYER_INCLUDE_DIR}/WinContext.h
        ${PLAYER_INCLUDE_DIR}/Splash.h
        ${PLAYER_INCLUDE_DIR}/ScriptUtils.h
        ${PLAYER_INCLUDE_DIR}/CmdlineParser.h
        ${PLAYER_INCLUDE_DIR}/Logger.h
        ${PLAYER_INCLUDE_DIR}/Utils.h
        )

set(PLAYER_SOURCES
        ${PLAYER_SOURCE_DIR}/Player.cpp
        ${PLAYER_SOURCE_DIR}/Game.cpp
        ${PLAYER_SOURCE_DIR}/GamePlayer.cpp
        ${PLAYER_SOURCE_DIR}/GameConfig.cpp
        ${PLAYER_SOURCE_DIR}/NeMoContext.cpp
        ${PLAYER_SOURCE_DIR}/WinContext.cpp
        ${PLAYER_SOURCE_DIR}/Splash.cpp
        ${PLAYER_SOURCE_DIR}/CmdlineParser.cpp
        ${PLAYER_SOURCE_DIR}/Logger.cpp
        ${PLAYER_SOURCE_DIR}/Utils.cpp
        ${PLAYER_SOURCE_DIR}/Player.rc
        )

add_executable(${PLAYER_NAME} ${PLAYER_HEADERS} ${PLAYER_SOURCES})

set_target_properties(${PLAYER_NAME} PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
        VERSION ${PLAYER_VERSION}
        WIN32_EXECUTABLE TRUE
        )

target_include_directories(${PLAYER_NAME} PRIVATE ${PLAYER_INCLUDE_DIR})

target_link_libraries(${PLAYER_NAME} PRIVATE CK2 VxMath delayimp.lib)

target_link_options(${PLAYER_NAME} PRIVATE
        /DELAYLOAD:CK2.dll
        /DELAYLOAD:VxMath.dll
        )

# Disable msvc unsafe warnings
target_compile_definitions(${PLAYER_NAME} PRIVATE
        $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        $<$<C_COMPILER_ID:MSVC>:_CRT_NONSTDC_NO_WARNINGS>
        )

install(TARGETS ${PLAYER_NAME} RUNTIME DESTINATION Bin)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Kakuty")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_GENERATOR "ZIP")
include(CPack)