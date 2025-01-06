#include "GameConfig.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "bp/Utils.h"
#include "Configuration.h"

constexpr const char *const PathNames[] = {
    "Config",
    "Log",
    "Cmo",
    "Root",
    "Plugin",
    "RenderEngines",
    "Managers",
    "BuildingBlocks",
    "Sounds",
    "Textures",
    "Data",
    "Scripts",
};

constexpr const char *const DefaultPaths[] = {
    "Player.json",
    "Player.log",
    "base.cmo",
    "..\\",
    "Plugins\\",
    "RenderEngines\\",
    "Managers\\",
    "BuildingBlocks\\",
    "Sounds\\",
    "Textures\\",
    "",
    "Scripts\\",
};

BpGameConfig *bpCreateGameConfig(const char *name) {
    return new BpGameConfig(name);
}

void bpDestroyGameConfig(BpGameConfig *config) {
    if (config) {
        delete config;
    }
}

BpConfig *bpGameConfigGet(const BpGameConfig *config) {
    if (!config || !config->Config)
        return nullptr;
    return config->Config;
}

bool bpGameConfigIsInitialized(const BpGameConfig *config) {
    if (!config)
        return false;
    return config->Initialized;
}

bool bpGameConfigInit(BpGameConfig *config) {
    if (!config || !config->Config)
        return false;

    if (config->Initialized)
        return true;

    bpGameConfigRelease(config);

    auto *cfg = config->Config;

    config->LogMode = cfg->AddEntryInt32("LogMode", BP_LOG_MODE_OVERWRITE, "Startup");
    if (!config->LogMode) {
        return false;
    }
    config->LogMode->AddRef();

    config->Verbose = cfg->AddEntryBool("Verbose", false, "Startup");
    if (!config->Verbose) {
        return false;
    }
    config->Verbose->AddRef();

    config->ManualSetup = cfg->AddEntryBool("ManualSetup", false, "Startup");
    if (!config->ManualSetup) {
        return false;
    }
    config->ManualSetup->AddRef();

    config->LoadAllManagers = cfg->AddEntryBool("LoadAllManagers", true, "Startup");
    if (!config->LoadAllManagers) {
        return false;
    }
    config->LoadAllManagers->AddRef();

    config->LoadAllBuildingBlocks = cfg->AddEntryBool("LoadAllBuildingBlocks", true, "Startup");
    if (!config->LoadAllBuildingBlocks) {
        return false;
    }
    config->LoadAllBuildingBlocks->AddRef();

    config->LoadAllPlugins = cfg->AddEntryBool("LoadAllPlugins", true, "Startup");
    if (!config->LoadAllPlugins) {
        return false;
    }
    config->LoadAllPlugins->AddRef();

    config->Driver = cfg->AddEntryInt32("Driver", 0, "Graphics");
    if (!config->Driver) {
        return false;
    }
    config->Driver->AddRef();

    config->ScreenMode = cfg->AddEntryInt32("ScreenMode", -1, "Graphics");
    if (!config->ScreenMode) {
        return false;
    }
    config->ScreenMode->AddRef();

    config->Width = cfg->AddEntryInt32("Width", BP_DEFAULT_WIDTH, "Graphics");
    if (!config->Width) {
        return false;
    }
    config->Width->AddRef();

    config->Height = cfg->AddEntryInt32("Height", BP_DEFAULT_HEIGHT, "Graphics");
    if (!config->Height) {
        return false;
    }
    config->Height->AddRef();

    config->Bpp = cfg->AddEntryInt32("BitsPerPixel", BP_DEFAULT_BPP, "Graphics");
    if (!config->Bpp) {
        return false;
    }
    config->Bpp->AddRef();

    config->Fullscreen = cfg->AddEntryBool("Fullscreen", false, "Graphics");
    if (!config->Fullscreen) {
        return false;
    }
    config->Fullscreen->AddRef();

    config->DisablePerspectiveCorrection = cfg->AddEntryBool("DisablePerspectiveCorrection", false, "Graphics");
    if (!config->DisablePerspectiveCorrection) {
        return false;
    }
    config->DisablePerspectiveCorrection->AddRef();

    config->ForceLinearFog = cfg->AddEntryBool("ForceLinearFog", false, "Graphics");
    if (!config->ForceLinearFog) {
        return false;
    }
    config->ForceLinearFog->AddRef();

    config->ForceLinearFog = cfg->AddEntryBool("ForceLinearFog", false, "Graphics");
    if (!config->ForceLinearFog) {
        return false;
    }
    config->ForceLinearFog->AddRef();

    config->ForceSoftware = cfg->AddEntryBool("ForceSoftware", false, "Graphics");
    if (!config->ForceSoftware) {
        return false;
    }
    config->ForceSoftware->AddRef();

    config->DisableFilter = cfg->AddEntryBool("DisableFilter", false, "Graphics");
    if (!config->DisableFilter) {
        return false;
    }
    config->DisableFilter->AddRef();

    config->EnsureVertexShader = cfg->AddEntryBool("EnsureVertexShader", false, "Graphics");
    if (!config->EnsureVertexShader) {
        return false;
    }
    config->EnsureVertexShader->AddRef();

    config->UseIndexBuffers = cfg->AddEntryBool("UseIndexBuffers", false, "Graphics");
    if (!config->UseIndexBuffers) {
        return false;
    }
    config->UseIndexBuffers->AddRef();

    config->DisableDithering = cfg->AddEntryBool("DisableDithering", false, "Graphics");
    if (!config->DisableDithering) {
        return false;
    }
    config->DisableDithering->AddRef();

    config->Antialias = cfg->AddEntryInt32("Antialias", 0, "Graphics");
    if (!config->Antialias) {
        return false;
    }
    config->Antialias->AddRef();

    config->DisableMipmap = cfg->AddEntryBool("DisableMipmap", false, "Graphics");
    if (!config->DisableMipmap) {
        return false;
    }
    config->DisableMipmap->AddRef();

    config->DisableSpecular = cfg->AddEntryBool("DisableSpecular", false, "Graphics");
    if (!config->DisableSpecular) {
        return false;
    }
    config->DisableSpecular->AddRef();

    config->EnableScreenDump = cfg->AddEntryBool("EnableScreenDump", false, "Graphics");
    if (!config->EnableScreenDump) {
        return false;
    }
    config->EnableScreenDump->AddRef();

    config->EnableDebugMode = cfg->AddEntryBool("EnableDebugMode", false, "Graphics");
    if (!config->EnableDebugMode) {
        return false;
    }
    config->EnableDebugMode->AddRef();

    config->VertexCache = cfg->AddEntryInt32("VertexCache", 16, "Graphics");
    if (!config->VertexCache) {
        return false;
    }
    config->VertexCache->AddRef();

    config->TextureCacheManagement = cfg->AddEntryBool("TextureCacheManagement", true, "Graphics");
    if (!config->TextureCacheManagement) {
        return false;
    }
    config->TextureCacheManagement->AddRef();

    config->SortTransparentObjects = cfg->AddEntryBool("SortTransparentObjects", true, "Graphics");
    if (!config->SortTransparentObjects) {
        return false;
    }
    config->SortTransparentObjects->AddRef();

    config->TextureVideoFormat = cfg->AddEntryString("TextureVideoFormat", "UNKNOWN_PF", "Graphics");
    if (!config->TextureVideoFormat) {
        return false;
    }
    config->TextureVideoFormat->AddRef();

    config->SpriteVideoFormat = cfg->AddEntryString("SpriteVideoFormat", "UNKNOWN_PF", "Graphics");
    if (!config->SpriteVideoFormat) {
        return false;
    }
    config->SpriteVideoFormat->AddRef();

    config->ChildWindowRendering = cfg->AddEntryBool("ChildWindowRendering", false, "Window");
    if (!config->ChildWindowRendering) {
        return false;
    }
    config->ChildWindowRendering->AddRef();

    config->Borderless = cfg->AddEntryBool("Borderless", false, "Window");
    if (!config->Borderless) {
        return false;
    }
    config->Borderless->AddRef();

    config->ClipCursor = cfg->AddEntryBool("ClipCursor", false, "Window");
    if (!config->ClipCursor) {
        return false;
    }
    config->ClipCursor->AddRef();

    config->AlwaysHandleInput = cfg->AddEntryBool("AlwaysHandleInput", false, "Window");
    if (!config->AlwaysHandleInput) {
        return false;
    }
    config->AlwaysHandleInput->AddRef();

    config->PauseOnDeactivated = cfg->AddEntryBool("PauseOnDeactivated", false, "Window");
    if (!config->PauseOnDeactivated) {
        return false;
    }
    config->PauseOnDeactivated->AddRef();

    config->PosX = cfg->AddEntryInt32("X", 2147483647, "Window");
    if (!config->PosX) {
        return false;
    }
    config->PosX->AddRef();

    config->PosY = cfg->AddEntryInt32("Y", 2147483647, "Window");
    if (!config->PosY) {
        return false;
    }
    config->PosY->AddRef();

    config->ApplyHotfix = cfg->AddEntryBool("ApplyHotfix", true, "Game");
    if (!config->ApplyHotfix) {
        return false;
    }
    config->ApplyHotfix->AddRef();

    config->LangId = cfg->AddEntryInt32("LangId", 1, "Game");
    if (!config->LangId) {
        return false;
    }
    config->LangId->AddRef();

    config->SkipOpening = cfg->AddEntryBool("SkipOpening", false, "Game");
    if (!config->SkipOpening) {
        return false;
    }
    config->SkipOpening->AddRef();

    config->UnlockFramerate = cfg->AddEntryBool("UnlockFramerate", false, "Game");
    if (!config->UnlockFramerate) {
        return false;
    }
    config->UnlockFramerate->AddRef();

    config->UnlockWidescreen = cfg->AddEntryBool("UnlockWidescreen", false, "Game");
    if (!config->UnlockWidescreen) {
        return false;
    }
    config->UnlockWidescreen->AddRef();

    config->UnlockHighResolution = cfg->AddEntryBool("UnlockHighResolution", false, "Game");
    if (!config->UnlockHighResolution) {
        return false;
    }
    config->UnlockHighResolution->AddRef();

    config->Debug = cfg->AddEntryBool("Debug", false, "Game");
    if (!config->Debug) {
        return false;
    }
    config->Debug->AddRef();

    config->Rookie = cfg->AddEntryBool("Rookie", false, "Game");
    if (!config->Rookie) {
        return false;
    }
    config->Rookie->AddRef();

    char szPath[MAX_PATH];
    for (int i = 0; i < BP_PATH_CATEGORY_COUNT; ++i) {
        const char *path;
        if (i < BP_PATH_PLUGINS) {
            path = DefaultPaths[i];
        } else {
            bpConcatPath(szPath, MAX_PATH, bpGetGamePath(config, BP_PATH_ROOT), DefaultPaths[i]);
            path = szPath;
        }

        config->Paths[i] = cfg->AddEntryString(PathNames[i], path, "Paths");
        if (!config->Paths[i]) {
            return false;
        }
        config->Paths[i]->AddRef();
    }

    config->Initialized = true;
    return true;
}

void bpGameConfigRelease(BpGameConfig *config) {
    if (!config || !config->Initialized)
        return;

    if (config->LogMode) {
        config->LogMode->Release();
        config->LogMode = nullptr;
    }

    if (config->Verbose) {
        config->Verbose->Release();
        config->Verbose = nullptr;
    }

    if (config->ManualSetup) {
        config->ManualSetup->Release();
        config->ManualSetup = nullptr;
    }

    if (config->LoadAllManagers) {
        config->LoadAllManagers->Release();
        config->LoadAllManagers = nullptr;
    }

    if (config->LoadAllBuildingBlocks) {
        config->LoadAllBuildingBlocks->Release();
        config->LoadAllBuildingBlocks = nullptr;
    }

    if (config->LoadAllPlugins) {
        config->LoadAllPlugins->Release();
        config->LoadAllPlugins = nullptr;
    }

    if (config->Driver) {
        config->Driver->Release();
        config->Driver = nullptr;
    }

    if (config->ScreenMode) {
        config->ScreenMode->Release();
        config->ScreenMode = nullptr;
    }

    if (config->Width) {
        config->Width->Release();
        config->Width = nullptr;
    }

    if (config->Height) {
        config->Height->Release();
        config->Height = nullptr;
    }

    if (config->Bpp) {
        config->Bpp->Release();
        config->Bpp = nullptr;
    }

    if (config->Fullscreen) {
        config->Fullscreen->Release();
        config->Fullscreen = nullptr;
    }

    if (config->DisablePerspectiveCorrection) {
        config->DisablePerspectiveCorrection->Release();
        config->DisablePerspectiveCorrection = nullptr;
    }

    if (config->ForceLinearFog) {
        config->ForceLinearFog->Release();
        config->ForceLinearFog = nullptr;
    }

    if (config->ForceSoftware) {
        config->ForceSoftware->Release();
        config->ForceSoftware = nullptr;
    }

    if (config->DisableFilter) {
        config->DisableFilter->Release();
        config->DisableFilter = nullptr;
    }

    if (config->EnsureVertexShader) {
        config->EnsureVertexShader->Release();
        config->EnsureVertexShader = nullptr;
    }

    if (config->UseIndexBuffers) {
        config->UseIndexBuffers->Release();
        config->UseIndexBuffers = nullptr;
    }

    if (config->DisableDithering) {
        config->DisableDithering->Release();
        config->DisableDithering = nullptr;
    }

    if (config->Antialias) {
        config->Antialias->Release();
        config->Antialias = nullptr;
    }

    if (config->DisableMipmap) {
        config->DisableMipmap->Release();
        config->DisableMipmap = nullptr;
    }

    if (config->DisableSpecular) {
        config->DisableSpecular->Release();
        config->DisableSpecular = nullptr;
    }

    if (config->EnableScreenDump) {
        config->EnableScreenDump->Release();
        config->EnableScreenDump = nullptr;
    }

    if (config->EnableDebugMode) {
        config->EnableDebugMode->Release();
        config->EnableDebugMode = nullptr;
    }

    if (config->VertexCache) {
        config->VertexCache->Release();
        config->VertexCache = nullptr;
    }

    if (config->TextureCacheManagement) {
        config->TextureCacheManagement->Release();
        config->TextureCacheManagement = nullptr;
    }

    if (config->SortTransparentObjects) {
        config->SortTransparentObjects->Release();
        config->SortTransparentObjects = nullptr;
    }

    if (config->TextureVideoFormat) {
        config->TextureVideoFormat->Release();
        config->TextureVideoFormat = nullptr;
    }

    if (config->SpriteVideoFormat) {
        config->SpriteVideoFormat->Release();
        config->SpriteVideoFormat = nullptr;
    }

    if (config->ChildWindowRendering) {
        config->ChildWindowRendering->Release();
        config->ChildWindowRendering = nullptr;
    }

    if (config->Borderless) {
        config->Borderless->Release();
        config->Borderless = nullptr;
    }

    if (config->ClipCursor) {
        config->ClipCursor->Release();
        config->ClipCursor = nullptr;
    }

    if (config->AlwaysHandleInput) {
        config->AlwaysHandleInput->Release();
        config->AlwaysHandleInput = nullptr;
    }

    if (config->PauseOnDeactivated) {
        config->PauseOnDeactivated->Release();
        config->PauseOnDeactivated = nullptr;
    }

    if (config->PosX) {
        config->PosX->Release();
        config->PosX = nullptr;
    }

    if (config->PosY) {
        config->PosY->Release();
        config->PosY = nullptr;
    }

    if (config->ApplyHotfix) {
        config->ApplyHotfix->Release();
        config->ApplyHotfix = nullptr;
    }

    if (config->LangId) {
        config->LangId->Release();
        config->LangId = nullptr;
    }

    if (config->SkipOpening) {
        config->SkipOpening->Release();
        config->SkipOpening = nullptr;
    }

    if (config->UnlockFramerate) {
        config->UnlockFramerate->Release();
        config->UnlockFramerate = nullptr;
    }

    if (config->UnlockWidescreen) {
        config->UnlockWidescreen->Release();
        config->UnlockWidescreen = nullptr;
    }

    if (config->UnlockHighResolution) {
        config->UnlockHighResolution->Release();
        config->UnlockHighResolution = nullptr;
    }

    if (config->Debug) {
        config->Debug->Release();
        config->Debug = nullptr;
    }

    if (config->Rookie) {
        config->Rookie->Release();
        config->Rookie = nullptr;
    }

    config->Initialized = false;
}

bool bpGameConfigRead(BpGameConfig *config, const char *json, size_t size) {
    if (!config || !config->Config)
        return false;
    return config->Config->Read(json, size, true);
}

char *bpWriteGameConfig(const BpGameConfig *config, size_t *size) {
    if (!config || !config->Config)
        return nullptr;
    return config->Config->Write(size);
}

void bpGameConfigFree(const BpGameConfig *config, char *json) {
    if (!config || !config->Config || !json)
        return;
    config->Config->Free(json);
}

bool bpGameConfigLoad(BpGameConfig *config, const char *filename) {
    if (!config)
        return false;

    if (!filename) {
        filename = bpGetGamePath(config, BP_PATH_CONFIG);
    }
    if (!filename || filename[0] == '\0') {
        return false;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return false;
    }

    size_t size = 0;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *json = new char[size + 1];
    fread(json, 1, size, fp);
    json[size] = '\0';
    fclose(fp);

    if (!config->Config->Read(json, size, true)) {
        delete[] json;
        return false;
    }

    delete[] json;
    return true;
}

bool bpGameConfigSave(const BpGameConfig *config, const char *filename) {
    if (!config || !config->Config)
        return false;

    if (!filename) {
        filename = bpGetGamePath(config, BP_PATH_CONFIG);
    }
    if (!filename || filename[0] == '\0') {
        return false;
    }

    size_t size;
    char *json = config->Config->Write(&size);
    if (!json) {
        return false;
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        config->Config->Free(json);
        return false;
    }

    fwrite(json, 1, size, fp);
    config->Config->Free(json);
    fclose(fp);

    return true;
}

bool bpSetGamePath(BpGameConfig *config, BpPathCategory category, const char *path) {
    if (!config || category < 0 || category >= BP_PATH_CATEGORY_COUNT || !path)
        return false;

    if (!config->Paths[category]) {
        auto *cfg = config->Config;
        if (!cfg) {
            return false;
        }
        config->Paths[category] = cfg->AddEntryString(PathNames[category], path, "Paths");
        if (!config->Paths[category]) {
            return false;
        }
        config->Paths[category]->AddRef();
        return true;
    }

    config->Paths[category]->SetString(path);

    return true;
}

const char *bpGetGamePath(const BpGameConfig *config, BpPathCategory category) {
    if (!config || category < 0 || category >= BP_PATH_CATEGORY_COUNT)
        return nullptr;

    if (!config->Paths[category]) {
        return nullptr;
    }

    return config->Paths[category]->GetString();
}

bool bpHasGamePath(const BpGameConfig *config, BpPathCategory category) {
    if (!config || category < 0 || category >= BP_PATH_CATEGORY_COUNT)
        return false;

    if (!config->Paths[category]) {
        return false;
    }

    return strcmp(config->Paths[category]->GetString(), "") != 0;
}

bool bpResetGamePath(BpGameConfig *config, BpPathCategory category) {
    if (!config || category < 0 || category > BP_PATH_CATEGORY_COUNT)
        return false;

    if (category == BP_PATH_CATEGORY_COUNT) {
        for (int i = 0; i < BP_PATH_CATEGORY_COUNT; ++i) {
            if (i < BP_PATH_PLUGINS) {
                bpSetGamePath(config, (BpPathCategory) i, DefaultPaths[i]);
            } else {
                char szPath[MAX_PATH];
                bpConcatPath(szPath, MAX_PATH, bpGetGamePath(config, BP_PATH_ROOT), DefaultPaths[i]);
                bpSetGamePath(config, (BpPathCategory) i, szPath);
            }
        }
    } else {
        if (category < BP_PATH_PLUGINS) {
            bpSetGamePath(config, category, DefaultPaths[category]);
        } else {
            char szPath[MAX_PATH];
            bpConcatPath(szPath, MAX_PATH, bpGetGamePath(config, BP_PATH_ROOT), DefaultPaths[category]);
            bpSetGamePath(config, category, szPath);
        }
    }

    return true;
}