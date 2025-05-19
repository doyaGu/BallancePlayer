#include "GameConfig.h"

#include "Utils.h"

CGameConfig::CGameConfig()
{
    logMode = eLogOverwrite;
    verbose = false;
    manualSetup = false;
    loadAllManagers = true;
    loadAllBuildingBlocks = true;
    loadAllPlugins = true;

    driver = 0;
    screenMode = -1;
    bpp = PLAYER_DEFAULT_BPP;
    width = PLAYER_DEFAULT_WIDTH;
    height = PLAYER_DEFAULT_HEIGHT;
    fullscreen = false;

    disablePerspectiveCorrection = false;
    forceLinearFog = false;
    forceSoftware = false;
    disableFilter = false;
    ensureVertexShader = false;
    useIndexBuffers = false;
    disableDithering = false;
    antialias = 0;
    disableMipmap = false;
    disableSpecular = false;
    enableScreenDump = false;
    enableDebugMode = false;
    vertexCache = 16;
    textureCacheManagement = true;
    sortTransparentObjects = true;
    textureVideoFormat = UNKNOWN_PF;
    spriteVideoFormat = UNKNOWN_PF;

    childWindowRendering = false;
    borderless = false;
    clipCursor = false;
    alwaysHandleInput = false;
    posX = 2147483647;
    posY = 2147483647;

    langId = 1;
    skipOpening = false;
    applyHotfix = true;
    unlockFramerate = false;
    unlockWidescreen = false;
    unlockHighResolution = false;
    debug = false;
    rookie = false;
}

CGameConfig &CGameConfig::operator=(const CGameConfig &config)
{
    if (this == &config)
        return *this;

    verbose = config.verbose;
    manualSetup =  config.manualSetup;
    loadAllManagers = config.loadAllManagers;
    loadAllBuildingBlocks = config.loadAllBuildingBlocks;
    loadAllPlugins = config.loadAllPlugins;

    driver = config.driver;
    screenMode = config.screenMode;
    bpp = config.bpp;
    width = config.width;
    height = config.height;
    fullscreen = config.fullscreen;

    disablePerspectiveCorrection = config.disablePerspectiveCorrection;
    forceLinearFog = config.forceLinearFog;
    forceSoftware = config.forceSoftware;
    disableFilter = config.disableFilter;
    ensureVertexShader = config.ensureVertexShader;
    useIndexBuffers = config.useIndexBuffers;
    disableDithering = config.disableDithering;
    antialias = config.antialias;
    disableMipmap = config.disableMipmap;
    disableSpecular = config.disableSpecular;
    enableScreenDump = config.enableScreenDump;
    enableDebugMode = config.enableDebugMode;
    vertexCache = config.vertexCache;
    textureCacheManagement = config.textureCacheManagement;
    sortTransparentObjects = config.sortTransparentObjects;
    textureVideoFormat = config.textureVideoFormat;
    spriteVideoFormat = config.spriteVideoFormat;

    childWindowRendering = config.childWindowRendering;
    borderless = config.borderless;
    clipCursor = config.clipCursor;
    alwaysHandleInput = config.alwaysHandleInput;
    posX = config.posX;
    posY = config.posY;

    langId = config.langId;
    skipOpening = config.skipOpening;
    applyHotfix = config.applyHotfix;
    unlockFramerate = config.unlockFramerate;
    unlockWidescreen = config.unlockWidescreen;
    unlockHighResolution = config.unlockHighResolution;
    debug = config.debug;
    rookie = config.rookie;

    int i;
    for (i = 0; i < ePathCategoryCount; ++i)
        m_Paths[i] = config.m_Paths[i];

    return *this;
}

void CGameConfig::SetPath(PathCategory category, const char *path)
{
    if (category < 0 || category >= ePathCategoryCount || !path)
        return;
    m_Paths[category] = path;
}

const char *CGameConfig::GetPath(PathCategory category) const
{
    if (category < 0 || category >= ePathCategoryCount)
        return NULL;
    return m_Paths[category].c_str();
}

bool CGameConfig::HasPath(PathCategory category) const
{
    if (category < 0 || category >= ePathCategoryCount)
        return false;
    return m_Paths[category].size() != 0;
}

void CGameConfig::LoadFromIni(const char *filename)
{
    if (!filename)
        return;

    if (filename[0] == '\0')
    {
        if (m_Paths[eConfigPath].size() == 0 || !utils::FileOrDirectoryExists(m_Paths[eConfigPath].c_str()))
            return;
        filename = m_Paths[eConfigPath].c_str();
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename))
    {
        utils::GetCurrentPath(path, MAX_PATH);
        utils::ConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }

    utils::IniGetInteger("Startup", "LogMode", logMode, filename);
    utils::IniGetBoolean("Startup", "Verbose", verbose, filename);
    utils::IniGetBoolean("Startup", "ManualSetup", manualSetup, filename);
    utils::IniGetBoolean("Startup", "LoadAllManagers", loadAllManagers, filename);
    utils::IniGetBoolean("Startup", "LoadAllBuildingBlocks", loadAllBuildingBlocks, filename);
    utils::IniGetBoolean("Startup", "LoadAllPlugins", loadAllPlugins, filename);

    utils::IniGetInteger("Graphics", "Driver", driver, filename);
    utils::IniGetInteger("Graphics", "BitsPerPixel", bpp, filename);
    utils::IniGetInteger("Graphics", "Width", width, filename);
    utils::IniGetInteger("Graphics", "Height", height, filename);
    utils::IniGetBoolean("Graphics", "FullScreen", fullscreen, filename);

    utils::IniGetBoolean("Graphics", "DisablePerspectiveCorrection", disablePerspectiveCorrection, filename);
    utils::IniGetBoolean("Graphics", "ForceLinearFog", forceLinearFog, filename);
    utils::IniGetBoolean("Graphics", "ForceSoftware", forceSoftware, filename);
    utils::IniGetBoolean("Graphics", "DisableFilter", disableFilter, filename);
    utils::IniGetBoolean("Graphics", "EnsureVertexShader", ensureVertexShader, filename);
    utils::IniGetBoolean("Graphics", "UseIndexBuffers", useIndexBuffers, filename);
    utils::IniGetBoolean("Graphics", "DisableDithering", disableDithering, filename);
    utils::IniGetInteger("Graphics", "Antialias", antialias, filename);
    utils::IniGetBoolean("Graphics", "DisableMipmap", disableMipmap, filename);
    utils::IniGetBoolean("Graphics", "DisableSpecular", disableSpecular, filename);
    utils::IniGetBoolean("Graphics", "EnableScreenDump", enableScreenDump, filename);
    utils::IniGetBoolean("Graphics", "EnableDebugMode", enableDebugMode, filename);
    utils::IniGetInteger("Graphics", "VertexCache", vertexCache, filename);
    utils::IniGetBoolean("Graphics", "TextureCacheManagement", textureCacheManagement, filename);
    utils::IniGetBoolean("Graphics", "SortTransparentObjects", sortTransparentObjects, filename);
    utils::IniGetPixelFormat("Graphics", "TextureVideoFormat", textureVideoFormat, filename);
    utils::IniGetPixelFormat("Graphics", "SpriteVideoFormat", spriteVideoFormat, filename);

    utils::IniGetBoolean("Window", "ChildWindowRendering", childWindowRendering, filename);
    utils::IniGetBoolean("Window", "Borderless", borderless, filename);
    utils::IniGetBoolean("Window", "ClipCursor", clipCursor, filename);
    utils::IniGetBoolean("Window", "AlwaysHandleInput", alwaysHandleInput, filename);
    utils::IniGetInteger("Window", "X", posX, filename);
    utils::IniGetInteger("Window", "Y", posY, filename);

    utils::IniGetInteger("Game", "Language", langId, filename);
    utils::IniGetBoolean("Game", "SkipOpening", skipOpening, filename);
    utils::IniGetBoolean("Game", "ApplyHotfix", applyHotfix, filename);
    utils::IniGetBoolean("Game", "UnlockFramerate", unlockFramerate, filename);
    utils::IniGetBoolean("Game", "UnlockWidescreen", unlockWidescreen, filename);
    utils::IniGetBoolean("Game", "UnlockHighResolution", unlockHighResolution, filename);
    utils::IniGetBoolean("Game", "Debug", debug, filename);
    utils::IniGetBoolean("Game", "Rookie", rookie, filename);
}

void CGameConfig::SaveToIni(const char *filename)
{
    if (!filename)
        return;

    if (filename[0] == '\0')
    {
        if (m_Paths[eConfigPath].size() == 0)
            return;
        filename = m_Paths[eConfigPath].c_str();
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename))
    {
        utils::GetCurrentPath(path, MAX_PATH);
        utils::ConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }

    utils::IniSetInteger("Startup", "LogMode", logMode, filename);
    utils::IniSetBoolean("Startup", "Verbose", verbose, filename);
    utils::IniSetBoolean("Startup", "ManualSetup", manualSetup, filename);
    utils::IniSetBoolean("Startup", "LoadAllManagers", loadAllManagers, filename);
    utils::IniSetBoolean("Startup", "LoadAllBuildingBlocks", loadAllBuildingBlocks, filename);
    utils::IniSetBoolean("Startup", "LoadAllPlugins", loadAllPlugins, filename);

    utils::IniSetInteger("Graphics", "Driver", driver, filename);
    utils::IniSetInteger("Graphics", "BitsPerPixel", bpp, filename);
    utils::IniSetInteger("Graphics", "Width", width, filename);
    utils::IniSetInteger("Graphics", "Height", height, filename);
    utils::IniSetBoolean("Graphics", "FullScreen", fullscreen, filename);

    utils::IniSetBoolean("Graphics", "DisablePerspectiveCorrection", disablePerspectiveCorrection, filename);
    utils::IniSetBoolean("Graphics", "ForceLinearFog", forceLinearFog, filename);
    utils::IniSetBoolean("Graphics", "ForceSoftware", forceSoftware, filename);
    utils::IniSetBoolean("Graphics", "DisableFilter", disableFilter, filename);
    utils::IniSetBoolean("Graphics", "EnsureVertexShader", ensureVertexShader, filename);
    utils::IniSetBoolean("Graphics", "UseIndexBuffers", useIndexBuffers, filename);
    utils::IniSetBoolean("Graphics", "DisableDithering", disableDithering, filename);
    utils::IniSetInteger("Graphics", "Antialias", antialias, filename);
    utils::IniSetBoolean("Graphics", "DisableMipmap", disableMipmap, filename);
    utils::IniSetBoolean("Graphics", "DisableSpecular", disableSpecular, filename);
    utils::IniSetBoolean("Graphics", "EnableScreenDump", enableScreenDump, filename);
    utils::IniSetBoolean("Graphics", "EnableDebugMode", enableDebugMode, filename);
    utils::IniSetInteger("Graphics", "VertexCache", vertexCache, filename);
    utils::IniSetBoolean("Graphics", "TextureCacheManagement", textureCacheManagement, filename);
    utils::IniSetBoolean("Graphics", "SortTransparentObjects", sortTransparentObjects, filename);
    utils::IniSetPixelFormat("Graphics", "TextureVideoFormat", textureVideoFormat, filename);
    utils::IniSetPixelFormat("Graphics", "SpriteVideoFormat", spriteVideoFormat, filename);

    utils::IniSetBoolean("Window", "ChildWindowRendering", childWindowRendering, filename);
    utils::IniSetBoolean("Window", "Borderless", borderless, filename);
    utils::IniSetBoolean("Window", "ClipCursor", clipCursor, filename);
    utils::IniSetBoolean("Window", "AlwaysHandleInput", alwaysHandleInput, filename);
    utils::IniSetInteger("Window", "X", posX, filename);
    utils::IniSetInteger("Window", "Y", posY, filename);

    utils::IniSetInteger("Game", "Language", langId, filename);
    utils::IniSetBoolean("Game", "SkipOpening", skipOpening, filename);
    utils::IniSetBoolean("Game", "ApplyHotfix", applyHotfix, filename);
    utils::IniSetBoolean("Game", "UnlockFramerate", unlockFramerate, filename);
    utils::IniSetBoolean("Game", "UnlockWidescreen", unlockWidescreen, filename);
    utils::IniSetBoolean("Game", "UnlockHighResolution", unlockHighResolution, filename);
}