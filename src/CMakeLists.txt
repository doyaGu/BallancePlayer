set(_player_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${_player_generated_dir}/config.h"
        @ONLY
)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/PlayerVersion.rc.in"
        "${_player_generated_dir}/PlayerVersion.generated.rc"
        @ONLY
)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/ConfigToolVersion.rc.in"
        "${_player_generated_dir}/ConfigToolVersion.generated.rc"
        @ONLY
)

set(_player_resource_file "${CMAKE_CURRENT_SOURCE_DIR}/Player.rc")
set_source_files_properties("${_player_resource_file}" PROPERTIES
        COMPILE_DEFINITIONS "PLAYER_ICON_PATH=\"${PLAYER_SOURCE_DIR}/Player.ico\";PLAYER_USE_GENERATED_VERSION_RC=1"
)

set(_config_tool_resource_file "${CMAKE_CURRENT_SOURCE_DIR}/ConfigTool.rc")
set_source_files_properties("${_config_tool_resource_file}" PROPERTIES
        COMPILE_DEFINITIONS "CONFIG_TOOL_ICON_PATH=\"${PLAYER_SOURCE_DIR}/Player.ico\";CONFIG_TOOL_USE_GENERATED_VERSION_RC=1"
)

set(PLAYER_HEADERS
        StaticPlugins.h
        GamePlayer.h
        GameConfig.h
        PlayerOptions.h
        Splash.h
        ScriptUtils.h
        CmdlineParser.h
        LockGuard.h
        Logger.h
        Utils.h
)

set(PLAYER_SOURCES
        Player.cpp
        GamePlayer.cpp
        GameConfig.cpp
        PlayerOptions.cpp
        Hotfix.cpp
        Splash.cpp
        CmdlineParser.cpp
        Logger.cpp
        Utils.cpp
        "${_player_resource_file}"
)

if (BALLANCE_BUILD_STATIC)
    list(APPEND PLAYER_SOURCES StaticPlugins.cpp)
endif ()

add_executable(${PLAYER_NAME} ${PLAYER_HEADERS} ${PLAYER_SOURCES})

set_target_properties(${PLAYER_NAME} PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
        WIN32_EXECUTABLE TRUE
)

set(_player_ck2_dep CK2)
set(_player_vxmath_dep VxMath)
if (BALLANCE_BUILD_STATIC)
    if (TARGET CK2Static)
        set(_player_ck2_dep CK2Static)
    endif ()
    if (TARGET VxMathStatic)
        set(_player_vxmath_dep VxMathStatic)
    endif ()
endif ()
target_link_libraries(${PLAYER_NAME} PRIVATE ${_player_ck2_dep} ${_player_vxmath_dep})

if (BALLANCE_BUILD_STATIC)
    target_compile_definitions(${PLAYER_NAME} PRIVATE BALLANCE_STATIC_MODULES)

    set(_ballance_static_module_targets
            CK2_3DStatic
            Dx8InputManagerStatic
            Dx8SoundManagerStatic
            ParameterOperationsStatic
            AVIReaderStatic
            ImageReaderStatic
            WavReaderStatic
            VirtoolsLoaderStatic
            3DTransStatic
            CamerasStatic
            CharactersStatic
            CollisionStatic
            ControllersStatic
            GridsStatic
            InterfaceStatic
            LightsStatic
            LogicsStatic
            MaterialsStatic
            MeshModifiersStatic
            MidiManagerStatic
            NarrativesStatic
            SoundsStatic
            VisualsStatic
            WorldEnvironmentStatic
            BuildingBlocksAddons1Static
            physics_RTStatic
            TT_DatabaseManager_RTStatic
            TT_Gravity_RTStatic
            TT_InterfaceManager_RTStatic
            TT_ParticleSystems_RTStatic
            TT_Toolbox_RTStatic
    )

    set(_ballance_static_rasterizer_targets)
    foreach (_rasterizer_target IN ITEMS CKDX9RasterizerStatic CKBgfxRasterizerStatic)
        if (TARGET ${_rasterizer_target})
            list(APPEND _ballance_static_rasterizer_targets ${_rasterizer_target})
        endif ()
    endforeach ()
    if (NOT _ballance_static_rasterizer_targets)
        message(FATAL_ERROR "BALLANCE_BUILD_STATIC requires a static rasterizer target")
    endif ()
    list(APPEND _ballance_static_module_targets ${_ballance_static_rasterizer_targets})

    foreach (_static_module_target IN LISTS _ballance_static_module_targets)
        if (NOT TARGET ${_static_module_target})
            message(FATAL_ERROR "BALLANCE_BUILD_STATIC requires target ${_static_module_target}")
        endif ()
    endforeach ()

    target_link_libraries(${PLAYER_NAME} PRIVATE ${_ballance_static_module_targets})
endif ()

target_include_directories(${PLAYER_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${_player_generated_dir}"
)

add_executable(ConfigTool
        ConfigTool.cpp
        GameConfig.cpp
        CmdlineParser.cpp
        Utils.cpp
        "${_config_tool_resource_file}"

        ConfigTool.h
        ConfigToolResource.h
        GameConfig.h
        CmdlineParser.h
        Utils.h
)

set_target_properties(ConfigTool PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_compile_definitions(ConfigTool PRIVATE
        CONFIGTOOL_STANDALONE
)

target_link_libraries(ConfigTool PRIVATE ${_player_vxmath_dep})

target_include_directories(ConfigTool PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${_player_generated_dir}"
)

install(TARGETS ${PLAYER_NAME} ConfigTool
        RUNTIME DESTINATION Bin COMPONENT Runtime
)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Kakuty")
set(CPACK_PACKAGE_VERSION "${PLAYER_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PLAYER_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_GENERATOR "ZIP")
include(CPack)
