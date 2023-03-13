#ifndef PLAYER_GAMECONFIG_H
#define PLAYER_GAMECONFIG_H

#include "config.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

class CmdlineParser;

enum PathCategory
{
    eConfigPath = 0,
    eRootPath = 1,
    ePluginPath = 2,
    eRenderEnginePath = 3,
    eManagerPath = 4,
    eBuildingBlockPath = 5,
    eSoundPath = 6,
    eBitmapPath = 7,
    eDataPath = 8,
    ePathCategoryCount = 9
};

class CGameConfig
{
public:
    // Startup Settings
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
    bool unlockFramerate;
    bool unlockWidescreen;
    bool unlockHighResolution;

    int antialias;
    bool disableFilter;
    bool disableDithering;
    bool disableMipmap;
    bool disableSpecular;

    // Window Settings
    bool borderless;
    bool clipMouse;
    bool alwaysHandleInput;
    bool pauseOnDeactivated;
    int posX;
    int posY;

    // Game Settings
    int langId;
    bool skipOpening;
    bool debug;
    bool rookie;

    CGameConfig();
    CGameConfig &operator=(const CGameConfig &config);

    void SetPath(PathCategory category, const char *path);
    const char *GetPath(PathCategory category) const;
    bool HasPath(PathCategory category) const;

    void LoadFromCmdline(CmdlineParser &parser);
    void LoadIniPathFromCmdline(CmdlineParser &parser);
    void LoadPathsFromCmdline(CmdlineParser &parser);
    void LoadFromIni(const char *filename = "");
    void SaveToIni(const char *filename = "");

private:
    char m_Paths[ePathCategoryCount][MAX_PATH];
};

#endif // PLAYER_GAMECONFIG_H