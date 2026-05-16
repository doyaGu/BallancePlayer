#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#include "CKAll.h"

#include "GameConfig.h"

struct SDL_Window;

class CGameInfo;
class InterfaceManager;
struct TTPlayerCommand;

class CGamePlayer
{
public:
    CGamePlayer();
    ~CGamePlayer();

    bool Init(const CGameConfig &config);
    bool Init(const CGameConfig &runtimeConfig, const CGameConfig &persistentConfig);
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

    bool InitWindow();
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

    void OnDestroy();
    void OnMove();
    void OnSize();
    void OnPaint();
    void OnClose();
    void OnActivateApp(bool active);
    void OnClick(bool dblClk = false);
    void OnExceptionCMO();
    void OnReturn();
    bool OnLoadCMO(const char *filename);
    void OnExitToSystem();
    void OnExitToTitle();
    int OnChangeScreenMode(int driver, int screenMode);
    void OnGoFullscreen(bool persistChange = true);
    void OnStopFullscreen(bool persistChange = true);
    void OnSwitchFullscreen();

    static int HandlePlayerCommand(InterfaceManager *manager, const TTPlayerCommand &command, void *userData);

    int m_State;
    WIN_HANDLE m_MainWindow;
    WIN_HANDLE m_RenderWindow;
    SDL_Window *m_SdlWindow;

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
    bool m_AltEnterShortcutDown;
    bool m_AltF4ShortcutDown;
    CGameConfig m_Config;
    CGameConfig m_PersistentConfig;
};

#endif /* PLAYER_GAMEPLAYER_H */
