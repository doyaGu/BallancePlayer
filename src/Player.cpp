#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "GamePlayer.h"
#include "PlayerOptions.h"
#include "Logger.h"
#include "Utils.h"
#include "VxWindowFunctions.h"

static bool EnsurePersistentConfigReady(CGameConfig &config);
static void UseExecutableDirectoryAsWorkingDirectory();
static void ShowErrorMessage(const char *text);

int main(int argc, char **argv)
{
    SDL_SetMainReady();
    UseExecutableDirectoryAsWorkingDirectory();

    CmdlineParser parser(argc, argv);

    CGameConfig persistentConfig;
    playeroptions::ApplyPathOptions(persistentConfig, parser);

    if (!EnsurePersistentConfigReady(persistentConfig))
        return -1;

    persistentConfig.LoadFromIni();
    CGameConfig runtimeConfig = persistentConfig;
    playeroptions::ApplyRuntimeOptions(runtimeConfig, parser);

    bool overwrite = runtimeConfig.logMode != eLogAppend;
    CLogger::Get().Open(runtimeConfig.GetPath(eLogPath), overwrite);
    if (runtimeConfig.verbose)
        CLogger::Get().SetLevel(CLogger::LEVEL_DEBUG);

    CGamePlayer player;
    if (!player.Init(runtimeConfig, persistentConfig))
    {
        CLogger::Get().Error("Failed to initialize player!");
        ShowErrorMessage("Failed to initialize player!");
        return -1;
    }

    CLogger::Get().Debug("Loading game composition: %s", runtimeConfig.GetPath(eCmoPath));
    if (!player.Load(runtimeConfig.GetPath(eCmoPath)))
    {
        CLogger::Get().Error("Failed to load game composition!");
        ShowErrorMessage("Failed to load game composition!");
        player.Shutdown();
        return -1;
    }

    player.Play();
    player.Run();
    player.Shutdown();

    return 0;
}

static void UseExecutableDirectoryAsWorkingDirectory()
{
    char modulePath[MAX_PATH];
    size_t len = VxGetModuleFileName(NULL, modulePath, MAX_PATH);
    if (len > 0 && len < MAX_PATH)
        utils::SetCurrentDirectoryToFileDirectory(modulePath);
}

static bool EnsurePersistentConfigReady(CGameConfig &config)
{
    if (!config.EnsureConfigPath())
    {
        ShowErrorMessage("Failed to determine configuration file path.");
        return false;
    }

    if (utils::FileOrDirectoryExists(config.GetPath(eConfigPath)))
        return true;

    return config.SaveToIni();
}

static void ShowErrorMessage(const char *text)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", text ? text : "Error", NULL);
}
