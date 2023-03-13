#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#include "CKAll.h"

#include "Window.h"
#include "GameConfig.h"

class CGameInfo;

class CGamePlayer
{
public:
    static CGamePlayer &GetInstance();

    ~CGamePlayer();

    bool Init(HINSTANCE hInstance, const CGameConfig &config);
    bool Load(const char *filename);

    void Run();
    bool Update();
    void Exit();

    void Play();
    void Pause();
    void Reset();

    CKContext *GetCKContext()
    {
        return m_CKContext;
    }

    CKRenderContext *GetRenderContext()
    {
        return m_RenderContext;
    }

    CKRenderManager *GetRenderManager()
    {
        return m_RenderManager;
    }

private:
    enum PlayerState
    {
        eInitial = 0,
        eReady = 1,
        ePlaying = 2,
        ePaused = 3,
        eFocusLost = 4,
    };

    CGamePlayer();
    CGamePlayer(const CGamePlayer &);
    CGamePlayer &operator=(const CGamePlayer &);

    CGameConfig &GetConfig()
    {
        return m_Config;
    }

    void Process();
    void Render();

    bool InitWindow(HINSTANCE hInstance);
    bool InitEngine(CWindow &mainWindow);
    bool InitDriver();

    bool FinishLoad();
    void ReportMissingGuids(CKFile *file, const char *resolvedFile);

    bool InitPlugins(CKPluginManager *pluginManager);
    bool LoadRenderEngines(CKPluginManager *pluginManager);
    bool LoadManagers(CKPluginManager *pluginManager);
    bool LoadBuildingBlocks(CKPluginManager *pluginManager);
    bool LoadPlugins(CKPluginManager *pluginManager);
    int InitRenderEngines(CKPluginManager *pluginManager);

    void ResizeWindow();

    int FindScreenMode(int width, int height, int bpp, int driver);
    bool GetDisplayMode(int &width, int &height, int &bpp, int driver, int screenMode);
    bool SetDefaultValuesForDriver();

    bool IsRenderFullscreen() const;
    bool GoFullscreen();
    bool StopFullscreen();

    bool ClipMouse(bool enable);

    bool RegisterMainWindowClass(HINSTANCE hInstance);
    bool RegisterRenderWindowClass(HINSTANCE hInstance);

    bool OpenSetupDialog();
    bool OpenAboutDialog();

    void OnDestroy();
    void OnMove();
    void OnPaint();
    void OnClose();
    void OnActivateApp(bool active);
    void OnSetCursor();
    void OnGetMinMaxInfo(LPMINMAXINFO lpmmi);
    int OnSysKeyDown(UINT uKey);
    void OnClick(bool dblClk = false);
    int OnCommand(UINT id, UINT code);
    void OnExceptionCMO();
    void OnReturn();
    bool OnLoadCMO(const char *filename);
    void OnExitToSystem();
    void OnExitToTitle();
    int OnChangeScreenMode(int driver, int screenMode);
    void OnGoFullscreen();
    void OnStopFullscreen();
    void OnSwitchFullscreen();

    void FillDriverList(HWND hWnd);
    void FillScreenModeList(HWND hWnd, int driver);

    static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK FullscreenSetupDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    int m_State;
    HINSTANCE m_hInstance;
    HACCEL m_hAccelTable;
    CWindow m_MainWindow;
    CWindow m_RenderWindow;

    CKContext *m_CKContext;
    CKRenderContext *m_RenderContext;
    CKRenderManager *m_RenderManager;
    CKMessageManager *m_MessageManager;
    CKTimeManager *m_TimeManager;
    CKAttributeManager *m_AttributeManager;
    CKInputManager *m_InputManager;

    CKMessageType m_MsgClick;
    CKMessageType m_MsgDoubleClick;

    CGameInfo *m_GameInfo;
    CGameConfig m_Config;
};

#endif /* PLAYER_GAMEPLAYER_H */