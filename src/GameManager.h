#ifndef BP_GAMEMANAGER_H
#define BP_GAMEMANAGER_H

#include "CKBaseManager.h"
#include "CKContext.h"

class GamePlayer;

#ifndef GAME_MANAGER_GUID
#define GAME_MANAGER_GUID CKGUID(0x14886190, 0xf743cb1)
#endif

class GameManager : public CKBaseManager {
public:
    explicit GameManager(CKContext *context);
    ~GameManager() override;

    CKERROR OnPostSpriteRender(CKRenderContext* dev) override;

    CKDWORD GetValidFunctionsMask() override {
        return CKMANAGER_FUNC_OnPostSpriteRender;
    }

    int GetFunctionPriority(CKMANAGER_FUNCTIONS Function) override {
        if (Function == CKMANAGER_FUNC_OnPostSpriteRender)
            return -10000; // Low Priority
        return 0;
    }

private:
    GamePlayer *m_Player = nullptr;
};

#endif // BP_GAMEMANAGER_H
