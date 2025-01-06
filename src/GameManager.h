#ifndef BP_GAMEMANAGER_H
#define BP_GAMEMANAGER_H

#include "CKBaseManager.h"
#include "CKContext.h"

class GamePlayer;

#ifndef GAME_MANAGER_GUID
#define GAME_MANAGER_GUID CKGUID(0x32a40332, 0x3bf12a51)
#endif

class GameManager : public CKBaseManager {
public:
    explicit GameManager(CKContext *context);
    ~GameManager() override;

    CKERROR PreClearAll() override;
    CKERROR PostClearAll() override;
    CKERROR PreProcess() override;
    CKERROR PostProcess() override;
    CKERROR SequenceAddedToScene(CKScene* scn, CK_ID* objids, int count) override;
    CKERROR SequenceRemovedFromScene(CKScene* scn, CK_ID* objids, int count) override;
    CKERROR PreLaunchScene(CKScene* OldScene, CKScene* NewScene) override;
    CKERROR PostLaunchScene(CKScene* OldScene, CKScene* NewScene) override;
    CKERROR OnCKInit() override;
    CKERROR OnCKEnd() override;
    CKERROR OnCKReset() override;
    CKERROR OnCKPostReset() override;
    CKERROR OnCKPause() override;
    CKERROR OnCKPlay() override;
    CKERROR SequenceToBeDeleted(CK_ID* objids, int count) override;
    CKERROR SequenceDeleted(CK_ID* objids, int count) override;
    CKERROR PreLoad() override;
    CKERROR PostLoad() override;
    CKERROR PreSave() override;
    CKERROR PostSave() override;
    CKERROR OnPreCopy(CKDependenciesContext& context) override;
    CKERROR OnPostCopy(CKDependenciesContext& context) override;
    CKERROR OnPreRender(CKRenderContext* dev) override;
    CKERROR OnPostRender(CKRenderContext* dev) override;
    CKERROR OnPostSpriteRender(CKRenderContext* dev) override;

    CKDWORD GetValidFunctionsMask() override {
        return CKMANAGER_FUNC_OnSequenceToBeDeleted |
               CKMANAGER_FUNC_OnSequenceDeleted |
               CKMANAGER_FUNC_PreProcess |
               CKMANAGER_FUNC_PostProcess |
               CKMANAGER_FUNC_PreClearAll |
               CKMANAGER_FUNC_PostClearAll |
               CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnCKEnd |
               CKMANAGER_FUNC_OnCKPlay |
               CKMANAGER_FUNC_OnCKReset |
               CKMANAGER_FUNC_OnCKPostReset |
               CKMANAGER_FUNC_OnCKPause |
               CKMANAGER_FUNC_PreLoad |
               CKMANAGER_FUNC_PostLoad |
               CKMANAGER_FUNC_PreSave |
               CKMANAGER_FUNC_PostSave |
               CKMANAGER_FUNC_OnSequenceAddedToScene |
               CKMANAGER_FUNC_OnSequenceRemovedFromScene |
               CKMANAGER_FUNC_PreLaunchScene |
               CKMANAGER_FUNC_PostLaunchScene |
               CKMANAGER_FUNC_OnPreCopy |
               CKMANAGER_FUNC_OnPostCopy |
               CKMANAGER_FUNC_OnPreRender |
               CKMANAGER_FUNC_OnPostRender |
               CKMANAGER_FUNC_OnPostSpriteRender;
    }

    int GetFunctionPriority(CKMANAGER_FUNCTIONS Function) override {
        if (Function == CKMANAGER_FUNC_PreProcess ||
            Function == CKMANAGER_FUNC_OnPostSpriteRender)
            return -10000; // Low Priority
        if (Function == CKMANAGER_FUNC_PostProcess)
            return 10000; // High Priority
        return 0;
    }

private:
    GamePlayer *m_Player;
};

#endif // BP_GAMEMANAGER_H
