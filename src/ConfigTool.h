#ifndef CONFIGTOOL_H
#define CONFIGTOOL_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

// Forward declarations
class CGameConfig;

// Public functions
bool ShowConfigTool(HINSTANCE hInstance, CGameConfig &config, bool loadIni = true);

#endif // CONFIGTOOL_H
