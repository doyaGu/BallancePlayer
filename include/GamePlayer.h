#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#include "Game.h"
#include "NeMoContext.h"
#include "WinContext.h"

class CGamePlayer
{
public:
    enum PlayerState
    {
        eInitial = 0,
        eReady = 1,
        ePlaying = 2,
        ePaused = 3,
        eFocusLost = 4
    };

    CGamePlayer();
    ~CGamePlayer();

    bool Init(HINSTANCE hInstance, HANDLE hMutex);

    void Run();
    bool Update();
    void Exit();

    bool Load(const char *filename);

    void Play();
    void Pause();
    void Reset();

    void OnDestroy();
    void OnMove();
    void OnSized();
    void OnPaint();
    void OnClose();
    void OnActivateApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnSetCursor();
    void OnGetMinMaxInfo(LPMINMAXINFO lpmmi);
    int OnSysKeyDown(UINT uKey);
    int OnCommand(UINT id, UINT code);
    void OnExceptionCMO(WPARAM wParam, LPARAM lParam);
    void OnReturn(WPARAM wParam, LPARAM lParam);
    bool OnLoadCMO(WPARAM wParam, LPARAM lParam);
    void OnExitToSystem(WPARAM wParam, LPARAM lParam);
    void OnExitToTitle(WPARAM wParam, LPARAM lParam);
    int OnChangeScreenMode(WPARAM wParam, LPARAM lParam);
    void OnGoFullscreen();
    void OnStopFullscreen();
    void OnSwitchFullscreen();

    static void Register(CGamePlayer *player);
    static CGamePlayer *GetInstance();

private:
    CGamePlayer(const CGamePlayer &);
    CGamePlayer &operator=(const CGamePlayer &);

    int InitEngine();
    bool ReInitEngine();
    bool LoadRenderEngine();
    bool LoadManagers();
    bool LoadBuildingBlocks();
    bool LoadPlugins();
    void RedirectLog();

    int m_State;
    CNeMoContext *m_NeMoContext;
    CWinContext *m_WinContext;
    CGame *m_Game;
    HANDLE m_hMutex;

    static CGamePlayer *s_Instance;
};

#endif /* PLAYER_GAMEPLAYER_H */