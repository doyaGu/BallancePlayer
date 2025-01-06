#include <cstdio>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <cxxopts.hpp>

#include "bp/Utils.h"
#include "bp/GamePlayer.h"
#include "GameConfig.h"
#include "Splash.h"
#include "ConsoleAttacher.h"
#include "FileLogger.h"
#include "LockGuard.h"

static HANDLE CreatNamedMutex();

static void AddCommandLineOptions(cxxopts::Options &options);
static void LoadGameConfigFromCommandLine(BpGameConfig *config, const cxxopts::ParseResult &result);
static void LoadGamePathsFromCommandLine(BpGameConfig *config, const cxxopts::ParseResult &result);
static void LoadGamePaths(BpGameConfig *config, const cxxopts::ParseResult &result);
static void InitLogger(BpLogger *logger, const BpGameConfig *config);

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

    cxxopts::Options options("Player", BP_DESCRIPTION);
    try {
        AddCommandLineOptions(options);
    } catch (const cxxopts::exceptions::specification &e) {
        bpLoggerError(logger, "Error adding command line options: %s", e.what());
        return -1;
    }

    cxxopts::ParseResult result;
    try {
        result = std::move(options.parse(__argc, __argv));
    } catch (const cxxopts::exceptions::parsing &e) {
        bpLoggerError(logger, "Error parsing command line: %s", e.what());
        return -1;
    }

    if (result.contains("help")) {
        const auto help = options.help();
        puts(help.c_str());
        return 0;
    }
    if (result.contains("version")) {
        puts(BP_VERSION);
        return 0;
    }

    BpGameConfig *config = bpCreateGameConfig();
    bpGameConfigInit(config);

    try {
        // Load paths from command line
        LoadGamePaths(config, result);
    } catch (const cxxopts::exceptions::option_has_no_value &e) {
        bpLoggerError(logger, "Error parsing command line: %s", e.what());
        return -1;
    }

    // Flush ini file if it doesn't exist
    if (!bpFileOrDirectoryExists(bpGetGamePath(config, BP_PATH_CONFIG)))
        bpGameConfigSave(config, nullptr);

    // Load configurations
    bpGameConfigLoad(config, bpGetGamePath(config, BP_PATH_CONFIG));
    try {
        LoadGameConfigFromCommandLine(config, result);
    } catch (const cxxopts::exceptions::option_has_no_value &e) {
        bpLoggerError(logger, "Error parsing command line: %s", e.what());
        return -1;
    }

    InitLogger(logger, config);

    Splash splash(hInstance);
    splash.Show();

    BpGamePlayer *player = bpCreateGamePlayer();

    if (!bpGamePlayerInit(player, hInstance, config)) {
        ::MessageBox(nullptr, TEXT("Failed to initialize player!"), TEXT("Error"), MB_OK);
        return -1;
    }

    if (!bpGamePlayerLoad(player, nullptr)) {
        ::MessageBox(nullptr, TEXT("Failed to load game composition!"), TEXT("Error"), MB_OK);
        return -1;
    }

    bpGamePlayerPlay(player);
    bpGamePlayerRun(player);
    bpGamePlayerShutdown(player);
    bpDestroyGamePlayer(player);

    // Save settings
    bpGameConfigSave(config, nullptr);
    bpDestroyGameConfig(config);

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

void AddCommandLineOptions(cxxopts::Options &options) {
    options.add_options()
            ("verbose", "Enable verbose mode")
            ("m,manual-setup", "Enable manual setup")
            ("load-required-managers", "Load required managers only")
            ("load-required-building-blocks", "Load required building blocks only")
            ("load-required-plugins", "Load required plugins only")
            ("video-driver", "Set video driver", cxxopts::value<long>())
            ("width", "Set window width", cxxopts::value<long>())
            ("height", "Set window height", cxxopts::value<long>())
            ("bpp", "Set bits per pixel", cxxopts::value<long>())
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
            ("plugins-path", "Set plugins path", cxxopts::value<std::string>())
            ("render-engines-path", "Set render engines path", cxxopts::value<std::string>())
            ("managers-path", "Set managers path", cxxopts::value<std::string>())
            ("building-blocks-path", "Set building blocks path", cxxopts::value<std::string>())
            ("sounds-path", "Set sounds path", cxxopts::value<std::string>())
            ("textures-path", "Set textures path", cxxopts::value<std::string>())
            ("data-path", "Set data path", cxxopts::value<std::string>())
            ("scripts-path", "Set scripts path", cxxopts::value<std::string>())
            ("v,version", "Print version")
            ("h,help", "Print usage");
}

void LoadGameConfigFromCommandLine(BpGameConfig *config, const cxxopts::ParseResult &result) {
    if (result.contains("verbose"))
        config->Verbose->SetBool(result["verbose"].as<bool>());
    if (result.contains("manual-setup"))
        config->ManualSetup->SetBool(result["manual-setup"].as<bool>());
    if (result.contains("load-required-managers"))
        config->LoadAllManagers->SetBool(!result["load-required-managers"].as<bool>());
    if (result.contains("load-required-building-blocks"))
        config->LoadAllBuildingBlocks->SetBool(!result["load-required-building-blocks"].as<bool>());
    if (result.contains("load-required-plugins"))
        config->LoadAllPlugins->SetBool(!result["load-required-plugins"].as<bool>());
    if (result.contains("video-driver"))
        config->Driver->SetInt32(result["video-driver"].as<int>());
    if (result.contains("width"))
        config->Width->SetInt32(result["width"].as<int>());
    if (result.contains("height"))
        config->Height->SetInt32(result["height"].as<int>());
    if (result.contains("bpp"))
        config->Bpp->SetInt32(result["bpp"].as<int>());
    if (result.contains("fullscreen"))
        config->Fullscreen->SetBool(result["fullscreen"].as<bool>());
    if (result.contains("disable-perspective-correction"))
        config->DisablePerspectiveCorrection->SetBool(result["disable-perspective-correction"].as<bool>());
    if (result.contains("force-linear-fog"))
        config->ForceLinearFog->SetBool(result["force-linear-fog"].as<bool>());
    if (result.contains("force-software"))
        config->ForceSoftware->SetBool(result["force-software"].as<bool>());
    if (result.contains("disable-filter"))
        config->DisableFilter->SetBool(result["disable-filter"].as<bool>());
    if (result.contains("ensure-vertex-shader"))
        config->EnsureVertexShader->SetBool(result["ensure-vertex-shader"].as<bool>());
    if (result.contains("use-index-buffers"))
        config->UseIndexBuffers->SetBool(result["use-index-buffers"].as<bool>());
    if (result.contains("disable-dithering"))
        config->DisableDithering->SetBool(result["disable-dithering"].as<bool>());
    if (result.contains("antialias"))
        config->Antialias->SetInt32(result["antialias"].as<int>());
    if (result.contains("disable-mipmap"))
        config->DisableMipmap->SetBool(result["disable-mipmap"].as<bool>());
    if (result.contains("disable-specular"))
        config->DisableSpecular->SetBool(result["disable-specular"].as<bool>());
    if (result.contains("enable-screen-dump"))
        config->EnableScreenDump->SetBool(result["enable-screen-dump"].as<bool>());
    if (result.contains("enable-debug-mode"))
        config->EnableDebugMode->SetBool(result["enable-debug-mode"].as<bool>());
    if (result.contains("vertex-cache"))
        config->VertexCache->SetInt32(result["vertex-cache"].as<int>());
    if (result.contains("disable-texture-cache-management"))
        config->TextureCacheManagement->SetBool(!result["disable-texture-cache-management"].as<bool>());
    if (result.contains("disable-sort-transparent-objects"))
        config->SortTransparentObjects->SetBool(!result["disable-sort-transparent-objects"].as<bool>());
    if (result.contains("texture-video-format")) {
        const auto &format = result["texture-video-format"].as<std::string>();
        config->TextureVideoFormat->SetUint32(bpString2PixelFormat(format.c_str(), format.length()));
    }
    if (result.contains("sprite-video-format")) {
        const auto &format = result["sprite-video-format"].as<std::string>();
        config->SpriteVideoFormat->SetUint32(bpString2PixelFormat(format.c_str(), format.length()));
    }
    if (result.contains("child-window-rendering"))
        config->ChildWindowRendering->SetBool(result["child-window-rendering"].as<bool>());
    if (result.contains("borderless"))
        config->Borderless->SetBool(result["borderless"].as<bool>());
    if (result.contains("clip-cursor"))
        config->ClipCursor->SetBool(result["clip-cursor"].as<bool>());
    if (result.contains("always-handle-input"))
        config->AlwaysHandleInput->SetBool(result["always-handle-input"].as<bool>());
    if (result.contains("pause-on-deactivated"))
        config->PauseOnDeactivated->SetBool(result["pause-on-deactivated"].as<bool>());
    if (result.contains("position-x"))
        config->PosX->SetInt32(result["position-x"].as<int>());
    if (result.contains("position-y"))
        config->PosY->SetInt32(result["position-y"].as<int>());
    if (result.contains("lang"))
        config->LangId->SetInt32(result["lang"].as<int>());
    if (result.contains("skip-opening"))
        config->SkipOpening->SetBool(result["skip-opening"].as<bool>());
    if (result.contains("disable-hotfix"))
        config->ApplyHotfix->SetBool(!result["disable-hotfix"].as<bool>());
    if (result.contains("unlock-framerate"))
        config->UnlockFramerate->SetBool(result["unlock-framerate"].as<bool>());
    if (result.contains("unlock-widescreen"))
        config->UnlockWidescreen->SetBool(result["unlock-widescreen"].as<bool>());
    if (result.contains("unlock-high-resolution"))
        config->UnlockHighResolution->SetBool(result["unlock-high-resolution"].as<bool>());
    if (result.contains("debug"))
        config->Debug->SetBool(result["debug"].as<bool>());
    if (result.contains("rookie"))
        config->Rookie->SetBool(result["rookie"].as<bool>());
}

void LoadGamePathsFromCommandLine(BpGameConfig *config, const cxxopts::ParseResult &result) {
    if (result.contains("config"))
        bpSetGamePath(config, BP_PATH_CONFIG, result["config"].as<std::string>().c_str());
    if (result.contains("log"))
        bpSetGamePath(config, BP_PATH_LOG, result["log"].as<std::string>().c_str());
    if (result.contains("cmo"))
        bpSetGamePath(config, BP_PATH_CMO, result["cmo"].as<std::string>().c_str());
    if (result.contains("root-path"))
        bpSetGamePath(config, BP_PATH_ROOT, result["root-path"].as<std::string>().c_str());
    if (result.contains("plugins-path"))
        bpSetGamePath(config, BP_PATH_PLUGINS, result["plugins-path"].as<std::string>().c_str());
    if (result.contains("render-engines-path"))
        bpSetGamePath(config, BP_PATH_RENDER_ENGINES, result["render-engines-path"].as<std::string>().c_str());
    if (result.contains("managers-path"))
        bpSetGamePath(config, BP_PATH_MANAGERS, result["managers-path"].as<std::string>().c_str());
    if (result.contains("building-blocks-path"))
        bpSetGamePath(config, BP_PATH_BUILDING_BLOCKS, result["building-blocks-path"].as<std::string>().c_str());
    if (result.contains("sounds-path"))
        bpSetGamePath(config, BP_PATH_SOUNDS, result["sounds-path"].as<std::string>().c_str());
    if (result.contains("textures-path"))
        bpSetGamePath(config, BP_PATH_TEXTURES, result["textures-path"].as<std::string>().c_str());
    if (result.contains("data-path"))
        bpSetGamePath(config, BP_PATH_DATA, result["data-path"].as<std::string>().c_str());
    if (result.contains("scripts-path"))
        bpSetGamePath(config, BP_PATH_SCRIPTS, result["scripts-path"].as<std::string>().c_str());
}

static void LoadGamePaths(BpGameConfig *config, const cxxopts::ParseResult &result) {
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

    bool useDefault;

    // Load paths
    LoadGamePathsFromCommandLine(config, result);

    // Set default value for the path if it was not specified in command line
    for (int p = 0; p < BP_PATH_CATEGORY_COUNT; ++p)
    {
        if (!bpHasGamePath(config, (BpPathCategory)p))
        {
            useDefault = true;
        }
        else if (!bpDirectoryExists(bpGetGamePath(config, (BpPathCategory)p)))
        {
            bpLogWarn("%s does not exist, using default path: %s", bpGetGamePath(config, (BpPathCategory)p), DefaultPaths[p]);
            useDefault = true;
        }
        else
        {
            useDefault = false;
        }

        if (useDefault)
        {
            if (p < BP_PATH_PLUGINS)
            {
                bpSetGamePath(config, (BpPathCategory)p, DefaultPaths[p]);
            }
            else
            {
                char szPath[MAX_PATH];
                bpConcatPath(szPath, MAX_PATH, bpGetGamePath(config, BP_PATH_ROOT), DefaultPaths[p]);
                bpSetGamePath(config, (BpPathCategory)p, szPath);
            }
        }
    }
}

void InitLogger(BpLogger *logger, const BpGameConfig *config) {
    if (!logger)
        return;

    if (config->Verbose->GetBool())
        bpLoggerSetLevel(logger, BP_LOG_DEBUG);

    bool overwrite = true;
    if (config->LogMode->GetBool() == BP_LOG_MODE_APPEND)
        overwrite = false;

    g_FileLogger.Attach(logger, bpGetGamePath(config, BP_PATH_LOG), overwrite);
}
