#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "GamePlayer.h"
#include "Splash.h"
#include "LockGuard.h"
#include "Logger.h"
#include "Utils.h"

static HANDLE CreateNamedMutex();
static void LoadPaths(CGameConfig &config, CmdlineParser &parser);
static void EnableDpiAwareness();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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
    config.LoadFromCmdline(parser);

    bool overwrite = true;
    if (config.logMode == eLogAppend)
        overwrite = false;

    CLogger::Get().Open(config.GetPath(eLogPath), overwrite);
    if (config.verbose)
        CLogger::Get().SetLevel(CLogger::LEVEL_DEBUG);

    EnableDpiAwareness();

    CSplash splash(hInstance);
    splash.Show();

    CGamePlayer &player = CGamePlayer::GetInstance();

    if (!player.Init(hInstance, config))
    {
        CLogger::Get().Error("Failed to initialize player!");
        ::MessageBox(NULL, TEXT("Failed to initialize player!"), TEXT("Error"), MB_OK);
        return -1;
    }

    if (!player.Load())
    {
        CLogger::Get().Error("Failed to load game composition!");
        ::MessageBox(NULL, TEXT("Failed to load game composition!"), TEXT("Error"), MB_OK);
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

    size_t crc = 0;
    utils::CRC32(buf, strlen(buf), 0, &crc);
    _snprintf(buf, MAX_PATH, "Ballance-%X", crc);

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

static void LoadPaths(CGameConfig &config, CmdlineParser &parser)
{
    static const char *const DefaultPaths[] = {
        "Player.ini",
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
    };

    char szPath[MAX_PATH];
    bool useDefault;

    // Load paths
    config.LoadPathsFromCmdline(parser);

    // Set default value for the path if it was not specified in command line
    for (int p = eConfigPath; p < ePathCategoryCount; ++p)
    {
        if (!config.HasPath((PathCategory)p))
        {
            useDefault = true;
        }
        else if (!utils::DirectoryExists(config.GetPath((PathCategory)p)))
        {
            CLogger::Get().Warn("%s does not exist, using default path: %s", config.GetPath((PathCategory)p), DefaultPaths[p]);
            useDefault = true;
        }
        else
        {
            useDefault = false;
        }

        if (useDefault)
        {
            if (p < ePluginPath)
            {
                config.SetPath((PathCategory)p, DefaultPaths[p]);
            }
            else
            {
                utils::ConcatPath(szPath, MAX_PATH, config.GetPath(eRootPath), DefaultPaths[p]);
                config.SetPath((PathCategory)p, szPath);
            }
        }
    }
}

void EnableDpiAwareness()
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
    typedef HRESULT(WINAPI * PFN_SetProcessDpiAwareness)(int);
    typedef DPI_AWARENESS_CONTEXT(WINAPI * PFN_SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
    typedef BOOL(WINAPI * PFN_SetProcessDPIAware)(void);

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
