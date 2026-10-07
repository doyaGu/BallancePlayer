# Player owns its module declarations in dynamic and static builds. Publish
# the resolved names for integrators without exposing the option logic.
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/PlayerStaticModules.cmake")
set(_player_runtime_modules "")
foreach (_module IN LISTS PLAYER_STATIC_MODULES)
    list(APPEND _player_runtime_modules "${PLAYER_STATIC_MODULE_${_module}_RUNTIME_TARGET}")
endforeach ()
set_property(GLOBAL PROPERTY PLAYER_RUNTIME_MODULES "${_player_runtime_modules}")

# The default rasterizer follows the providers in this build. The generated
# configuration header and the installed Player.ini share this value.
if (TARGET CKSdlGpuRasterizer)
    set(_player_default_rasterizer "sdlgpu")
else ()
    set(_player_default_rasterizer "null")
endif ()

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
        IniFile.h
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
        IniFile.cpp
        Logger.cpp
        Utils.cpp
)

if (WIN32)
    list(APPEND PLAYER_SOURCES "${_player_resource_file}")
endif ()

if (BALLANCE_BUILD_STATIC)
    list(APPEND PLAYER_SOURCES StaticPlugins.cpp)

    set(PLAYER_STATIC_PLUGIN_DECLARATIONS "")
    set(PLAYER_STATIC_PLUGIN_ENTRIES "")
    foreach (_player_static_module IN LISTS PLAYER_STATIC_MODULES)
        set(_player_static_module_prefix "PLAYER_STATIC_MODULE_${_player_static_module}")
        set(_player_static_macro "${${_player_static_module_prefix}_COMPILE_DEFINITION}")
        set(_player_static_name "${${_player_static_module_prefix}_DISPLAY_NAME}")
        set(_player_static_info_count "${${_player_static_module_prefix}_GET_INFO_COUNT}")
        set(_player_static_info "${${_player_static_module_prefix}_GET_INFO}")
        set(_player_static_reader "${${_player_static_module_prefix}_GET_READER}")
        set(_player_static_register "${${_player_static_module_prefix}_REGISTER_DECLARATIONS}")

        if (${_player_static_module_prefix}_LINK_ONLY)
            continue()
        endif ()

        string(APPEND PLAYER_STATIC_PLUGIN_DECLARATIONS
                "#if defined(${_player_static_macro})\n")
        if (_player_static_info_count)
            string(APPEND PLAYER_STATIC_PLUGIN_DECLARATIONS
                    "extern int ${_player_static_info_count}();\n")
        endif ()
        string(APPEND PLAYER_STATIC_PLUGIN_DECLARATIONS
                "extern CKPluginInfo *${_player_static_info}(int);\n")
        if (_player_static_reader)
            string(APPEND PLAYER_STATIC_PLUGIN_DECLARATIONS
                    "extern CKDataReader *${_player_static_reader}(int);\n")
        endif ()
        if (_player_static_register)
            string(APPEND PLAYER_STATIC_PLUGIN_DECLARATIONS
                    "extern void ${_player_static_register}(XObjectDeclarationArray *);\n")
        endif ()
        string(APPEND PLAYER_STATIC_PLUGIN_DECLARATIONS "#endif\n\n")

        foreach (_callback IN ITEMS
                _player_static_info_count
                _player_static_info
                _player_static_reader
                _player_static_register)
            if (NOT ${_callback})
                set(${_callback}_value NULL)
            else ()
                set(${_callback}_value "${${_callback}}")
            endif ()
        endforeach ()

        string(APPEND PLAYER_STATIC_PLUGIN_ENTRIES
                "#if defined(${_player_static_macro})\n"
                "    {\"${_player_static_name}\", ${_player_static_info_count_value}, ${_player_static_info_value}, ${_player_static_reader_value}, ${_player_static_register_value}},\n"
                "#endif\n")
    endforeach ()

    configure_file(
            "${CMAKE_CURRENT_LIST_DIR}/StaticPluginRegistry.generated.h.in"
            "${_player_generated_dir}/StaticPluginRegistry.generated.h"
            @ONLY
    )
endif ()

add_executable(${PLAYER_NAME} ${PLAYER_HEADERS} ${PLAYER_SOURCES})
target_compile_features(${PLAYER_NAME} PRIVATE cxx_std_17)

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

    foreach (_player_static_module IN LISTS PLAYER_STATIC_MODULES)
        set(_player_static_module_prefix "PLAYER_STATIC_MODULE_${_player_static_module}")
        set(_player_static_target "${${_player_static_module_prefix}_STATIC_TARGET}")
        set(_player_static_macro "${${_player_static_module_prefix}_COMPILE_DEFINITION}")
        set(_player_static_required "${${_player_static_module_prefix}_REQUIRED}")
        _player_add_static_module(
                "${_player_static_target}"
                "${_player_static_macro}"
                "${_player_static_required}"
        )
    endforeach ()

    if (NOT _ballance_static_module_targets)
        message(FATAL_ERROR "BALLANCE_BUILD_STATIC did not find any static module targets")
    endif ()

    target_link_libraries(${PLAYER_NAME} PRIVATE ${_ballance_static_module_targets})
endif ()

target_include_directories(${PLAYER_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${_player_generated_dir}"
)

set(_player_runtime_dependency_args)
set(_player_sdl_runtime_target "")
if (TARGET SDL3::SDL3-shared)
    set(_player_sdl_runtime_target SDL3::SDL3-shared)
elseif (TARGET SDL3::SDL3)
    get_target_property(_player_sdl_runtime_target SDL3::SDL3 ALIASED_TARGET)
    if (NOT _player_sdl_runtime_target)
        set(_player_sdl_runtime_target SDL3::SDL3)
    endif ()
    get_target_property(_player_sdl_target_type ${_player_sdl_runtime_target} TYPE)
    if (NOT _player_sdl_target_type STREQUAL "SHARED_LIBRARY")
        set(_player_sdl_runtime_target "")
    endif ()
endif ()

if (_player_sdl_runtime_target)
    if (APPLE)
        # Resolve package-manager symlinks before staging the SDL dylib. The
        # include/exclude pair keeps the dependency set limited to SDL itself.
        list(APPEND _player_runtime_dependency_args
                RUNTIME_DEPENDENCIES
                POST_INCLUDE_FILES "$<TARGET_FILE:${_player_sdl_runtime_target}>"
                POST_EXCLUDE_REGEXES ".*"
        )
    else ()
        get_target_property(_player_sdl_runtime_imported ${_player_sdl_runtime_target} IMPORTED)
        if (_player_sdl_runtime_imported)
            install(IMPORTED_RUNTIME_ARTIFACTS ${_player_sdl_runtime_target}
                    RUNTIME DESTINATION Bin COMPONENT Runtime
                    LIBRARY DESTINATION Bin COMPONENT Runtime
            )
        else ()
            install(TARGETS ${_player_sdl_runtime_target}
                    RUNTIME DESTINATION Bin COMPONENT Runtime
                    LIBRARY DESTINATION Bin COMPONENT Runtime
            )
        endif ()
    endif ()
endif ()

install(TARGETS ${PLAYER_NAME}
        ${_player_runtime_dependency_args}
        RUNTIME DESTINATION Bin COMPONENT Runtime
        LIBRARY DESTINATION Bin COMPONENT Runtime
        FRAMEWORK DESTINATION Bin COMPONENT Runtime
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Player.ini.in"
        "${CMAKE_CURRENT_BINARY_DIR}/Player.ini" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Player.ini"
        DESTINATION Bin
        COMPONENT Runtime
)
