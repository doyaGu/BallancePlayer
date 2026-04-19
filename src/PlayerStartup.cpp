#include "PlayerStartup.h"

#include "GameConfig.h"
#include "Utils.h"

StartupConfigAction GetStartupConfigAction(const CGameConfig &config)
{
    return utils::FileOrDirectoryExists(config.GetPath(eConfigPath))
        ? eUseExistingConfig
        : eShowInitialConfigDialog;
}
