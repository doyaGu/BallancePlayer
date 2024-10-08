#include "GameConfig.h"

#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include "CmdlineParser.h"
#include "Utils.h"

static void ParseConfigsFromCmdline(CmdlineParser &parser, CGameConfig &config)
{
    CmdlineArg arg;
    long value = 0;
    XString str;

    while (!parser.Done())
    {
        if (parser.Next(arg, "--verbose", '\0'))
        {
            config.verbose = true;
            continue;
        }
        if (parser.Next(arg, "--manual-setup", 'm'))
        {
            config.manualSetup = true;
            continue;
        }
        if (parser.Next(arg, "--load-required-managers", '\0'))
        {
            config.loadAllManagers = false;
            continue;
        }
        if (parser.Next(arg, "--load-required-building-blocks", '\0'))
        {
            config.loadAllBuildingBlocks = false;
            continue;
        }
        if (parser.Next(arg, "--load-required-plugins", '\0'))
        {
            config.loadAllPlugins = false;
            continue;
        }
        if (parser.Next(arg, "--video-driver", 'v', 1))
        {
            if (arg.GetValue(0, value))
                config.driver = value;
            continue;
        }
        if (parser.Next(arg, "--bpp", 'b', 1))
        {
            if (arg.GetValue(0, value))
                config.bpp = value;
            continue;
        }
        if (parser.Next(arg, "--width", 'w', 1))
        {
            if (arg.GetValue(0, value))
                config.width = value;
            continue;
        }
        if (parser.Next(arg, "--height", 'h', 1))
        {
            if (arg.GetValue(0, value))
                config.height = value;
            continue;
        }
        if (parser.Next(arg, "--fullscreen", 'f'))
        {
            config.fullscreen = true;
            continue;
        }
        if (parser.Next(arg, "--disable--perspective-correction", '\0'))
        {
            config.disablePerspectiveCorrection = true;
            continue;
        }
        if (parser.Next(arg, "--force-linear-fog", '\0'))
        {
            config.forceLinearFog = true;
            continue;
        }
        if (parser.Next(arg, "--force-software", '\0'))
        {
            config.forceSoftware = true;
            continue;
        }
        if (parser.Next(arg, "--disable-filter", '\0'))
        {
            config.disableFilter = true;
            continue;
        }
        if (parser.Next(arg, "--ensure-vertex-shader", '\0'))
        {
            config.ensureVertexShader = true;
            continue;
        }
        if (parser.Next(arg, "--use-index-buffers", '\0'))
        {
            config.useIndexBuffers = true;
            continue;
        }
        if (parser.Next(arg, "--disable-dithering", '\0'))
        {
            config.disableDithering = true;
            continue;
        }
        if (parser.Next(arg, "--antialias", '\0', 1))
        {
            if (arg.GetValue(0, value))
                config.antialias = value;
            continue;
        }
        if (parser.Next(arg, "--disable-mipmap", '\0'))
        {
            config.disableMipmap = true;
            continue;
        }
        if (parser.Next(arg, "--disable-specular", '\0'))
        {
            config.disableSpecular = true;
            continue;
        }
        if (parser.Next(arg, "--enable-screen-dump", '\0'))
        {
            config.enableScreenDump = true;
            continue;
        }
        if (parser.Next(arg, "--enable-debug-mode", '\0'))
        {
            config.enableDebugMode = true;
            continue;
        }
        if (parser.Next(arg, "--vertex-cache", '\0', 1))
        {
            if (arg.GetValue(0, value))
                config.vertexCache = value;
            continue;
        }
        if (parser.Next(arg, "--disable-texture-cache-management", 's'))
        {
            config.textureCacheManagement = false;
            continue;
        }
        if (parser.Next(arg, "--disable-sort-transparent-objects", 's'))
        {
            config.sortTransparentObjects = false;
            continue;
        }
        if (parser.Next(arg, "--texture-video-format", '\0', 1))
        {
            if (arg.GetValue(0, str))
                config.textureVideoFormat = utils::String2PixelFormat(str.CStr(), 16);
            break;
        }
        if (parser.Next(arg, "--sprite-video-format", '\0', 1))
        {
            if (arg.GetValue(0, str))
                config.spriteVideoFormat = utils::String2PixelFormat(str.CStr(), 16);
            break;
        }
        if (parser.Next(arg, "--child-window-rendering", 's'))
        {
            config.childWindowRendering = true;
            continue;
        }
        if (parser.Next(arg, "--borderless", 'c'))
        {
            config.borderless = true;
            continue;
        }
        if (parser.Next(arg, "--clip-cursor", '\0'))
        {
            config.clipCursor = true;
            continue;
        }
        if (parser.Next(arg, "--always-handle-input", '\0'))
        {
            config.alwaysHandleInput = true;
            continue;
        }
        if (parser.Next(arg, "--pause-on-deactivated", 'p'))
        {
            config.pauseOnDeactivated = true;
            continue;
        }
        if (parser.Next(arg, "--position-x", 'x', 1))
        {
            if (arg.GetValue(0, value))
                config.posX = value;
            continue;
        }
        if (parser.Next(arg, "--position-y", 'y', 1))
        {
            if (arg.GetValue(0, value))
                config.posY = value;
            continue;
        }
        if (parser.Next(arg, "--lang", 'l'))
        {
            if (arg.GetValue(0, value))
                config.langId = value;
            continue;
        }
        if (parser.Next(arg, "--skip-opening", '\0'))
        {
            config.skipOpening = true;
            continue;
        }
        if (parser.Next(arg, "--disable-hotfix", '\0'))
        {
            config.applyHotfix = false;
            continue;
        }
        if (parser.Next(arg, "--unlock-framerate", 'u'))
        {
            config.unlockFramerate = true;
            continue;
        }
        if (parser.Next(arg, "--unlock-widescreen", '\0'))
        {
            config.unlockWidescreen = true;
            continue;
        }
        if (parser.Next(arg, "--unlock-high-resolution", '\0'))
        {
            config.unlockHighResolution = true;
            continue;
        }
        if (parser.Next(arg, "--debug", 'd'))
        {
            config.debug = true;
            continue;
        }
        if (parser.Next(arg, "--rookie", 'r'))
        {
            config.rookie = true;
            continue;
        }
        parser.Skip();
    }
    parser.Reset();
}

static bool IniGetString(const char *section, const char *name, char *str, int size, const char *filename)
{
    return ::GetPrivateProfileStringA(section, name, "", str, size, filename) != 0;
}

static bool IniGetInteger(const char *section, const char *name, int &value, const char *filename)
{
    char buf[512];
    ::GetPrivateProfileStringA(section, name, "", buf, 512, filename);
    if (strcmp(buf, "") == 0)
        return false;
    int val = strtol(buf, NULL, 10);
    if (val == 0 && strcmp(buf, "0") != 0)
        return false;
    value = val;
    return true;
}

static bool IniGetBoolean(const char *section, const char *name, bool &value, const char *filename)
{
    UINT val = ::GetPrivateProfileIntA(section, name, -1, filename);
    if (val == -1)
        return false;
    value = (val == 1);
    return true;
}

static bool IniGetPixelFormat(const char *section, const char *name, VX_PIXELFORMAT &value, const char *filename)
{
    char buf[16];
    ::GetPrivateProfileStringA(section, name, "", buf, 16, filename);
    if (strcmp(buf, "") == 0)
        return false;

    value = utils::String2PixelFormat(buf, sizeof(buf));
    return true;
}

static bool IniSetString(const char *section, const char *name, const char *str, const char *filename)
{
    return ::WritePrivateProfileStringA(section, name, str, filename) != 0;
}

static bool IniSetInteger(const char *section, const char *name, int value, const char *filename)
{
    char buf[64];
    sprintf(buf, "%d", value);
    return ::WritePrivateProfileStringA(section, name, buf, filename) != 0;
}

static bool IniSetBoolean(const char *section, const char *name, bool value, const char *filename)
{
    const char *buf = (value) ? "1" : "0";
    return ::WritePrivateProfileStringA(section, name, buf, filename) != 0;
}

static bool IniSetPixelFormat(const char *section, const char *name, VX_PIXELFORMAT value, const char *filename)
{
    return ::WritePrivateProfileStringA(section, name, utils::PixelFormat2String(value), filename) != 0;
}

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
    pauseOnDeactivated = false;
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
    loadAllBuildingBlocks = config.loadAllPlugins;
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
    pauseOnDeactivated = config.pauseOnDeactivated;
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
    return m_Paths[category].CStr();
}

bool CGameConfig::HasPath(PathCategory category) const
{
    if (category < 0 || category >= ePathCategoryCount)
        return false;
    return m_Paths[category].Length() != 0;
}

void CGameConfig::LoadFromCmdline(CmdlineParser &parser)
{
    ParseConfigsFromCmdline(parser, *this);
}

void CGameConfig::LoadPathsFromCmdline(CmdlineParser &parser)
{
    CmdlineArg arg;
    XString path;
    while (!parser.Done())
    {
        if (parser.Next(arg, "--config", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eConfigPath] = path;
            break;
        }
        if (parser.Next(arg, "--log", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eLogPath] = path;
            continue;
        }
        if (parser.Next(arg, "--cmo", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eCmoPath] = path;
            continue;
        }
        if (parser.Next(arg, "--root-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eRootPath] = path;
            continue;
        }
        if (parser.Next(arg, "--plugin-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[ePluginPath] = path;
            continue;
        }
        if (parser.Next(arg, "--render-engine-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eRenderEnginePath] = path;
            continue;
        }
        if (parser.Next(arg, "--manager-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eManagerPath] = path;
            continue;
        }
        if (parser.Next(arg, "--building-block-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eBuildingBlockPath] = path;
            continue;
        }
        if (parser.Next(arg, "--sound-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eSoundPath] = path;
            continue;
        }
        if (parser.Next(arg, "--bitmap-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eBitmapPath] = path;
            continue;
        }
        if (parser.Next(arg, "--data-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                m_Paths[eDataPath] = path;
            continue;
        }
        parser.Skip();
    }
    parser.Reset();
}

void CGameConfig::LoadFromIni(const char *filename)
{
    if (!filename)
        return;

    if (filename[0] == '\0')
    {
        if (m_Paths[eConfigPath].Length() == 0 || !utils::FileOrDirectoryExists(m_Paths[eConfigPath].CStr()))
            return;
        filename = m_Paths[eConfigPath].CStr();
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename))
    {
        ::GetCurrentDirectoryA(MAX_PATH, path);
        utils::ConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }

    IniGetInteger("Startup", "LogMode", logMode, filename);
    IniGetBoolean("Startup", "Verbose", verbose, filename);
    IniGetBoolean("Startup", "ManualSetup", manualSetup, filename);
    IniGetBoolean("Startup", "LoadAllManagers", loadAllManagers, filename);
    IniGetBoolean("Startup", "LoadAllBuildingBlocks", loadAllBuildingBlocks, filename);
    IniGetBoolean("Startup", "LoadAllPlugins", loadAllPlugins, filename);

    IniGetInteger("Graphics", "Driver", driver, filename);
    IniGetInteger("Graphics", "BitsPerPixel", bpp, filename);
    IniGetInteger("Graphics", "Width", width, filename);
    IniGetInteger("Graphics", "Height", height, filename);
    IniGetBoolean("Graphics", "FullScreen", fullscreen, filename);

    IniGetBoolean("Graphics", "DisablePerspectiveCorrection", disablePerspectiveCorrection, filename);
    IniGetBoolean("Graphics", "ForceLinearFog", forceLinearFog, filename);
    IniGetBoolean("Graphics", "ForceSoftware", forceSoftware, filename);
    IniGetBoolean("Graphics", "DisableFilter", disableFilter, filename);
    IniGetBoolean("Graphics", "EnsureVertexShader", ensureVertexShader, filename);
    IniGetBoolean("Graphics", "UseIndexBuffers", useIndexBuffers, filename);
    IniGetBoolean("Graphics", "DisableDithering", disableDithering, filename);
    IniGetInteger("Graphics", "Antialias", antialias, filename);
    IniGetBoolean("Graphics", "DisableMipmap", disableMipmap, filename);
    IniGetBoolean("Graphics", "DisableSpecular", disableSpecular, filename);
    IniGetBoolean("Graphics", "EnableScreenDump", enableScreenDump, filename);
    IniGetBoolean("Graphics", "EnableDebugMode", enableDebugMode, filename);
    IniGetInteger("Graphics", "VertexCache", vertexCache, filename);
    IniGetBoolean("Graphics", "TextureCacheManagement", textureCacheManagement, filename);
    IniGetBoolean("Graphics", "SortTransparentObjects", sortTransparentObjects, filename);
    IniGetPixelFormat("Graphics", "TextureVideoFormat", textureVideoFormat, filename);
    IniGetPixelFormat("Graphics", "SpriteVideoFormat", spriteVideoFormat, filename);

    IniGetBoolean("Window", "ChildWindowRendering", childWindowRendering, filename);
    IniGetBoolean("Window", "Borderless", borderless, filename);
    IniGetBoolean("Window", "ClipCursor", clipCursor, filename);
    IniGetBoolean("Window", "AlwaysHandleInput", alwaysHandleInput, filename);
    IniGetBoolean("Window", "PauseOnDeactivated", pauseOnDeactivated, filename);
    IniGetInteger("Window", "X", posX, filename);
    IniGetInteger("Window", "Y", posY, filename);

    IniGetInteger("Game", "Language", langId, filename);
    IniGetBoolean("Game", "SkipOpening", skipOpening, filename);
    IniGetBoolean("Game", "ApplyHotfix", applyHotfix, filename);
    IniGetBoolean("Game", "UnlockFramerate", unlockFramerate, filename);
    IniGetBoolean("Game", "UnlockWidescreen", unlockWidescreen, filename);
    IniGetBoolean("Game", "UnlockHighResolution", unlockHighResolution, filename);
    IniGetBoolean("Game", "Debug", debug, filename);
    IniGetBoolean("Game", "Rookie", rookie, filename);
}

void CGameConfig::SaveToIni(const char *filename)
{
    if (!filename)
        return;

    if (filename[0] == '\0')
    {
        if (m_Paths[eConfigPath].Length() == 0)
            return;
        filename = m_Paths[eConfigPath].CStr();
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename))
    {
        ::GetCurrentDirectoryA(MAX_PATH, path);
        utils::ConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }

    IniSetInteger("Startup", "LogMode", logMode, filename);
    IniSetBoolean("Startup", "Verbose", verbose, filename);
    IniSetBoolean("Startup", "ManualSetup", manualSetup, filename);
    IniSetBoolean("Startup", "LoadAllManagers", loadAllManagers, filename);
    IniSetBoolean("Startup", "LoadAllBuildingBlocks", loadAllBuildingBlocks, filename);
    IniSetBoolean("Startup", "LoadAllPlugins", loadAllPlugins, filename);

    IniSetInteger("Graphics", "Driver", driver, filename);
    IniSetInteger("Graphics", "BitsPerPixel", bpp, filename);
    IniSetInteger("Graphics", "Width", width, filename);
    IniSetInteger("Graphics", "Height", height, filename);
    IniSetBoolean("Graphics", "FullScreen", fullscreen, filename);

    IniSetBoolean("Graphics", "DisablePerspectiveCorrection", disablePerspectiveCorrection, filename);
    IniSetBoolean("Graphics", "ForceLinearFog", forceLinearFog, filename);
    IniSetBoolean("Graphics", "ForceSoftware", forceSoftware, filename);
    IniSetBoolean("Graphics", "DisableFilter", disableFilter, filename);
    IniSetBoolean("Graphics", "EnsureVertexShader", ensureVertexShader, filename);
    IniSetBoolean("Graphics", "UseIndexBuffers", useIndexBuffers, filename);
    IniSetBoolean("Graphics", "DisableDithering", disableDithering, filename);
    IniSetInteger("Graphics", "Antialias", antialias, filename);
    IniSetBoolean("Graphics", "DisableMipmap", disableMipmap, filename);
    IniSetBoolean("Graphics", "DisableSpecular", disableSpecular, filename);
    IniSetBoolean("Graphics", "EnableScreenDump", enableScreenDump, filename);
    IniSetBoolean("Graphics", "EnableDebugMode", enableDebugMode, filename);
    IniSetInteger("Graphics", "VertexCache", vertexCache, filename);
    IniSetBoolean("Graphics", "TextureCacheManagement", textureCacheManagement, filename);
    IniSetBoolean("Graphics", "SortTransparentObjects", sortTransparentObjects, filename);
    IniSetPixelFormat("Graphics", "TextureVideoFormat", textureVideoFormat, filename);
    IniSetPixelFormat("Graphics", "SpriteVideoFormat", spriteVideoFormat, filename);

    IniSetBoolean("Window", "ChildWindowRendering", childWindowRendering, filename);
    IniSetBoolean("Window", "Borderless", borderless, filename);
    IniSetBoolean("Window", "ClipCursor", clipCursor, filename);
    IniSetBoolean("Window", "AlwaysHandleInput", alwaysHandleInput, filename);
    IniSetBoolean("Window", "PauseOnDeactivated", pauseOnDeactivated, filename);
    IniSetInteger("Window", "X", posX, filename);
    IniSetInteger("Window", "Y", posY, filename);

    IniSetInteger("Game", "Language", langId, filename);
    IniSetBoolean("Game", "SkipOpening", skipOpening, filename);
    IniSetBoolean("Game", "ApplyHotfix", applyHotfix, filename);
    IniSetBoolean("Game", "UnlockFramerate", unlockFramerate, filename);
    IniSetBoolean("Game", "UnlockWidescreen", unlockWidescreen, filename);
    IniSetBoolean("Game", "UnlockHighResolution", unlockHighResolution, filename);
}