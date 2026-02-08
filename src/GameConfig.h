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

// Master list of all configuration fields
// Format: X_TYPE(section, key, member_name, default_value)
#define GAMECONFIG_FIELDS \
  X_INT  ("Startup",  "LogMode",                 logMode,                 1) \
  X_BOOL ("Startup",  "Verbose",                 verbose,                 false) \
  X_BOOL ("Startup",  "ManualSetup",             manualSetup,             false) \
  X_INT  ("Graphics", "Driver",                  driver,                  0) \
  X_INT  ("Graphics", "BitsPerPixel",            bpp,                     PLAYER_DEFAULT_BPP) \
  X_INT  ("Graphics", "Width",                   width,                   PLAYER_DEFAULT_WIDTH) \
  X_INT  ("Graphics", "Height",                  height,                  PLAYER_DEFAULT_HEIGHT) \
  X_INT  ("Graphics", "Antialias",               antialias,               0) \
  X_INT  ("Graphics", "VertexCache",             vertexCache,             16) \
  X_INT  ("Window",   "X",                       posX,                    2147483647) \
  X_INT  ("Window",   "Y",                       posY,                    2147483647) \
  X_INT  ("Game",     "Language",                langId,                  1) \
  X_BOOL ("Graphics", "FullScreen",              fullscreen,              false) \
  X_BOOL ("Graphics", "DisablePerspectiveCorrection", disablePerspectiveCorrection, false) \
  X_BOOL ("Graphics", "ForceLinearFog",          forceLinearFog,          false) \
  X_BOOL ("Graphics", "ForceSoftware",           forceSoftware,           false) \
  X_BOOL ("Graphics", "DisableFilter",           disableFilter,           false) \
  X_BOOL ("Graphics", "EnsureVertexShader",      ensureVertexShader,      false) \
  X_BOOL ("Graphics", "UseIndexBuffers",         useIndexBuffers,         false) \
  X_BOOL ("Graphics", "DisableDithering",        disableDithering,        false) \
  X_BOOL ("Graphics", "DisableMipmap",           disableMipmap,           false) \
  X_BOOL ("Graphics", "DisableSpecular",         disableSpecular,         false) \
  X_BOOL ("Graphics", "EnableScreenDump",        enableScreenDump,        false) \
  X_BOOL ("Graphics", "EnableDebugMode",         enableDebugMode,         false) \
  X_BOOL ("Graphics", "TextureCacheManagement",  textureCacheManagement,  true) \
  X_BOOL ("Graphics", "SortTransparentObjects",  sortTransparentObjects,  true) \
  X_BOOL ("Window",   "ChildWindowRendering",    childWindowRendering,    false) \
  X_BOOL ("Window",   "Borderless",              borderless,              false) \
  X_BOOL ("Window",   "ClipCursor",              clipCursor,              false) \
  X_BOOL ("Window",   "AlwaysHandleInput",       alwaysHandleInput,       false) \
  X_BOOL ("Game",     "SkipOpening",             skipOpening,             false) \
  X_BOOL ("Game",     "ApplyHotfix",             applyHotfix,             true) \
  X_BOOL ("Game",     "UnlockFramerate",         unlockFramerate,         false) \
  X_BOOL ("Game",     "UnlockWidescreen",        unlockWidescreen,        false) \
  X_BOOL ("Game",     "UnlockHighResolution",    unlockHighResolution,    false) \
  X_PF   ("Graphics", "TextureVideoFormat",      textureVideoFormat,      UNKNOWN_PF) \
  X_PF   ("Graphics", "SpriteVideoFormat",       spriteVideoFormat,       UNKNOWN_PF)

class CGameConfig
{
public:
    // Auto-generated data members from the master list
    #define X_BOOL(sec,key,member,def) bool member;
    #define X_INT(sec,key,member,def)  int  member;
    #define X_PF(sec,key,member,def)   VX_PIXELFORMAT member;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    // Non-INI members (not persisted)
    int screenMode;
    bool debug;
    bool rookie;

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
        #define X_BOOL(sec,key,member,def) eLoadedBool_##member,
        #define X_INT(sec,key,member,def)  eLoadedInt_##member,
        #define X_PF(sec,key,member,def)   eLoadedPixel_##member,
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
