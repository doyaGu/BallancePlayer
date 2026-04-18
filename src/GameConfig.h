#ifndef PLAYER_GAMECONFIG_H
#define PLAYER_GAMECONFIG_H

#include <string>

#include <config.h>

#include "VxMathDefines.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

class CmdlineParser;

enum PathCategory
{
    eConfigPath = 0,
    eLogPath,
    eCmoPath,
    eRootPath,
    ePluginPath,
    eRenderEnginePath,
    eManagerPath,
    eBuildingBlockPath,
    eSoundPath,
    eBitmapPath,
    eDataPath,
    ePathCategoryCount
};

enum LogMode
{
    eLogAppend = 0,
    eLogOverwrite,
};

// Master list of all configuration fields.
// Format:
//   X_BOOL(section, key, member, default, cli_long, cli_short, cli_value)
//   X_INT(section, key, member, default, cli_long, cli_short)
//   X_PF(section, key, member, default, cli_long, cli_short)
#define GAMECONFIG_FIELDS \
  X_INT  ("Startup",  "LogMode",                 logMode,                 1,                  0,                                      '\0') \
  X_BOOL ("Startup",  "Verbose",                 verbose,                 false,              "--verbose",                             '\0', true) \
  X_BOOL ("Startup",  "ManualSetup",             manualSetup,             false,              "--manual-setup",                        'm',  true) \
  X_INT  ("Graphics", "Driver",                  driver,                  0,                  "--video-driver",                        'v') \
  X_INT  ("Graphics", "BitsPerPixel",            bpp,                     PLAYER_DEFAULT_BPP, "--bpp",                                 'b') \
  X_INT  ("Graphics", "Width",                   width,                   PLAYER_DEFAULT_WIDTH, "--width",                              'w') \
  X_INT  ("Graphics", "Height",                  height,                  PLAYER_DEFAULT_HEIGHT, "--height",                            'h') \
  X_INT  ("Graphics", "Antialias",               antialias,               0,                  "--antialias",                           '\0') \
  X_INT  ("Graphics", "VertexCache",             vertexCache,             16,                 "--vertex-cache",                        '\0') \
  X_INT  ("Window",   "X",                       posX,                    2147483647,         "--position-x",                          'x') \
  X_INT  ("Window",   "Y",                       posY,                    2147483647,         "--position-y",                          'y') \
  X_INT  ("Game",     "Language",                langId,                  1,                  "--lang",                                'l') \
  X_BOOL ("Graphics", "FullScreen",              fullscreen,              false,              "--fullscreen",                          'f',  true) \
  X_BOOL ("Graphics", "DisablePerspectiveCorrection", disablePerspectiveCorrection, false,     "--disable--perspective-correction",     '\0', true) \
  X_BOOL ("Graphics", "ForceLinearFog",          forceLinearFog,          false,              "--force-linear-fog",                    '\0', true) \
  X_BOOL ("Graphics", "ForceSoftware",           forceSoftware,           false,              "--force-software",                      '\0', true) \
  X_BOOL ("Graphics", "DisableFilter",           disableFilter,           false,              "--disable-filter",                      '\0', true) \
  X_BOOL ("Graphics", "EnsureVertexShader",      ensureVertexShader,      false,              "--ensure-vertex-shader",                '\0', true) \
  X_BOOL ("Graphics", "UseIndexBuffers",         useIndexBuffers,         false,              "--use-index-buffers",                   '\0', true) \
  X_BOOL ("Graphics", "DisableDithering",        disableDithering,        false,              "--disable-dithering",                   '\0', true) \
  X_BOOL ("Graphics", "DisableMipmap",           disableMipmap,           false,              "--disable-mipmap",                      '\0', true) \
  X_BOOL ("Graphics", "DisableSpecular",         disableSpecular,         false,              "--disable-specular",                    '\0', true) \
  X_BOOL ("Graphics", "EnableScreenDump",        enableScreenDump,        false,              "--enable-screen-dump",                  '\0', true) \
  X_BOOL ("Graphics", "EnableDebugMode",         enableDebugMode,         false,              "--enable-debug-mode",                   '\0', true) \
  X_BOOL ("Graphics", "TextureCacheManagement",  textureCacheManagement,  true,               "--disable-texture-cache-management",    's',  false) \
  X_BOOL ("Graphics", "SortTransparentObjects",  sortTransparentObjects,  true,               "--disable-sort-transparent-objects",    'o',  false) \
  X_BOOL ("Window",   "ChildWindowRendering",    childWindowRendering,    false,              "--child-window-rendering",              'C',  true) \
  X_BOOL ("Window",   "Borderless",              borderless,              false,              "--borderless",                          'c',  true) \
  X_BOOL ("Window",   "ClipCursor",              clipCursor,              false,              "--clip-cursor",                         '\0', true) \
  X_BOOL ("Window",   "AlwaysHandleInput",       alwaysHandleInput,       false,              "--always-handle-input",                 '\0', true) \
  X_BOOL ("Game",     "SkipOpening",             skipOpening,             false,              "--skip-opening",                        '\0', true) \
  X_BOOL ("Game",     "ApplyHotfix",             applyHotfix,             true,               "--disable-hotfix",                      '\0', false) \
  X_BOOL ("Game",     "UnlockFramerate",         unlockFramerate,         false,              "--unlock-framerate",                    'u',  true) \
  X_BOOL ("Game",     "UnlockWidescreen",        unlockWidescreen,        false,              "--unlock-widescreen",                   '\0', true) \
  X_BOOL ("Game",     "UnlockHighResolution",    unlockHighResolution,    false,              "--unlock-high-resolution",              '\0', true) \
  X_BOOL ("Game",     "Debug",                   debug,                   false,              "--debug",                               'd',  true) \
  X_BOOL ("Game",     "Rookie",                  rookie,                  false,              "--rookie",                              'r',  true) \
  X_PF   ("Graphics", "TextureVideoFormat",      textureVideoFormat,      UNKNOWN_PF,         "--texture-video-format",                '\0') \
  X_PF   ("Graphics", "SpriteVideoFormat",       spriteVideoFormat,       UNKNOWN_PF,         "--sprite-video-format",                 '\0')

#define GAMECONFIG_PATH_FIELDS \
  X_PATH(eConfigPath,          "Player.ini",        "--config",               false) \
  X_PATH(eLogPath,             "Player.log",        "--log",                  false) \
  X_PATH(eCmoPath,             "base.cmo",          "--cmo",                  false) \
  X_PATH(eRootPath,            "..\\",              "--root-path",            true) \
  X_PATH(ePluginPath,          "Plugins\\",         "--plugin-path",          true) \
  X_PATH(eRenderEnginePath,    "RenderEngines\\",   "--render-engine-path",   true) \
  X_PATH(eManagerPath,         "Managers\\",        "--manager-path",         true) \
  X_PATH(eBuildingBlockPath,   "BuildingBlocks\\",  "--building-block-path",  true) \
  X_PATH(eSoundPath,           "Sounds\\",          "--sound-path",           true) \
  X_PATH(eBitmapPath,          "Textures\\",        "--bitmap-path",          true) \
  X_PATH(eDataPath,            "",                  "--data-path",            true)

class CGameConfig
{
public:
    // Auto-generated data members from the master list
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) bool member;
    #define X_INT(sec,key,member,def,cliLong,cliShort)  int  member;
    #define X_PF(sec,key,member,def,cliLong,cliShort)   VX_PIXELFORMAT member;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    // Non-INI members (not persisted)
    int screenMode;

    CGameConfig();
    CGameConfig &operator=(const CGameConfig &config);

    bool HasPath(PathCategory category) const;
    const char *GetPath(PathCategory category) const;
    void SetPath(PathCategory category, const char *path);
    bool ResetPath(PathCategory category = ePathCategoryCount);

    void LoadFromIni(const char *filename = "");
    void SaveToIni(const char *filename = "");

private:
    std::string m_Paths[ePathCategoryCount];

    // Auto-generated enum indices for loaded value snapshots
    enum {
        #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) eLoadedBool_##member,
        #define X_INT(sec,key,member,def,cliLong,cliShort)  eLoadedInt_##member,
        #define X_PF(sec,key,member,def,cliLong,cliShort)   eLoadedPixel_##member,
            GAMECONFIG_FIELDS
        #undef X_BOOL
        #undef X_INT
        #undef X_PF
        eLoadedSentinel
    };

    struct LoadedIntValue
    {
        bool hasValue;
        int value;
    };

    struct LoadedBoolValue
    {
        bool hasValue;
        bool value;
    };

    struct LoadedPixelValue
    {
        bool hasValue;
        VX_PIXELFORMAT value;
    };

    // Single arrays for all loaded values (indexed by enum)
    LoadedIntValue   m_LoadedInts[eLoadedSentinel];
    LoadedBoolValue  m_LoadedBools[eLoadedSentinel];
    LoadedPixelValue m_LoadedPixels[eLoadedSentinel];

    unsigned long m_ConfigTimestampLow;
    unsigned long m_ConfigTimestampHigh;
    bool m_ConfigTimestampValid;
    std::string m_LastConfigAbsolutePath;

    void ResetLoadedSnapshots();
    void StoreLoadedInt(int index, int value);
    void StoreLoadedBool(int index, bool value);
    void StoreLoadedPixel(int index, VX_PIXELFORMAT value);
    bool ShouldOverrideInt(int index, int newValue) const;
    bool ShouldOverrideBool(int index, bool newValue) const;
    bool ShouldOverridePixel(int index, VX_PIXELFORMAT newValue) const;
    void CaptureCurrentValuesAsLoaded();
    void MergeExternalChanges(const char *filename);
    void SetLastConfigAbsolutePath(const char *path);
    bool IsSameConfigPath(const char *path) const;
};

#endif // PLAYER_GAMECONFIG_H
