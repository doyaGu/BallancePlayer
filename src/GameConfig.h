#ifndef BP_GAMECONFIG_PRIVATE_H
#define BP_GAMECONFIG_PRIVATE_H

#include "bp/GameConfig.h"
#include "Configuration.h"

struct BpGameConfig {
    BpConfig *Config = nullptr;
    bool Initialized = false;

    // Startup Settings
    BpConfigEntry *LogMode = nullptr;
    BpConfigEntry *Verbose = nullptr;
    BpConfigEntry *ManualSetup = nullptr;
    BpConfigEntry *LoadAllManagers = nullptr;
    BpConfigEntry *LoadAllBuildingBlocks = nullptr;
    BpConfigEntry *LoadAllPlugins = nullptr;

    // Graphics
    BpConfigEntry *Driver = nullptr;
    BpConfigEntry *ScreenMode = nullptr;
    BpConfigEntry *Width = nullptr;
    BpConfigEntry *Height = nullptr;
    BpConfigEntry *Bpp = nullptr;
    BpConfigEntry *Fullscreen = nullptr;

    BpConfigEntry *DisablePerspectiveCorrection = nullptr;
    BpConfigEntry *ForceLinearFog = nullptr;
    BpConfigEntry *ForceSoftware = nullptr;
    BpConfigEntry *DisableFilter = nullptr;
    BpConfigEntry *EnsureVertexShader = nullptr;
    BpConfigEntry *UseIndexBuffers = nullptr;
    BpConfigEntry *DisableDithering = nullptr;
    BpConfigEntry *Antialias = nullptr;
    BpConfigEntry *DisableMipmap = nullptr;
    BpConfigEntry *DisableSpecular = nullptr;
    BpConfigEntry *EnableScreenDump = nullptr;
    BpConfigEntry *EnableDebugMode = nullptr;
    BpConfigEntry *VertexCache = nullptr;
    BpConfigEntry *TextureCacheManagement = nullptr;
    BpConfigEntry *SortTransparentObjects = nullptr;
    BpConfigEntry *TextureVideoFormat = nullptr;
    BpConfigEntry *SpriteVideoFormat = nullptr;

    // Window Settings
    BpConfigEntry *ChildWindowRendering = nullptr;
    BpConfigEntry *Borderless = nullptr;
    BpConfigEntry *ClipCursor = nullptr;
    BpConfigEntry *AlwaysHandleInput = nullptr;
    BpConfigEntry *PauseOnDeactivated = nullptr;
    BpConfigEntry *PosX = nullptr;
    BpConfigEntry *PosY = nullptr;

    // Game Settings
    BpConfigEntry *ApplyHotfix = nullptr;
    BpConfigEntry *LangId = nullptr;
    BpConfigEntry *SkipOpening = nullptr;
    BpConfigEntry *UnlockFramerate = nullptr;
    BpConfigEntry *UnlockWidescreen = nullptr;
    BpConfigEntry *UnlockHighResolution = nullptr;
    BpConfigEntry *Debug = nullptr;
    BpConfigEntry *Rookie = nullptr;

    // Paths
    BpConfigEntry *Paths[BP_PATH_CATEGORY_COUNT] = {nullptr};

    explicit BpGameConfig(const char *name) {
        Config = bpGetConfig(name);
        Config->AddRef();
    }

    ~BpGameConfig() {
        bpGameConfigRelease(this);
        Config->Release();
        Config = nullptr;
    }
};

#endif // BP_GAMECONFIG_PRIVATE_H
