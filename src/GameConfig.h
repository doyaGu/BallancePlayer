#ifndef PLAYER_GAMECONFIG_H
#define PLAYER_GAMECONFIG_H

#include "config.h"

#include "VxMathDefines.h"
#include "XString.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

enum PathCategory {
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

enum LogMode {
    eLogAppend = 0,
    eLogOverwrite,
};

class GameConfig {
public:
    // Startup Settings
    int logMode;
    bool verbose;
    bool manualSetup;
    bool loadAllManagers;
    bool loadAllBuildingBlocks;
    bool loadAllPlugins;

    // Graphics
    int driver;
    int screenMode;
    int bpp;
    int width;
    int height;
    bool fullscreen;

    bool disablePerspectiveCorrection;
    bool forceLinearFog;
    bool forceSoftware;
    bool disableFilter;
    bool ensureVertexShader;
    bool useIndexBuffers;
    bool disableDithering;
    int antialias;
    bool disableMipmap;
    bool disableSpecular;
    bool enableScreenDump;
    bool enableDebugMode;
    int vertexCache;
    bool textureCacheManagement;
    bool sortTransparentObjects;
    VX_PIXELFORMAT textureVideoFormat;
    VX_PIXELFORMAT spriteVideoFormat;

    // Window Settings
    bool childWindowRendering;
    bool borderless;
    bool clipCursor;
    bool alwaysHandleInput;
    bool pauseOnDeactivated;
    int posX;
    int posY;

    // Game Settings
    int langId;
    bool skipOpening;
    bool applyHotfix;
    bool unlockFramerate;
    bool unlockWidescreen;
    bool unlockHighResolution;
    bool debug;
    bool rookie;

    GameConfig();
    GameConfig &operator=(const GameConfig &config);

    void SetPath(PathCategory category, const char *path);
    const char *GetPath(PathCategory category) const;
    bool HasPath(PathCategory category) const;

    void LoadFromIni(const char *filename = "");
    void SaveToIni(const char *filename = "");

private:
    XString m_Paths[ePathCategoryCount];
};

#endif // PLAYER_GAMECONFIG_H
