set(_player_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${_player_generated_dir}/config.h"
        @ONLY
)

if (WIN32 AND NOT PLAYER_ENABLE_SDL3)
    configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/PlayerVersion.rc.in"
            "${_player_generated_dir}/PlayerVersion.generated.rc"
            @ONLY
    )

    set(_player_resource_file "${CMAKE_CURRENT_SOURCE_DIR}/Player.rc")
    set_source_files_properties("${_player_resource_file}" PROPERTIES
            COMPILE_DEFINITIONS "PLAYER_ICON_PATH=\"${PLAYER_SOURCE_DIR}/Player.ico\";PLAYER_USE_GENERATED_VERSION_RC=1"
    )
endif ()

set(PLAYER_HEADERS
        StaticPlugins.h
        GamePlayer.h
        GameConfig.h
        PlayerOptions.h
        ScriptUtils.h
        CmdlineParser.h
        Logger.h
        Utils.h
)

set(PLAYER_SOURCES
        Player.cpp
        GamePlayer.cpp
        GameConfig.cpp
        PlayerOptions.cpp
        Hotfix.cpp
        CmdlineParser.cpp
        Logger.cpp
        Utils.cpp
)

if (WIN32 AND NOT PLAYER_ENABLE_SDL3)
    list(APPEND PLAYER_HEADERS ConfigDialog.h Splash.h LockGuard.h)
    list(APPEND PLAYER_SOURCES ConfigDialog.cpp Splash.cpp "${_player_resource_file}")
endif ()

if (BALLANCE_BUILD_STATIC_PLAYER)
    list(APPEND PLAYER_SOURCES StaticPlugins.cpp)
endif ()

add_executable(${PLAYER_NAME} ${PLAYER_HEADERS} ${PLAYER_SOURCES})

set_target_properties(${PLAYER_NAME} PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
        WIN32_EXECUTABLE $<AND:$<BOOL:${WIN32}>,$<NOT:$<BOOL:${PLAYER_ENABLE_SDL3}>>>
)

set(_player_ck2_dep CK2)
set(_player_vxmath_dep VxMath)
if (BALLANCE_BUILD_STATIC_PLAYER)
    if (TARGET CK2Static)
        set(_player_ck2_dep CK2Static)
    endif ()
    if (TARGET VxMathStatic)
        set(_player_vxmath_dep VxMathStatic)
    endif ()
endif ()
target_link_libraries(${PLAYER_NAME} PRIVATE ${_player_ck2_dep} ${_player_vxmath_dep})
if (PLAYER_ENABLE_SDL3)
    target_link_libraries(${PLAYER_NAME} PRIVATE SDL3::SDL3)
    target_compile_definitions(${PLAYER_NAME} PRIVATE PLAYER_ENABLE_SDL3=1 SDL_MAIN_HANDLED)
endif ()

if (BALLANCE_BUILD_STATIC_PLAYER)
    target_compile_definitions(${PLAYER_NAME} PRIVATE BALLANCE_STATIC_MODULES)

    set(_ballance_static_player_module_targets
            CK2_3DStatic
            SdlInputManagerStatic
            SdlSoundManagerStatic
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

    set(_ballance_static_player_rasterizer_targets)
    foreach (_rasterizer_target IN ITEMS CKDX9RasterizerStatic CKBgfxRasterizerStatic)
        if (TARGET ${_rasterizer_target})
            list(APPEND _ballance_static_player_rasterizer_targets ${_rasterizer_target})
        endif ()
    endforeach ()
    if (NOT _ballance_static_player_rasterizer_targets)
        message(FATAL_ERROR "BALLANCE_BUILD_STATIC_PLAYER requires a static rasterizer target")
    endif ()
    list(APPEND _ballance_static_player_module_targets ${_ballance_static_player_rasterizer_targets})

    foreach (_static_module_target IN LISTS _ballance_static_player_module_targets)
        if (NOT TARGET ${_static_module_target})
            message(FATAL_ERROR "BALLANCE_BUILD_STATIC_PLAYER requires target ${_static_module_target}")
        endif ()
    endforeach ()

    target_link_libraries(${PLAYER_NAME} PRIVATE ${_ballance_static_player_module_targets})
endif ()

target_include_directories(${PLAYER_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${_player_generated_dir}"
)

install(TARGETS ${PLAYER_NAME}
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
