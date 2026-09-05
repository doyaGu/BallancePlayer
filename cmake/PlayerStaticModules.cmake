# Declarative static-module contract for BallancePlayer.
#
# This file is consumed both by Player and by the superproject's consistency
# check. Keep module-specific build and registration metadata together here.

set(PLAYER_STATIC_MODULES)

function(_player_declare_static_module module_id)
    set(_options REQUIRED LINK_ONLY)
    set(_one_value_args
            RUNTIME_TARGET
            STATIC_TARGET
            COMPILE_DEFINITION
            DISPLAY_NAME
            GET_INFO_COUNT
            GET_INFO
            GET_READER
            REGISTER_DECLARATIONS
    )
    cmake_parse_arguments(PARSE_ARGV 1 _module "${_options}" "${_one_value_args}" "")

    if (_module_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
                "Unexpected arguments for static module ${module_id}: ${_module_UNPARSED_ARGUMENTS}")
    endif ()
    if (module_id IN_LIST PLAYER_STATIC_MODULES)
        message(FATAL_ERROR "Static module ${module_id} is declared more than once")
    endif ()
    foreach (_required_field IN ITEMS RUNTIME_TARGET STATIC_TARGET COMPILE_DEFINITION)
        if (NOT _module_${_required_field})
            message(FATAL_ERROR "Static module ${module_id} requires ${_required_field}")
        endif ()
    endforeach ()

    if (_module_LINK_ONLY)
        foreach (_plugin_field IN ITEMS
                DISPLAY_NAME GET_INFO_COUNT GET_INFO GET_READER REGISTER_DECLARATIONS)
            if (_module_${_plugin_field})
                message(FATAL_ERROR
                        "Link-only static module ${module_id} cannot define ${_plugin_field}")
            endif ()
        endforeach ()
    elseif (NOT _module_DISPLAY_NAME OR NOT _module_GET_INFO)
        message(FATAL_ERROR
                "Static module ${module_id} requires DISPLAY_NAME and GET_INFO")
    endif ()

    list(APPEND PLAYER_STATIC_MODULES "${module_id}")
    set(PLAYER_STATIC_MODULES "${PLAYER_STATIC_MODULES}" PARENT_SCOPE)
    set(PLAYER_STATIC_MODULE_${module_id}_REQUIRED "${_module_REQUIRED}" PARENT_SCOPE)
    set(PLAYER_STATIC_MODULE_${module_id}_LINK_ONLY "${_module_LINK_ONLY}" PARENT_SCOPE)
    foreach (_field IN LISTS _one_value_args)
        set(PLAYER_STATIC_MODULE_${module_id}_${_field} "${_module_${_field}}" PARENT_SCOPE)
    endforeach ()
endfunction()

_player_declare_static_module(CK2_3D
        RUNTIME_TARGET CK2_3D
        STATIC_TARGET CK2_3DStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_CK2_3D
        REQUIRED
        DISPLAY_NAME CK2_3D
        GET_INFO CKGet_CK2_3D_PluginInfo
)
_player_declare_static_module(SdlInputManager
        RUNTIME_TARGET SdlInputManager
        STATIC_TARGET SdlInputManagerStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_SDLINPUTMANAGER
        REQUIRED
        DISPLAY_NAME SdlInputManager
        GET_INFO CKGet_InputManager_PluginInfo
)
_player_declare_static_module(SdlSoundManager
        RUNTIME_TARGET SdlSoundManager
        STATIC_TARGET SdlSoundManagerStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_SDLSOUNDMANAGER
        REQUIRED
        DISPLAY_NAME SdlSoundManager
        GET_INFO CKGet_SoundManager_PluginInfo
)
_player_declare_static_module(ParameterOperations
        RUNTIME_TARGET ParameterOperations
        STATIC_TARGET ParameterOperationsStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_PARAMETEROPERATIONS
        REQUIRED
        DISPLAY_NAME ParameterOperations
        GET_INFO CKGet_ParamOp_PluginInfo
)
_player_declare_static_module(AVIReader
        RUNTIME_TARGET AVIReader
        STATIC_TARGET AVIReaderStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_AVIREADER
        REQUIRED
        DISPLAY_NAME AVIReader
        GET_INFO_COUNT CKGet_AviReader_PluginInfoCount
        GET_INFO CKGet_AviReader_PluginInfo
        GET_READER CKGet_AviReader_Reader
)
_player_declare_static_module(ImageReader
        RUNTIME_TARGET ImageReader
        STATIC_TARGET ImageReaderStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_IMAGEREADER
        REQUIRED
        DISPLAY_NAME ImageReader
        GET_INFO_COUNT CKGet_ImageReader_PluginInfoCount
        GET_INFO CKGet_ImageReader_PluginInfo
        GET_READER CKGet_ImageReader_Reader
)
_player_declare_static_module(WavReader
        RUNTIME_TARGET WavReader
        STATIC_TARGET WavReaderStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_WAVREADER
        REQUIRED
        DISPLAY_NAME WavReader
        GET_INFO_COUNT CKGet_WavReader_PluginInfoCount
        GET_INFO CKGet_WavReader_PluginInfo
        GET_READER CKGet_WavReader_Reader
)
_player_declare_static_module(VirtoolsLoader
        RUNTIME_TARGET VirtoolsLoader
        STATIC_TARGET VirtoolsLoaderStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_VIRTOOLSLOADER
        REQUIRED
        DISPLAY_NAME VirtoolsLoader
        GET_INFO_COUNT CKGet_NemoLoader_PluginInfoCount
        GET_INFO CKGet_NemoLoader_PluginInfo
        GET_READER CKGet_NemoLoader_Reader
)
_player_declare_static_module(3DTrans
        RUNTIME_TARGET 3DTrans
        STATIC_TARGET 3DTransStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_3DTRANS
        DISPLAY_NAME 3DTransfo
        GET_INFO_COUNT CKGet_3DTransfo_PluginInfoCount
        GET_INFO CKGet_3DTransfo_PluginInfo
        REGISTER_DECLARATIONS Register_3DTransfo_BehaviorDeclarations
)
_player_declare_static_module(Cameras
        RUNTIME_TARGET Cameras
        STATIC_TARGET CamerasStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_CAMERAS
        DISPLAY_NAME Cameras
        GET_INFO_COUNT CKGet_Cameras_PluginInfoCount
        GET_INFO CKGet_Cameras_PluginInfo
        REGISTER_DECLARATIONS Register_Cameras_BehaviorDeclarations
)
_player_declare_static_module(Characters
        RUNTIME_TARGET Characters
        STATIC_TARGET CharactersStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_CHARACTERS
        DISPLAY_NAME Characters
        GET_INFO_COUNT CKGet_Characters_PluginInfoCount
        GET_INFO CKGet_Characters_PluginInfo
        REGISTER_DECLARATIONS Register_Characters_BehaviorDeclarations
)
_player_declare_static_module(Collision
        RUNTIME_TARGET Collision
        STATIC_TARGET CollisionStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_COLLISION
        DISPLAY_NAME Collisions
        GET_INFO_COUNT CKGet_Collisions_PluginInfoCount
        GET_INFO CKGet_Collisions_PluginInfo
        REGISTER_DECLARATIONS Register_Collisions_BehaviorDeclarations
)
_player_declare_static_module(Controllers
        RUNTIME_TARGET Controllers
        STATIC_TARGET ControllersStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_CONTROLLERS
        DISPLAY_NAME Controllers
        GET_INFO_COUNT CKGet_Controllers_PluginInfoCount
        GET_INFO CKGet_Controllers_PluginInfo
        REGISTER_DECLARATIONS Register_Controllers_BehaviorDeclarations
)
_player_declare_static_module(Grids
        RUNTIME_TARGET Grids
        STATIC_TARGET GridsStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_GRIDS
        DISPLAY_NAME Grids
        GET_INFO_COUNT CKGet_Grids_PluginInfoCount
        GET_INFO CKGet_Grids_PluginInfo
        REGISTER_DECLARATIONS Register_Grids_BehaviorDeclarations
)
_player_declare_static_module(Interface
        RUNTIME_TARGET Interface
        STATIC_TARGET InterfaceStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_INTERFACE
        DISPLAY_NAME Interface
        GET_INFO_COUNT CKGet_Interface_PluginInfoCount
        GET_INFO CKGet_Interface_PluginInfo
        REGISTER_DECLARATIONS Register_Interface_BehaviorDeclarations
)
_player_declare_static_module(Lights
        RUNTIME_TARGET Lights
        STATIC_TARGET LightsStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_LIGHTS
        DISPLAY_NAME Lights
        GET_INFO_COUNT CKGet_Lights_PluginInfoCount
        GET_INFO CKGet_Lights_PluginInfo
        REGISTER_DECLARATIONS Register_Lights_BehaviorDeclarations
)
_player_declare_static_module(Logics
        RUNTIME_TARGET Logics
        STATIC_TARGET LogicsStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_LOGICS
        DISPLAY_NAME Logics
        GET_INFO_COUNT CKGet_Logics_PluginInfoCount
        GET_INFO CKGet_Logics_PluginInfo
        REGISTER_DECLARATIONS Register_Logics_BehaviorDeclarations
)
_player_declare_static_module(Materials
        RUNTIME_TARGET Materials
        STATIC_TARGET MaterialsStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_MATERIALS
        DISPLAY_NAME Materials
        GET_INFO_COUNT CKGet_Materials_PluginInfoCount
        GET_INFO CKGet_Materials_PluginInfo
        REGISTER_DECLARATIONS Register_Materials_BehaviorDeclarations
)
_player_declare_static_module(MeshModifiers
        RUNTIME_TARGET MeshModifiers
        STATIC_TARGET MeshModifiersStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_MESHMODIFIERS
        DISPLAY_NAME MeshModifiers
        GET_INFO_COUNT CKGet_MeshModifiers_PluginInfoCount
        GET_INFO CKGet_MeshModifiers_PluginInfo
        REGISTER_DECLARATIONS Register_MeshModifiers_BehaviorDeclarations
)
_player_declare_static_module(MidiManager
        RUNTIME_TARGET MidiManager
        STATIC_TARGET MidiManagerStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_MIDIMANAGER
        DISPLAY_NAME MidiManager
        GET_INFO_COUNT CKGet_MidiBehaviors_PluginInfoCount
        GET_INFO CKGet_MidiBehaviors_PluginInfo
        REGISTER_DECLARATIONS Register_MidiBehaviors_BehaviorDeclarations
)
_player_declare_static_module(Narratives
        RUNTIME_TARGET Narratives
        STATIC_TARGET NarrativesStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_NARRATIVES
        DISPLAY_NAME Narratives
        GET_INFO_COUNT CKGet_Narratives_PluginInfoCount
        GET_INFO CKGet_Narratives_PluginInfo
        REGISTER_DECLARATIONS Register_Narratives_BehaviorDeclarations
)
_player_declare_static_module(Sounds
        RUNTIME_TARGET Sounds
        STATIC_TARGET SoundsStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_SOUNDS
        DISPLAY_NAME Sounds
        GET_INFO_COUNT CKGet_Sounds_PluginInfoCount
        GET_INFO CKGet_Sounds_PluginInfo
        REGISTER_DECLARATIONS Register_Sounds_BehaviorDeclarations
)
_player_declare_static_module(Visuals
        RUNTIME_TARGET Visuals
        STATIC_TARGET VisualsStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_VISUALS
        DISPLAY_NAME Visuals
        GET_INFO_COUNT CKGet_Visuals_PluginInfoCount
        GET_INFO CKGet_Visuals_PluginInfo
        REGISTER_DECLARATIONS Register_Visuals_BehaviorDeclarations
)
_player_declare_static_module(WorldEnvironment
        RUNTIME_TARGET WorldEnvironment
        STATIC_TARGET WorldEnvironmentStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_WORLDENVIRONMENT
        DISPLAY_NAME WorldEnvironment
        GET_INFO_COUNT CKGet_WorldEnvironment_PluginInfoCount
        GET_INFO CKGet_WorldEnvironment_PluginInfo
        REGISTER_DECLARATIONS Register_WorldEnvironment_BehaviorDeclarations
)
_player_declare_static_module(BuildingBlocksAddons1
        RUNTIME_TARGET BuildingBlocksAddons1
        STATIC_TARGET BuildingBlocksAddons1Static
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_BUILDINGBLOCKSADDONS1
        DISPLAY_NAME BuildingBlocksAddons1
        GET_INFO_COUNT CKGet_BBAddons_PluginInfoCount
        GET_INFO CKGet_BBAddons_PluginInfo
        REGISTER_DECLARATIONS Register_BBAddons_BehaviorDeclarations
)
_player_declare_static_module(physics_RT
        RUNTIME_TARGET physics_RT
        STATIC_TARGET physics_RTStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_PHYSICS_RT
        DISPLAY_NAME physics_RT
        GET_INFO_COUNT CKGet_TT_Physics_PluginInfoCount
        GET_INFO CKGet_TT_Physics_PluginInfo
        REGISTER_DECLARATIONS Register_TT_Physics_BehaviorDeclarations
)
_player_declare_static_module(TT_DatabaseManager_RT
        RUNTIME_TARGET TT_DatabaseManager_RT
        STATIC_TARGET TT_DatabaseManager_RTStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_TT_DATABASEMANAGER_RT
        DISPLAY_NAME TT_DatabaseManager_RT
        GET_INFO_COUNT CKGet_TT_Database_Manager_PluginInfoCount
        GET_INFO CKGet_TT_Database_Manager_PluginInfo
        REGISTER_DECLARATIONS Register_TT_Database_Manager_BehaviorDeclarations
)
_player_declare_static_module(TT_Gravity_RT
        RUNTIME_TARGET TT_Gravity_RT
        STATIC_TARGET TT_Gravity_RTStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_TT_GRAVITY_RT
        DISPLAY_NAME TT_Gravity_RT
        GET_INFO_COUNT CKGet_TT_Gravity_PluginInfoCount
        GET_INFO CKGet_TT_Gravity_PluginInfo
        REGISTER_DECLARATIONS Register_TT_Gravity_BehaviorDeclarations
)
_player_declare_static_module(TT_InterfaceManager_RT
        RUNTIME_TARGET TT_InterfaceManager_RT
        STATIC_TARGET TT_InterfaceManager_RTStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_TT_INTERFACEMANAGER_RT
        DISPLAY_NAME TT_InterfaceManager_RT
        GET_INFO_COUNT CKGet_TT_Interface_Manager_PluginInfoCount
        GET_INFO CKGet_TT_Interface_Manager_PluginInfo
        REGISTER_DECLARATIONS Register_TT_Interface_Manager_BehaviorDeclarations
)
_player_declare_static_module(TT_ParticleSystems_RT
        RUNTIME_TARGET TT_ParticleSystems_RT
        STATIC_TARGET TT_ParticleSystems_RTStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_TT_PARTICLESYSTEMS_RT
        DISPLAY_NAME TT_ParticleSystems_RT
        GET_INFO_COUNT CKGet_TT_ParticleSystems_PluginInfoCount
        GET_INFO CKGet_TT_ParticleSystems_PluginInfo
        REGISTER_DECLARATIONS Register_TT_ParticleSystems_BehaviorDeclarations
)
_player_declare_static_module(TT_Toolbox_RT
        RUNTIME_TARGET TT_Toolbox_RT
        STATIC_TARGET TT_Toolbox_RTStatic
        COMPILE_DEFINITION BALLANCE_STATIC_HAVE_TT_TOOLBOX_RT
        DISPLAY_NAME TT_Toolbox_RT
        GET_INFO_COUNT CKGet_TT_Toolbox_PluginInfoCount
        GET_INFO CKGet_TT_Toolbox_PluginInfo
        REGISTER_DECLARATIONS Register_TT_Toolbox_BehaviorDeclarations
)
if (CKRE_BUILD_BGFX_RASTERIZER)
    _player_declare_static_module(CKBgfxRasterizer
            RUNTIME_TARGET CKBgfxRasterizer
            STATIC_TARGET CKBgfxRasterizerStatic
            COMPILE_DEFINITION BALLANCE_STATIC_HAVE_CKBGFXRASTERIZER
            REQUIRED
            LINK_ONLY
    )
endif ()
if (NOT DEFINED CKRE_BUILD_SDL_GPU_RASTERIZER OR CKRE_BUILD_SDL_GPU_RASTERIZER)
    _player_declare_static_module(CKSdlGpuRasterizer
            RUNTIME_TARGET CKSdlGpuRasterizer
            STATIC_TARGET CKSdlGpuRasterizerStatic
            COMPILE_DEFINITION BALLANCE_STATIC_HAVE_CKSDLGPURASTERIZER
            LINK_ONLY
    )
endif ()
