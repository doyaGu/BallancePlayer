set(_player_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${_player_generated_dir}/config.h"
        @ONLY
)

if (WIN32)
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
        PlayerSdlShortcuts.h
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
        PlayerSdlShortcuts.cpp
        Hotfix.cpp
        CmdlineParser.cpp
        Logger.cpp
        Utils.cpp
)

if (WIN32)
    list(APPEND PLAYER_SOURCES "${_player_resource_file}")
endif ()

if (BALLANCE_BUILD_STATIC)
    list(APPEND PLAYER_SOURCES StaticPlugins.cpp)
endif ()

add_executable(${PLAYER_NAME} ${PLAYER_HEADERS} ${PLAYER_SOURCES})

set_target_properties(${PLAYER_NAME} PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
)

if (WIN32)
    set_target_properties(${PLAYER_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    if (MSVC)
        target_link_options(${PLAYER_NAME} PRIVATE "/ENTRY:mainCRTStartup")
    endif ()
endif ()

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
target_link_libraries(${PLAYER_NAME} PRIVATE SDL3::SDL3)
target_compile_definitions(${PLAYER_NAME} PRIVATE SDL_MAIN_HANDLED)

if (BALLANCE_BUILD_STATIC)
    target_compile_definitions(${PLAYER_NAME} PRIVATE BALLANCE_STATIC_MODULES)

    set(_ballance_static_module_targets)
    function(_player_add_static_module target_name macro_name required)
        if (TARGET ${target_name})
            list(APPEND _ballance_static_module_targets ${target_name})
            target_compile_definitions(${PLAYER_NAME} PRIVATE "${macro_name}=1")
        elseif (required)
            message(FATAL_ERROR "BALLANCE_BUILD_STATIC requires target ${target_name}")
        endif ()
        set(_ballance_static_module_targets "${_ballance_static_module_targets}" PARENT_SCOPE)
    endfunction()

    _player_add_static_module(CK2_3DStatic BALLANCE_STATIC_HAVE_CK2_3D TRUE)
    _player_add_static_module(SdlInputManagerStatic BALLANCE_STATIC_HAVE_SDLINPUTMANAGER TRUE)
    _player_add_static_module(SdlSoundManagerStatic BALLANCE_STATIC_HAVE_SDLSOUNDMANAGER TRUE)
    _player_add_static_module(ParameterOperationsStatic BALLANCE_STATIC_HAVE_PARAMETEROPERATIONS TRUE)
    _player_add_static_module(AVIReaderStatic BALLANCE_STATIC_HAVE_AVIREADER TRUE)
    _player_add_static_module(ImageReaderStatic BALLANCE_STATIC_HAVE_IMAGEREADER TRUE)
    _player_add_static_module(WavReaderStatic BALLANCE_STATIC_HAVE_WAVREADER TRUE)
    _player_add_static_module(VirtoolsLoaderStatic BALLANCE_STATIC_HAVE_VIRTOOLSLOADER TRUE)
    _player_add_static_module(3DTransStatic BALLANCE_STATIC_HAVE_3DTRANS FALSE)
    _player_add_static_module(CamerasStatic BALLANCE_STATIC_HAVE_CAMERAS FALSE)
    _player_add_static_module(CharactersStatic BALLANCE_STATIC_HAVE_CHARACTERS FALSE)
    _player_add_static_module(CollisionStatic BALLANCE_STATIC_HAVE_COLLISION FALSE)
    _player_add_static_module(ControllersStatic BALLANCE_STATIC_HAVE_CONTROLLERS FALSE)
    _player_add_static_module(GridsStatic BALLANCE_STATIC_HAVE_GRIDS FALSE)
    _player_add_static_module(InterfaceStatic BALLANCE_STATIC_HAVE_INTERFACE FALSE)
    _player_add_static_module(LightsStatic BALLANCE_STATIC_HAVE_LIGHTS FALSE)
    _player_add_static_module(LogicsStatic BALLANCE_STATIC_HAVE_LOGICS FALSE)
    _player_add_static_module(MaterialsStatic BALLANCE_STATIC_HAVE_MATERIALS FALSE)
    _player_add_static_module(MeshModifiersStatic BALLANCE_STATIC_HAVE_MESHMODIFIERS FALSE)
    _player_add_static_module(MidiManagerStatic BALLANCE_STATIC_HAVE_MIDIMANAGER FALSE)
    _player_add_static_module(NarrativesStatic BALLANCE_STATIC_HAVE_NARRATIVES FALSE)
    _player_add_static_module(SoundsStatic BALLANCE_STATIC_HAVE_SOUNDS FALSE)
    _player_add_static_module(VisualsStatic BALLANCE_STATIC_HAVE_VISUALS FALSE)
    _player_add_static_module(WorldEnvironmentStatic BALLANCE_STATIC_HAVE_WORLDENVIRONMENT FALSE)
    _player_add_static_module(BuildingBlocksAddons1Static BALLANCE_STATIC_HAVE_BUILDINGBLOCKSADDONS1 FALSE)
    _player_add_static_module(physics_RTStatic BALLANCE_STATIC_HAVE_PHYSICS_RT FALSE)
    _player_add_static_module(TT_DatabaseManager_RTStatic BALLANCE_STATIC_HAVE_TT_DATABASEMANAGER_RT FALSE)
    _player_add_static_module(TT_Gravity_RTStatic BALLANCE_STATIC_HAVE_TT_GRAVITY_RT FALSE)
    _player_add_static_module(TT_InterfaceManager_RTStatic BALLANCE_STATIC_HAVE_TT_INTERFACEMANAGER_RT FALSE)
    _player_add_static_module(TT_ParticleSystems_RTStatic BALLANCE_STATIC_HAVE_TT_PARTICLESYSTEMS_RT FALSE)
    _player_add_static_module(TT_Toolbox_RTStatic BALLANCE_STATIC_HAVE_TT_TOOLBOX_RT FALSE)
    _player_add_static_module(CKBgfxRasterizerStatic BALLANCE_STATIC_HAVE_CKBGFXRASTERIZER TRUE)

    if (NOT _ballance_static_module_targets)
        message(FATAL_ERROR "BALLANCE_BUILD_STATIC did not find any static module targets")
    endif ()

    target_link_libraries(${PLAYER_NAME} PRIVATE ${_ballance_static_module_targets})
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
