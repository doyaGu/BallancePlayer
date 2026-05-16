#include "StaticPlugins.h"

#include "CKPluginManager.h"

#ifdef BALLANCE_STATIC_MODULES

typedef CKPluginInfo *(*StaticPluginGetInfoFunction)(int);
typedef int (*StaticPluginGetInfoCountFunction)();
typedef CKDataReader *(*StaticPluginGetReaderFunction)(int);
typedef void (*StaticPluginRegisterDeclarationsFunction)(XObjectDeclarationArray *);

struct StaticPlugin {
    const char *Name;
    StaticPluginGetInfoCountFunction GetInfoCount;
    StaticPluginGetInfoFunction GetInfo;
    StaticPluginGetReaderFunction GetReader;
    StaticPluginRegisterDeclarationsFunction RegisterDeclarations;
};

extern CKPluginInfo *CKGet_CK2_3D_PluginInfo(int);

extern CKPluginInfo *CKGet_InputManager_PluginInfo(int);
extern CKPluginInfo *CKGet_SoundManager_PluginInfo(int);
extern CKPluginInfo *CKGet_ParamOp_PluginInfo(int);

extern int CKGet_AviReader_PluginInfoCount();
extern CKPluginInfo *CKGet_AviReader_PluginInfo(int);
extern CKDataReader *CKGet_AviReader_Reader(int);
extern int CKGet_ImageReader_PluginInfoCount();
extern CKPluginInfo *CKGet_ImageReader_PluginInfo(int);
extern CKDataReader *CKGet_ImageReader_Reader(int);
extern int CKGet_WavReader_PluginInfoCount();
extern CKPluginInfo *CKGet_WavReader_PluginInfo(int);
extern CKDataReader *CKGet_WavReader_Reader(int);
extern int CKGet_NemoLoader_PluginInfoCount();
extern CKPluginInfo *CKGet_NemoLoader_PluginInfo(int);
extern CKDataReader *CKGet_NemoLoader_Reader(int);

extern int CKGet_3DTransfo_PluginInfoCount();
extern CKPluginInfo *CKGet_3DTransfo_PluginInfo(int);
extern void Register_3DTransfo_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Cameras_PluginInfoCount();
extern CKPluginInfo *CKGet_Cameras_PluginInfo(int);
extern void Register_Cameras_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Characters_PluginInfoCount();
extern CKPluginInfo *CKGet_Characters_PluginInfo(int);
extern void Register_Characters_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Collisions_PluginInfoCount();
extern CKPluginInfo *CKGet_Collisions_PluginInfo(int);
extern void Register_Collisions_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Controllers_PluginInfoCount();
extern CKPluginInfo *CKGet_Controllers_PluginInfo(int);
extern void Register_Controllers_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Grids_PluginInfoCount();
extern CKPluginInfo *CKGet_Grids_PluginInfo(int);
extern void Register_Grids_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Interface_PluginInfoCount();
extern CKPluginInfo *CKGet_Interface_PluginInfo(int);
extern void Register_Interface_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Lights_PluginInfoCount();
extern CKPluginInfo *CKGet_Lights_PluginInfo(int);
extern void Register_Lights_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Logics_PluginInfoCount();
extern CKPluginInfo *CKGet_Logics_PluginInfo(int);
extern void Register_Logics_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Materials_PluginInfoCount();
extern CKPluginInfo *CKGet_Materials_PluginInfo(int);
extern void Register_Materials_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_MeshModifiers_PluginInfoCount();
extern CKPluginInfo *CKGet_MeshModifiers_PluginInfo(int);
extern void Register_MeshModifiers_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_MidiBehaviors_PluginInfoCount();
extern CKPluginInfo *CKGet_MidiBehaviors_PluginInfo(int);
extern void Register_MidiBehaviors_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Narratives_PluginInfoCount();
extern CKPluginInfo *CKGet_Narratives_PluginInfo(int);
extern void Register_Narratives_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Sounds_PluginInfoCount();
extern CKPluginInfo *CKGet_Sounds_PluginInfo(int);
extern void Register_Sounds_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_Visuals_PluginInfoCount();
extern CKPluginInfo *CKGet_Visuals_PluginInfo(int);
extern void Register_Visuals_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_WorldEnvironment_PluginInfoCount();
extern CKPluginInfo *CKGet_WorldEnvironment_PluginInfo(int);
extern void Register_WorldEnvironment_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_BBAddons_PluginInfoCount();
extern CKPluginInfo *CKGet_BBAddons_PluginInfo(int);
extern void Register_BBAddons_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_TT_Physics_PluginInfoCount();
extern CKPluginInfo *CKGet_TT_Physics_PluginInfo(int);
extern void Register_TT_Physics_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_TT_Database_Manager_PluginInfoCount();
extern CKPluginInfo *CKGet_TT_Database_Manager_PluginInfo(int);
extern void Register_TT_Database_Manager_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_TT_Gravity_PluginInfoCount();
extern CKPluginInfo *CKGet_TT_Gravity_PluginInfo(int);
extern void Register_TT_Gravity_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_TT_Interface_Manager_PluginInfoCount();
extern CKPluginInfo *CKGet_TT_Interface_Manager_PluginInfo(int);
extern void Register_TT_Interface_Manager_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_TT_ParticleSystems_PluginInfoCount();
extern CKPluginInfo *CKGet_TT_ParticleSystems_PluginInfo(int);
extern void Register_TT_ParticleSystems_BehaviorDeclarations(XObjectDeclarationArray *);
extern int CKGet_TT_Toolbox_PluginInfoCount();
extern CKPluginInfo *CKGet_TT_Toolbox_PluginInfo(int);
extern void Register_TT_Toolbox_BehaviorDeclarations(XObjectDeclarationArray *);

static const StaticPlugin kStaticPlugins[] = {
    {"CK2_3D", NULL, CKGet_CK2_3D_PluginInfo, NULL, NULL},
    {"SdlInputManager", NULL, CKGet_InputManager_PluginInfo, NULL, NULL},
    {"SdlSoundManager", NULL, CKGet_SoundManager_PluginInfo, NULL, NULL},
    {"ParameterOperations", NULL, CKGet_ParamOp_PluginInfo, NULL, NULL},
    {"AVIReader", CKGet_AviReader_PluginInfoCount, CKGet_AviReader_PluginInfo, CKGet_AviReader_Reader, NULL},
    {"ImageReader", CKGet_ImageReader_PluginInfoCount, CKGet_ImageReader_PluginInfo, CKGet_ImageReader_Reader, NULL},
    {"WavReader", CKGet_WavReader_PluginInfoCount, CKGet_WavReader_PluginInfo, CKGet_WavReader_Reader, NULL},
    {"VirtoolsLoader", CKGet_NemoLoader_PluginInfoCount, CKGet_NemoLoader_PluginInfo, CKGet_NemoLoader_Reader, NULL},
    {"3DTransfo", CKGet_3DTransfo_PluginInfoCount, CKGet_3DTransfo_PluginInfo, NULL, Register_3DTransfo_BehaviorDeclarations},
    {"Cameras", CKGet_Cameras_PluginInfoCount, CKGet_Cameras_PluginInfo, NULL, Register_Cameras_BehaviorDeclarations},
    {"Characters", CKGet_Characters_PluginInfoCount, CKGet_Characters_PluginInfo, NULL, Register_Characters_BehaviorDeclarations},
    {"Collisions", CKGet_Collisions_PluginInfoCount, CKGet_Collisions_PluginInfo, NULL, Register_Collisions_BehaviorDeclarations},
    {"Controllers", CKGet_Controllers_PluginInfoCount, CKGet_Controllers_PluginInfo, NULL, Register_Controllers_BehaviorDeclarations},
    {"Grids", CKGet_Grids_PluginInfoCount, CKGet_Grids_PluginInfo, NULL, Register_Grids_BehaviorDeclarations},
    {"Interface", CKGet_Interface_PluginInfoCount, CKGet_Interface_PluginInfo, NULL, Register_Interface_BehaviorDeclarations},
    {"Lights", CKGet_Lights_PluginInfoCount, CKGet_Lights_PluginInfo, NULL, Register_Lights_BehaviorDeclarations},
    {"Logics", CKGet_Logics_PluginInfoCount, CKGet_Logics_PluginInfo, NULL, Register_Logics_BehaviorDeclarations},
    {"Materials", CKGet_Materials_PluginInfoCount, CKGet_Materials_PluginInfo, NULL, Register_Materials_BehaviorDeclarations},
    {"MeshModifiers", CKGet_MeshModifiers_PluginInfoCount, CKGet_MeshModifiers_PluginInfo, NULL, Register_MeshModifiers_BehaviorDeclarations},
    {"MidiManager", CKGet_MidiBehaviors_PluginInfoCount, CKGet_MidiBehaviors_PluginInfo, NULL, Register_MidiBehaviors_BehaviorDeclarations},
    {"Narratives", CKGet_Narratives_PluginInfoCount, CKGet_Narratives_PluginInfo, NULL, Register_Narratives_BehaviorDeclarations},
    {"Sounds", CKGet_Sounds_PluginInfoCount, CKGet_Sounds_PluginInfo, NULL, Register_Sounds_BehaviorDeclarations},
    {"Visuals", CKGet_Visuals_PluginInfoCount, CKGet_Visuals_PluginInfo, NULL, Register_Visuals_BehaviorDeclarations},
    {"WorldEnvironment", CKGet_WorldEnvironment_PluginInfoCount, CKGet_WorldEnvironment_PluginInfo, NULL, Register_WorldEnvironment_BehaviorDeclarations},
    {"BuildingBlocksAddons1", CKGet_BBAddons_PluginInfoCount, CKGet_BBAddons_PluginInfo, NULL, Register_BBAddons_BehaviorDeclarations},
    {"physics_RT", CKGet_TT_Physics_PluginInfoCount, CKGet_TT_Physics_PluginInfo, NULL, Register_TT_Physics_BehaviorDeclarations},
    {"TT_DatabaseManager_RT", CKGet_TT_Database_Manager_PluginInfoCount, CKGet_TT_Database_Manager_PluginInfo, NULL, Register_TT_Database_Manager_BehaviorDeclarations},
    {"TT_Gravity_RT", CKGet_TT_Gravity_PluginInfoCount, CKGet_TT_Gravity_PluginInfo, NULL, Register_TT_Gravity_BehaviorDeclarations},
    {"TT_InterfaceManager_RT", CKGet_TT_Interface_Manager_PluginInfoCount, CKGet_TT_Interface_Manager_PluginInfo, NULL, Register_TT_Interface_Manager_BehaviorDeclarations},
    {"TT_ParticleSystems_RT", CKGet_TT_ParticleSystems_PluginInfoCount, CKGet_TT_ParticleSystems_PluginInfo, NULL, Register_TT_ParticleSystems_BehaviorDeclarations},
    {"TT_Toolbox_RT", CKGet_TT_Toolbox_PluginInfoCount, CKGet_TT_Toolbox_PluginInfo, NULL, Register_TT_Toolbox_BehaviorDeclarations},
};

bool RegisterStaticPlugins(CKPluginManager *pluginManager)
{
    if (!pluginManager)
        return false;

    const int pluginCount = sizeof(kStaticPlugins) / sizeof(kStaticPlugins[0]);
    for (int i = 0; i < pluginCount; ++i) {
        const StaticPlugin &plugin = kStaticPlugins[i];
        CKERROR err = pluginManager->RegisterStaticPlugin(
                const_cast<CKSTRING>(plugin.Name),
                plugin.GetInfoCount,
                plugin.GetInfo,
                plugin.GetReader,
                plugin.RegisterDeclarations);
        if (err != CK_OK && err != CKERR_ALREADYPRESENT)
            return false;
    }

    return true;
}

#endif
