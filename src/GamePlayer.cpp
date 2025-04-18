#include "GamePlayer.h"

#include <cstdio>
#include <cstring>

#if BP_ENABLE_IMGUI
#include "imgui_impl_ck2.h"
#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include "backends/imgui_impl_win32.h"
#endif

#include "bp/CK2.h"
#include "bp/Utils.h"
#include "InterfaceManager.h"

#include "resource.h"

extern HMODULE g_ModuleHandle;

#define ARRAY_NUM(Array) \
    (sizeof(Array) / sizeof(Array[0]))

#if BP_ENABLE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

extern bool EditScript(CKLevel *level, const BpGameConfig &config);

BpGamePlayer *bpCreateGamePlayer(const char *name) {
    if (!name || name[0] == '\0') {
        // Use a default name if none provided
        name = BP_DEFAULT_NAME;
    }
    return new GamePlayer(name);
}

void bpDestroyGamePlayer(BpGamePlayer *player) {
    if (!player)
        return;
    delete player;
}

BpGamePlayer *bpGetGamePlayerById(int id) {
    return GamePlayer::Get(id);
}

BpGamePlayer *bpGetGamePlayerByName(const char *name) {
    return GamePlayer::Get(name);
}

const char *bpGamePlayerGetName(const BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetName();
}

int bpGamePlayerGetId(const BpGamePlayer *player) {
    if (!player)
        return -1;
    auto *p = (GamePlayer *) player;
    return p->GetId();
}

BpGamePlayerState bpGamePlayerGetState(const BpGamePlayer *player) {
    if (!player)
        return BP_PLAYER_INITIAL;
    auto *p = (GamePlayer *) player;
    return p->GetState();
}

BpLogger *bpGamePlayerGetLogger(const BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetLogger();
}

BpGameConfig *bpGamePlayerGetConfig(const BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return &p->GetGameConfig();
}

void *bpGamePlayerGetMainWindow(const BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetMainWindow();
}

void *bpGamePlayerGetRenderWindow(const BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetRenderWindow();
}

bool bpGamePlayerInit(BpGamePlayer *player, void *hInstance) {
    if (!player)
        return false;
    auto *p = (GamePlayer *) player;
    return p->Init((HINSTANCE) hInstance);
}

bool bpGamePlayerLoad(BpGamePlayer *player, const char *filename) {
    if (!player)
        return false;
    auto *p = (GamePlayer *) player;
    return p->Load(filename);
}

void bpGamePlayerRun(BpGamePlayer *player) {
    if (!player)
        return;
    auto *p = (GamePlayer *) player;
    p->Run();
}

bool bpGamePlayerUpdate(BpGamePlayer *player) {
    if (!player)
        return false;
    auto *p = (GamePlayer *) player;
    return p->Update();
}

void bpGamePlayerProcess(BpGamePlayer *player) {
    if (!player)
        return;
    auto *p = (GamePlayer *) player;
    p->Process();
}

void bpGamePlayerRender(BpGamePlayer *player) {
    if (!player)
        return;
    auto *p = (GamePlayer *) player;
    p->Render();
}

bool bpGamePlayerShutdown(BpGamePlayer *player) {
    if (!player)
        return false;
    auto *p = (GamePlayer *) player;
    p->Shutdown();
    return true;
}

void bpGamePlayerPlay(BpGamePlayer *player) {
    if (!player)
        return;
    auto *p = (GamePlayer *) player;
    p->Play();
}

void bpGamePlayerPause(BpGamePlayer *player) {
    if (!player)
        return;
    auto *p = (GamePlayer *) player;
    p->Pause();
}

void bpGamePlayerReset(BpGamePlayer *player) {
    if (!player)
        return;
    auto *p = (GamePlayer *) player;
    p->Reset();
}

BpGamePlayer *bpGetGamePlayerByContext(CKContext *context) {
    return GamePlayer::Get(context);
}

CKContext *bpGetCKContext(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetCKContext();
}

CKRenderContext *bpGetRenderContext(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetRenderContext();
}

CKPluginManager *bpGetPluginManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetPluginManager();
}

CKAttributeManager *bpGetAttributeManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetAttributeManager();
}

CKMessageManager *bpGetMessageManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetMessageManager();
}

CKTimeManager *bpGetTimeManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetTimeManager();
}

CKSoundManager *bpGetSoundManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetSoundManager();
}

CKInputManager *bpGetInputManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetInputManager();
}

CKBehaviorManager *bpGetBehaviorManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetBehaviorManager();
}

CKFloorManager *bpGetFloorManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetFloorManager();
}

CKCollisionManager *bpGetCollisionManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetCollisionManager();
}

CKRenderManager *bpGetRenderManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetRenderManager();
}

CKParameterManager *bpGetParameterManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetParameterManager();
}

CKPathManager *bpGetPathManager(BpGamePlayer *player) {
    if (!player)
        return nullptr;
    auto *p = (GamePlayer *) player;
    return p->GetPathManager();
}

std::mutex GamePlayer::s_MapMutex;
int GamePlayer::s_PlayerCount = 0;
std::unordered_map<int, GamePlayer *> GamePlayer::s_IdToPlayerMap;
std::unordered_map<std::string, GamePlayer *> GamePlayer::s_NameToPlayerMap;
std::unordered_map<CKContext *, GamePlayer *> GamePlayer::s_CKContextToPlayerMap;

GamePlayer *GamePlayer::Get(int id) {
    std::lock_guard<std::mutex> lock(s_MapMutex);
    auto it = s_IdToPlayerMap.find(id);
    return it != s_IdToPlayerMap.end() ? it->second : nullptr;
}

GamePlayer *GamePlayer::Get(const char *name) {
    std::lock_guard<std::mutex> lock(s_MapMutex);
    if (!name) name = BP_DEFAULT_NAME;
    auto it = s_NameToPlayerMap.find(name);
    return it != s_NameToPlayerMap.end() ? it->second : nullptr;
}

GamePlayer *GamePlayer::Get(CKContext *context) {
    std::lock_guard<std::mutex> lock(s_MapMutex);
    auto it = s_CKContextToPlayerMap.find(context);
    return it != s_CKContextToPlayerMap.end() ? it->second : nullptr;
}

GamePlayer::GamePlayer(const char *name) {
    m_Name = name ? name : BP_DEFAULT_NAME;
    m_Logger = bpGetLogger(name);
    m_Logger->AddRef();

    m_Id = RegisterPlayer(this);
}

GamePlayer::~GamePlayer() {
    Shutdown();

    if (m_Logger) {
        m_Logger->Release();
        m_Logger = nullptr;
    }

    UnregisterPlayer(this);
}

bool GamePlayer::Init(HINSTANCE hInstance) {
    if (m_State != BP_PLAYER_INITIAL)
        return true;

    if (!InitWindow(hInstance)) {
        m_Logger->Error("Failed to initialize window!");
        return false;
    }

    if (!InitEngine(*m_MainWindow)) {
        m_Logger->Error("Failed to initialize CK Engine!");
        ShutdownWindow();
        return false;
    }

    if (!InitDriver()) {
        m_Logger->Error("Failed to initialize Render Driver!");
        Shutdown();
        return false;
    }

    int driver = m_GameConfig[BP_CONFIG_DRIVER].GetInt32();
    int bpp = m_GameConfig[BP_CONFIG_BPP].GetInt32();
    int width = m_GameConfig[BP_CONFIG_WIDTH].GetInt32();
    int height = m_GameConfig[BP_CONFIG_HEIGHT].GetInt32();
    bool fullscreen = m_GameConfig[BP_CONFIG_FULLSCREEN].GetBool();
    bool borderless = m_GameConfig[BP_CONFIG_BORDERLESS].GetBool();
    bool childWindowRendering = m_GameConfig[BP_CONFIG_CHILD_WINDOW_RENDERING].GetBool();

    if (borderless && m_MainWindow->GetStyle() & WS_CAPTION) {
        m_MainWindow->SetStyle(m_MainWindow->GetStyle() & ~WS_CAPTION);
    } else if (!borderless && !(m_MainWindow->GetStyle() & WS_CAPTION)) {
        m_MainWindow->SetStyle(m_MainWindow->GetStyle() | WS_CAPTION);
    }

    // Check if we need to resize the window
    int clientWidth, clientHeight;
    if (m_MainWindow->GetClientSize(clientWidth, clientHeight)) {
        if (clientWidth != width || clientHeight != height) {
            ResizeWindow();
        }
    }

    // Create render context
    HWND handle = !childWindowRendering ? m_MainWindow->GetHandle() : m_RenderWindow->GetHandle();
    CKRECT rect = {0, 0, width, height};
    m_RenderContext = m_RenderManager->CreateRenderContext(handle, driver, &rect, FALSE, bpp);
    if (!m_RenderContext) {
        m_Logger->Error("Failed to create Render Context!");
        return false;
    }
    m_Logger->Debug("Render Context created.");

    if (fullscreen)
        OnGoFullscreen();

    m_MainWindow->Show();
    m_MainWindow->SetFocus();

#if BP_ENABLE_IMGUI
    if (!InitImGuiContext()) {
        m_Logger->Error("Failed to initialize ImGui!");

        Shutdown();
        return false;
    }
    m_Logger->Debug("ImGui is initialized.");
#endif

    m_State = BP_PLAYER_READY;
    return true;
}

bool GamePlayer::Load(const char *filename) {
    if (m_State == BP_PLAYER_INITIAL) {
        m_Logger->Error("Cannot load file before initializing the player!");
        return false;
    }

    if (!filename || filename[0] == '\0') {
        filename = bpGetGamePath(&m_GameConfig, BP_PATH_CMO);
        if (!filename || filename[0] == '\0') {
            m_Logger->Error("Failed to get default filename");
            return false;
        }
    }

    if (!m_CKContext) {
        m_Logger->Error("CKContext is null");
        return false;
    }

    CKPathManager *pm = m_CKContext->GetPathManager();
    if (!pm) {
        m_Logger->Error("PathManager is null");
        return false;
    }

    XString resolvedFile = filename;
    CKERROR err = pm->ResolveFileName(resolvedFile, DATA_PATH_IDX);
    if (err != CK_OK) {
        m_Logger->Error("Failed to resolve filename %s", filename);
        return false;
    }

    m_CKContext->Reset();
    m_CKContext->ClearAll();

    // Load the file and fills the array with loaded objects
    CKFile *f = m_CKContext->CreateCKFile();
    if (!f) {
        m_Logger->Error("Failed to create CKFile");
        return false;
    }

    err = f->OpenFile(resolvedFile.Str(), (CK_LOAD_FLAGS) (CK_LOAD_DEFAULT | CK_LOAD_CHECKDEPENDENCIES));
    if (err != CK_OK) {
        // something failed
        if (err == CKERR_PLUGINSMISSING) {
            // log the missing guids
            ReportMissingGuids(f, resolvedFile.CStr());
        }
        m_CKContext->DeleteCKFile(f);

        m_Logger->Error("Failed to open file: %s", resolvedFile.CStr());
        return false;
    }

    CKObjectArray *array = CreateCKObjectArray();
    if (!array) {
        m_CKContext->DeleteCKFile(f);
        m_Logger->Error("Failed to create object array");
        return false;
    }

    err = f->LoadFileData(array);
    if (err != CK_OK) {
        m_CKContext->DeleteCKFile(f);
        DeleteCKObjectArray(array);
        m_Logger->Error("Failed to load file data (error: %d)", err);
        return false;
    }

    m_CKContext->DeleteCKFile(f);
    DeleteCKObjectArray(array);

    return FinishLoad(filename);
}

void GamePlayer::Run() {
    while (Update())
        continue;
}

bool GamePlayer::Update() {
    MSG msg;
    if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            return false;

        if (!::TranslateAccelerator(m_MainWindow->GetHandle(), m_hAccelTable, &msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    } else {
        float beforeRender = 0.0f;
        float beforeProcess = 0.0f;
        m_TimeManager->GetTimeToWaitForLimits(beforeRender, beforeProcess);
        if (beforeProcess <= 0) {
            m_TimeManager->ResetChronos(FALSE, TRUE);
            Process();
        }
        if (beforeRender <= 0) {
            m_TimeManager->ResetChronos(TRUE, FALSE);
            Render();
        }
    }

    return true;
}

void GamePlayer::Process() {
#if BP_ENABLE_IMGUI
    ImGui_ImplCK2_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif

    m_CKContext->Process();

#if BP_ENABLE_IMGUI
    ImGuiIO &io = ImGui::GetIO();
    static bool cursorVisibilityChanged = false;
    if (!m_InputManager->GetCursorVisibility()) {
        if (io.WantCaptureMouse) {
            m_InputManager->ShowCursor(TRUE);
            cursorVisibilityChanged = true;
        }
    } else if (!io.WantCaptureMouse) {
        if (cursorVisibilityChanged) {
            m_InputManager->ShowCursor(FALSE);
            cursorVisibilityChanged = false;
        }
    }

    ImGui::EndFrame();
    m_ImGuiReady = true;
#endif
}

void GamePlayer::Render() {
    if (m_RenderContext) {
        m_RenderContext->Render();
    } else {
        if (m_Logger) {
            m_Logger->Warn("Render called with null render context");
        }
    }
}

#if BP_ENABLE_IMGUI
void GamePlayer::RenderImGui() {
    if (m_ImGuiReady) {
        ImGui::Render();
        ImGui_ImplCK2_RenderDrawData(ImGui::GetDrawData());
        m_ImGuiReady = false;
    }
}
#endif

void GamePlayer::Shutdown() {
    if (m_GameInfo) {
        delete m_GameInfo;
        m_GameInfo = nullptr;
    }

    ShutdownEngine();
    ShutdownWindow();

    if (m_State != BP_PLAYER_INITIAL) {
        m_State = BP_PLAYER_INITIAL;
        m_Logger->Debug("Game is shut down.");
    }
}

void GamePlayer::Play() {
    m_State = BP_PLAYER_PLAYING;
    m_CKContext->Play();
    m_Logger->Debug("Game is playing.");
}

void GamePlayer::Pause() {
    m_State = BP_PLAYER_PAUSED;
    m_CKContext->Pause();
    m_Logger->Debug("Game is paused.");
}

void GamePlayer::Reset() {
    m_State = BP_PLAYER_PLAYING;
    m_CKContext->Reset();
    m_CKContext->Play();
    m_Logger->Debug("Game is reset.");
}

bool GamePlayer::InitWindow(HINSTANCE hInstance) {
    m_hInstance = hInstance ? hInstance : g_ModuleHandle;
    if (!m_hInstance) {
        m_Logger->Error("Failed to get the instance handle!");
        return false;
    }

    if (!Window::RegisterWindowClass(
        m_hInstance,
        std::string("Ballance"),
        WndProc,
        CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_PLAYER)),
        ::LoadCursor(nullptr, IDC_ARROW),
        (HBRUSH) ::GetStockObject(BLACK_BRUSH))) {
        m_Logger->Error("Failed to register main window class!");
        return false;
    }

    m_Logger->Debug("Main window class registered.");

    int width = m_GameConfig[BP_CONFIG_WIDTH].GetInt32();
    int height = m_GameConfig[BP_CONFIG_HEIGHT].GetInt32();
    bool fullscreen = m_GameConfig[BP_CONFIG_FULLSCREEN].GetBool();
    bool borderless = m_GameConfig[BP_CONFIG_BORDERLESS].GetBool();
    bool childWindowRendering = m_GameConfig[BP_CONFIG_CHILD_WINDOW_RENDERING].GetBool();

    if (childWindowRendering) {
        if (!Window::RegisterWindowClass(
            m_hInstance,
            std::string("Ballance Render"),
            WndProc,
            CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS)) {
            m_Logger->Error("Failed to register render window class!");
            Window::UnregisterWindowClass(m_hInstance, std::string("Ballance"));
            return false;
        }

        m_Logger->Debug("Render window class registered.");
    }

    int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    int x = m_GameConfig[BP_CONFIG_X].GetInt32();
    if (x <= -width || x >= screenWidth)
        x = (screenWidth - width) / 2;
    int y = m_GameConfig[BP_CONFIG_Y].GetInt32();
    if (y <= -height || y >= screenHeight)
        y = (screenHeight - height) / 2;

    m_MainWindow = std::make_unique<Window>();

    WindowStyles windowStyles;
    windowStyles.style = (fullscreen || borderless) ? WS_POPUP : (WS_POPUP | WS_CAPTION);
    windowStyles.exStyle = WS_EX_LEFT;

    WindowCreateParamsA createParams;
    createParams.className = "Ballance";
    createParams.title = "Ballance";
    createParams.x = x;
    createParams.y = y;
    createParams.width = width;
    createParams.height = height;
    createParams.instance = m_hInstance;
    createParams.styles = windowStyles;
    // createParams.param = this;  // Important: pass this pointer for message routing

    if (!m_MainWindow->Create(createParams)) {
        m_Logger->Error("Failed to create main window!");
        Window::UnregisterWindowClass(m_hInstance, std::string("Ballance"));
        if (childWindowRendering) {
            Window::UnregisterWindowClass(m_hInstance, std::string("Ballance Render"));
        }
        return false;
    }

    m_MainWindow->EnableDpiAwareness(true);
    m_Logger->Debug("Main window created.");

    if (childWindowRendering) {
        m_RenderWindow = std::make_unique<Window>();

        WindowStyles renderStyles;
        renderStyles.style = WS_CHILD | WS_VISIBLE;
        renderStyles.exStyle = WS_EX_TOPMOST;

        WindowCreateParams renderParams;
        renderParams.className = "Ballance Render";
        renderParams.title = "Ballance Render";
        renderParams.x = 0;
        renderParams.y = 0;
        renderParams.width = width;
        renderParams.height = height;
        renderParams.parent = m_MainWindow->GetHandle();
        renderParams.instance = m_hInstance;
        renderParams.styles = renderStyles;
        // renderParams.param = this;  // Important: pass this pointer for message routing

        if (!m_RenderWindow->Create(renderParams)) {
            m_Logger->Error("Failed to create render window!");
            m_MainWindow = nullptr;
            Window::UnregisterWindowClass(m_hInstance, std::string("Ballance"));
            Window::UnregisterWindowClass(m_hInstance, std::string("Ballance Render"));
            return false;
        }

        m_RenderWindow->EnableDpiAwareness(true);
        m_Logger->Debug("Render window created.");
    }

    SetupWindowEventHandlers(*m_MainWindow);
    if (childWindowRendering) {
        SetupWindowEventHandlers(*m_RenderWindow);
    }

    m_hAccelTable = ::LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCEL));
    if (!m_hAccelTable) {
        m_Logger->Error("Failed to load the accelerator table!");

        m_MainWindow->Destroy();
        if (childWindowRendering)
            m_RenderWindow->Destroy();
        Window::UnregisterWindowClass(m_hInstance, std::string("Ballance"));
        if (childWindowRendering) {
            Window::UnregisterWindowClass(m_hInstance, std::string("Ballance Render"));
        }
        return false;
    }

    m_GameConfig[BP_CONFIG_X] = x;
    m_GameConfig[BP_CONFIG_Y] = y;

    return true;
}

void GamePlayer::ShutdownWindow() {
    if (m_hAccelTable) {
        ::DestroyAcceleratorTable(m_hAccelTable);
        m_hAccelTable = nullptr;
    }

    m_RenderWindow = nullptr;
    m_MainWindow = nullptr;

    if (m_hInstance) {
        Window::UnregisterWindowClass(m_hInstance, std::string("Ballance Render"));
        Window::UnregisterWindowClass(m_hInstance, std::string("Ballance"));
    }
}

static CKERROR LogRedirect(CKUICallbackStruct &cbStruct, void *data) {
    if (cbStruct.Reason == CKUIM_OUTTOCONSOLE || cbStruct.Reason == CKUIM_OUTTOINFOBAR) {
        auto *logger = (BpLogger *) data;
        logger->Info(cbStruct.ConsoleString);
    }
    return CK_OK;
}

bool GamePlayer::InitEngine(Window &mainWindow) {
    if (CKStartUp() != CK_OK) {
        m_Logger->Error("CK Engine can not start up!");
        return false;
    }

    m_Logger->Debug("CK Engine starts up successfully.");

    CKPluginManager *pluginManager = CKGetPluginManager();
    if (!InitPlugins(pluginManager)) {
        m_Logger->Error("Failed to initialize plugins.");
        CKShutdown();
        return false;
    }

    int renderEngine = FindRenderEngine(pluginManager);
    if (renderEngine == -1) {
        m_Logger->Error("Failed to initialize render engine.");
        CKShutdown();
        return false;
    }

    m_Logger->Debug("Render Engine initialized.");
#if CKVERSION == 0x13022002
    CKERROR res = CKCreateContext(&m_CKContext, mainWindow.GetHandle(), renderEngine, 0);
#else
    CKERROR res = CKCreateContext(&m_CKContext, mainWindow.GetHandle(), 0);
#endif
    if (res != CK_OK) {
        m_Logger->Error("Failed to initialize CK Engine.");
        ShutdownEngine();
        return false;
    }

    m_Logger->Debug("CK Engine initialized.");

    RegisterContext(m_CKContext);
    m_CKContext->SetVirtoolsVersion(CK_VIRTOOLS_DEV, 0x2000043);
    m_CKContext->SetInterfaceMode(FALSE, LogRedirect, m_Logger);

    if (!SetupManagers()) {
        m_Logger->Error("Failed to setup managers.");
        ShutdownEngine();
        return false;
    }

    if (!SetupPaths()) {
        m_Logger->Error("Failed to setup paths.");
        ShutdownEngine();
        return false;
    }

    return true;
}

void GamePlayer::ShutdownEngine() {
    if (m_CKContext) {
        if (m_State != BP_PLAYER_INITIAL) {
            m_CKContext->Reset();
        }

#if BP_ENABLE_IMGUI
        ShutdownImGuiContext();
#endif

        if (m_State != BP_PLAYER_INITIAL) {
            m_CKContext->ClearAll();
        }

        if (m_RenderManager && m_RenderContext) {
            m_RenderManager->DestroyRenderContext(m_RenderContext);
            m_RenderContext = nullptr;
        }

        CKCloseContext(m_CKContext);
        UnregisterContext(m_CKContext);
        m_CKContext = nullptr;

        m_PluginManager = nullptr;
        m_AttributeManager = nullptr;
        m_MessageManager = nullptr;
        m_TimeManager = nullptr;
        m_SoundManager = nullptr;
        m_InputManager = nullptr;
        m_BehaviorManager = nullptr;
        m_FloorManager = nullptr;
        m_CollisionManager = nullptr;
        m_RenderManager = nullptr;
        m_ParameterManager = nullptr;
        m_PathManager = nullptr;
#if BP_ENABLE_IMGUI
        m_GameManager = nullptr;
#endif

        CKShutdown();
    }
}

bool GamePlayer::InitDriver() {
    int driverCount = m_RenderManager->GetRenderDriverCount();
    if (driverCount == 0) {
        m_Logger->Error("No render driver found.");
        return false;
    }

    if (driverCount == 1) {
        m_Logger->Debug("Found a render driver");
    } else {
        m_Logger->Debug("Found %s render drivers", driverCount);
    }

    if (m_GameConfig[BP_CONFIG_MANUAL_SETUP] == true) {
        OpenSetupDialog();
        m_GameConfig[BP_CONFIG_MANUAL_SETUP] = false;
    }

    bool tryFailed = false;

    int driver = m_GameConfig[BP_CONFIG_DRIVER].GetInt32();
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(driver);
    if (!drDesc) {
        m_Logger->Error("Unable to find driver %d", driver);
        m_GameConfig[BP_CONFIG_DRIVER] = 0;
        tryFailed = true;
        if (!OpenSetupDialog()) {
            SetDefaultValuesForDriver();
        }
        driver = m_GameConfig[BP_CONFIG_DRIVER].GetInt32();
    }

    if (tryFailed) {
        drDesc = m_RenderManager->GetRenderDriverDescription(driver);
        if (!drDesc) {
            m_Logger->Error("Unable to find driver %d", driver);
            return false;
        }
    }

    driver = m_GameConfig[BP_CONFIG_DRIVER].GetInt32();
    int width = m_GameConfig[BP_CONFIG_WIDTH].GetInt32();
    int height = m_GameConfig[BP_CONFIG_HEIGHT].GetInt32();
    int bpp = m_GameConfig[BP_CONFIG_BPP].GetInt32();

    m_Logger->Debug("Render Driver ID: %d", driver);
    m_Logger->Debug("Render Driver Name: %s", drDesc->DriverName);
    m_Logger->Debug("Render Driver Desc: %s", drDesc->DriverDesc);

    tryFailed = false;

    int screenMode = FindScreenMode(width, height, bpp, driver);
    if (screenMode == -1) {
        m_Logger->Error("Unable to find screen mode: %d x %d x %d", width, height, bpp);
        tryFailed = true;
        if (!OpenSetupDialog()) {
            SetDefaultValuesForDriver();
        }
    }

    if (tryFailed) {
        screenMode = FindScreenMode(width, height, bpp, driver);
        if (screenMode == -1) {
            m_Logger->Error("Unable to find screen mode: %d x %d x %d", width, height, bpp);
            return false;
        }
    }

    m_GameConfig[BP_CONFIG_SCREEN_MODE] = screenMode;
    m_Logger->Debug("Screen Mode: %d x %d x %d", width, height, bpp);

    return true;
}

#if BP_ENABLE_IMGUI
bool GamePlayer::InitImGuiContext() {
    m_ImGuiContext = ImGui::CreateContext();
    if (m_ImGuiContext == nullptr) {
        m_Logger->Error("Failed to create ImGui context");
        return false;
    }

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFontConfig font;
    font.SizePixels = 32.0f;
    io.Fonts->AddFontDefault(&font);

    if (!ImGui_ImplWin32_Init(m_MainWindow.GetHandle())) {
        m_Logger->Error("Failed to initialize platform backend for ImGui");
        ImGui::DestroyContext(m_ImGuiContext);
        m_ImGuiContext = nullptr;
        return false;
    }

    if (!ImGui_ImplCK2_Init(m_CKContext)) {
        m_Logger->Error("Failed to initialize render backend for ImGui");
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext(m_ImGuiContext);
        m_ImGuiContext = nullptr;
        return false;
    }

    return true;
}

void GamePlayer::ShutdownImGuiContext() {
    if (m_ImGuiContext) {
        ImGui_ImplCK2_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        m_ImGuiContext = nullptr;
    }
}
#endif

bool GamePlayer::FinishLoad(const char *filename) {
    auto *man = (InterfaceManager *) m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (man) {
        int driver = m_GameConfig[BP_CONFIG_DRIVER].GetInt32();
        int screenMode = m_GameConfig[BP_CONFIG_SCREEN_MODE].GetInt32();
        bool rookie = m_GameConfig[BP_CONFIG_ROOKIE].GetBool();
        man->SetDriver(driver);
        man->SetScreenMode(screenMode);
        man->SetRookie(rookie);
        man->SetTaskSwitchEnabled(true);

        if (m_GameInfo) {
            delete m_GameInfo;
            m_GameInfo = nullptr;
        }

        m_GameInfo = new GameInfo;
        strcpy(m_GameInfo->path, ".");
        strcpy(m_GameInfo->fileName, filename);
        man->SetGameInfo(m_GameInfo);
    }

    // Retrieve the level
    CKLevel *level = m_CKContext->GetCurrentLevel();
    if (!level) {
        m_Logger->Error("Failed to retrieve the level!");
        return false;
    }

    // Add the render context to the level
    level->AddRenderContext(m_RenderContext, TRUE);

    // Take the first camera we found and attach the viewpoint to it.
    // (in case it is not set by the composition later)
    const XObjectPointerArray cams = m_CKContext->GetObjectListByType(CKCID_CAMERA, TRUE);
    if (cams.Size() != 0)
        m_RenderContext->AttachViewpointToCamera((CKCamera *) cams[0]);

    // Hide curves ?
    int curveCount = m_CKContext->GetObjectsCountByClassID(CKCID_CURVE);
    CK_ID *curve_ids = m_CKContext->GetObjectsListByClassID(CKCID_CURVE);
    for (int i = 0; i < curveCount; ++i) {
        CKMesh *mesh = ((CKCurve *) m_CKContext->GetObject(curve_ids[i]))->GetCurrentMesh();
        if (mesh)
            mesh->Show(CKHIDE);
    }

    if (m_GameConfig[BP_CONFIG_APPLY_HOTFIX] == true &&
        m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID) != nullptr) {
        if (!EditScript(level, m_GameConfig)) {
            m_Logger->Warn("Failed to apply hotfixes on script!");
        }

        m_Logger->Debug("Hotfixes applied on script.");
    }

    // Launch the default scene
    level->LaunchScene(nullptr);

    // ReRegister OnClick Message in case it changed
    m_MsgClick = m_MessageManager->AddMessageType((CKSTRING) "OnClick");
    m_MsgDoubleClick = m_MessageManager->AddMessageType((CKSTRING) "OnDblClick");

    // Render the first frame
    m_RenderContext->Render();

    return true;
}

void GamePlayer::ReportMissingGuids(CKFile *file, const char *resolvedFile) {
    // retrieve the list of missing plugins/guids
    XClassArray<CKFilePluginDependencies> *p = file->GetMissingPlugins();
    for (CKFilePluginDependencies *it = p->Begin(); it != p->End(); it++) {
        int count = (*it).m_Guids.Size();
        for (int i = 0; i < count; i++) {
            if (!((*it).ValidGuids[i])) {
                if (resolvedFile)
                    m_Logger->Error("File Name : %s\nMissing GUIDS:\n", resolvedFile);
                m_Logger->Error("%x,%x\n", (*it).m_Guids[i].d1, (*it).m_Guids[i].d2);
            }
        }
    }
}

bool GamePlayer::InitPlugins(CKPluginManager *pluginManager) {
    if (!pluginManager)
        return false;

    if (!LoadRenderEngines(pluginManager)) {
        m_Logger->Error("Failed to load render engine!");
        return false;
    }

    if (!LoadManagers(pluginManager)) {
        m_Logger->Error("Failed to load managers!");
        return false;
    }

    if (!LoadBuildingBlocks(pluginManager)) {
        m_Logger->Error("Failed to load building blocks!");
        return false;
    }

    if (!LoadPlugins(pluginManager)) {
        m_Logger->Error("Failed to load plugins!");
        return false;
    }

    return true;
}

bool GamePlayer::LoadRenderEngines(CKPluginManager *pluginManager) {
    if (!pluginManager)
        return false;

    const char *path = bpGetGamePath(&m_GameConfig, BP_PATH_RENDER_ENGINES);
    if (!bpDirectoryExists(path) || pluginManager->ParsePlugins((CKSTRING) (path)) == 0) {
        m_Logger->Error("Render engine parse error.");
        return false;
    }

    m_Logger->Debug("Render engine loaded.");

    return true;
}

bool GamePlayer::LoadManagers(CKPluginManager *pluginManager) {
    constexpr const char *const VirtoolsManagers[] = {
        "Dx5InputManager.dll",
        "DX7SoundManager.dll",
        "ParameterOperations.dll",
    };

    if (!pluginManager)
        return false;

    const char *path = bpGetGamePath(&m_GameConfig, BP_PATH_MANAGERS);
    if (!bpDirectoryExists(path)) {
        m_Logger->Error("Managers directory does not exist!");
        return false;
    }

    m_Logger->Debug("Loading managers from %s", path);

    if (m_GameConfig[BP_CONFIG_LOAD_ALL_MANAGERS] == true) {
        if (pluginManager->ParsePlugins((CKSTRING) path) == 0) {
            m_Logger->Error("Managers parse error.");
            return false;
        }
    } else {
        char szPath[MAX_PATH];
        for (int i = 0; i < ARRAY_NUM(VirtoolsManagers); ++i) {
            bpConcatPath(szPath, MAX_PATH, path, VirtoolsManagers[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK) {
                m_Logger->Error("Unable to register manager: %s", VirtoolsManagers[i]);
                return false;
            }
        }
    }

    m_Logger->Debug("Managers loaded.");

    return true;
}

bool GamePlayer::LoadBuildingBlocks(CKPluginManager *pluginManager) {
    constexpr const char *const VirtoolsBuildingBlocks[] = {
        "3DTransfo.dll",
        "BuildingBlocksAddons1.dll",
        "Cameras.dll",
        "Collisions.dll",
        "Controllers.dll",
        "Interface.dll",
        "Lights.dll",
        "Logics.dll",
        "Materials.dll",
        "Narratives.dll",
        "Sounds.dll",
        "Visuals.dll",
        "WorldEnvironments.dll",
    };

    constexpr const char *const CustomBuildingBlocks[] = {
        "physics_RT.dll",
        "TT_DatabaseManager_RT.dll",
        "TT_Gravity_RT.dll",
        "TT_InterfaceManager_RT.dll",
        "TT_ParticleSystems_RT.dll",
        "TT_Toolbox_RT.dll",
    };

    if (!pluginManager)
        return false;

    const char *path = bpGetGamePath(&m_GameConfig, BP_PATH_BUILDING_BLOCKS);
    if (!bpDirectoryExists(path)) {
        m_Logger->Error("BuildingBlocks directory does not exist!");
        return false;
    }

    m_Logger->Debug("Loading building blocks from %s", path);

    if (m_GameConfig[BP_CONFIG_LOAD_ALL_BUILDING_BLOCKS] == true) {
        if (pluginManager->ParsePlugins((CKSTRING) path) == 0) {
            m_Logger->Error("Behaviors parse error.");
            return false;
        }
    } else {
        char szPath[MAX_PATH];
        int i;

        for (i = 0; i < ARRAY_NUM(VirtoolsBuildingBlocks); ++i) {
            bpConcatPath(szPath, MAX_PATH, path, VirtoolsBuildingBlocks[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK) {
                m_Logger->Error("Unable to register building blocks: %s", VirtoolsBuildingBlocks[i]);
                return false;
            }
        }

        for (i = 0; i < ARRAY_NUM(CustomBuildingBlocks); ++i) {
            bpConcatPath(szPath, MAX_PATH, path, CustomBuildingBlocks[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK) {
                m_Logger->Error("Unable to register building blocks: %s", CustomBuildingBlocks[i]);
                return false;
            }
        }
    }

    m_Logger->Debug("Building blocks loaded.");

    return true;
}

bool GamePlayer::LoadPlugins(CKPluginManager *pluginManager) {
    constexpr const char *const VirtoolsPlugins[] = {
        "AVIReader.dll",
        "ImageReader.dll",
        "VirtoolsLoaderR.dll",
        "WavReader.dll",
    };

    if (!pluginManager)
        return false;

    const char *path = bpGetGamePath(&m_GameConfig, BP_PATH_PLUGINS);
    if (!bpDirectoryExists(path)) {
        m_Logger->Error("Plugins directory does not exist!");
        return false;
    }

    m_Logger->Debug("Loading plugins from %s", path);

    if (m_GameConfig[BP_CONFIG_LOAD_ALL_PLUGINS] == true) {
        if (pluginManager->ParsePlugins((CKSTRING) path) == 0) {
            m_Logger->Error("Plugins parse error.");
            return false;
        }
    } else {
        char szPath[MAX_PATH];
        for (int i = 0; i < ARRAY_NUM(VirtoolsPlugins); ++i) {
            bpConcatPath(szPath, MAX_PATH, path, VirtoolsPlugins[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK) {
                m_Logger->Error("Unable to register plugin: %s", VirtoolsPlugins[i]);
                return false;
            }
        }
    }

    m_Logger->Debug("Plugins loaded.");

    return true;
}

int GamePlayer::FindRenderEngine(CKPluginManager *pluginManager) {
    if (!pluginManager)
        return -1;

    int count = pluginManager->GetPluginCount(CKPLUGIN_RENDERENGINE_DLL);
    for (int i = 0; i < count; ++i) {
        CKPluginEntry *entry = pluginManager->GetPluginInfo(CKPLUGIN_RENDERENGINE_DLL, i);
        if (!entry)
            break;

        CKPluginDll *dll = pluginManager->GetPluginDllInfo(entry->m_PluginDllIndex);
        if (!dll)
            break;

        char filename[MAX_PATH];
        _splitpath(dll->m_DllFileName.Str(), nullptr, nullptr, filename, nullptr);
        if (!_strnicmp("CK2_3D", filename, strlen(filename)))
            return i;
    }

    return -1;
}

bool GamePlayer::UnloadPlugins(CKPluginManager *pluginManager, CK_PLUGIN_TYPE type, CKGUID guid) {
    if (!pluginManager)
        return false;

    const int count = pluginManager->GetPluginCount(type);

    for (int i = 0; i < count; ++i) {
        auto *entry = pluginManager->GetPluginInfo(type, i);
        auto &info = entry->m_PluginInfo;
        if (info.m_GUID == guid) {
            if (pluginManager->UnLoadPluginDll(entry->m_PluginDllIndex) == CK_OK)
                return true;
            break;
        }
    }

    return false;
}

bool GamePlayer::SetupManagers() {
    m_PluginManager = CKGetPluginManager();
    if (!m_PluginManager) {
        m_Logger->Error("Unable to get Plugin Manager.");
        return false;
    }

    m_AttributeManager = m_CKContext->GetAttributeManager();
    if (!m_AttributeManager) {
        m_Logger->Error("Unable to get Attribute Manager.");
        return false;
    }

    m_MessageManager = m_CKContext->GetMessageManager();
    if (!m_MessageManager) {
        m_Logger->Error("Unable to get Message Manager.");
        return false;
    }

    m_TimeManager = m_CKContext->GetTimeManager();
    if (!m_TimeManager) {
        m_Logger->Error("Unable to get Time Manager.");
        return false;
    }

    m_SoundManager = (CKSoundManager *) m_CKContext->GetManagerByGuid(SOUND_MANAGER_GUID);
    if (!m_SoundManager) {
        m_Logger->Error("Unable to get Sound Manager.");
        return false;
    }

    m_InputManager = (CKInputManager *) m_CKContext->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!m_InputManager) {
        m_Logger->Error("Unable to get Input Manager.");
        return false;
    }

    m_BehaviorManager = m_CKContext->GetBehaviorManager();
    if (!m_BehaviorManager) {
        m_Logger->Error("Unable to get Behavior Manager.");
        return false;
    }

    m_FloorManager = (CKFloorManager *) m_CKContext->GetManagerByGuid(FLOOR_MANAGER_GUID);
    if (!m_FloorManager) {
        m_Logger->Error("Unable to get Floor Manager.");
        return false;
    }

    m_CollisionManager = (CKCollisionManager *) m_CKContext->GetManagerByGuid(COLLISION_MANAGER_GUID);
    if (!m_CollisionManager) {
        m_Logger->Error("Unable to get Collision Manager.");
        return false;
    }

    m_RenderManager = m_CKContext->GetRenderManager();
    if (!m_RenderManager) {
        m_Logger->Error("Unable to get Render Manager.");
        return false;
    }

    m_RenderManager->SetRenderOptions((CKSTRING) "DisablePerspectiveCorrection", m_GameConfig[BP_CONFIG_DISABLE_PERSPECTIVE_CORRECTION].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "ForceLinearFog", m_GameConfig[BP_CONFIG_FORCE_LINEAR_FOG].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "ForceSoftware", m_GameConfig[BP_CONFIG_FORCE_SOFTWARE].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "DisableFilter", m_GameConfig[BP_CONFIG_DISABLE_FILTER].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "EnsureVertexShader", m_GameConfig[BP_CONFIG_ENSURE_VERTEX_SHADER].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "UseIndexBuffers", m_GameConfig[BP_CONFIG_USE_INDEX_BUFFERS].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "DisableDithering", m_GameConfig[BP_CONFIG_DISABLE_DITHERING].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "Antialias", m_GameConfig[BP_CONFIG_ANTIALIAS].GetInt32());
    m_RenderManager->SetRenderOptions((CKSTRING) "DisableMipmap", m_GameConfig[BP_CONFIG_DISABLE_MIPMAP].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "DisableSpecular", m_GameConfig[BP_CONFIG_DISABLE_SPECULAR].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "EnableScreenDump", m_GameConfig[BP_CONFIG_ENABLE_SCREEN_DUMP].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "EnableDebugMode", m_GameConfig[BP_CONFIG_ENABLE_DEBUG_MODE].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "VertexCache", m_GameConfig[BP_CONFIG_VERTEX_CACHE].GetInt32());
    m_RenderManager->SetRenderOptions((CKSTRING) "TextureCacheManagement", m_GameConfig[BP_CONFIG_TEXTURE_CACHE_MANAGEMENT].GetBool());
    m_RenderManager->SetRenderOptions((CKSTRING) "SortTransparentObjects", m_GameConfig[BP_CONFIG_SORT_TRANSPARENT_OBJECTS].GetBool());
    auto textureVideoFormat = m_GameConfig[BP_CONFIG_TEXTURE_VIDEO_FORMAT].GetInt32();
    if (textureVideoFormat != UNKNOWN_PF)
        m_RenderManager->SetRenderOptions((CKSTRING) "TextureVideoFormat", textureVideoFormat);
    auto spriteVideoFormat = m_GameConfig[BP_CONFIG_SPRITE_VIDEO_FORMAT].GetInt32();
    if (spriteVideoFormat != UNKNOWN_PF)
        m_RenderManager->SetRenderOptions((CKSTRING) "SpriteVideoFormat", spriteVideoFormat);

    m_ParameterManager = (CKParameterManager *) m_CKContext->GetManagerByGuid(PARAMETER_MANAGER_GUID);
    if (!m_ParameterManager) {
        m_Logger->Error("Unable to get Parameter Manager.");
        return false;
    }

    m_PathManager = m_CKContext->GetPathManager();
    if (!m_PathManager) {
        m_Logger->Error("Unable to get Path Manager.");
        return false;
    }

#if BP_ENABLE_IMGUI
    m_GameManager = new GameManager(m_CKContext);
#endif

    return true;
}

bool GamePlayer::SetupPaths() {
    CKPathManager *pm = m_CKContext->GetPathManager();
    if (!pm) {
        m_Logger->Error("Unable to get Path Manager.");
        return false;
    }

    char path[MAX_PATH];
    char dir[MAX_PATH];
    ::GetCurrentDirectoryA(MAX_PATH, dir);

    if (!bpDirectoryExists(bpGetGamePath(&m_GameConfig, BP_PATH_DATA))) {
        m_Logger->Error("Data path is not found.");
        return false;
    }
    _snprintf(path, MAX_PATH, "%s\\%s", dir, bpGetGamePath(&m_GameConfig, BP_PATH_DATA));
    XString dataPath = path;
    pm->AddPath(DATA_PATH_IDX, dataPath);
    m_Logger->Debug("Data path: %s", dataPath.CStr());

    if (!bpDirectoryExists(bpGetGamePath(&m_GameConfig, BP_PATH_SOUNDS))) {
        m_Logger->Error("Sounds path is not found.");
        return false;
    }
    _snprintf(path, MAX_PATH, "%s\\%s", dir, bpGetGamePath(&m_GameConfig, BP_PATH_SOUNDS));
    XString soundPath = path;
    pm->AddPath(SOUND_PATH_IDX, soundPath);
    m_Logger->Debug("Sounds path: %s", soundPath.CStr());

    if (!bpDirectoryExists(bpGetGamePath(&m_GameConfig, BP_PATH_TEXTURES))) {
        m_Logger->Error("Textures path is not found.");
        return false;
    }
    _snprintf(path, MAX_PATH, "%s\\%s", dir, bpGetGamePath(&m_GameConfig, BP_PATH_TEXTURES));
    XString bitmapPath = path;
    pm->AddPath(BITMAP_PATH_IDX, bitmapPath);
    m_Logger->Debug("Textures path: %s", bitmapPath.CStr());

    if (bpDirectoryExists(bpGetGamePath(&m_GameConfig, BP_PATH_SCRIPTS))) {
        _snprintf(path, MAX_PATH, "%s\\%s", dir, bpGetGamePath(&m_GameConfig, BP_PATH_SCRIPTS));
        XString scriptPath = path;
        XString category = "Script Paths";
        int catIdx = pm->GetCategoryIndex(category);
        if (catIdx == -1) {
            catIdx = pm->AddCategory(category);
        }
        pm->AddPath(catIdx, scriptPath);
        m_Logger->Debug("Scripts path: %s", bitmapPath.CStr());
    }

    return true;
}

void GamePlayer::SetupWindowEventHandlers(Window &window) {
    // Setup message handling for each event type
    window.RegisterMessageHandler(WM_PAINT, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
        OnPaint();
        return false; // Allow default processing
    });

    window.RegisterMessageHandler(WM_MOVE, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
        OnMove();
        return false; // Allow default processing
    });

    window.RegisterMessageHandler(WM_SIZE, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
        OnSize();
        return false; // Allow default processing
    });

    window.RegisterMessageHandler(
        WM_ACTIVATEAPP, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
            OnActivateApp(wParam == WA_ACTIVE);
            return false; // Allow default processing
        });

    window.RegisterMessageHandler(
        WM_SETCURSOR, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
            OnSetCursor();
            return false; // Allow default processing
        });

    window.RegisterMessageHandler(
        WM_GETMINMAXINFO, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
            OnGetMinMaxInfo((LPMINMAXINFO) lParam);
            return false; // Allow default processing
        });

    window.RegisterMessageHandler(
        WM_LBUTTONDOWN, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
            OnClick(false);
            return false; // Allow default processing
        });

    window.RegisterMessageHandler(
        WM_LBUTTONDBLCLK, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
            OnClick(true);
            return false; // Allow default processing
        });

    window.RegisterMessageHandler(
        WM_SYSKEYDOWN, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
            int res = OnSysKeyDown((UINT) wParam);
            if (res != 0) {
                result = 0;
                return true; // Prevent default processing
            }
            return false; // Allow default processing
        });

    window.RegisterMessageHandler(WM_COMMAND, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
        int res = OnCommand(LOWORD(wParam), HIWORD(wParam));
        if (res != 0) {
            result = 0;
            return true; // Prevent default processing
        }
        return false; // Allow default processing
    });

    window.RegisterMessageHandler(WM_CLOSE, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
        OnClose();
        return true; // Prevent default processing
    });

    window.RegisterMessageHandler(WM_DESTROY, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
        OnDestroy();
        return true; // Prevent default processing
    });

#if BP_ENABLE_IMGUI
    window.RegisterMessageHandler(WM_IME_COMPOSITION, [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& result) -> bool {
        int res = OnImeComposition(window.GetHandle(), (UINT)lParam);
        if (res == 1) {
            return true; // Prevent default processing
        }
        return false; // Allow default processing
    });

    // Add ImGui message handling
    window.RegisterMessageHandler(0, [](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& result) -> bool {
        if (ImGui::GetCurrentContext() && ImGui_ImplWin32_WndProcHandler(nullptr, msg, wParam, lParam)) {
            result = 1;
            return true; // Prevent default processing for ImGui messages
        }
        return false; // Allow default processing
    });
#endif

    // Register custom Ballance messages
    window.RegisterMessageHandler(TT_MSG_NO_GAMEINFO,
                                  [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
                                      OnExceptionCMO();
                                      return true; // Prevent default processing
                                  });

    window.RegisterMessageHandler(TT_MSG_CMO_RESTART,
                                  [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
                                      OnReturn();
                                      return true; // Prevent default processing
                                  });

    window.RegisterMessageHandler(TT_MSG_CMO_LOAD,
                                  [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
                                      OnLoadCMO((const char *) wParam);
                                      return true; // Prevent default processing
                                  });

    window.RegisterMessageHandler(TT_MSG_EXIT_TO_SYS,
                                  [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
                                      OnExitToSystem();
                                      return true; // Prevent default processing
                                  });

    window.RegisterMessageHandler(TT_MSG_EXIT_TO_TITLE,
                                  [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
                                      OnExitToTitle();
                                      return true; // Prevent default processing
                                  });

    window.RegisterMessageHandler(TT_MSG_SCREEN_MODE_CHG,
                                  [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
                                      result = OnChangeScreenMode((int) lParam, (int) wParam);
                                      return true; // Prevent default processing
                                  });

    window.RegisterMessageHandler(TT_MSG_GO_FULLSCREEN,
                                  [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
                                      OnGoFullscreen();
                                      return true; // Prevent default processing
                                  });

    window.RegisterMessageHandler(TT_MSG_STOP_FULLSCREEN,
                                  [this](UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) -> bool {
                                      OnStopFullscreen();
                                      return true; // Prevent default processing
                                  });
}

void GamePlayer::ResizeWindow() {
    if (!m_MainWindow)
        return;

    int width = m_GameConfig[BP_CONFIG_WIDTH].GetInt32();
    int height = m_GameConfig[BP_CONFIG_HEIGHT].GetInt32();

    RECT rc = {0, 0, width, height};
    ::AdjustWindowRect(&rc, m_MainWindow->GetStyle(), FALSE);

    m_MainWindow->Resize(rc.right - rc.left, rc.bottom - rc.top);
    if (m_GameConfig[BP_CONFIG_CHILD_WINDOW_RENDERING].GetBool() && m_RenderWindow)
        m_RenderWindow->Resize(width, height);
}

int GamePlayer::FindScreenMode(int width, int height, int bpp, int driver) {
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(driver);
    if (!drDesc) {
        m_Logger->Error("Unable to find render driver %d.", driver);
        return false;
    }

#if CKVERSION == 0x13022002
    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;
#else
    XArray<VxDisplayMode> &dm = drDesc->DisplayModes;
    const int dmCount = dm.Size();
#endif

    int refreshRate = 0;
    for (int i = 0; i < dmCount; ++i) {
        if (dm[i].Width == width &&
            dm[i].Height == height &&
            dm[i].Bpp == bpp) {
            if (dm[i].RefreshRate > refreshRate)
                refreshRate = dm[i].RefreshRate;
        }
    }

    int screenMode = -1;
    for (int j = 0; j < dmCount; ++j) {
        if (dm[j].Width == width &&
            dm[j].Height == height &&
            dm[j].Bpp == bpp &&
            dm[j].RefreshRate == refreshRate) {
            screenMode = j;
            break;
        }
    }

    return screenMode;
}

bool GamePlayer::GetDisplayMode(int &width, int &height, int &bpp, int driver, int screenMode) {
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(driver);
    if (!drDesc)
        return false;

#if CKVERSION == 0x13022002
    if (screenMode < 0 || screenMode >= drDesc->DisplayModeCount)
        return false;
#else
    if (screenMode < 0 || screenMode >= drDesc->DisplayModes.Size())
        return false;
#endif

    const VxDisplayMode &dm = drDesc->DisplayModes[screenMode];
    width = dm.Width;
    height = dm.Height;
    bpp = dm.Bpp;

    return true;
}

void GamePlayer::SetDefaultValuesForDriver() {
    m_GameConfig[BP_CONFIG_DRIVER] = 0;
    m_GameConfig[BP_CONFIG_WIDTH] = BP_DEFAULT_WIDTH;
    m_GameConfig[BP_CONFIG_HEIGHT] = BP_DEFAULT_HEIGHT;
    m_GameConfig[BP_CONFIG_BPP] = BP_DEFAULT_BPP;
}

bool GamePlayer::IsRenderFullscreen() const {
    if (!m_RenderContext)
        return false;
    return m_RenderContext->IsFullScreen() == TRUE;
}

bool GamePlayer::GoFullscreen() {
    if (!m_RenderContext || IsRenderFullscreen())
        return false;

    m_GameConfig[BP_CONFIG_FULLSCREEN] = true;
    if (m_RenderContext->GoFullScreen(m_GameConfig[BP_CONFIG_WIDTH].GetInt32(),
                                      m_GameConfig[BP_CONFIG_HEIGHT].GetInt32(),
                                      m_GameConfig[BP_CONFIG_BPP].GetInt32(),
                                      m_GameConfig[BP_CONFIG_DRIVER].GetInt32()) != CK_OK) {
        m_GameConfig[BP_CONFIG_FULLSCREEN] = false;
        m_Logger->Debug("GoFullScreen Failed");
        return false;
    }

    return true;
}

bool GamePlayer::StopFullscreen() {
    if (!m_RenderContext || !IsRenderFullscreen())
        return false;

    if (m_RenderContext->StopFullScreen() != CK_OK) {
        m_Logger->Debug("StopFullscreen Failed");
        return false;
    }

    m_GameConfig[BP_CONFIG_FULLSCREEN] = false;
    return true;
}

void GamePlayer::RegisterContext(CKContext *context) {
    if (!context)
        return;

    std::lock_guard<std::mutex> lock(s_MapMutex);
    s_CKContextToPlayerMap[context] = this;
}

bool GamePlayer::UnregisterContext(CKContext *context) {
    std::lock_guard<std::mutex> lock(s_MapMutex);

    auto it = std::find_if(s_CKContextToPlayerMap.begin(), s_CKContextToPlayerMap.end(),
                           [context, this](const auto &p) {
                               return p.first == context && p.second == this;
                           });
    if (it == s_CKContextToPlayerMap.end())
        return false;

    s_CKContextToPlayerMap.erase(it);
    return true;
}

bool GamePlayer::ClipCursor() {
    if (m_GameConfig[BP_CONFIG_CLIP_CURSOR] == true) {
        RECT rect;
        m_MainWindow->GetClientRect(rect);
        m_MainWindow->ClientToScreen(rect);
        return ::ClipCursor(&rect) == TRUE;
    }
    return ::ClipCursor(nullptr) == TRUE;
}

bool GamePlayer::OpenSetupDialog() {
    return m_SetupDialog.Show(m_hInstance, m_RenderManager, &m_GameConfig);
}

bool GamePlayer::OpenAboutDialog() {
    return ::DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_ABOUT), nullptr, AboutDlgProc, 0) == IDOK;
}

void GamePlayer::OnDestroy() {
    ::ClipCursor(nullptr);
    ::PostQuitMessage(0);
}

void GamePlayer::OnMove() {
    if (!m_MainWindow || m_GameConfig[BP_CONFIG_FULLSCREEN].GetBool())
        return;

    int x, y;
    if (m_MainWindow->GetPosition(x, y)) {
        m_GameConfig[BP_CONFIG_X] = x;
        m_GameConfig[BP_CONFIG_Y] = y;
    }
}

void GamePlayer::OnSize() {
    ClipCursor();
}

void GamePlayer::OnPaint() {
    if (m_RenderContext && m_GameConfig[BP_CONFIG_FULLSCREEN] == false)
        Render();
}

void GamePlayer::OnClose() {
    m_MainWindow->Destroy();
}

void GamePlayer::OnActivateApp(bool active) {
    static bool wasPlaying = false;
    static bool wasFullscreen = false;
    static bool firstDeActivate = true;

    if (m_State == BP_PLAYER_INITIAL)
        return;

    if (!active) {
        if (m_CKContext) {
            if (firstDeActivate)
                wasPlaying = m_CKContext->IsPlaying() == TRUE;

            if (m_GameConfig[BP_CONFIG_ALWAYS_HANDLE_INPUT] == false)
                m_InputManager->Pause(TRUE);

            ::ClipCursor(nullptr);

            if (m_RenderContext && IsRenderFullscreen()) {
                if (firstDeActivate)
                    wasFullscreen = true;
                OnStopFullscreen();
            } else if (firstDeActivate) {
                wasFullscreen = false;
            }
        }
        firstDeActivate = false;
        m_State = BP_PLAYER_FOCUS_LOST;
    } else {
        if (wasPlaying)
            Play();

        if (wasFullscreen && !firstDeActivate)
            OnGoFullscreen();

        ClipCursor();

        if (m_GameConfig[BP_CONFIG_ALWAYS_HANDLE_INPUT] == false)
            m_InputManager->Pause(FALSE);

        firstDeActivate = true;
        m_State = BP_PLAYER_PLAYING;
    }
}

void GamePlayer::OnSetCursor() {
    if (m_State == BP_PLAYER_PAUSED)
        ::SetCursor(::LoadCursor(nullptr, IDC_ARROW));
}

void GamePlayer::OnGetMinMaxInfo(LPMINMAXINFO lpmmi) {
    if (lpmmi) {
        lpmmi->ptMinTrackSize.x = BP_DEFAULT_WIDTH;
        lpmmi->ptMinTrackSize.y = BP_DEFAULT_HEIGHT;
    }
}

int GamePlayer::OnSysKeyDown(UINT uKey) {
    // Manage system key (ALT + KEY)
    switch (uKey) {
    case VK_RETURN:
        // ALT + ENTER -> SwitchFullscreen
        OnSwitchFullscreen();
        break;

    case VK_F4:
        // ALT + F4 -> Quit the application
        m_MainWindow->PostMessage(TT_MSG_EXIT_TO_SYS, 0, 0);
        return 1;

    default:
        break;
    }
    return 0;
}

void GamePlayer::OnClick(bool dblClk) {
    if (!m_RenderManager)
        return;

    POINT pt;
    ::GetCursorPos(&pt);
    if (m_GameConfig[BP_CONFIG_CHILD_WINDOW_RENDERING] == true)
        m_RenderWindow->ScreenToClient(pt);
    else
        m_MainWindow->ScreenToClient(pt);

    CKMessageType msgType = (!dblClk) ? m_MsgClick : m_MsgDoubleClick;

#if CKVERSION == 0x13022002
    CKPOINT ckpt = {(int) pt.x, (int) pt.y};
    CKPICKRESULT res;
    CKObject *obj = m_RenderContext->Pick(ckpt, &res, FALSE);
    if (obj && CKIsChildClassOf(obj, CKCID_BEOBJECT))
        m_MessageManager->SendMessageSingle(msgType, (CKBeObject *) obj, nullptr);
    if (res.Sprite) {
        CKObject *sprite = m_CKContext->GetObject(res.Sprite);
        if (sprite)
            m_MessageManager->SendMessageSingle(msgType, (CKBeObject *) sprite, nullptr);
    }
#else
    VxIntersectionDesc desc;
    CKObject *obj = m_RenderContext->Pick(pt.x, pt.y, &desc);
    if (obj && CKIsChildClassOf(obj, CKCID_BEOBJECT))
        m_MessageManager->SendMessageSingle(msgType, (CKBeObject *)obj);
    if (desc.Sprite)
        m_MessageManager->SendMessageSingle(msgType, (CKBeObject *)desc.Sprite);
#endif
}

int GamePlayer::OnCommand(UINT id, UINT code) {
    if (id == IDM_APP_ABOUT) {
        OpenAboutDialog();
    }
    return 0;
}

#if BP_ENABLE_IMGUI
int GamePlayer::OnImeComposition(HWND hWnd, UINT gcs) {
    if (gcs & GCS_RESULTSTR) {
        if (!ImGui::GetCurrentContext())
            return 0;

        HIMC hIMC = ImmGetContext(hWnd);
        LONG len = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, nullptr, 0) / (LONG) sizeof(WCHAR);
        if (len > (LONG) m_CompositionString.size())
            m_CompositionString.resize(len);
        ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, m_CompositionString.data(), len * sizeof(WCHAR));
        ImmReleaseContext(hWnd, hIMC);
        for (int i = 0; i < len; ++i) {
            ImGui::GetIO().AddInputCharacterUTF16(m_CompositionString[i]);
        }
    }
    return 1;
}
#endif

void GamePlayer::OnExceptionCMO() {
    m_Logger->Error("Exception in the CMO - Abort");
    m_MainWindow->PostMessage(TT_MSG_EXIT_TO_SYS, 0, 0);
}

void GamePlayer::OnReturn() {
    XString filename = m_GameInfo->fileName;
    if (!Load(filename.CStr())) {
        OnClose();
        return;
    }

    Play();
}

bool GamePlayer::OnLoadCMO(const char *filename) {
    return Load(filename);
}

void GamePlayer::OnExitToSystem() {
    bool fullscreen = m_GameConfig[BP_CONFIG_FULLSCREEN].GetBool();
    OnStopFullscreen();
    m_GameConfig[BP_CONFIG_FULLSCREEN] = fullscreen;

    m_MainWindow->PostMessage(WM_CLOSE, 0, 0);
}

void GamePlayer::OnExitToTitle() {}

int GamePlayer::OnChangeScreenMode(int driver, int screenMode) {
    int width, height, bpp;
    if (!GetDisplayMode(width, height, bpp, driver, screenMode)) {
        m_Logger->Error("Failed to change screen mode.");
        return 0;
    }

    bool fullscreen = m_GameConfig[BP_CONFIG_FULLSCREEN].GetBool();
    if (fullscreen)
        StopFullscreen();

    ClipCursor();

    m_GameConfig[BP_CONFIG_DRIVER] = driver;
    m_GameConfig[BP_CONFIG_SCREEN_MODE] = screenMode;
    m_GameConfig[BP_CONFIG_WIDTH] = width;
    m_GameConfig[BP_CONFIG_HEIGHT] = height;
    m_GameConfig[BP_CONFIG_BPP] = bpp;

    InterfaceManager *im = (InterfaceManager *) m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    im->SetDriver(driver);
    im->SetScreenMode(screenMode);

    ResizeWindow();
    m_RenderContext->Resize();

    if (fullscreen)
        GoFullscreen();

    return 1;
}

void GamePlayer::OnGoFullscreen() {
    if (!m_RenderContext)
        return;

    ::ClipCursor(nullptr);

    if (GoFullscreen()) {
        int width = m_GameConfig[BP_CONFIG_WIDTH].GetInt32();
        int height = m_GameConfig[BP_CONFIG_HEIGHT].GetInt32();
        bool childWindowRendering = m_GameConfig[BP_CONFIG_CHILD_WINDOW_RENDERING].GetBool();

        // Update main window style and size
        m_MainWindow->SetStyle(WS_POPUP);
        m_MainWindow->SetPositionAndSize(0, 0, width, height);

        // Show and focus
        m_MainWindow->Show();
        m_MainWindow->SetFocus();

        // Handle render window if needed
        if (childWindowRendering && m_RenderWindow) {
            m_RenderWindow->Show();
            m_RenderWindow->SetFocus();
        }

        // Update windows
        m_MainWindow->Update();
        if (childWindowRendering && m_RenderWindow) {
            m_RenderWindow->Update();
        }
    }
}

void GamePlayer::OnStopFullscreen() {
    if (!m_RenderContext)
        return;

    if (StopFullscreen()) {
        int x = m_GameConfig[BP_CONFIG_X].GetInt32();
        int y = m_GameConfig[BP_CONFIG_Y].GetInt32();
        int width = m_GameConfig[BP_CONFIG_WIDTH].GetInt32();
        int height = m_GameConfig[BP_CONFIG_HEIGHT].GetInt32();
        bool childWindowRendering = m_GameConfig[BP_CONFIG_CHILD_WINDOW_RENDERING].GetBool();
        bool borderless = m_GameConfig[BP_CONFIG_BORDERLESS].GetBool();

        // Calculate new style and window size
        DWORD style = borderless ? WS_POPUP : (WS_POPUP | WS_CAPTION);
        RECT rc = {0, 0, width, height};
        ::AdjustWindowRect(&rc, style, FALSE);

        // Update window style and position
        m_MainWindow->SetStyle(style);
        m_MainWindow->SetPositionAndSize(x, y, rc.right - rc.left, rc.bottom - rc.top);

        // Show and focus
        m_MainWindow->Show();
        m_MainWindow->SetFocus();

        // Handle render window if needed
        if (childWindowRendering && m_RenderWindow) {
            m_RenderWindow->Resize(width, height);
            m_RenderWindow->SetFocus();
        }

        // Update windows
        m_MainWindow->Update();
        if (childWindowRendering && m_RenderWindow) {
            m_RenderWindow->Update();
        }
    }

    ClipCursor();
}

void GamePlayer::OnSwitchFullscreen() {
    if (m_State == BP_PLAYER_INITIAL)
        return;

    if (!IsRenderFullscreen())
        OnGoFullscreen();
    else
        OnStopFullscreen();
}

int GamePlayer::RegisterPlayer(GamePlayer *player) {
    if (!player)
        return -1;

    std::lock_guard<std::mutex> lock(s_MapMutex);

    auto it = std::find_if(s_IdToPlayerMap.begin(), s_IdToPlayerMap.end(), [player](const auto &p) {
        return p.second == player;
    });
    if (it != s_IdToPlayerMap.end())
        return it->first;

    int id = s_PlayerCount++;
    s_IdToPlayerMap[id] = player;

    const char *name = player->GetName();
    if (name && name[0] != '\0') {
        s_NameToPlayerMap[name] = player;
    } else {
        s_NameToPlayerMap["unknown"] = player;
    }

    return id;
}

bool GamePlayer::UnregisterPlayer(GamePlayer *player) {
    if (!player)
        return false;

    std::lock_guard<std::mutex> lock(s_MapMutex);

    auto it = std::find_if(s_IdToPlayerMap.begin(), s_IdToPlayerMap.end(), [player](const auto &p) {
        return p.second == player;
    });
    if (it == s_IdToPlayerMap.end())
        return false;

    const char *name = player->GetName();
    if (name && name[0] != '\0') {
        s_NameToPlayerMap.erase(name);
    } else {
        s_NameToPlayerMap.erase("unknown");
    }

    s_IdToPlayerMap.erase(it);
    return true;
}

LRESULT GamePlayer::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
#if BP_ENABLE_IMGUI
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return 1;
#endif

    auto *pWindow = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (pWindow) {
        return pWindow->HandleMessage(uMsg, wParam, lParam);
    }

    if (uMsg == WM_CREATE) {
        // Store Window instance pointer in window's user data
        auto *cs = reinterpret_cast<CREATESTRUCT *>(lParam);
        pWindow = static_cast<Window *>(cs->lpCreateParams);
        if (pWindow) {
            ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        }
    }

    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

BOOL GamePlayer::AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                ::EndDialog(hWnd, LOWORD(wParam));
                return TRUE;
            }
            break;

        default:
            break;
    }

    return FALSE;
}
