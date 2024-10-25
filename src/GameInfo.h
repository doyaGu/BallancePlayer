#ifndef PLAYER_GAMEINFO_H
#define PLAYER_GAMEINFO_H

#include <string.h>

class CGameInfo
{
public:
    CGameInfo()
        : next(NULL),
          hkRoot(0x80000001),
          gameScore(0),
          levelScore(0),
          type(0),
          gameID(0),
          levelID(0),
          gameBonus(0),
          levelBonus(0),
          levelReached(0)
    {
        memset(gameName, 0, sizeof(gameName));
        memset(levelName, 0, sizeof(levelName));
        memset(fileName, 0, sizeof(fileName));
        memset(path, 0, sizeof(path));
        strcpy(regSubkey, "\"Software\\\\Ballance\\\\Settings\"");
    }

    virtual ~CGameInfo() {}

    CGameInfo *next;
    char gameName[128];
    char levelName[128];
    char fileName[128];
    char path[128];
    char regSubkey[512];
    unsigned long hkRoot;
    int gameScore;
    int levelScore;
    int type;
    int gameID;
    int levelID;
    int gameBonus;
    int levelBonus;
    int levelReached;
};

#endif // PLAYER_GAMEINFO_H