#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

#include "CKAll.h"

#include "GameConfig.h"

class CGameInfo;

class CGamePlayer
{
public:
    CGamePlayer();
    ~CGamePlayer();

    bool Init(HINSTANCE hInstance, const CGameConfig &config);
    bool Load(const char *filename = NULL);

    void Run();
    bool Update();
    void Process();
    void Render();
    void Shutdown();

    void Play();
    void Pause();
    void Reset();

    CGameConfig &GetConfig() { return m_Config; }
    CKContext *GetCKContext() const { return m_CKContext; }
    CKRenderContext *GetRenderContext() const { return m_RenderContext; }
    CKRenderManager *GetRenderManager() const { return m_RenderManager; }

private:
    enum PlayerState
    {
        eInitial = 0,
        eReady,
        ePlaying,
        ePaused,
        eFocusLost,
    };

    CGamePlayer(const CGamePlayer &);
    CGamePlayer &operator=(const CGamePlayer &);

    bool InitWindow(HINSTANCE hInstance);
    void ShutdownWindow();

    bool InitEngine(HWND mainWindow);
    void ShutdownEngine();

    bool InitDriver();

    bool FinishLoad(const char *filename);
    void ReportMissingGuids(CKFile *file, const char *resolvedFile);

    bool InitPlugins(CKPluginManager *pluginManager);
    bool LoadRenderEngines(CKPluginManager *pluginManager);
    bool LoadManagers(CKPluginManager *pluginManager);
    bool LoadBuildingBlocks(CKPluginManager *pluginManager);
    bool LoadPlugins(CKPluginManager *pluginManager);
    bool UnloadPlugins(CKPluginManager *pluginManager, CK_PLUGIN_TYPE type, CKGUID guid);

    int FindRenderEngine(CKPluginManager *pluginManager);

    bool SetupManagers();
    bool SetupPaths();

    void ResizeWindow();

    int FindScreenMode(int width, int height, int bpp, int driver);
    bool GetDisplayMode(int &width, int &height, int &bpp, int driver, int screenMode);
    void SetDefaultValuesForDriver();

    bool IsRenderFullscreen() const;
    bool GoFullscreen();
    bool StopFullscreen();

    bool ClipCursor();

    bool OpenSetupDialog();
    bool OpenAboutDialog();

    void OnDestroy();
    void OnMove();
    void OnSize();
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

    LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool FillDriverList(HWND hWnd);
    bool FillScreenModeList(HWND hWnd, int driver);

    static bool RegisterMainWindowClass(HINSTANCE hInstance);
    static bool RegisterRenderWindowClass(HINSTANCE hInstance);
    static bool UnregisterMainWindowClass(HINSTANCE hInstance);
    static bool UnregisterRenderWindowClass(HINSTANCE hInstance);

    static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK FullscreenSetupDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    int m_State;
    HINSTANCE m_hInstance;
    HACCEL m_hAccelTable;
    HWND m_MainWindow;
    HWND m_RenderWindow;

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