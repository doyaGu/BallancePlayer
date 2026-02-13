#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <tchar.h>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "GamePlayer.h"
#include "Splash.h"
#include "LockGuard.h"
#include "Logger.h"
#include "Utils.h"

static HANDLE CreateNamedMutex();
static void ParseConfigsFromCmdline(CGameConfig &config, CmdlineParser &parser);
static void LoadPaths(CGameConfig &config, CmdlineParser &parser);
static void EnableDpiAwareness();

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    HANDLE hMutex = CreateNamedMutex();
    if (!hMutex)
    {
        ::MessageBox(NULL, TEXT("Another player is running!"), TEXT("Error"), MB_OK);
        return -1;
    }

    LockGuard guard(hMutex);

    CmdlineParser parser(__argc, __argv);
    CGameConfig config;

    // Load paths from command line
    LoadPaths(config, parser);

    // Flush ini file if it doesn't exist
    if (!utils::FileOrDirectoryExists(config.GetPath(eConfigPath)))
        config.SaveToIni();

    // Load configurations
    config.LoadFromIni();
    ParseConfigsFromCmdline(config, parser);

    bool overwrite = true;
    if (config.logMode == eLogAppend)
        overwrite = false;

    CLogger::Get().Open(config.GetPath(eLogPath), overwrite);
    if (config.verbose)
        CLogger::Get().SetLevel(CLogger::LEVEL_DEBUG);

    EnableDpiAwareness();

    CSplash splash(hInstance);
    splash.Show();

    CGamePlayer player;
    if (!player.Init(config, hInstance))
    {
        CLogger::Get().Error("Failed to initialize player!");
        ::MessageBox(NULL, TEXT("Failed to initialize player!"), TEXT("Error"), MB_OK);
        return -1;
    }

    if (!player.Load())
    {
        CLogger::Get().Error("Failed to load game composition!");
        ::MessageBox(NULL, TEXT("Failed to load game composition!"), TEXT("Error"), MB_OK);
        player.Shutdown();
        return -1;
    }

    player.Play();
    player.Run();
    player.Shutdown();

    return 0;
}

static HANDLE CreateNamedMutex()
{
    char buf[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];

    if (!::GetModuleFileNameA(NULL, buf, MAX_PATH))
    {
        CLogger::Get().Error("Failed to get module filename, error code: %d", GetLastError());
        return NULL;
    }

    _splitpath(buf, drive, dir, filename, NULL);
    _snprintf(buf, MAX_PATH, "%s%s", drive, dir);

    unsigned int crc32 = 0;
    utils::CRC32(buf, strlen(buf), 0, &crc32);
    _snprintf(buf, MAX_PATH, "Ballance-%X", crc32);

    HANDLE hMutex = ::CreateMutexA(NULL, FALSE, buf);
    DWORD error = ::GetLastError();

    if (!hMutex)
    {
        CLogger::Get().Error("Failed to create mutex, error code: %d", error);
        return NULL;
    }

    if (error == ERROR_ALREADY_EXISTS)
    {
        ::CloseHandle(hMutex);
        return NULL;
    }

    return hMutex;
}

static void ParseConfigsFromCmdline(CGameConfig &config, CmdlineParser &parser)
{
    CmdlineArg arg;
    long value = 0;
    std::string str;

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
                config.textureVideoFormat = utils::String2PixelFormat(str.c_str(), 16);
            continue;
        }
        if (parser.Next(arg, "--sprite-video-format", '\0', 1))
        {
            if (arg.GetValue(0, str))
                config.spriteVideoFormat = utils::String2PixelFormat(str.c_str(), 16);
            continue;
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

static void LoadPaths(CGameConfig &config, CmdlineParser &parser)
{
    // Load paths
    CmdlineArg arg;
    std::string path;
    while (!parser.Done())
    {
        if (parser.Next(arg, "--config", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eConfigPath, path.c_str());
            break;
        }
        if (parser.Next(arg, "--log", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eLogPath, path.c_str());
            continue;
        }
        if (parser.Next(arg, "--cmo", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eCmoPath, path.c_str());
            continue;
        }
        if (parser.Next(arg, "--root-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eRootPath, path.c_str());
            continue;
        }
        if (parser.Next(arg, "--plugin-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(ePluginPath, path.c_str());
            continue;
        }
        if (parser.Next(arg, "--render-engine-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eRenderEnginePath, path.c_str());
            continue;
        }
        if (parser.Next(arg, "--manager-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eManagerPath, path.c_str());
            continue;
        }
        if (parser.Next(arg, "--building-block-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eBuildingBlockPath, path.c_str());
            continue;
        }
        if (parser.Next(arg, "--sound-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eSoundPath, path.c_str());
            continue;
        }
        if (parser.Next(arg, "--bitmap-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eBitmapPath, path.c_str());
            continue;
        }
        if (parser.Next(arg, "--data-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                config.SetPath(eDataPath, path.c_str());
            continue;
        }
        parser.Skip();
    }
    parser.Reset();

    // Set default value for the path if it was not specified in command line
    for (int p = eConfigPath; p < ePathCategoryCount; ++p)
    {
        if (!utils::DirectoryExists(config.GetPath((PathCategory)p)))
        {
            CLogger::Get().Warn("%s does not exist, using default path", config.GetPath((PathCategory)p));
            config.ResetPath((PathCategory)p);
        }
    }
}

static void EnableDpiAwareness()
{
    // DPI awareness enums and types for VC6.0 compatibility
#ifndef PROCESS_DPI_UNAWARE
#define PROCESS_DPI_UNAWARE 0
#define PROCESS_SYSTEM_DPI_AWARE 1
#define PROCESS_PER_MONITOR_DPI_AWARE 2
#endif

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
    typedef HANDLE DPI_AWARENESS_CONTEXT;
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE ((DPI_AWARENESS_CONTEXT) - 3)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT) - 4)
#endif

    // Function pointer types
    typedef HRESULT(WINAPI *PFN_SetProcessDpiAwareness)(int);
    typedef DPI_AWARENESS_CONTEXT(WINAPI *PFN_SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
    typedef BOOL(WINAPI *PFN_SetProcessDPIAware)();

    // Since VC6.0 lacks many modern Windows types, we'll use a simpler approach
    // to check the Windows version using GetVersionEx()
    BOOL isWin10OrGreater = FALSE;
    BOOL isWin81OrGreater = FALSE;

    OSVERSIONINFO osvi;
    ::ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (::GetVersionEx(&osvi))
    {
        // Windows 10 is 6.2+ (reported as 6.2 due to compatibility)
        // Windows 8.1 is 6.3
        if (osvi.dwMajorVersion > 6 ||
            (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 3))
        {
            isWin81OrGreater = TRUE;
        }

        // For Windows 10, we can't reliably detect it this way,
        // but we'll try loading the Windows 10 specific API
        isWin10OrGreater = (osvi.dwMajorVersion > 6);
    }

    // Try Windows 10+ API first
    if (isWin10OrGreater)
    {
        HINSTANCE user32_dll = ::LoadLibraryA("user32.dll");
        if (user32_dll)
        {
            PFN_SetThreadDpiAwarenessContext SetThreadDpiAwarenessContextFn =
                (PFN_SetThreadDpiAwarenessContext)::GetProcAddress(user32_dll, "SetThreadDpiAwarenessContext");
            if (SetThreadDpiAwarenessContextFn)
            {
                SetThreadDpiAwarenessContextFn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
                ::FreeLibrary(user32_dll);
                return;
            }
            ::FreeLibrary(user32_dll);
        }
    }

    // Try Windows 8.1+ API
    if (isWin81OrGreater)
    {
        HINSTANCE shcore_dll = ::LoadLibraryA("shcore.dll");
        if (shcore_dll)
        {
            PFN_SetProcessDpiAwareness SetProcessDpiAwarenessFn =
                (PFN_SetProcessDpiAwareness)::GetProcAddress(shcore_dll, "SetProcessDpiAwareness");
            if (SetProcessDpiAwarenessFn)
            {
                SetProcessDpiAwarenessFn(PROCESS_PER_MONITOR_DPI_AWARE);
                ::FreeLibrary(shcore_dll);
                return;
            }
            ::FreeLibrary(shcore_dll);
        }
    }

    // Fall back to Windows Vista+ API
    HINSTANCE user32_dll = ::LoadLibraryA("user32.dll");
    if (user32_dll)
    {
        PFN_SetProcessDPIAware SetProcessDPIAwareFn =
            (PFN_SetProcessDPIAware)::GetProcAddress(user32_dll, "SetProcessDPIAware");
        if (SetProcessDPIAwareFn)
        {
            SetProcessDPIAwareFn();
        }
        ::FreeLibrary(user32_dll);
    }
}
