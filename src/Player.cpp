#include <cstdio>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <cxxopts.hpp>

#include "GameConfig.h"
#include "GamePlayer.h"
#include "ConsoleAttacher.h"
#include "LockGuard.h"
#include "Logger.h"
#include "Utils.h"

static HANDLE CreatNamedMutex();

void AddCommandLineOptions(cxxopts::Options &options);
void LoadConfigsFromCommandLine(CGameConfig &config, const cxxopts::ParseResult &result);
void LoadPathsFromCommandLine(CGameConfig &config, const cxxopts::ParseResult &result);

static void LoadPaths(CGameConfig &config, const cxxopts::ParseResult &result);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HANDLE hMutex = CreatNamedMutex();
    if (!hMutex)
    {
        ::MessageBox(NULL, TEXT("Another player is running!"), TEXT("Error"), MB_OK);
        return -1;
    }

    LockGuard guard(hMutex);

    ConsoleAttacher consoleAttacher;

    cxxopts::Options options("Player", PLAYER_DESCRIPTION);
    try
    {
        AddCommandLineOptions(options);
    }
    catch (const cxxopts::exceptions::specification &e)
    {
        CLogger::Get().Error("Error adding command line options: %s", e.what());
        return -1;
    }

    cxxopts::ParseResult result;
    try
    {
        result = std::move(options.parse(__argc, __argv));
    }
    catch (const cxxopts::exceptions::parsing &e)
    {
        CLogger::Get().Error("Error parsing command line: %s", e.what());
        return -1;
    }

    if (result.contains("help"))
    {
        const auto help = options.help();
        puts(help.c_str());
        return 0;
    }
    if (result.contains("version"))
    {
        puts(PLAYER_VERSION);
        return 0;
    }

    CGameConfig config;

    try
    {
        // Load paths from command line
        LoadPaths(config, result);
    }
    catch (const cxxopts::exceptions::option_has_no_value &e)
    {
        CLogger::Get().Error("Error parsing command line: %s", e.what());
        return -1;
    }

    // Flush ini file if it doesn't exist
    if (!utils::FileOrDirectoryExists(config.GetPath(eConfigPath)))
        config.SaveToIni();

    // Load configurations
    config.LoadFromIni();
    try
    {
        LoadConfigsFromCommandLine(config, result);
    }
    catch (const cxxopts::exceptions::option_has_no_value &e)
    {
        CLogger::Get().Error("Error parsing command line: %s", e.what());
        return -1;
    }

    bool overwrite = true;
    if (config.logMode == eLogAppend)
        overwrite = false;

    CLogger::Get().Open(config.GetPath(eLogPath), overwrite);
    if (config.verbose)
        CLogger::Get().SetLevel(CLogger::LEVEL_DEBUG);

    CGamePlayer &player = CGamePlayer::GetInstance();

    if (!player.Init(hInstance, config))
    {
        ::MessageBox(NULL, TEXT("Failed to initialize player!"), TEXT("Error"), MB_OK);
        return -1;
    }

    if (!player.Load())
    {
        ::MessageBox(NULL, TEXT("Failed to load game composition!"), TEXT("Error"), MB_OK);
        return -1;
    }

    player.Play();
    player.Run();
    player.Exit();

    return 0;
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

static void LoadPaths(CGameConfig &config, const cxxopts::ParseResult &result)
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
    LoadPathsFromCommandLine(config, result);

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
