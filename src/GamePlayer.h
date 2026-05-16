#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#include "CKAll.h"

#include "GameConfig.h"

#if PLAYER_ENABLE_SDL3
struct SDL_Window;
typedef void *PLAYER_INSTANCE_HANDLE;
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
typedef HINSTANCE PLAYER_INSTANCE_HANDLE;
#endif

#if !PLAYER_ENABLE_SDL3
#if defined(_MSC_VER) && (_MSC_VER <= 1200)
typedef BOOL PLAYER_DIALOG_RESULT;
#else
typedef INT_PTR PLAYER_DIALOG_RESULT;
#endif
#endif

class CGameInfo;
class InterfaceManager;
struct TTPlayerCommand;

class CGamePlayer
{
public:
    CGamePlayer();
    ~CGamePlayer();

    bool Init(const CGameConfig &config, PLAYER_INSTANCE_HANDLE hInstance = NULL);
    bool Init(const CGameConfig &runtimeConfig, const CGameConfig &persistentConfig, PLAYER_INSTANCE_HANDLE hInstance = NULL);
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
    CGameConfig &GetPersistentConfig() { return m_PersistentConfig; }
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

    bool InitWindow(PLAYER_INSTANCE_HANDLE hInstance);
    void ShutdownWindow();

    bool InitEngine(WIN_HANDLE mainWindow);
    void ShutdownEngine();

    bool InitDriver();

    bool FinishLoad(const char *filename, const char *resolvedFile);
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
    void DrainPlayerCommands();
    int ExecutePlayerCommand(InterfaceManager *manager, const TTPlayerCommand &command);

    void ResizeWindow();

    int FindScreenMode(int width, int height, int bpp, int driver);
    bool GetDisplayMode(int &width, int &height, int &bpp, int driver, int screenMode);
    void SetDefaultValuesForDriver();
    void SyncPersistentDisplayConfig();
    void SyncPersistentWindowPosition();

    bool IsRenderFullscreen() const;
    bool GoFullscreen();
    bool StopFullscreen();
    void SetFullscreenDisplayMode();
    void RestoreDisplayMode();
    void SetFullscreenWindowStyle();
    void SetWindowedWindowStyle();

    bool ClipCursor();
    bool ReleaseCursorClip();

    bool OpenSetupDialog();
    bool OpenAboutDialog();
    bool OpenConfigDialog();

    void OnDestroy();
    void OnMove();
    void OnSize();
    void OnPaint();
    void OnClose();
    void OnActivateApp(bool active);
#if !PLAYER_ENABLE_SDL3
    void OnSetCursor();
    void OnGetMinMaxInfo(LPMINMAXINFO lpmmi);
    int OnSysKeyDown(UINT uKey);
#endif
    void OnClick(bool dblClk = false);
#if !PLAYER_ENABLE_SDL3
    int OnCommand(UINT id, UINT code);
#endif
    void OnExceptionCMO();
    void OnReturn();
    bool OnLoadCMO(const char *filename);
    void OnExitToSystem();
    void OnExitToTitle();
    int OnChangeScreenMode(int driver, int screenMode);
    void OnGoFullscreen(bool persistChange = true);
    void OnStopFullscreen(bool persistChange = true);
    void OnSwitchFullscreen();

#if !PLAYER_ENABLE_SDL3
    LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool FillDriverList(HWND hWnd);
    bool FillScreenModeList(HWND hWnd, int driver);

    static bool RegisterMainWindowClass(HINSTANCE hInstance);
    static bool RegisterRenderWindowClass(HINSTANCE hInstance);
    static bool UnregisterMainWindowClass(HINSTANCE hInstance);
    static bool UnregisterRenderWindowClass(HINSTANCE hInstance);

    static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static PLAYER_DIALOG_RESULT CALLBACK FullscreenSetupDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static PLAYER_DIALOG_RESULT CALLBACK AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
    static int HandlePlayerCommand(InterfaceManager *manager, const TTPlayerCommand &command, void *userData);

    int m_State;
    PLAYER_INSTANCE_HANDLE m_hInstance;
#if !PLAYER_ENABLE_SDL3
    HACCEL m_hAccelTable;
#endif
    WIN_HANDLE m_MainWindow;
    WIN_HANDLE m_RenderWindow;
#if PLAYER_ENABLE_SDL3
    SDL_Window *m_SdlWindow;
#endif

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
    CGameConfig m_PersistentConfig;
};

#endif /* PLAYER_GAMEPLAYER_H */
