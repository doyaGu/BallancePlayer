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

static void LoadPaths(CGameConfig &config, CmdlineParser &parser)
{
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
    config.LoadPathsFromCmdline(parser);
}

static HANDLE CreatNamedMutex()
{
    char buf[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    ::GetModuleFileNameA(NULL, buf, MAX_PATH);
    _splitpath(buf, drive, dir, filename, NULL);
    _snprintf(buf, MAX_PATH, "%s%s", drive, dir);

    size_t crc = 0;
    utils::CRC32(buf, strlen(buf), 0, &crc);
    _snprintf(buf, MAX_PATH, "Ballance-%X", crc);
    HANDLE hMutex = ::CreateMutexA(NULL, FALSE, buf);
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
        return NULL;
    return hMutex;
}

static bool SetupGameDirectories(CGameConfig &config)
{
    char szPath[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
    _splitpath(szPath, drive, dir, filename, NULL);
    _snprintf(szPath, MAX_PATH, "%s%s..\\", drive, dir);
    if (!utils::DirectoryExists(szPath))
        CLogger::Get().Error("The root directory is not valid!");

    config.SetPath(eRootPath, szPath);
    config.SetPath(eDataPath, szPath);

    const char *const dirs[] = {
        "Plugins",
        "RenderEngines",
        "Managers",
        "BuildingBlocks",
        "Sounds",
        "Textures"};

    bool useDefaultDir = false;
    for (int p = ePluginPath; p < ePathCategoryCount; ++p)
    {
        if (!config.HasPath((PathCategory)p))
        {
            useDefaultDir = true;
        }
        else if (!utils::DirectoryExists(config.GetPath((PathCategory)p)))
        {
            CLogger::Get().Warn("%s does not exist, use default directory instead", config.GetPath((PathCategory)p), dirs[p - ePluginPath]);
            useDefaultDir = true;
        }

        if (useDefaultDir)
        {
            utils::ConcatPath(szPath, MAX_PATH, config.GetPath(eRootPath), dirs[p - ePluginPath]);
            strcat(szPath, "\\");
            config.SetPath((PathCategory)p, szPath);
            useDefaultDir = false;
        }

        if (!utils::DirectoryExists(config.GetPath((PathCategory)p)))
        {
            CLogger::Get().Error("Directory [%s] is not found", dirs[p - ePluginPath]);
            return false;
        }
    }

    return true;
}

class LockGuard
{
public:
    explicit LockGuard(HANDLE mutex) : m_Mutex(mutex) {}

    ~LockGuard()
    {
        Release();
    }

    void Release()
    {
        ::CloseHandle(m_Mutex);
        m_Mutex = NULL;
    }

private:
    LockGuard(const LockGuard &);
    LockGuard &operator=(const LockGuard &);

    HANDLE m_Mutex;
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HANDLE hMutex = CreatNamedMutex();
    if (!hMutex)
        return -1;

    LockGuard guard(hMutex);

    CmdlineParser parser(__argc, __argv);

    InitLogger();

    CGameConfig config;

    // Load paths from command line
    LoadPaths(config, parser);

    // Load settings
    config.LoadFromIni();
    config.LoadFromCmdline(parser);

    if (config.debug && CLogger::Get().GetLevel() != CLogger::LEVEL_DEBUG)
    {
        CLogger::Get().SetLevel(CLogger::LEVEL_DEBUG);
    }

    if (!SetupGameDirectories(config))
    {
        CLogger::Get().Error("Failed to setup game directories");
        return -1;
    }

    CGamePlayer &player = CGamePlayer::GetInstance();
    if (!player.Init(hInstance, config))
    {
        ::MessageBox(NULL, TEXT("Player Initialization Failed!"), TEXT("Error"), MB_OK);
        return -1;
    }

    if (!player.Load("base.cmo"))
    {
        ::MessageBox(NULL, TEXT("Game Load Failed!"), TEXT("Error"), MB_OK);
        return -1;
    }

    player.Play();
    player.Run();
    player.Exit();

    return 0;
}
