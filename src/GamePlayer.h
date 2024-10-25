#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#include <string>

#include "CKAll.h"

#include "imgui.h"

#include "Window.h"
#include "GameConfig.h"

class GameInfo;

class GamePlayer {
public:
    static GamePlayer &GetInstance();

    GamePlayer(const GamePlayer &) = delete;
    GamePlayer(GamePlayer &&) = delete;

    ~GamePlayer();

    GamePlayer &operator=(const GamePlayer &) = delete;
    GamePlayer &operator=(GamePlayer &&) = delete;

    bool Init(HINSTANCE hInstance, const GameConfig &config);
    bool Load(const char *filename = nullptr);

    void Run();
    bool Update();
    void Exit();

    void Play();
    void Pause();
    void Reset();

    CKContext *GetCKContext() {
        return m_CKContext;
    }

    CKRenderContext *GetRenderContext() {
        return m_RenderContext;
    }

    CKRenderManager *GetRenderManager() {
        return m_RenderManager;
    }

private:
    enum PlayerState {
        eInitial = 0,
        eReady,
        ePlaying,
        ePaused,
        eFocusLost,
    };

    GamePlayer();

    GameConfig &GetConfig() {
        return m_Config;
    }

    void Process();
    void Render();

    bool InitWindow(HINSTANCE hInstance);
    bool InitEngine(CWindow &mainWindow);
    bool InitDriver();
    bool InitImGuiContext();

    bool FinishLoad();
    void ReportMissingGuids(CKFile *file, const char *resolvedFile);

    bool InitPlugins(CKPluginManager *pluginManager);
    bool LoadRenderEngines(CKPluginManager *pluginManager);
    bool LoadManagers(CKPluginManager *pluginManager);
    bool LoadBuildingBlocks(CKPluginManager *pluginManager);
    bool LoadPlugins(CKPluginManager *pluginManager);
    int InitRenderEngines(CKPluginManager *pluginManager);

    bool SetupManagers();
    bool SetupPaths();

    void ResizeWindow();

    int FindScreenMode(int width, int height, int bpp, int driver);
    bool GetDisplayMode(int &width, int &height, int &bpp, int driver, int screenMode);
    void SetDefaultValuesForDriver();

    bool IsRenderFullscreen() const;
    bool GoFullscreen();
    bool StopFullscreen();

    bool RegisterMainWindowClass(HINSTANCE hInstance);
    bool RegisterRenderWindowClass(HINSTANCE hInstance);

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
    int OnImeComposition(HWND hWnd, UINT gcs);

    void OnExceptionCMO();
    void OnReturn();
    bool OnLoadCMO(const char *filename);
    void OnExitToSystem();
    void OnExitToTitle();
    int OnChangeScreenMode(int driver, int screenMode);
    void OnGoFullscreen();
    void OnStopFullscreen();
    void OnSwitchFullscreen();

    bool FillDriverList(HWND hWnd);
    bool FillScreenModeList(HWND hWnd, int driver);

    static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK FullscreenSetupDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    int m_State = eInitial;
    HINSTANCE m_hInstance = nullptr;
    HACCEL m_hAccelTable = nullptr;
    CWindow m_MainWindow;
    CWindow m_RenderWindow;

    ImGuiContext *m_ImGuiContext = nullptr;
    bool m_ImGuiReady = false;
    std::wstring m_CompositionString;

    CKContext *m_CKContext = nullptr;
    CKRenderContext *m_RenderContext = nullptr;
    CKRenderManager *m_RenderManager = nullptr;
    CKMessageManager *m_MessageManager = nullptr;
    CKTimeManager *m_TimeManager = nullptr;
    CKAttributeManager *m_AttributeManager = nullptr;
    CKInputManager *m_InputManager = nullptr;

    CKMessageType m_MsgClick = -1;
    CKMessageType m_MsgDoubleClick = -1;

    GameInfo *m_GameInfo = nullptr;
    GameConfig m_Config;
};

#endif // PLAYER_GAMEPLAYER_H
