#ifndef BP_GAMECONFIG_PRIVATE_H
#define BP_GAMECONFIG_PRIVATE_H

#include "bp/GameConfig.h"
#include "Configuration.h"

struct BpGameConfig {
    BpConfig *Config;

    // Startup Settings
    BpConfigEntry *LogMode;
    BpConfigEntry *Verbose;
    BpConfigEntry *ManualSetup;
    BpConfigEntry *LoadAllManagers;
    BpConfigEntry *LoadAllBuildingBlocks;
    BpConfigEntry *LoadAllPlugins;

    // Graphics
    BpConfigEntry *Driver;
    BpConfigEntry *ScreenMode;
    BpConfigEntry *Width;
    BpConfigEntry *Height;
    BpConfigEntry *Bpp;
    BpConfigEntry *Fullscreen;

    BpConfigEntry *DisablePerspectiveCorrection;
    BpConfigEntry *ForceLinearFog;
    BpConfigEntry *ForceSoftware;
    BpConfigEntry *DisableFilter;
    BpConfigEntry *EnsureVertexShader;
    BpConfigEntry *UseIndexBuffers;
    BpConfigEntry *DisableDithering;
    BpConfigEntry *Antialias;
    BpConfigEntry *DisableMipmap;
    BpConfigEntry *DisableSpecular;
    BpConfigEntry *EnableScreenDump;
    BpConfigEntry *EnableDebugMode;
    BpConfigEntry *VertexCache;
    BpConfigEntry *TextureCacheManagement;
    BpConfigEntry *SortTransparentObjects;
    BpConfigEntry *TextureVideoFormat;
    BpConfigEntry *SpriteVideoFormat;

    // Window Settings
    BpConfigEntry *ChildWindowRendering;
    BpConfigEntry *Borderless;
    BpConfigEntry *ClipCursor;
    BpConfigEntry *AlwaysHandleInput;
    BpConfigEntry *PauseOnDeactivated;
    BpConfigEntry *PosX;
    BpConfigEntry *PosY;

    // Game Settings
    BpConfigEntry *LangId;
    BpConfigEntry *SkipOpening;
    BpConfigEntry *ApplyHotfix;
    BpConfigEntry *UnlockFramerate;
    BpConfigEntry *UnlockWidescreen;
    BpConfigEntry *UnlockHighResolution;
    BpConfigEntry *Debug;
    BpConfigEntry *Rookie;

    // Paths
    BpConfigEntry *Paths[BP_PATH_CATEGORY_COUNT];
};

#endif // BP_GAMECONFIG_PRIVATE_H
