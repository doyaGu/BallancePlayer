#define _WIN32_WINNT 0x0502
#include <windows.h>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "GamePlayer.h"
#include "Logger.h"
#include "Utils.h"

static void InitLogger()
{
    char szPath[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
    _splitpath(szPath, drive, dir, filename, NULL);
    sprintf(szPath, "%s%s%s.log", drive, dir, filename);
    CLogger::Get().Open(szPath, false);

#ifndef NDEBUG
    CLogger::Get().SetLevel(CLogger::LEVEL_DEBUG);
#endif
}

static void LoadPaths(CmdlineParser &parser)
{
    CGameConfig &config = CGameConfig::Get();

    // First check whether the config file is specified in command line
    config.LoadIniPathFromCmdline(parser);

    // If no config file is specified, use the default path
    if (!config.HasPath(eConfigPath) || !utils::FileOrDirectoryExists(config.GetPath(eConfigPath)))
    {
        char szPath[MAX_PATH];
        char drive[4];
        char dir[MAX_PATH];
        char filename[MAX_PATH];
        ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
        _splitpath(szPath, drive, dir, filename, NULL);
        sprintf(szPath, "%s%s%s.ini", drive, dir, filename);
        config.SetPath(eConfigPath, szPath);
        if (!utils::FileOrDirectoryExists(szPath))
            config.SaveToIni();
    }

    // Load paths
    config.LoadPathsFromIni();
    config.LoadPathsFromCmdline(parser);
}

static bool DetectVirtoolsDlls(const char *path)
{
    if (!path || strcmp(path, "") == 0)
        return false;

    static const char *dlls[3] = {"CK2.dll", "CKZlib.dll", "VxMath.dll"};
    char szPath[MAX_PATH];
    for (int i = 0; i < 3; ++i)
    {
        utils::ConcatPath(szPath, MAX_PATH, path, dlls[i]);
        if (!utils::FileOrDirectoryExists(szPath))
            return false;
    }
    return true;
}

static bool GetDefaultRootPath(char *path, size_t size)
{
    char szPath[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
    _splitpath(szPath, drive, dir, filename, NULL);

    _snprintf(szPath, size, "%s%s", drive, dir);
    if (DetectVirtoolsDlls(szPath))
    {
        _snprintf(path, size, "%s%s..\\", drive, dir);
        return true;
    }

    _snprintf(szPath, size, "%s%sBin", drive, dir);
    if (DetectVirtoolsDlls(szPath))
    {
        _snprintf(path, size, "%s%s", drive, dir);
        return true;
    }

    return false;
}

static bool DetectGameDirectory()
{
    CGameConfig &config = CGameConfig::Get();
    char szPath[MAX_PATH];
    bool valid = false;

    // If the root path is specified, check it first
    if (config.HasPath(eRootPath))
    {
        utils::ConcatPath(szPath, MAX_PATH, config.GetPath(eRootPath), "Bin");
        if (DetectVirtoolsDlls(szPath))
            valid = true;
    }

    // Try the default root path
    if (!valid && GetDefaultRootPath(szPath, MAX_PATH))
    {
        config.SetPath(eRootPath, szPath);
        valid = true;
    }

    if (!valid && utils::BrowsePath("Please specify the game root directory", szPath))
    {
        config.SetPath(eRootPath, szPath);
        utils::ConcatPath(szPath, MAX_PATH, config.GetPath(eRootPath), "Bin");
        if (DetectVirtoolsDlls(szPath))
            valid = true;
    }

    return valid;
}

static bool SetupGameDirectories()
{
    CGameConfig &config = CGameConfig::Get();

    if (!config.HasPath(eRootPath))
    {
        CLogger::Get().Error("Game root path is not specified");
        return false;
    }

    const char *const dirs[] = {
        "Plugins",
        "RenderEngines",
        "Managers",
        "BuildingBlocks",
        "Sounds",
        "Textures"};
    const char *rootPath = config.GetPath(eRootPath);

    config.SetPath(eDataPath, rootPath);

    char szPath[MAX_PATH];
    bool useDefaultDir = false;
    for (int p = ePluginPath; p < ePathCategoryCount; ++p)
    {
        if (!config.HasPath((PathCategory)p))
        {
            CLogger::Get().Debug("%s directory is not set, use default directory instead.", dirs[p - ePluginPath]);
            useDefaultDir = true;
        }
        else if (!utils::DirectoryExists(config.GetPath((PathCategory)p)))
        {
            CLogger::Get().Warn("%s does not exist, use default directory instead.", config.GetPath((PathCategory)p), dirs[p - ePluginPath]);
            useDefaultDir = true;
        }

        if (useDefaultDir)
        {
            utils::ConcatPath(szPath, MAX_PATH, rootPath, dirs[p - ePluginPath]);
            strcat(szPath, "\\");
            config.SetPath((PathCategory)p, szPath);
            useDefaultDir = false;
        }

        if (!utils::DirectoryExists(config.GetPath((PathCategory)p)))
        {
            CLogger::Get().Error("%s directory is not found!", dirs[p - ePluginPath]);
            return false;
        }
    }

    return true;
}

static void SetupCurrentDirectory()
{
    CGameConfig &config = CGameConfig::Get();
    char szPath[MAX_PATH];

    utils::ConcatPath(szPath, MAX_PATH, config.GetPath(eRootPath), "Bin");
    // Some game scripts rely on the relative path, the current directory should keep the same with original player
    ::SetCurrentDirectoryA(szPath);
    // Add the path to delay-loaded DLLs
    ::SetDllDirectoryA(szPath);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HANDLE hMutex = ::CreateMutex(NULL, FALSE, TEXT("Ballance"));
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
        return -1;

    CmdlineParser parser(__argc, __argv);

    InitLogger();

    LoadPaths(parser);

    // Check that the game root path is valid
    // The game directory should have the Bin directory which contains the Virtools Engine dlls
    if (!DetectGameDirectory())
    {
        ::MessageBox(NULL, TEXT("Unable to locate CK2.dll!"), TEXT("Error"), MB_OK);
        return -1;
    }

    // Load settings
    CGameConfig &config = CGameConfig::Get();
    config.LoadFromIni();
    config.LoadFromCmdline(parser);

    if (config.debug && CLogger::Get().GetLevel() != CLogger::LEVEL_DEBUG)
    {
        CLogger::Get().SetLevel(CLogger::LEVEL_DEBUG);
    }

    SetupCurrentDirectory();

    // Virtools SDK is available from here

    if (!SetupGameDirectories())
    {
        ::MessageBox(NULL, TEXT("Failed to setup all directories!"), TEXT("Error"), MB_OK);
        return -1;
    }

    CGamePlayer player;
    if (!player.Init(hInstance, hMutex))
    {
        ::MessageBox(NULL, TEXT("Initialization failed!"), TEXT("Error"), MB_OK);
        return -1;
    }

    if (!player.Load("base.cmo"))
    {
        ::MessageBox(NULL, TEXT("Unable to find base.cmo!"), TEXT("Error"), MB_OK);
        return -1;
    }

    player.Run();
    player.Exit();

    return 0;
}
