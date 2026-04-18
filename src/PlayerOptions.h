#ifndef PLAYER_PLAYEROPTIONS_H
#define PLAYER_PLAYEROPTIONS_H

#include "GameConfig.h"

class CmdlineParser;

namespace playeroptions
{
    void ApplyPathOptions(CGameConfig &config, CmdlineParser &parser);
    void ApplyConfigToolPathOptions(CGameConfig &config, CmdlineParser &parser);
    void ApplyConfigOptions(CGameConfig &config, CmdlineParser &parser);

    int GetConfigOptionCount();
    int GetPathOptionCount();
    bool IsConfigToolMode(CmdlineParser &parser);
    bool HasConfigOption(const char *longopt, char shortopt);
    bool HasPathOption(const char *longopt);
}

#endif // PLAYER_PLAYEROPTIONS_H
