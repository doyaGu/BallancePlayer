#ifndef PLAYER_CONFIGDIALOG_H
#define PLAYER_CONFIGDIALOG_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

// Forward declarations
class CGameConfig;

bool ShowConfigDialog(HINSTANCE hInstance, CGameConfig &config, bool loadIni = true);

#endif // PLAYER_CONFIGDIALOG_H
