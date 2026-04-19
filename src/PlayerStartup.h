#ifndef PLAYER_PLAYERSTARTUP_H
#define PLAYER_PLAYERSTARTUP_H

class CGameConfig;

enum StartupConfigAction
{
    eUseExistingConfig,
    eShowInitialConfigDialog
};

StartupConfigAction GetStartupConfigAction(const CGameConfig &config);

#endif // PLAYER_PLAYERSTARTUP_H
