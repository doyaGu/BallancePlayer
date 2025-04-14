#include "GameConfig.h"

#include <cstring>

#include "bp/Utils.h"

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

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
    "Player.ini",
    "Player.log",
    "base.cmo",
    ".." PATH_SEPARATOR,
    "Plugins" PATH_SEPARATOR,
    "RenderEngines" PATH_SEPARATOR,
    "Managers" PATH_SEPARATOR,
    "BuildingBlocks" PATH_SEPARATOR,
    "Sounds" PATH_SEPARATOR,
    "Textures" PATH_SEPARATOR,
    "",
    "Scripts" PATH_SEPARATOR,
};

void bpGameConfigReset(BpGameConfig *config) {
    if (config)
        config->Reset();
}

bool bpGameConfigLoad(BpGameConfig *config, const char *filename) {
    if (!config)
        return false;
    return config->Load(filename);
}

bool bpGameConfigSave(const BpGameConfig *config, const char *filename) {
    if (!config)
        return false;
    return config->Save(filename);
}

BpValue *bpGameConfigGetValue(BpGameConfig *config, BpConfigType type) {
    if (!config)
        return nullptr;
    return &config->Get(type);
}

bool bpHasGamePath(const BpGameConfig *config, BpPathCategory category) {
    if (!config)
        return false;
    return config->HasPath(category);
}

const char *bpGetGamePath(const BpGameConfig *config, BpPathCategory category) {
    if (!config)
        return nullptr;
    return config->GetPath(category);
}

bool bpSetGamePath(BpGameConfig *config, BpPathCategory category, const char *path) {
    if (!config)
        return false;
    config->SetPath(category, path);
    return true;
}

bool bpResetGamePath(BpGameConfig *config, BpPathCategory category) {
    if (!config)
        return false;
    return config->ResetPath(category);
}

BpGameConfig::BpGameConfig() {
    Reset();
}

void BpGameConfig::Reset() {
    Values[BP_CONFIG_VERBOSE] = false;
    Values[BP_CONFIG_LOG_MODE] = BP_LOG_MODE_OVERWRITE;
    Values[BP_CONFIG_MANUAL_SETUP] = false;
    Values[BP_CONFIG_LOAD_ALL_MANAGERS] = true;
    Values[BP_CONFIG_LOAD_ALL_BUILDING_BLOCKS] = true;
    Values[BP_CONFIG_LOAD_ALL_PLUGINS] = true;

    Values[BP_CONFIG_DRIVER] = 0;
    Values[BP_CONFIG_SCREEN_MODE] = -1;
    Values[BP_CONFIG_BPP] = BP_DEFAULT_BPP;
    Values[BP_CONFIG_WIDTH] = BP_DEFAULT_WIDTH;
    Values[BP_CONFIG_HEIGHT] = BP_DEFAULT_HEIGHT;
    Values[BP_CONFIG_FULLSCREEN] = false;

    Values[BP_CONFIG_DISABLE_PERSPECTIVE_CORRECTION] = false;
    Values[BP_CONFIG_FORCE_LINEAR_FOG] = false;
    Values[BP_CONFIG_FORCE_SOFTWARE] = false;
    Values[BP_CONFIG_DISABLE_FILTER] = false;
    Values[BP_CONFIG_ENSURE_VERTEX_SHADER] = false;
    Values[BP_CONFIG_USE_INDEX_BUFFERS] = false;
    Values[BP_CONFIG_DISABLE_DITHERING] = false;
    Values[BP_CONFIG_ANTIALIAS] = 0;
    Values[BP_CONFIG_DISABLE_MIPMAP] = false;
    Values[BP_CONFIG_DISABLE_SPECULAR] = false;
    Values[BP_CONFIG_ENABLE_SCREEN_DUMP] = false;
    Values[BP_CONFIG_ENABLE_DEBUG_MODE] = false;
    Values[BP_CONFIG_VERTEX_CACHE] = 16;
    Values[BP_CONFIG_TEXTURE_CACHE_MANAGEMENT] = true;
    Values[BP_CONFIG_SORT_TRANSPARENT_OBJECTS] = true;
    Values[BP_CONFIG_TEXTURE_VIDEO_FORMAT] = 0;
    Values[BP_CONFIG_SPRITE_VIDEO_FORMAT] = 0;

    Values[BP_CONFIG_CHILD_WINDOW_RENDERING] = false;
    Values[BP_CONFIG_BORDERLESS] = false;
    Values[BP_CONFIG_CLIP_CURSOR] = false;
    Values[BP_CONFIG_ALWAYS_HANDLE_INPUT] = false;
    Values[BP_CONFIG_X] = 2147483647;
    Values[BP_CONFIG_Y] = 2147483647;

    Values[BP_CONFIG_APPLY_HOTFIX] = true;
    Values[BP_CONFIG_LANG_ID] = 1;
    Values[BP_CONFIG_SKIP_OPENING] = false;
    Values[BP_CONFIG_UNLOCK_FRAMERATE] = false;
    Values[BP_CONFIG_UNLOCK_WIDESCREEN] = false;
    Values[BP_CONFIG_UNLOCK_HIGH_RESOLUTION] = false;
    Values[BP_CONFIG_DEBUG] = false;
    Values[BP_CONFIG_ROOKIE] = false;

    ResetPath(BP_PATH_CATEGORY_COUNT);
}

bool IniGetInteger(BpValue &value, const char *section, const char *key, const char *filename) {
    if (!filename || filename[0] == '\0')
        return false;

    int v;
    if (!bpIniGetInteger(section, key, &v, filename))
        return false;

    value = v;
    return true;
}

bool IniGetBoolean(BpValue &value, const char *section, const char *key, const char *filename) {
    if (!filename || filename[0] == '\0')
        return false;

    bool v;
    if (!bpIniGetBoolean(section, key, &v, filename))
        return false;

    value = v;
    return true;
}

bool IniGetPixelFormat(BpValue &value, const char *section, const char *key, const char *filename) {
    if (!filename || filename[0] == '\0')
        return false;

    int v;
    if (!bpIniGetPixelFormat(section, key, &v, filename))
        return false;

    value = v;
    return true;
}

bool IniSetInteger(const BpValue &value, const char *section, const char *key, const char *filename) {
    if (!filename || filename[0] == '\0')
        return false;

    return bpIniSetInteger(section, key, value.GetInt32(), filename);
}

bool IniSetBoolean(const BpValue &value, const char *section, const char *key, const char *filename) {
    if (!filename || filename[0] == '\0')
        return false;

    return bpIniSetBoolean(section, key, value.GetBool(), filename);
}

bool IniSetPixelFormat(const BpValue &value, const char *section, const char *key, const char *filename) {
    if (!filename || filename[0] == '\0')
        return false;

    return bpIniSetPixelFormat(section, key, value.GetInt32(), filename);
}

bool BpGameConfig::Load(const char *filename) {
    if (!filename) {
        filename = GetPath(BP_PATH_CONFIG);
    }
    if (!filename || filename[0] == '\0') {
        return false;
    }

    char path[MAX_PATH];
    if (!bpIsAbsolutePath(filename)) {
        bpGetCurrentDirectory(path, MAX_PATH);
        bpConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }

    IniGetBoolean(Values[BP_CONFIG_VERBOSE], "Startup", "Verbose", filename);
    IniGetInteger(Values[BP_CONFIG_LOG_MODE], "Startup", "LogMode", filename);
    IniGetBoolean(Values[BP_CONFIG_MANUAL_SETUP], "Startup", "ManualSetup", filename);
    IniGetBoolean(Values[BP_CONFIG_LOAD_ALL_MANAGERS], "Startup", "LoadAllManagers", filename);
    IniGetBoolean(Values[BP_CONFIG_LOAD_ALL_BUILDING_BLOCKS], "Startup", "LoadAllBuildingBlocks", filename);
    IniGetBoolean(Values[BP_CONFIG_LOAD_ALL_PLUGINS], "Startup", "LoadAllPlugins", filename);

    IniGetInteger(Values[BP_CONFIG_DRIVER], "Graphics", "Driver", filename);
    IniGetInteger(Values[BP_CONFIG_SCREEN_MODE], "Graphics", "ScreenMode", filename);
    IniGetInteger(Values[BP_CONFIG_BPP], "Graphics", "BitsPerPixel", filename);
    IniGetInteger(Values[BP_CONFIG_WIDTH], "Graphics", "Width", filename);
    IniGetInteger(Values[BP_CONFIG_HEIGHT], "Graphics", "Height", filename);
    IniGetBoolean(Values[BP_CONFIG_FULLSCREEN], "Graphics", "FullScreen", filename);

    IniGetBoolean(Values[BP_CONFIG_DISABLE_PERSPECTIVE_CORRECTION], "Graphics", "DisablePerspectiveCorrection", filename);
    IniGetBoolean(Values[BP_CONFIG_FORCE_LINEAR_FOG], "Graphics", "ForceLinearFog", filename);
    IniGetBoolean(Values[BP_CONFIG_FORCE_SOFTWARE], "Graphics", "ForceSoftware", filename);
    IniGetBoolean(Values[BP_CONFIG_DISABLE_FILTER], "Graphics", "DisableFilter", filename);
    IniGetBoolean(Values[BP_CONFIG_ENSURE_VERTEX_SHADER], "Graphics", "EnsureVertexShader", filename);
    IniGetBoolean(Values[BP_CONFIG_USE_INDEX_BUFFERS], "Graphics", "UseIndexBuffers", filename);
    IniGetBoolean(Values[BP_CONFIG_DISABLE_DITHERING], "Graphics", "DisableDithering", filename);
    IniGetInteger(Values[BP_CONFIG_ANTIALIAS], "Graphics", "Antialias", filename);
    IniGetBoolean(Values[BP_CONFIG_DISABLE_MIPMAP], "Graphics", "DisableMipmap", filename);
    IniGetBoolean(Values[BP_CONFIG_DISABLE_SPECULAR], "Graphics", "DisableSpecular", filename);
    IniGetBoolean(Values[BP_CONFIG_ENABLE_SCREEN_DUMP], "Graphics", "EnableScreenDump", filename);
    IniGetBoolean(Values[BP_CONFIG_ENABLE_DEBUG_MODE], "Graphics", "EnableDebugMode", filename);
    IniGetInteger(Values[BP_CONFIG_VERTEX_CACHE], "Graphics", "VertexCache", filename);
    IniGetBoolean(Values[BP_CONFIG_TEXTURE_CACHE_MANAGEMENT], "Graphics", "TextureCacheManagement", filename);
    IniGetBoolean(Values[BP_CONFIG_SORT_TRANSPARENT_OBJECTS], "Graphics", "SortTransparentObjects", filename);
    IniGetPixelFormat(Values[BP_CONFIG_TEXTURE_VIDEO_FORMAT], "Graphics", "TextureVideoFormat", filename);
    IniGetPixelFormat(Values[BP_CONFIG_SPRITE_VIDEO_FORMAT], "Graphics", "SpriteVideoFormat", filename);

    IniGetBoolean(Values[BP_CONFIG_CHILD_WINDOW_RENDERING], "Window", "ChildWindowRendering", filename);
    IniGetBoolean(Values[BP_CONFIG_BORDERLESS], "Window", "Borderless", filename);
    IniGetBoolean(Values[BP_CONFIG_CLIP_CURSOR], "Window", "ClipCursor", filename);
    IniGetBoolean(Values[BP_CONFIG_ALWAYS_HANDLE_INPUT], "Window", "AlwaysHandleInput", filename);
    IniGetInteger(Values[BP_CONFIG_X], "Window", "X", filename);
    IniGetInteger(Values[BP_CONFIG_Y], "Window", "Y", filename);

    IniGetInteger(Values[BP_CONFIG_LANG_ID], "Game", "Language", filename);
    IniGetBoolean(Values[BP_CONFIG_SKIP_OPENING], "Game", "SkipOpening", filename);
    IniGetBoolean(Values[BP_CONFIG_APPLY_HOTFIX], "Game", "ApplyHotfix", filename);
    IniGetBoolean(Values[BP_CONFIG_UNLOCK_FRAMERATE], "Game", "UnlockFramerate", filename);
    IniGetBoolean(Values[BP_CONFIG_UNLOCK_WIDESCREEN], "Game", "UnlockWidescreen", filename);
    IniGetBoolean(Values[BP_CONFIG_UNLOCK_HIGH_RESOLUTION], "Game", "UnlockHighResolution", filename);
    IniGetBoolean(Values[BP_CONFIG_DEBUG], "Game", "Debug", filename);
    IniGetBoolean(Values[BP_CONFIG_ROOKIE], "Game", "Rookie", filename);

    return true;
}

bool BpGameConfig::Save(const char *filename) const {
    if (!filename) {
        filename = GetPath(BP_PATH_CONFIG);
    }
    if (!filename || filename[0] == '\0') {
        return false;
    }

    char path[MAX_PATH];
    if (!bpIsAbsolutePath(filename)) {
        bpGetCurrentDirectory(path, MAX_PATH);
        bpConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }

    IniSetBoolean(Values[BP_CONFIG_VERBOSE], "Startup", "Verbose", filename);
    IniSetInteger(Values[BP_CONFIG_LOG_MODE], "Startup", "LogMode", filename);
    IniSetBoolean(Values[BP_CONFIG_MANUAL_SETUP], "Startup", "ManualSetup", filename);
    IniSetBoolean(Values[BP_CONFIG_LOAD_ALL_MANAGERS], "Startup", "LoadAllManagers", filename);
    IniSetBoolean(Values[BP_CONFIG_LOAD_ALL_BUILDING_BLOCKS], "Startup", "LoadAllBuildingBlocks", filename);
    IniSetBoolean(Values[BP_CONFIG_LOAD_ALL_PLUGINS], "Startup", "LoadAllPlugins", filename);

    IniSetInteger(Values[BP_CONFIG_DRIVER], "Graphics", "Driver", filename);
    IniSetInteger(Values[BP_CONFIG_BPP], "Graphics", "BitsPerPixel", filename);
    IniSetInteger(Values[BP_CONFIG_WIDTH], "Graphics", "Width", filename);
    IniSetInteger(Values[BP_CONFIG_HEIGHT], "Graphics", "Height", filename);
    IniSetBoolean(Values[BP_CONFIG_FULLSCREEN], "Graphics", "FullScreen", filename);

    IniSetBoolean(Values[BP_CONFIG_DISABLE_PERSPECTIVE_CORRECTION], "Graphics", "DisablePerspectiveCorrection", filename);
    IniSetBoolean(Values[BP_CONFIG_FORCE_LINEAR_FOG], "Graphics", "ForceLinearFog", filename);
    IniSetBoolean(Values[BP_CONFIG_FORCE_SOFTWARE], "Graphics", "ForceSoftware", filename);
    IniSetBoolean(Values[BP_CONFIG_DISABLE_FILTER], "Graphics", "DisableFilter", filename);
    IniSetBoolean(Values[BP_CONFIG_ENSURE_VERTEX_SHADER], "Graphics", "EnsureVertexShader", filename);
    IniSetBoolean(Values[BP_CONFIG_USE_INDEX_BUFFERS], "Graphics", "UseIndexBuffers", filename);
    IniSetBoolean(Values[BP_CONFIG_DISABLE_DITHERING], "Graphics", "DisableDithering", filename);
    IniSetInteger(Values[BP_CONFIG_ANTIALIAS], "Graphics", "Antialias", filename);
    IniSetBoolean(Values[BP_CONFIG_DISABLE_MIPMAP], "Graphics", "DisableMipmap", filename);
    IniSetBoolean(Values[BP_CONFIG_DISABLE_SPECULAR], "Graphics", "DisableSpecular", filename);
    IniSetBoolean(Values[BP_CONFIG_ENABLE_SCREEN_DUMP], "Graphics", "EnableScreenDump", filename);
    IniSetBoolean(Values[BP_CONFIG_ENABLE_DEBUG_MODE], "Graphics", "EnableDebugMode", filename);
    IniSetInteger(Values[BP_CONFIG_VERTEX_CACHE], "Graphics", "VertexCache", filename);
    IniSetBoolean(Values[BP_CONFIG_TEXTURE_CACHE_MANAGEMENT], "Graphics", "TextureCacheManagement", filename);
    IniSetBoolean(Values[BP_CONFIG_SORT_TRANSPARENT_OBJECTS], "Graphics", "SortTransparentObjects", filename);
    IniSetPixelFormat(Values[BP_CONFIG_TEXTURE_VIDEO_FORMAT], "Graphics", "TextureVideoFormat", filename);
    IniSetPixelFormat(Values[BP_CONFIG_SPRITE_VIDEO_FORMAT], "Graphics", "SpriteVideoFormat", filename);

    IniSetBoolean(Values[BP_CONFIG_CHILD_WINDOW_RENDERING], "Window", "ChildWindowRendering", filename);
    IniSetBoolean(Values[BP_CONFIG_BORDERLESS], "Window", "Borderless", filename);
    IniSetBoolean(Values[BP_CONFIG_CLIP_CURSOR], "Window", "ClipCursor", filename);
    IniSetBoolean(Values[BP_CONFIG_ALWAYS_HANDLE_INPUT], "Window", "AlwaysHandleInput", filename);
    IniSetInteger(Values[BP_CONFIG_X], "Window", "X", filename);
    IniSetInteger(Values[BP_CONFIG_Y], "Window", "Y", filename);

    IniSetInteger(Values[BP_CONFIG_LANG_ID], "Game", "Language", filename);
    IniSetBoolean(Values[BP_CONFIG_SKIP_OPENING], "Game", "SkipOpening", filename);
    IniSetBoolean(Values[BP_CONFIG_APPLY_HOTFIX], "Game", "ApplyHotfix", filename);
    IniSetBoolean(Values[BP_CONFIG_UNLOCK_FRAMERATE], "Game", "UnlockFramerate", filename);
    IniSetBoolean(Values[BP_CONFIG_UNLOCK_WIDESCREEN], "Game", "UnlockWidescreen", filename);
    IniSetBoolean(Values[BP_CONFIG_UNLOCK_HIGH_RESOLUTION], "Game", "UnlockHighResolution", filename);
    IniSetBoolean(Values[BP_CONFIG_DEBUG], "Game", "Debug", filename);
    IniSetBoolean(Values[BP_CONFIG_ROOKIE], "Game", "Rookie", filename);

    return true;
}

bool BpGameConfig::ResetPath(BpPathCategory category) {
    if (category < 0 || category > BP_PATH_CATEGORY_COUNT)
        return false;

    if (category == BP_PATH_CATEGORY_COUNT) {
        // Reset all paths
        for (int i = 0; i < BP_PATH_CATEGORY_COUNT; ++i) {
            if (i < BP_PATH_PLUGINS) {
                SetPath(static_cast<BpPathCategory>(i), DefaultPaths[i]);
            } else {
                char szPath[MAX_PATH] = {};
                const char *rootPath = GetPath(BP_PATH_ROOT);
                if (!rootPath)
                    return false;

                if (!bpConcatPath(szPath, MAX_PATH, rootPath, DefaultPaths[i]))
                    return false;

                SetPath(static_cast<BpPathCategory>(i), szPath);
            }
        }
    } else {
        // Reset specific path
        if (category < BP_PATH_PLUGINS) {
            SetPath(category, DefaultPaths[category]);
        } else {
            char szPath[MAX_PATH] = {};
            const char *rootPath = GetPath(BP_PATH_ROOT);
            if (!rootPath)
                return false;

            if (!bpConcatPath(szPath, MAX_PATH, rootPath, DefaultPaths[category]))
                return false;

            SetPath(category, szPath);
        }
    }

    return true;
}
