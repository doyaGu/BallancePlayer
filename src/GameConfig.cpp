#include "GameConfig.h"

#include <cstdio>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <cxxopts.hpp>

#include "Utils.h"

void AddCommandLineOptions(cxxopts::Options &options) {
    options.add_options()
            ("verbose", "Enable verbose mode")
            ("m,manual-setup", "Enable manual setup")
            ("load-required-managers", "Load required managers only")
            ("load-required-building-blocks", "Load required building blocks only")
            ("load-required-plugins", "Load required plugins only")
            ("video-driver", "Set video driver", cxxopts::value<long>())
            ("bpp", "Set bits per pixel", cxxopts::value<long>())
            ("width", "Set window width", cxxopts::value<long>())
            ("height", "Set window height", cxxopts::value<long>())
            ("fullscreen", "Enable fullscreen mode")
            ("disable-perspective-correction", "Disable perspective correction")
            ("force-linear-fog", "Force linear fog")
            ("force-software", "Force software rendering")
            ("disable-filter", "Disable filter")
            ("ensure-vertex-shader", "Ensure vertex shader")
            ("use-index-buffers", "Use index buffers")
            ("disable-dithering", "Disable dithering")
            ("antialias", "Set antialias level", cxxopts::value<long>())
            ("disable-mipmap", "Disable mipmap")
            ("disable-specular", "Disable specular")
            ("enable-screen-dump", "Enable screen dump")
            ("enable-debug-mode", "Enable debug mode")
            ("vertex-cache", "Set vertex cache size", cxxopts::value<long>())
            ("disable-texture-cache-management", "Disable texture cache management")
            ("disable-sort-transparent-objects", "Disable sort transparent objects")
            ("texture-video-format", "Set texture video format", cxxopts::value<std::string>())
            ("sprite-video-format", "Set sprite video format", cxxopts::value<std::string>())
            ("child-window-rendering", "Enable child window rendering")
            ("borderless", "Enable borderless mode")
            ("clip-cursor", "Enable clip cursor")
            ("always-handle-input", "Always handle input")
            ("pause-on-deactivated", "Pause on deactivated")
            ("x,position-x", "Set window position x", cxxopts::value<long>())
            ("y,position-y", "Set window position y", cxxopts::value<long>())
            ("l,lang", "Set language id", cxxopts::value<long>())
            ("skip-opening", "Skip opening")
            ("disable-hotfix", "Disable hotfix")
            ("unlock-framerate", "Unlock framerate")
            ("unlock-widescreen", "Unlock widescreen")
            ("unlock-high-resolution", "Unlock high resolution")
            ("debug", "Enable debug mode")
            ("rookie", "Enable rookie mode")
            ("config", "Set config file path", cxxopts::value<std::string>())
            ("log", "Set log file path", cxxopts::value<std::string>())
            ("cmo", "Set cmo file path", cxxopts::value<std::string>())
            ("root-path", "Set root path", cxxopts::value<std::string>())
            ("plugin-path", "Set plugin path", cxxopts::value<std::string>())
            ("render-engine-path", "Set render engine path", cxxopts::value<std::string>())
            ("manager-path", "Set manager path", cxxopts::value<std::string>())
            ("building-block-path", "Set building block path", cxxopts::value<std::string>())
            ("sound-path", "Set sound path", cxxopts::value<std::string>())
            ("bitmap-path", "Set bitmap path", cxxopts::value<std::string>())
            ("data-path", "Set data path", cxxopts::value<std::string>())
            ("v,version", "Print version")
            ("h,help", "Print usage");
}

void LoadConfigsFromCommandLine(GameConfig &config, const cxxopts::ParseResult &result) {
    if (result.contains("verbose"))
        config.verbose = result["verbose"].as<bool>();
    if (result.contains("manual-setup"))
        config.manualSetup = result["manual-setup"].as<bool>();
    if (result.contains("load-required-managers"))
        config.loadAllManagers = result["load-required-managers"].as<bool>();
    if (result.contains("load-required-building-blocks"))
        config.loadAllBuildingBlocks = result["load-required-building-blocks"].as<bool>();
    if (result.contains("load-required-plugins"))
        config.loadAllPlugins = result["load-required-plugins"].as<bool>();
    if (result.contains("video-driver"))
        config.driver = result["video-driver"].as<int>();
    if (result.contains("bpp"))
        config.bpp = result["bpp"].as<int>();
    if (result.contains("width"))
        config.width = result["width"].as<int>();
    if (result.contains("height"))
        config.height = result["height"].as<int>();
    if (result.contains("fullscreen"))
        config.fullscreen = result["fullscreen"].as<bool>();
    if (result.contains("disable-perspective-correction"))
        config.disablePerspectiveCorrection = result["disable-perspective-correction"].as<bool>();
    if (result.contains("force-linear-fog"))
        config.forceLinearFog = result["force-linear-fog"].as<bool>();
    if (result.contains("force-software"))
        config.forceSoftware = result["force-software"].as<bool>();
    if (result.contains("disable-filter"))
        config.disableFilter = result["disable-filter"].as<bool>();
    if (result.contains("ensure-vertex-shader"))
        config.ensureVertexShader = result["ensure-vertex-shader"].as<bool>();
    if (result.contains("use-index-buffers"))
        config.useIndexBuffers = result["use-index-buffers"].as<bool>();
    if (result.contains("disable-dithering"))
        config.disableDithering = result["disable-dithering"].as<bool>();
    if (result.contains("antialias"))
        config.antialias = result["antialias"].as<int>();
    if (result.contains("disable-mipmap"))
        config.disableMipmap = result["disable-mipmap"].as<bool>();
    if (result.contains("disable-specular"))
        config.disableSpecular = result["disable-specular"].as<bool>();
    if (result.contains("enable-screen-dump"))
        config.enableScreenDump = result["enable-screen-dump"].as<bool>();
    if (result.contains("enable-debug-mode"))
        config.enableDebugMode = result["enable-debug-mode"].as<bool>();
    if (result.contains("vertex-cache"))
        config.vertexCache = result["vertex-cache"].as<int>();
    if (result.contains("disable-texture-cache-management"))
        config.textureCacheManagement = result["disable-texture-cache-management"].as<bool>();
    if (result.contains("disable-sort-transparent-objects"))
        config.sortTransparentObjects = result["disable-sort-transparent-objects"].as<bool>();
    if (result.contains("texture-video-format")) {
        const auto &format = result["texture-video-format"].as<std::string>();
        config.textureVideoFormat = utils::String2PixelFormat(format.c_str(), format.length());
    }
    if (result.contains("sprite-video-format")) {
        const auto &format = result["sprite-video-format"].as<std::string>();
        config.spriteVideoFormat = utils::String2PixelFormat(format.c_str(), format.length());
    }
    if (result.contains("child-window-rendering"))
        config.childWindowRendering = result["child-window-rendering"].as<bool>();
    if (result.contains("borderless"))
        config.borderless = result["borderless"].as<bool>();
    if (result.contains("clip-cursor"))
        config.clipCursor = result["clip-cursor"].as<bool>();
    if (result.contains("always-handle-input"))
        config.alwaysHandleInput = result["always-handle-input"].as<bool>();
    if (result.contains("pause-on-deactivated"))
        config.pauseOnDeactivated = result["pause-on-deactivated"].as<bool>();
    if (result.contains("position-x"))
        config.posX = result["position-x"].as<int>();
    if (result.contains("position-y"))
        config.posY = result["position-y"].as<int>();
    if (result.contains("lang"))
        config.langId = result["lang"].as<int>();
    if (result.contains("skip-opening"))
        config.skipOpening = result["skip-opening"].as<bool>();
    if (result.contains("disable-hotfix"))
        config.applyHotfix = result["disable-hotfix"].as<bool>();
    if (result.contains("unlock-framerate"))
        config.unlockFramerate = result["unlock-framerate"].as<bool>();
    if (result.contains("unlock-widescreen"))
        config.unlockWidescreen = result["unlock-widescreen"].as<bool>();
    if (result.contains("unlock-high-resolution"))
        config.unlockHighResolution = result["unlock-high-resolution"].as<bool>();
    if (result.contains("debug"))
        config.debug = result["debug"].as<bool>();
    if (result.contains("rookie"))
        config.rookie = result["rookie"].as<bool>();
}

void LoadPathsFromCommandLine(GameConfig &config, const cxxopts::ParseResult &result) {
    if (result.contains("config"))
        config.SetPath(eConfigPath, result["config"].as<std::string>().c_str());
    if (result.contains("log"))
        config.SetPath(eLogPath, result["log"].as<std::string>().c_str());
    if (result.contains("cmo"))
        config.SetPath(eCmoPath, result["cmo"].as<std::string>().c_str());
    if (result.contains("root-path"))
        config.SetPath(eRootPath, result["root-path"].as<std::string>().c_str());
    if (result.contains("plugin-path"))
        config.SetPath(ePluginPath, result["plugin-path"].as<std::string>().c_str());
    if (result.contains("render-engine-path"))
        config.SetPath(eRenderEnginePath, result["render-engine-path"].as<std::string>().c_str());
    if (result.contains("manager-path"))
        config.SetPath(eManagerPath, result["manager-path"].as<std::string>().c_str());
    if (result.contains("building-block-path"))
        config.SetPath(eBuildingBlockPath, result["building-block-path"].as<std::string>().c_str());
    if (result.contains("sound-path"))
        config.SetPath(eSoundPath, result["sound-path"].as<std::string>().c_str());
    if (result.contains("bitmap-path"))
        config.SetPath(eBitmapPath, result["bitmap-path"].as<std::string>().c_str());
    if (result.contains("data-path"))
        config.SetPath(eDataPath, result["data-path"].as<std::string>().c_str());
}

static bool IniGetString(const char *section, const char *name, char *str, int size, const char *filename) {
    return ::GetPrivateProfileStringA(section, name, "", str, size, filename) != 0;
}

static bool IniGetInteger(const char *section, const char *name, int &value, const char *filename) {
    char buf[512];
    ::GetPrivateProfileStringA(section, name, "", buf, 512, filename);
    if (strcmp(buf, "") == 0)
        return false;
    const int val = strtol(buf, nullptr, 10);
    if (val == 0 && strcmp(buf, "0") != 0)
        return false;
    value = val;
    return true;
}

static bool IniGetBoolean(const char *section, const char *name, bool &value, const char *filename) {
    const UINT val = ::GetPrivateProfileIntA(section, name, -1, filename);
    if (val == -1)
        return false;
    value = (val == 1);
    return true;
}

static bool IniGetPixelFormat(const char *section, const char *name, VX_PIXELFORMAT &value, const char *filename) {
    char buf[16];
    ::GetPrivateProfileStringA(section, name, "", buf, 16, filename);
    if (strcmp(buf, "") == 0)
        return false;

    value = utils::String2PixelFormat(buf, sizeof(buf));
    return true;
}

static bool IniSetString(const char *section, const char *name, const char *str, const char *filename) {
    return ::WritePrivateProfileStringA(section, name, str, filename) != 0;
}

static bool IniSetInteger(const char *section, const char *name, int value, const char *filename) {
    char buf[64];
    sprintf(buf, "%d", value);
    return ::WritePrivateProfileStringA(section, name, buf, filename) != 0;
}

static bool IniSetBoolean(const char *section, const char *name, bool value, const char *filename) {
    const char *buf = (value) ? "1" : "0";
    return ::WritePrivateProfileStringA(section, name, buf, filename) != 0;
}

static bool IniSetPixelFormat(const char *section, const char *name, VX_PIXELFORMAT value, const char *filename) {
    return ::WritePrivateProfileStringA(section, name, utils::PixelFormat2String(value), filename) != 0;
}

GameConfig::GameConfig() {
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

GameConfig &GameConfig::operator=(const GameConfig &config) {
    if (this == &config)
        return *this;

    verbose = config.verbose;
    manualSetup = config.manualSetup;
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

void GameConfig::SetPath(PathCategory category, const char *path) {
    if (category < 0 || category >= ePathCategoryCount || !path)
        return;
    m_Paths[category] = path;
}

const char *GameConfig::GetPath(PathCategory category) const {
    if (category < 0 || category >= ePathCategoryCount)
        return nullptr;
    return m_Paths[category].CStr();
}

bool GameConfig::HasPath(PathCategory category) const {
    if (category < 0 || category >= ePathCategoryCount)
        return false;
    return m_Paths[category].Length() != 0;
}

void GameConfig::LoadFromIni(const char *filename) {
    if (!filename)
        return;

    if (filename[0] == '\0') {
        if (m_Paths[eConfigPath].Length() == 0 || !utils::FileOrDirectoryExists(m_Paths[eConfigPath].CStr()))
            return;
        filename = m_Paths[eConfigPath].CStr();
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename)) {
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

void GameConfig::SaveToIni(const char *filename) {
    if (!filename)
        return;

    if (filename[0] == '\0') {
        if (m_Paths[eConfigPath].Length() == 0)
            return;
        filename = m_Paths[eConfigPath].CStr();
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename)) {
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
