#ifndef BP_GAMEPLAYER_PRIVATE_H
#define BP_GAMEPLAYER_PRIVATE_H

#include <string>
#include <unordered_map>

#include "CKAll.h"

#if BP_ENABLE_IMGUI
#include "imgui.h"
#endif

#include "bp/Defines.h"
#include "bp/GamePlayer.h"
#include "Logger.h"
#include "Window.h"
#include "GameConfig.h"
#if BP_ENABLE_IMGUI
#include "GameManager.h"
#endif

class GameInfo;

struct BpGamePlayer {
    virtual ~BpGamePlayer() = default;
};

class GamePlayer : public BpGamePlayer {
public:
    static GamePlayer *Get(int id);
    static GamePlayer *Get(const char *name);
    static GamePlayer *Get(HWND hWnd);
    static GamePlayer *Get(CKContext *context);

    GamePlayer(const GamePlayer &) = delete;
    GamePlayer(GamePlayer &&) = delete;

    explicit GamePlayer(const char *name);
    ~GamePlayer() override;

    GamePlayer &operator=(const GamePlayer &) = delete;
    GamePlayer &operator=(GamePlayer &&) = delete;

    bool Init(HINSTANCE hInstance);
    bool Load(const char *filename = nullptr);

    void Run();
    bool Update();
    void Process();
    void Render();
#if BP_ENABLE_IMGUI
    void RenderImGui();
#endif
    void Shutdown();

    void Play();
    void Pause();
    void Reset();

    int GetId() const { return m_Id; }
    const char *GetName() const { return m_Name.c_str(); }
    BpGamePlayerState GetState() const { return m_State; }
    BpLogger *GetLogger() const { return m_Logger; }
    BpGameConfig &GetGameConfig() { return m_GameConfig; }

    HWND GetMainWindow() const { return m_MainWindow.GetHandle(); }
    HWND GetRenderWindow() const { return m_RenderWindow.GetHandle(); }

    CKContext *GetCKContext() const { return m_CKContext; }
    CKRenderContext *GetRenderContext() const { return m_RenderContext; }

    CKPluginManager *GetPluginManager() const { return m_PluginManager; }
    CKAttributeManager *GetAttributeManager() const { return m_AttributeManager; }
    CKMessageManager *GetMessageManager() const { return m_MessageManager; }
    CKTimeManager *GetTimeManager() const { return m_TimeManager; }
    CKSoundManager *GetSoundManager() const { return m_SoundManager; }
    CKInputManager *GetInputManager() const { return m_InputManager; }
    CKBehaviorManager *GetBehaviorManager() const { return m_BehaviorManager; }
    CKFloorManager *GetFloorManager() const { return m_FloorManager; }
    CKCollisionManager *GetCollisionManager() const { return m_CollisionManager; }
    CKRenderManager *GetRenderManager() const { return m_RenderManager; }
    CKParameterManager *GetParameterManager() const { return m_ParameterManager; }
    CKPathManager *GetPathManager() const { return m_PathManager; }

private:
    bool InitWindow(HINSTANCE hInstance);
    void ShutdownWindow();

    bool InitEngine(CWindow &mainWindow);
    void ShutdownEngine();

    bool InitDriver();
#if BP_ENABLE_IMGUI
    bool InitImGuiContext();
    void ShutdownImGuiContext();
#endif

    bool FinishLoad(const char *filename);
    void ReportMissingGuids(CKFile *file, const char *resolvedFile);

    bool InitPlugins(CKPluginManager *pluginManager);
    bool LoadRenderEngines(CKPluginManager *pluginManager);
    bool LoadManagers(CKPluginManager *pluginManager);
    bool LoadBuildingBlocks(CKPluginManager *pluginManager);
    bool LoadPlugins(CKPluginManager *pluginManager);
    int FindRenderEngine(CKPluginManager *pluginManager);
    bool UnloadPlugins(CKPluginManager *pluginManager, CK_PLUGIN_TYPE type, CKGUID guid);

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

    void RegisterWindow(HWND hWnd);
    bool UnregisterWindow(HWND hWnd);

    void RegisterContext(CKContext *context);
    bool UnregisterContext(CKContext *context);

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
#if BP_ENABLE_IMGUI
    int OnImeComposition(HWND hWnd, UINT gcs);
#endif

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

    static int RegisterPlayer(GamePlayer *player);
    static bool UnregisterPlayer(GamePlayer *player);

    static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK FullscreenSetupDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static int s_PlayerCount;
    static std::unordered_map<int, GamePlayer *> s_IdToPlayerMap;
    static std::unordered_map<std::string, GamePlayer *> s_NameToPlayerMap;
    static std::unordered_map<HWND, GamePlayer *> s_WindowToPlayerMap;
    static std::unordered_map<CKContext *, GamePlayer *> s_CKContextToPlayerMap;

    int m_Id = -1;
    std::string m_Name;
    BpLogger *m_Logger = nullptr;
    BpGameConfig m_GameConfig;
    BpGamePlayerState m_State = BP_PLAYER_INITIAL;

    HINSTANCE m_hInstance = nullptr;
    HACCEL m_hAccelTable = nullptr;
    WNDCLASSEX m_MainWndClass = {};
    WNDCLASS m_RenderWndClass = {};
    CWindow m_MainWindow;
    CWindow m_RenderWindow;

#if BP_ENABLE_IMGUI
    ImGuiContext *m_ImGuiContext = nullptr;
    bool m_ImGuiReady = false;
    std::wstring m_CompositionString;
#endif

    CKContext *m_CKContext = nullptr;
    CKRenderContext *m_RenderContext = nullptr;

    CKPluginManager *m_PluginManager = nullptr;
    CKAttributeManager *m_AttributeManager = nullptr;
    CKMessageManager *m_MessageManager = nullptr;
    CKTimeManager *m_TimeManager = nullptr;
    CKSoundManager *m_SoundManager = nullptr;
    CKInputManager *m_InputManager = nullptr;
    CKBehaviorManager *m_BehaviorManager = nullptr;
    CKFloorManager *m_FloorManager = nullptr;
    CKCollisionManager *m_CollisionManager = nullptr;
    CKRenderManager *m_RenderManager = nullptr;
    CKParameterManager *m_ParameterManager = nullptr;
    CKPathManager *m_PathManager = nullptr;
#if BP_ENABLE_IMGUI
    GameManager *m_GameManager = nullptr;
#endif

    CKMessageType m_MsgClick = -1;
    CKMessageType m_MsgDoubleClick = -1;

    GameInfo *m_GameInfo = nullptr;
};

#endif // BP_GAMEPLAYER_PRIVATE_H
