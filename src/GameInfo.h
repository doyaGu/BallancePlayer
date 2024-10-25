#ifndef PLAYER_GAMEINFO_H
#define PLAYER_GAMEINFO_H

class GameInfo {
public:
    GameInfo() = default;
    virtual ~GameInfo() = default;

    GameInfo *next = nullptr;
    char gameName[128] = {};
    char levelName[128] = {};
    char fileName[128] = {};
    char path[128] = {};
    char regSubkey[512] = R"("Software\\Ballance\\Settings")";
    unsigned long hkRoot = 0x80000001;
    int gameScore = 0;
    int levelScore = 0;
    int type = 0;
    int gameID = 0;
    int levelID = 0;
    int gameBonus = 0;
    int levelBonus = 0;
    int levelReached = 0;
};

#endif // PLAYER_GAMEINFO_H
