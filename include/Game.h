#ifndef PLAYER_GAME_H
#define PLAYER_GAME_H

#include "CKLevel.h"

class CGame;
class CGameInfo;
class CNeMoContext;
class CGameEventHandler;

typedef void (*GAMEEVENT_CALLBACK)(CGame *game, void *argument);

class CGame
{
    friend CGameEventHandler;

public:
    CGame();
    ~CGame();

    bool Load();

    bool IsInGame() const;
    bool IsPaused() const;
    bool IsPlaying() const;

    void ExitGame();
    void PauseLevel();
    void UnpauseLevel();

    void ShouldResize();
    void ShowCursor(bool show);
    bool GetCursorVisibility() const;

    CGameInfo *NewGameInfo();
    CGameInfo *GetGameInfo() const;
    void SetGameInfo(CGameInfo *gameInfo);

    void HandleGameEvent();
    void HandlePostGameEvent();

private:
    bool FinishLoad();
    void InitEventHandler();
    bool GetGameplay();
    void HandleLevelEvent();
    void HandleGameplayEvent();
    void SendMessageToLevel(const char *msg);
    void SendMessageToGameplay(const char *msg);
    void AddPostEventCallback(GAMEEVENT_CALLBACK callback);
    void SyncCamerasWithScreen();

    CNeMoContext *m_NeMoContext;
    CGameInfo *m_GameInfo;
    CKLevel *m_Level;
    CKGroup *m_Gameplay;
    bool m_InGame;
    bool m_Paused;
    bool m_ShowCursor;
    XHashTable<GAMEEVENT_CALLBACK, XString> m_LevelEventHandlers;
    XHashTable<GAMEEVENT_CALLBACK, XString> m_GameplayEventHandlers;
    XArray<GAMEEVENT_CALLBACK> m_PostEventCallbacks;
};

#endif /* PLAYER_GAME_H */