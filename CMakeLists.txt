cmake_minimum_required(VERSION 3.12)

project(BallancePlayer
        VERSION 0.3.2.0
        DESCRIPTION "The brand-new player for Ballance"
        HOMEPAGE_URL https://github.com/doyaGu/BallancePlayer
        )

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED NO)
set(CMAKE_CXX_EXTENSIONS YES)

# Add path for custom modules
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as no build type was specified")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type (Debug/Release)" FORCE)
endif ()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
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

set(PLAYER_VERSION ${CMAKE_PROJECT_VERSION})

set(PLAYER_SCREEN_WIDTH 640 CACHE STRING "Player screen width default value" FORCE)
set(PLAYER_SCREEN_HEIGHT 480 CACHE STRING "Player screen height default value" FORCE)
set(PLAYER_SCREEN_BPP 32 CACHE STRING "Player screen bpp default value" FORCE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Player.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/Player.rc)

set(PLAYER_HEADERS
        GamePlayer.h
        GameConfig.h
        Window.h
        Splash.h
        ScriptUtils.h
        CmdlineParser.h
        Logger.h
        Utils.h
        )

set(PLAYER_SOURCES
        Player.cpp
        GamePlayer.cpp
        GameConfig.cpp
        Hotfix.cpp
        Splash.cpp
        CmdlineParser.cpp
        Logger.cpp
        Utils.cpp
        Player.rc
        )

add_executable(${PLAYER_NAME} ${PLAYER_HEADERS} ${PLAYER_SOURCES})

set_target_properties(${PLAYER_NAME} PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
        WIN32_EXECUTABLE TRUE
        )

target_link_libraries(${PLAYER_NAME} PRIVATE CK2 VxMath)

install(TARGETS ${PLAYER_NAME} RUNTIME DESTINATION Bin)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Kakuty")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_GENERATOR "ZIP")
include(CPack)