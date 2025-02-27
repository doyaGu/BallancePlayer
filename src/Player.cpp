#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include "bp/GamePlayer.h"
#include "bp/GameConfig.h"
#include "bp/Utils.h"
#include "Splash.h"
#include "CmdlineParser.h"
#include "ConsoleAttacher.h"
#include "FileLogger.h"
#include "LockGuard.h"

static HANDLE CreatNamedMutex();
static void ParseConfigsFromCmdline(CmdlineParser &parser, BpGameConfig *config);
static void ParsePathsFromCmdline(CmdlineParser &parser, BpGameConfig *config);
static void InitLogger(BpLogger *logger, BpGameConfig *config);

static FileLogger g_FileLogger;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HANDLE hMutex = CreatNamedMutex();
    if (!hMutex) {
        ::MessageBox(nullptr, TEXT("Another player is running!"), TEXT("Error"), MB_OK);
        return -1;
    }

    LockGuard guard(hMutex);

    ConsoleAttacher consoleAttacher;

    auto *logger = bpGetDefaultLogger();

    BpGamePlayer *player = bpCreateGamePlayer(nullptr);
    if (!player) {
        ::MessageBox(nullptr, TEXT("Failed to create player!"), TEXT("Error"), MB_OK);
        return -1;
    }

    BpGameConfig *config = bpGamePlayerGetConfig(player);

    CmdlineParser parser(__argc, __argv);
    ParsePathsFromCmdline(parser, config);

    if (bpFileOrDirectoryExists(bpGetGamePath(config, BP_PATH_CONFIG))) {
        if (!bpGameConfigLoad(config, nullptr)) {
            bpLoggerWarn(logger, "Failed to load game configuration from file!");
        }
    } else {
        if (!bpGameConfigSave(config, nullptr)) {
            bpLoggerWarn(logger, "Failed to save game configuration to file!");
        }
    }

    ParseConfigsFromCmdline(parser, config);

    InitLogger(logger, config);

    Splash splash(hInstance);
    splash.Show();

    if (!bpGamePlayerInit(player, hInstance)) {
        ::MessageBox(nullptr, TEXT("Failed to initialize player!"), TEXT("Error"), MB_OK);
        bpDestroyGamePlayer(player);
        return -1;
    }

    if (!bpGamePlayerLoad(player, nullptr)) {
        ::MessageBox(nullptr, TEXT("Failed to load game composition!"), TEXT("Error"), MB_OK);
        bpDestroyGamePlayer(player);
        return -1;
    }

    bpGamePlayerPlay(player);
    bpGamePlayerRun(player);
    bpGamePlayerShutdown(player);

    // Save settings before destroying the player
    bpGameConfigSave(config, nullptr);
    // Destroy the player
    bpDestroyGamePlayer(player);

    return 0;
}

static HANDLE CreatNamedMutex() {
    char buf[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    ::GetModuleFileNameA(nullptr, buf, MAX_PATH);
    _splitpath(buf, drive, dir, filename, nullptr);
    _snprintf(buf, MAX_PATH, "%s%s", drive, dir);

    size_t crc = 0;
    bpCRC32(buf, strlen(buf), 0, &crc);
    _snprintf(buf, MAX_PATH, "Ballance-%X", crc);
    HANDLE hMutex = ::CreateMutexA(nullptr, FALSE, buf);
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
        return nullptr;
    return hMutex;
}

static void ParseConfigsFromCmdline(CmdlineParser &parser, BpGameConfig *config) {
    if (!config)
        return;

    CmdlineArg arg;
    long value = 0;
    XString str;

    while (!parser.Done()) {
        if (parser.Next(arg, "--verbose", '\0')) {
            BpValue *verbose = bpGameConfigGetValue(config, BP_CONFIG_VERBOSE);
            if (verbose)
                *verbose = true;
            continue;
        }
        if (parser.Next(arg, "--manual-setup", 'm')) {
            BpValue *manualSetup = bpGameConfigGetValue(config, BP_CONFIG_MANUAL_SETUP);
            if (manualSetup)
                *manualSetup = true;
            continue;
        }
        if (parser.Next(arg, "--load-required-managers", '\0')) {
            BpValue *loadAllManagers = bpGameConfigGetValue(config, BP_CONFIG_LOAD_ALL_MANAGERS);
            if (loadAllManagers)
                *loadAllManagers = false;
            continue;
        }
        if (parser.Next(arg, "--load-required-building-blocks", '\0')) {
            BpValue *loadAllBuildingBlocks = bpGameConfigGetValue(config, BP_CONFIG_LOAD_ALL_BUILDING_BLOCKS);
            if (loadAllBuildingBlocks)
                *loadAllBuildingBlocks = false;
            continue;
        }
        if (parser.Next(arg, "--load-required-plugins", '\0')) {
            BpValue *loadAllPlugins = bpGameConfigGetValue(config, BP_CONFIG_LOAD_ALL_PLUGINS);
            if (loadAllPlugins)
                *loadAllPlugins = false;
            continue;
        }
        if (parser.Next(arg, "--video-driver", 'v', 1)) {
            if (arg.GetValue(0, value)) {
                BpValue *driver = bpGameConfigGetValue(config, BP_CONFIG_DRIVER);
                if (driver)
                    *driver = value;
            }
            continue;
        }
        if (parser.Next(arg, "--bpp", 'b', 1)) {
            if (arg.GetValue(0, value)) {
                BpValue *bpp = bpGameConfigGetValue(config, BP_CONFIG_BPP);
                if (bpp)
                    *bpp = value;
            }
            continue;
        }
        if (parser.Next(arg, "--width", 'w', 1)) {
            if (arg.GetValue(0, value)) {
                BpValue *width = bpGameConfigGetValue(config, BP_CONFIG_WIDTH);
                if (width)
                    *width = value;
            }
            continue;
        }
        if (parser.Next(arg, "--height", 'h', 1)) {
            if (arg.GetValue(0, value)) {
                BpValue *height = bpGameConfigGetValue(config, BP_CONFIG_HEIGHT);
                if (height)
                    *height = value;
            }
            continue;
        }
        if (parser.Next(arg, "--fullscreen", 'f')) {
            BpValue *fullscreen = bpGameConfigGetValue(config, BP_CONFIG_FULLSCREEN);
            if (fullscreen)
                *fullscreen = true;
            continue;
        }
        if (parser.Next(arg, "--disable--perspective-correction", '\0')) {
            BpValue *disablePerspectiveCorrection = bpGameConfigGetValue(
                config, BP_CONFIG_DISABLE_PERSPECTIVE_CORRECTION);
            if (disablePerspectiveCorrection)
                *disablePerspectiveCorrection = true;
            continue;
        }
        if (parser.Next(arg, "--force-linear-fog", '\0')) {
            BpValue *forceLinearFog = bpGameConfigGetValue(config, BP_CONFIG_FORCE_LINEAR_FOG);
            if (forceLinearFog)
                *forceLinearFog = true;
            continue;
        }
        if (parser.Next(arg, "--force-software", '\0')) {
            BpValue *forceSoftware = bpGameConfigGetValue(config, BP_CONFIG_FORCE_SOFTWARE);
            if (forceSoftware)
                *forceSoftware = true;
            continue;
        }
        if (parser.Next(arg, "--disable-filter", '\0')) {
            BpValue *disableFilter = bpGameConfigGetValue(config, BP_CONFIG_DISABLE_FILTER);
            if (disableFilter)
                *disableFilter = true;
            continue;
        }
        if (parser.Next(arg, "--ensure-vertex-shader", '\0')) {
            BpValue *ensureVertexShader = bpGameConfigGetValue(config, BP_CONFIG_ENSURE_VERTEX_SHADER);
            if (ensureVertexShader)
                *ensureVertexShader = true;
            continue;
        }
        if (parser.Next(arg, "--use-index-buffers", '\0')) {
            BpValue *useIndexBuffers = bpGameConfigGetValue(config, BP_CONFIG_USE_INDEX_BUFFERS);
            if (useIndexBuffers)
                *useIndexBuffers = true;
            continue;
        }
        if (parser.Next(arg, "--disable-dithering", '\0')) {
            BpValue *disableDithering = bpGameConfigGetValue(config, BP_CONFIG_DISABLE_DITHERING);
            if (disableDithering)
                *disableDithering = true;
            continue;
        }
        if (parser.Next(arg, "--antialias", '\0', 1)) {
            if (arg.GetValue(0, value)) {
                BpValue *antialias = bpGameConfigGetValue(config, BP_CONFIG_ANTIALIAS);
                if (antialias)
                    *antialias = value;
            }
            continue;
        }
        if (parser.Next(arg, "--disable-mipmap", '\0')) {
            BpValue *disableMipmap = bpGameConfigGetValue(config, BP_CONFIG_DISABLE_MIPMAP);
            if (disableMipmap)
                *disableMipmap = true;
            continue;
        }
        if (parser.Next(arg, "--disable-specular", '\0')) {
            BpValue *disableSpecular = bpGameConfigGetValue(config, BP_CONFIG_DISABLE_SPECULAR);
            if (disableSpecular)
                *disableSpecular = true;
            continue;
        }
        if (parser.Next(arg, "--enable-screen-dump", '\0')) {
            BpValue *enableScreenDump = bpGameConfigGetValue(config, BP_CONFIG_ENABLE_SCREEN_DUMP);
            if (enableScreenDump)
                *enableScreenDump = true;
            continue;
        }
        if (parser.Next(arg, "--enable-debug-mode", '\0')) {
            BpValue *enableDebugMode = bpGameConfigGetValue(config, BP_CONFIG_ENABLE_DEBUG_MODE);
            if (enableDebugMode)
                *enableDebugMode = true;
            continue;
        }
        if (parser.Next(arg, "--vertex-cache", '\0', 1)) {
            if (arg.GetValue(0, value)) {
                BpValue *vertexCache = bpGameConfigGetValue(config, BP_CONFIG_VERTEX_CACHE);
                if (vertexCache)
                    *vertexCache = value;
            }
            continue;
        }
        if (parser.Next(arg, "--disable-texture-cache-management", 's')) {
            BpValue *disableTextureCacheManagement = bpGameConfigGetValue(config, BP_CONFIG_TEXTURE_CACHE_MANAGEMENT);
            if (disableTextureCacheManagement)
                *disableTextureCacheManagement = false;
            continue;
        }
        if (parser.Next(arg, "--disable-sort-transparent-objects", 's')) {
            BpValue *disableSortTransparentObjects = bpGameConfigGetValue(config, BP_CONFIG_SORT_TRANSPARENT_OBJECTS);
            if (disableSortTransparentObjects)
                *disableSortTransparentObjects = false;
            continue;
        }
        if (parser.Next(arg, "--texture-video-format", '\0', 1)) {
            if (arg.GetValue(0, str)) {
                BpValue *textureVideoFormat = bpGameConfigGetValue(config, BP_CONFIG_TEXTURE_VIDEO_FORMAT);
                if (textureVideoFormat)
                    *textureVideoFormat = bpString2PixelFormat(str.CStr());
            }
            break;
        }
        if (parser.Next(arg, "--sprite-video-format", '\0', 1)) {
            if (arg.GetValue(0, str)) {
                BpValue *spriteVideoFormat = bpGameConfigGetValue(config, BP_CONFIG_SPRITE_VIDEO_FORMAT);
                if (spriteVideoFormat)
                    *spriteVideoFormat = bpString2PixelFormat(str.CStr());
            }
            break;
        }
        if (parser.Next(arg, "--child-window-rendering", 's')) {
            BpValue *childWindowRendering = bpGameConfigGetValue(config, BP_CONFIG_CHILD_WINDOW_RENDERING);
            if (childWindowRendering)
                *childWindowRendering = true;
            continue;
        }
        if (parser.Next(arg, "--borderless", 'c')) {
            BpValue *borderless = bpGameConfigGetValue(config, BP_CONFIG_BORDERLESS);
            if (borderless)
                *borderless = true;
            continue;
        }
        if (parser.Next(arg, "--clip-cursor", '\0')) {
            BpValue *clipCursor = bpGameConfigGetValue(config, BP_CONFIG_CLIP_CURSOR);
            if (clipCursor)
                *clipCursor = true;
            continue;
        }
        if (parser.Next(arg, "--always-handle-input", '\0')) {
            BpValue *alwaysHandleInput = bpGameConfigGetValue(config, BP_CONFIG_ALWAYS_HANDLE_INPUT);
            if (alwaysHandleInput)
                *alwaysHandleInput = true;
            continue;
        }
        if (parser.Next(arg, "--position-x", 'x', 1)) {
            if (arg.GetValue(0, value)) {
                BpValue *posX = bpGameConfigGetValue(config, BP_CONFIG_X);
                if (posX)
                    *posX = value;
            }
            continue;
        }
        if (parser.Next(arg, "--position-y", 'y', 1)) {
            if (arg.GetValue(0, value)) {
                BpValue *posY = bpGameConfigGetValue(config, BP_CONFIG_Y);
                if (posY)
                    *posY = value;
            }
            continue;
        }
        if (parser.Next(arg, "--lang", 'l')) {
            if (arg.GetValue(0, value)) {
                BpValue *langId = bpGameConfigGetValue(config, BP_CONFIG_LANG_ID);
                if (langId)
                    *langId = value;
            }
            continue;
        }
        if (parser.Next(arg, "--skip-opening", '\0')) {
            BpValue *skipOpening = bpGameConfigGetValue(config, BP_CONFIG_SKIP_OPENING);
            if (skipOpening)
                *skipOpening = true;
            continue;
        }
        if (parser.Next(arg, "--disable-hotfix", '\0')) {
            BpValue *applyHotfix = bpGameConfigGetValue(config, BP_CONFIG_APPLY_HOTFIX);
            if (applyHotfix)
                *applyHotfix = false;
            continue;
        }
        if (parser.Next(arg, "--unlock-framerate", 'u')) {
            BpValue *unlockFramerate = bpGameConfigGetValue(config, BP_CONFIG_UNLOCK_FRAMERATE);
            if (unlockFramerate)
                *unlockFramerate = true;
            continue;
        }
        if (parser.Next(arg, "--unlock-widescreen", '\0')) {
            BpValue *unlockWidescreen = bpGameConfigGetValue(config, BP_CONFIG_UNLOCK_WIDESCREEN);
            if (unlockWidescreen)
                *unlockWidescreen = true;
            continue;
        }
        if (parser.Next(arg, "--unlock-high-resolution", '\0')) {
            BpValue *unlockHighResolution = bpGameConfigGetValue(config, BP_CONFIG_UNLOCK_HIGH_RESOLUTION);
            if (unlockHighResolution)
                *unlockHighResolution = true;
            continue;
        }
        if (parser.Next(arg, "--debug", 'd')) {
            BpValue *debug = bpGameConfigGetValue(config, BP_CONFIG_DEBUG);
            if (debug)
                *debug = true;
            continue;
        }
        if (parser.Next(arg, "--rookie", 'r')) {
            BpValue *rookie = bpGameConfigGetValue(config, BP_CONFIG_ROOKIE);
            if (rookie)
                *rookie = true;
            continue;
        }
        parser.Skip();
    }
    parser.Reset();
}

static void ParsePathsFromCmdline(CmdlineParser &parser, BpGameConfig *config) {
    if (!config)
        return;

    CmdlineArg arg;
    XString path;
    while (!parser.Done()) {
        if (parser.Next(arg, "--config", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_CONFIG, path.CStr());
            break;
        }
        if (parser.Next(arg, "--log", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_LOG, path.CStr());
            continue;
        }
        if (parser.Next(arg, "--cmo", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_CMO, path.CStr());
            continue;
        }
        if (parser.Next(arg, "--root-path", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_ROOT, path.CStr());
            continue;
        }
        if (parser.Next(arg, "--plugin-path", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_PLUGINS, path.CStr());
            continue;
        }
        if (parser.Next(arg, "--render-engine-path", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_RENDER_ENGINES, path.CStr());
            continue;
        }
        if (parser.Next(arg, "--manager-path", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_MANAGERS, path.CStr());
            continue;
        }
        if (parser.Next(arg, "--building-block-path", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_BUILDING_BLOCKS, path.CStr());
            continue;
        }
        if (parser.Next(arg, "--sound-path", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_SOUNDS, path.CStr());
            continue;
        }
        if (parser.Next(arg, "--bitmap-path", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_TEXTURES, path.CStr());
            continue;
        }
        if (parser.Next(arg, "--data-path", '\0', 1)) {
            if (arg.GetValue(0, path))
                bpSetGamePath(config, BP_PATH_DATA, path.CStr());
            continue;
        }
        parser.Skip();
    }
    parser.Reset();
}

void InitLogger(BpLogger *logger, BpGameConfig *config) {
    if (!logger)
        return;

    BpValue *verbose = bpGameConfigGetValue(config, BP_CONFIG_VERBOSE);
    if (verbose && verbose->GetBool()) {
        bpLoggerSetLevel(logger, BP_LOG_DEBUG);
    }

    bool overwrite = true;
    BpValue *logMode = bpGameConfigGetValue(config, BP_CONFIG_LOG_MODE);
    if (logMode)
        overwrite = logMode->GetInt32() == BP_LOG_MODE_OVERWRITE;

    g_FileLogger.Attach(logger, bpGetGamePath(config, BP_PATH_LOG), overwrite);
}
