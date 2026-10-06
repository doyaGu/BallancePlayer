#ifndef PLAYER_PLAYEROPTIONS_H
#define PLAYER_PLAYEROPTIONS_H

#include "GameConfig.h"

class CmdlineParser;

namespace playeroptions
{
    void ApplyPathOptions(CGameConfig &config, CmdlineParser &parser);
    bool ApplyConfigOptions(CGameConfig &config, CmdlineParser &parser);
    bool ApplyRuntimeOptions(CGameConfig &config, CmdlineParser &parser);

    bool IsRasterizerName(const char *name);
    const char *RasterizerNameForDriver(const char *description);
    int GetConfigOptionCount();
    int GetPathOptionCount();
    bool HasConfigOption(const char *longopt, char shortopt);
    bool HasPathOption(const char *longopt);
}

#endif // PLAYER_PLAYEROPTIONS_H
