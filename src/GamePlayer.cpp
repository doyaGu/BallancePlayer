#include "GamePlayer.h"

#include <cstdio>
#include <cstring>

#include "imgui_impl_ck2.h"
#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include "backends/imgui_impl_win32.h"

#include "Logger.h"
#include "Utils.h"
#include "InterfaceManager.h"

#include "resource.h"

#define ARRAY_NUM(Array) \
    (sizeof(Array) / sizeof(Array[0]))

#define INPUTMANAGER_GUID CKGUID(0xF787C904, 0)
#define SOUNDMANAGER_GUID CKGUID(0x77135393, 0x225c679a)

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern bool EditScript(CKLevel *level, const GameConfig &config);

GamePlayer &GamePlayer::GetInstance() {
    static GamePlayer player;
    return player;
}

GamePlayer::~GamePlayer() {
    Exit();
}

bool GamePlayer::Init(HINSTANCE hInstance, const GameConfig &config) {
    if (m_State != eInitial)
        return true;

    if (!hInstance)
        return false;

    m_hInstance = hInstance;
    m_Config = config;

    if (!InitWindow(hInstance)) {
        Logger::Get().Error("Failed to initialize window!");
        return false;
    }

    if (!InitEngine(m_MainWindow)) {
        Logger::Get().Error("Failed to initialize CK Engine!");
        return false;
    }

    if (!InitDriver()) {
        Logger::Get().Error("Failed to initialize Render Driver!");
        return false;
    }

    RECT rc;
    m_MainWindow.GetClientRect(&rc);
    if (rc.right - rc.left != m_Config.width || rc.bottom - rc.top != m_Config.height) {
        ResizeWindow();
    }

    HWND handle = !m_Config.childWindowRendering ? m_MainWindow.GetHandle() : m_RenderWindow.GetHandle();
    CKRECT rect = {0, 0, m_Config.width, m_Config.height};
    m_RenderContext = m_RenderManager->CreateRenderContext(handle, m_Config.driver, &rect, FALSE, m_Config.bpp);
    if (!m_RenderContext) {
        Logger::Get().Error("Failed to create Render Context!");
        return false;
    }
    Logger::Get().Debug("Render Context created.");

    if (m_Config.fullscreen)
        OnGoFullscreen();

    m_MainWindow.Show();
    m_MainWindow.SetFocus();

    if (!InitImGuiContext()) {
        Logger::Get().Error("Failed to initialize ImGui!");
        return false;
    }
    Logger::Get().Debug("ImGui is initialized.");

    m_State = eReady;
    return true;
}

bool GamePlayer::Load(const char *filename) {
    if (m_State == eInitial)
        return false;

    if (!filename || filename[0] == '\0')
        filename = m_Config.GetPath(eCmoPath);

    if (!m_CKContext)
        return false;

    CKPathManager *pm = m_CKContext->GetPathManager();
    if (!pm)
        return false;

    XString resolvedFile = filename;
    CKERROR err = pm->ResolveFileName(resolvedFile, DATA_PATH_IDX);
    if (err != CK_OK) {
        Logger::Get().Error("Failed to resolve filename %s", filename);
        return false;
    }

    m_CKContext->Reset();
    m_CKContext->ClearAll();

    // Load the file and fills the array with loaded objects
    CKFile *f = m_CKContext->CreateCKFile();
    CKERROR res = f->OpenFile(resolvedFile.Str(), (CK_LOAD_FLAGS) (CK_LOAD_DEFAULT | CK_LOAD_CHECKDEPENDENCIES));
    if (res != CK_OK) {
        // something failed
        if (res == CKERR_PLUGINSMISSING) {
            // log the missing guids
            ReportMissingGuids(f, resolvedFile.CStr());
        }
        m_CKContext->DeleteCKFile(f);

        Logger::Get().Error("Failed to open file: %s", resolvedFile.CStr());
        return false;
    }

    CKObjectArray *array = CreateCKObjectArray();
    res = f->LoadFileData(array);
    if (res != CK_OK) {
        m_CKContext->DeleteCKFile(f);
        return false;
    }

    m_CKContext->DeleteCKFile(f);
    DeleteCKObjectArray(array);

    auto *man = (InterfaceManager *) m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (man) {
        man->SetDriver(m_Config.driver);
        man->SetScreenMode(m_Config.screenMode);
        man->SetRookie(m_Config.rookie);
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

    return FinishLoad();
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

        if (!::TranslateAccelerator(m_MainWindow.GetHandle(), m_hAccelTable, &msg)) {
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

void GamePlayer::Exit() {
    if (m_GameInfo) {
        delete m_GameInfo;
        m_GameInfo = nullptr;
    }

    if (m_CKContext) {
        m_CKContext->Reset();
        m_CKContext->ClearAll();

        if (m_ImGuiContext != nullptr) {
            ImGui_ImplCK2_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            m_ImGuiContext = nullptr;
        }

        if (m_RenderManager && m_RenderContext) {
            m_RenderManager->DestroyRenderContext(m_RenderContext);
            m_RenderContext = nullptr;
        }

        RemoveInputManager();
        RemoveSoundManager();
        CKCloseContext(m_CKContext);
        CKShutdown();
        m_CKContext = nullptr;
    }

    // Save settings
    if (m_Config.HasPath(eConfigPath))
        m_Config.SaveToIni();
}

void GamePlayer::Play() {
    m_State = ePlaying;
    m_CKContext->Play();
    Logger::Get().Debug("Game is playing.");
}

void GamePlayer::Pause() {
    m_State = ePaused;
    m_CKContext->Pause();
    Logger::Get().Debug("Game is paused.");
}

void GamePlayer::Reset() {
    m_State = ePlaying;
    m_CKContext->Reset();
    m_CKContext->Play();
    Logger::Get().Debug("Game is reset.");
}

GamePlayer::GamePlayer() = default;

void GamePlayer::Process() {
    ImGui_ImplCK2_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiIO &io = ImGui::GetIO();

    if (io.WantCaptureKeyboard) {
        m_InputManager->DisableDevice(CK_INPUT_DEVICE_KEYBOARD);
    }

    if (io.WantCaptureMouse) {
        m_InputManager->DisableDevice(CK_INPUT_DEVICE_MOUSE);
    }

    m_CKContext->Process();

    if (io.WantCaptureKeyboard) {
        m_InputManager->EnableDevice(CK_INPUT_DEVICE_KEYBOARD);
    }

    if (io.WantCaptureMouse) {
        m_InputManager->EnableDevice(CK_INPUT_DEVICE_MOUSE);
    }

    static bool cursorVisibilityChanged = false;
    if (!m_InputManager->GetCursorVisibility()) {
        if (io.WantCaptureMouse) {
            m_InputManager->ShowCursor(TRUE);
            cursorVisibilityChanged = true;
        }
    } else if (cursorVisibilityChanged) {
        if (!io.WantCaptureMouse) {
            m_InputManager->ShowCursor(FALSE);
            cursorVisibilityChanged = false;
        }
    }

    ImGui::EndFrame();
    m_ImGuiReady = true;
}

void GamePlayer::Render() {
    m_RenderContext->ChangeCurrentRenderOptions(0, CK_RENDER_DOBACKTOFRONT);
    m_RenderContext->Render();
    m_RenderContext->ChangeCurrentRenderOptions(CK_RENDER_DOBACKTOFRONT, 0);

    if (m_ImGuiReady) {
        ImGui::Render();
        ImGui_ImplCK2_RenderDrawData(ImGui::GetDrawData());
        m_ImGuiReady = false;
    }

    m_RenderContext->BackToFront();
}

static CKERROR LogRedirect(CKUICallbackStruct &cbStruct, void *) {
    if (cbStruct.Reason == CKUIM_OUTTOCONSOLE ||
        cbStruct.Reason == CKUIM_OUTTOINFOBAR ||
        cbStruct.Reason == CKUIM_DEBUGMESSAGESEND) {
        static XString text = "";
        if (text.Compare(cbStruct.ConsoleString)) {
            Logger::Get().Info(cbStruct.ConsoleString);
            text = cbStruct.ConsoleString;
        }
    }
    return CK_OK;
}

bool GamePlayer::InitWindow(HINSTANCE hInstance) {
    if (!RegisterMainWindowClass(hInstance)) {
        Logger::Get().Error("Failed to register main window class!");
        return false;
    }

    Logger::Get().Debug("Main window class registered.");

    if (m_Config.childWindowRendering) {
        if (!RegisterRenderWindowClass(hInstance)) {
            Logger::Get().Error("Failed to register render window class!");
            return false;
        }

        Logger::Get().Debug("Render window class registered.");
    }

    DWORD style = (m_Config.fullscreen || m_Config.borderless) ? WS_POPUP : WS_POPUP | WS_CAPTION;

    RECT rect = {0, 0, m_Config.width, m_Config.height};
    ::AdjustWindowRect(&rect, style, FALSE);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    int x = m_Config.posX;
    if (x <= -width || x >= screenWidth)
        x = (screenWidth - width) / 2;
    int y = m_Config.posY;
    if (y <= -height || y >= screenHeight)
        y = (screenHeight - height) / 2;

    if (!m_MainWindow.CreateEx(WS_EX_LEFT, TEXT("Ballance"), TEXT("Ballance"), style,
                               x, y, width, height, nullptr, nullptr, hInstance, nullptr)) {
        Logger::Get().Error("Failed to create main window!");
        return false;
    }

    Logger::Get().Debug("Main window created.");

    if (m_Config.childWindowRendering) {
        if (!m_RenderWindow.CreateEx(WS_EX_TOPMOST, TEXT("Ballance Render"), TEXT("Ballance Render"), WS_CHILD | WS_VISIBLE,
                                     0, 0, m_Config.width, m_Config.height, m_MainWindow.GetHandle(), nullptr, hInstance, nullptr)) {
            Logger::Get().Error("Failed to create render window!");
            return false;
        }

        Logger::Get().Debug("Render window created.");
    }

    m_hAccelTable = ::LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCEL));
    if (!m_hAccelTable) {
        Logger::Get().Error("Failed to load the accelerator table!");
        return false;
    }

    m_Config.posX = x;
    m_Config.posY = y;

    return true;
}

bool GamePlayer::InitEngine(CWindow &mainWindow) {
    if (CKStartUp() != CK_OK) {
        Logger::Get().Error("CK Engine can not start up!");
        return false;
    }

    Logger::Get().Debug("CK Engine starts up successfully.");

    CKPluginManager *pluginManager = CKGetPluginManager();
    if (!InitPlugins(pluginManager)) {
        Logger::Get().Error("Failed to initialize plugins.");
        return false;
    }

    int renderEngine = InitRenderEngines(pluginManager);
    if (renderEngine == -1) {
        Logger::Get().Error("Failed to initialize render engine.");
        return false;
    }

    Logger::Get().Debug("Render Engine initialized.");
#if CKVERSION == 0x13022002
    CKERROR res = CKCreateContext(&m_CKContext, mainWindow.GetHandle(), renderEngine, 0);
#else
    CKERROR res = CKCreateContext(&m_CKContext, mainWindow.GetHandle(), 0);
#endif
    if (res != CK_OK) {
        Logger::Get().Error("Failed to initialize CK Engine.");
        return false;
    }

    Logger::Get().Debug("CK Engine initialized.");

    m_CKContext->SetVirtoolsVersion(CK_VIRTOOLS_DEV, 0x2000043);
    m_CKContext->SetInterfaceMode(FALSE, LogRedirect, nullptr);

    if (!SetupManagers()) {
        Logger::Get().Error("Failed to setup managers.");
        return false;
    }

    if (!SetupPaths()) {
        Logger::Get().Error("Failed to setup paths.");
        return false;
    }

    return true;
}

bool GamePlayer::InitDriver() {
    int driverCount = m_RenderManager->GetRenderDriverCount();
    if (driverCount == 0) {
        Logger::Get().Error("No render driver found.");
        return false;
    }

    if (driverCount == 1) {
        Logger::Get().Debug("Found a render driver");
    } else {
        Logger::Get().Debug("Found %s render drivers", driverCount);
    }

    if (m_Config.manualSetup)
        OpenSetupDialog();

    m_Config.manualSetup = false;
    bool tryFailed = false;

    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Config.driver);
    if (!drDesc) {
        Logger::Get().Error("Unable to find driver %d", m_Config.driver);
        m_Config.driver = 0;
        tryFailed = true;
        if (!OpenSetupDialog()) {
            SetDefaultValuesForDriver();
        }
    }

    if (tryFailed) {
        drDesc = m_RenderManager->GetRenderDriverDescription(m_Config.driver);
        if (!drDesc) {
            Logger::Get().Error("Unable to find driver %d", m_Config.driver);
            return false;
        }
    }

    Logger::Get().Debug("Render Driver ID: %d", m_Config.driver);
    Logger::Get().Debug("Render Driver Name: %s", drDesc->DriverName);
    Logger::Get().Debug("Render Driver Desc: %s", drDesc->DriverDesc);

    tryFailed = false;

    m_Config.screenMode = FindScreenMode(m_Config.width, m_Config.height, m_Config.bpp, m_Config.driver);
    if (m_Config.screenMode == -1) {
        Logger::Get().Error("Unable to find screen mode: %d x %d x %d", m_Config.width, m_Config.height, m_Config.bpp);
        tryFailed = true;
        if (!OpenSetupDialog()) {
            SetDefaultValuesForDriver();
        }
    }

    if (tryFailed) {
        m_Config.screenMode = FindScreenMode(m_Config.width, m_Config.height, m_Config.bpp, m_Config.driver);
        if (m_Config.screenMode == -1) {
            Logger::Get().Error("Unable to find screen mode: %d x %d x %d", m_Config.width, m_Config.height, m_Config.bpp);
            return false;
        }
    }

    Logger::Get().Debug("Screen Mode: %d x %d x %d", m_Config.width, m_Config.height, m_Config.bpp);

    return true;
}

bool GamePlayer::InitImGuiContext() {
    m_ImGuiContext = ImGui::CreateContext();
    if (m_ImGuiContext == nullptr) {
        Logger::Get().Error("Failed to create ImGui context");
        return false;
    }

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFontConfig font;
    font.SizePixels = 32.0f;
    io.Fonts->AddFontDefault(&font);

    if (!ImGui_ImplWin32_Init(m_MainWindow.GetHandle())) {
        Logger::Get().Error("Failed to initialize platform backend for ImGui");
        return false;
    }

    if (!ImGui_ImplCK2_Init(m_CKContext)) {
        Logger::Get().Error("Failed to initialize render backend for ImGui");
        return false;
    }

    return true;
}

bool GamePlayer::FinishLoad() {
    // Retrieve the level
    CKLevel *level = m_CKContext->GetCurrentLevel();
    if (!level) {
        Logger::Get().Error("Failed to retrieve the level!");
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

    if (m_Config.applyHotfix && m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID) != nullptr) {
        if (!EditScript(level, m_Config)) {
            Logger::Get().Warn("Failed to apply hotfixes on script!");
        }

        Logger::Get().Debug("Hotfixes applied on script.");
    }

    // Launch the default scene
    level->LaunchScene(nullptr);

    // ReRegister OnClick Message in case it changed
    m_MsgClick = m_MessageManager->AddMessageType("OnClick");
    m_MsgDoubleClick = m_MessageManager->AddMessageType("OnDblClick");

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
                    Logger::Get().Error("File Name : %s\nMissing GUIDS:\n", resolvedFile);
                Logger::Get().Error("%x,%x\n", (*it).m_Guids[i].d1, (*it).m_Guids[i].d2);
            }
        }
    }
}

bool GamePlayer::InitPlugins(CKPluginManager *pluginManager) {
    if (!pluginManager)
        return false;

    if (!LoadRenderEngines(pluginManager)) {
        Logger::Get().Error("Failed to load render engine!");
        return false;
    }

    if (!LoadManagers(pluginManager)) {
        Logger::Get().Error("Failed to load managers!");
        return false;
    }

    if (!LoadBuildingBlocks(pluginManager)) {
        Logger::Get().Error("Failed to load building blocks!");
        return false;
    }

    if (!LoadPlugins(pluginManager)) {
        Logger::Get().Error("Failed to load plugins!");
        return false;
    }

    // Unload Input Manager Dll
    UnloadPlugins(pluginManager, CKPLUGIN_MANAGER_DLL, INPUTMANAGER_GUID);

    // Unload Sound Manager Dll
    UnloadPlugins(pluginManager, CKPLUGIN_MANAGER_DLL, SOUNDMANAGER_GUID);

    return true;
}

bool GamePlayer::LoadRenderEngines(CKPluginManager *pluginManager) {
    if (!pluginManager)
        return false;

    const char *path = m_Config.GetPath(eRenderEnginePath);
    if (!utils::DirectoryExists(path) || pluginManager->ParsePlugins((CKSTRING) (path)) == 0) {
        Logger::Get().Error("Render engine parse error.");
        return false;
    }

    Logger::Get().Debug("Render engine loaded.");

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

    const char *path = m_Config.GetPath(eManagerPath);
    if (!utils::DirectoryExists(path)) {
        Logger::Get().Error("Managers directory does not exist!");
        return false;
    }

    Logger::Get().Debug("Loading managers from %s", path);

    if (m_Config.loadAllManagers) {
        if (pluginManager->ParsePlugins((CKSTRING) path) == 0) {
            Logger::Get().Error("Managers parse error.");
            return false;
        }
    } else {
        char szPath[MAX_PATH];
        for (int i = 0; i < ARRAY_NUM(VirtoolsManagers); ++i) {
            utils::ConcatPath(szPath, MAX_PATH, path, VirtoolsManagers[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK) {
                Logger::Get().Error("Unable to register manager: %s", VirtoolsManagers[i]);
                return false;
            }
        }
    }

    Logger::Get().Debug("Managers loaded.");

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

    const char *path = m_Config.GetPath(eBuildingBlockPath);
    if (!utils::DirectoryExists(path)) {
        Logger::Get().Error("BuildingBlocks directory does not exist!");
        return false;
    }

    Logger::Get().Debug("Loading building blocks from %s", path);

    if (m_Config.loadAllBuildingBlocks) {
        if (pluginManager->ParsePlugins((CKSTRING) path) == 0) {
            Logger::Get().Error("Behaviors parse error.");
            return false;
        }
    } else {
        char szPath[MAX_PATH];
        int i;

        for (i = 0; i < ARRAY_NUM(VirtoolsBuildingBlocks); ++i) {
            utils::ConcatPath(szPath, MAX_PATH, path, VirtoolsBuildingBlocks[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK) {
                Logger::Get().Error("Unable to register building blocks: %s", VirtoolsBuildingBlocks[i]);
                return false;
            }
        }

        for (i = 0; i < ARRAY_NUM(CustomBuildingBlocks); ++i) {
            utils::ConcatPath(szPath, MAX_PATH, path, CustomBuildingBlocks[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK) {
                Logger::Get().Error("Unable to register building blocks: %s", CustomBuildingBlocks[i]);
                return false;
            }
        }
    }

    Logger::Get().Debug("Building blocks loaded.");

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

    const char *path = m_Config.GetPath(ePluginPath);
    if (!utils::DirectoryExists(path)) {
        Logger::Get().Error("Plugins directory does not exist!");
        return false;
    }

    Logger::Get().Debug("Loading plugins from %s", path);

    if (m_Config.loadAllPlugins) {
        if (pluginManager->ParsePlugins((CKSTRING) path) == 0) {
            Logger::Get().Error("Plugins parse error.");
            return false;
        }
    } else {
        char szPath[MAX_PATH];
        for (int i = 0; i < ARRAY_NUM(VirtoolsPlugins); ++i) {
            utils::ConcatPath(szPath, MAX_PATH, path, VirtoolsPlugins[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK) {
                Logger::Get().Error("Unable to register plugin: %s", VirtoolsPlugins[i]);
                return false;
            }
        }
    }

    Logger::Get().Debug("Plugins loaded.");

    return true;
}

int GamePlayer::InitRenderEngines(CKPluginManager *pluginManager) {
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

void GamePlayer::CreateInputManager() {
    CKInitializeParameterTypes(m_CKContext);
    CKInitializeOperationTypes(m_CKContext);
    CKInitializeOperationFunctions(m_CKContext);

    if (!m_InputManager)
        m_InputManager = new InputManager(m_CKContext);
}

void GamePlayer::RemoveInputManager() {
    if (m_InputManager) {
        delete m_InputManager;
        m_InputManager = nullptr;
    }

    CKUnInitializeParameterTypes(m_CKContext);
    CKUnInitializeOperationTypes(m_CKContext);
}

void GamePlayer::CreateSoundManager() {
    if (!m_SoundManager)
        m_SoundManager = new SoundManager(m_CKContext);
}

void GamePlayer::RemoveSoundManager() {
    if (m_SoundManager) {
        delete m_SoundManager;
        m_SoundManager = nullptr;
    }
}

bool GamePlayer::SetupManagers() {
    m_RenderManager = m_CKContext->GetRenderManager();
    if (!m_RenderManager) {
        Logger::Get().Error("Unable to get Render Manager.");
        return false;
    }

    m_RenderManager->SetRenderOptions("DisablePerspectiveCorrection", m_Config.disablePerspectiveCorrection);
    m_RenderManager->SetRenderOptions("ForceLinearFog", m_Config.forceLinearFog);
    m_RenderManager->SetRenderOptions("ForceSoftware", m_Config.forceSoftware);
    m_RenderManager->SetRenderOptions("DisableFilter", m_Config.disableFilter);
    m_RenderManager->SetRenderOptions("EnsureVertexShader", m_Config.ensureVertexShader);
    m_RenderManager->SetRenderOptions("UseIndexBuffers", m_Config.useIndexBuffers);
    m_RenderManager->SetRenderOptions("DisableDithering", m_Config.disableDithering);
    m_RenderManager->SetRenderOptions("Antialias", m_Config.antialias);
    m_RenderManager->SetRenderOptions("DisableMipmap", m_Config.disableMipmap);
    m_RenderManager->SetRenderOptions("DisableSpecular", m_Config.disableSpecular);
    m_RenderManager->SetRenderOptions("EnableScreenDump", m_Config.enableScreenDump);
    m_RenderManager->SetRenderOptions("EnableDebugMode", m_Config.enableDebugMode);
    m_RenderManager->SetRenderOptions("VertexCache", m_Config.vertexCache);
    m_RenderManager->SetRenderOptions("TextureCacheManagement", m_Config.textureCacheManagement);
    m_RenderManager->SetRenderOptions("SortTransparentObjects", m_Config.sortTransparentObjects);
    if (m_Config.textureVideoFormat != UNKNOWN_PF)
        m_RenderManager->SetRenderOptions("TextureVideoFormat", m_Config.textureVideoFormat);
    if (m_Config.spriteVideoFormat != UNKNOWN_PF)
        m_RenderManager->SetRenderOptions("SpriteVideoFormat", m_Config.spriteVideoFormat);

    m_MessageManager = m_CKContext->GetMessageManager();
    if (!m_MessageManager) {
        Logger::Get().Error("Unable to get Message Manager.");
        return false;
    }

    m_TimeManager = m_CKContext->GetTimeManager();
    if (!m_TimeManager) {
        Logger::Get().Error("Unable to get Time Manager.");
        return false;
    }

    m_AttributeManager = m_CKContext->GetAttributeManager();
    if (!m_AttributeManager) {
        Logger::Get().Error("Unable to get Attribute Manager.");
        return false;
    }

    CreateInputManager();
    CreateSoundManager();

    return true;
}

bool GamePlayer::SetupPaths() {
    CKPathManager *pm = m_CKContext->GetPathManager();
    if (!pm) {
        Logger::Get().Error("Unable to get Path Manager.");
        return false;
    }

    char path[MAX_PATH];
    char dir[MAX_PATH];
    ::GetCurrentDirectoryA(MAX_PATH, dir);

    if (!utils::DirectoryExists(m_Config.GetPath(eDataPath))) {
        Logger::Get().Error("Data path is not found.");
        return false;
    }
    _snprintf(path, MAX_PATH, "%s\\%s", dir, m_Config.GetPath(eDataPath));
    XString dataPath = path;
    pm->AddPath(DATA_PATH_IDX, dataPath);
    Logger::Get().Debug("Data path: %s", dataPath.CStr());

    if (!utils::DirectoryExists(m_Config.GetPath(eSoundPath))) {
        Logger::Get().Error("Sounds path is not found.");
        return false;
    }
    _snprintf(path, MAX_PATH, "%s\\%s", dir, m_Config.GetPath(eSoundPath));
    XString soundPath = path;
    pm->AddPath(SOUND_PATH_IDX, soundPath);
    Logger::Get().Debug("Sounds path: %s", soundPath.CStr());

    if (!utils::DirectoryExists(m_Config.GetPath(eBitmapPath))) {
        Logger::Get().Error("Bitmap path is not found.");
        return false;
    }
    _snprintf(path, MAX_PATH, "%s\\%s", dir, m_Config.GetPath(eBitmapPath));
    XString bitmapPath = path;
    pm->AddPath(BITMAP_PATH_IDX, bitmapPath);
    Logger::Get().Debug("Bitmap path: %s", bitmapPath.CStr());

    return true;
}

void GamePlayer::ResizeWindow() {
    RECT rc = {0, 0, m_Config.width, m_Config.height};
    ::AdjustWindowRect(&rc, m_MainWindow.GetStyle(), FALSE);
    m_MainWindow.SetPos(nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
    if (m_Config.childWindowRendering)
        m_RenderWindow.SetPos(nullptr, 0, 0, m_Config.width, m_Config.height, SWP_NOMOVE | SWP_NOZORDER);
}

int GamePlayer::FindScreenMode(int width, int height, int bpp, int driver) {
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(driver);
    if (!drDesc) {
        Logger::Get().Error("Unable to find render driver %d.", driver);
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
    m_Config.width = PLAYER_DEFAULT_WIDTH;
    m_Config.height = PLAYER_DEFAULT_HEIGHT;
    m_Config.bpp = PLAYER_DEFAULT_BPP;
    m_Config.driver = 0;
}

bool GamePlayer::IsRenderFullscreen() const {
    if (!m_RenderContext)
        return false;
    return m_RenderContext->IsFullScreen() == TRUE;
}

bool GamePlayer::GoFullscreen() {
    if (!m_RenderContext || IsRenderFullscreen())
        return false;

    m_Config.fullscreen = true;
    if (m_RenderContext->GoFullScreen(m_Config.width, m_Config.height, m_Config.bpp, m_Config.driver) != CK_OK) {
        m_Config.fullscreen = false;
        Logger::Get().Debug("GoFullScreen Failed");
        return false;
    }

    return true;
}

bool GamePlayer::StopFullscreen() {
    if (!m_RenderContext || !IsRenderFullscreen())
        return false;

    if (m_RenderContext->StopFullScreen() != CK_OK) {
        Logger::Get().Debug("StopFullscreen Failed");
        return false;
    }

    m_Config.fullscreen = false;
    return true;
}

bool GamePlayer::RegisterMainWindowClass(HINSTANCE hInstance) {
    WNDCLASSEX mainWndClass;
    memset(&mainWndClass, 0, sizeof(WNDCLASSEX));

    mainWndClass.lpfnWndProc = MainWndProc;
    mainWndClass.cbSize = sizeof(WNDCLASSEX);
    mainWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    mainWndClass.cbClsExtra = 0;
    mainWndClass.cbWndExtra = 0;
    mainWndClass.hInstance = hInstance;
    mainWndClass.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLAYER));
    mainWndClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    mainWndClass.hbrBackground = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
    mainWndClass.lpszMenuName = nullptr;
    mainWndClass.lpszClassName = TEXT("Ballance");
    mainWndClass.hIconSm = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLAYER));

    return ::RegisterClassEx(&mainWndClass) != 0;
}

bool GamePlayer::RegisterRenderWindowClass(HINSTANCE hInstance) {
    WNDCLASS renderWndClass;
    memset(&renderWndClass, 0, sizeof(WNDCLASS));

    renderWndClass.lpfnWndProc = MainWndProc;
    renderWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    renderWndClass.hInstance = hInstance;
    renderWndClass.lpszClassName = TEXT("Ballance Render");

    return ::RegisterClass(&renderWndClass) != 0;
}

bool GamePlayer::ClipCursor() {
    if (m_Config.clipCursor) {
        RECT rect;
        m_MainWindow.GetClientRect(&rect);
        m_MainWindow.ClientToScreen(&rect);
        return ::ClipCursor(&rect) == TRUE;
    }
    return ::ClipCursor(nullptr) == TRUE;
}

bool GamePlayer::OpenSetupDialog() {
    return ::DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_FULLSCREEN_SETUP), nullptr, FullscreenSetupDlgProc, 0) == IDOK;
}

bool GamePlayer::OpenAboutDialog() {
    return ::DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_ABOUT), nullptr, AboutDlgProc, 0) == IDOK;
}

void GamePlayer::OnDestroy() {
    ::ClipCursor(nullptr);
    ::PostQuitMessage(0);
}

void GamePlayer::OnMove() {
    if (!m_Config.fullscreen) {
        RECT rect;
        m_MainWindow.GetWindowRect(&rect);
        m_Config.posX = rect.left;
        m_Config.posY = rect.top;
    }
}

void GamePlayer::OnSize() {
    ClipCursor();
}

void GamePlayer::OnPaint() {
    if (m_RenderContext && !m_Config.fullscreen)
        Render();
}

void GamePlayer::OnClose() {
    m_MainWindow.Destroy();
}

void GamePlayer::OnActivateApp(bool active) {
    static bool wasPlaying = false;
    static bool wasFullscreen = false;
    static bool firstDeActivate = true;

    if (m_State == eInitial)
        return;

    if (!active) {
        if (m_CKContext) {
            if (firstDeActivate)
                wasPlaying = m_CKContext->IsPlaying() == TRUE;

            if (m_Config.pauseOnDeactivated)
                Pause();
            else if (!m_Config.alwaysHandleInput)
                m_InputManager->Pause(TRUE);

            ::ClipCursor(nullptr);

            if (m_RenderContext && IsRenderFullscreen()) {
                if (firstDeActivate)
                    wasFullscreen = true;

                OnStopFullscreen();

                Pause();
                if (wasPlaying && !m_Config.pauseOnDeactivated)
                    Play();
            } else if (firstDeActivate) {
                wasFullscreen = false;
            }
        }
        firstDeActivate = false;
        m_State = eFocusLost;
    } else {
        if (wasFullscreen && !firstDeActivate)
            OnGoFullscreen();

        ClipCursor();

        if (!m_Config.alwaysHandleInput)
            m_InputManager->Pause(FALSE);

        if (wasPlaying)
            Play();

        firstDeActivate = true;
        m_State = ePlaying;
    }
}

void GamePlayer::OnSetCursor() {
    if (m_State == ePaused)
        ::SetCursor(::LoadCursor(nullptr, IDC_ARROW));
}

void GamePlayer::OnGetMinMaxInfo(LPMINMAXINFO lpmmi) {
    if (lpmmi) {
        lpmmi->ptMinTrackSize.x = PLAYER_DEFAULT_WIDTH;
        lpmmi->ptMinTrackSize.y = PLAYER_DEFAULT_HEIGHT;
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
            m_MainWindow.PostMsg(TT_MSG_EXIT_TO_SYS, 0, 0);
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
    if (m_Config.childWindowRendering)
        m_RenderWindow.ScreenToClient(&pt);
    else
        m_MainWindow.ScreenToClient(&pt);

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
    switch (id) {
        case IDM_APP_ABOUT:
            Pause();
            OpenAboutDialog();
            Play();
            break;

        default:
            break;
    }
    return 0;
}

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

void GamePlayer::OnExceptionCMO() {
    Logger::Get().Error("Exception in the CMO - Abort");
    m_MainWindow.PostMsg(TT_MSG_EXIT_TO_SYS, 0, 0);
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
    return Load((const char *) filename);
}

void GamePlayer::OnExitToSystem() {
    bool fullscreen = m_Config.fullscreen;
    OnStopFullscreen();
    m_Config.fullscreen = fullscreen;

    m_MainWindow.PostMsg(WM_CLOSE, 0, 0);
}

void GamePlayer::OnExitToTitle() {}

int GamePlayer::OnChangeScreenMode(int driver, int screenMode) {
    int width, height, bpp;
    if (!GetDisplayMode(width, height, bpp, driver, screenMode)) {
        Logger::Get().Error("Failed to change screen mode.");
        return 0;
    }

    bool fullscreen = m_Config.fullscreen;
    if (fullscreen)
        StopFullscreen();

    ClipCursor();

    m_Config.driver = driver;
    m_Config.screenMode = screenMode;
    m_Config.width = width;
    m_Config.height = height;
    m_Config.bpp = bpp;

    InterfaceManager *im = (InterfaceManager *) m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    im->SetDriver(m_Config.driver);
    im->SetScreenMode(m_Config.screenMode);

    ResizeWindow();
    m_RenderContext->Resize();

    if (fullscreen)
        GoFullscreen();

    return 1;
}

void GamePlayer::OnGoFullscreen() {
    Pause();

    ::ClipCursor(nullptr);

    if (GoFullscreen()) {
        m_MainWindow.SetStyle(WS_POPUP);
        m_MainWindow.SetPos(nullptr, 0, 0, m_Config.width, m_Config.height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

        m_MainWindow.Show();
        m_MainWindow.SetFocus();

        if (m_Config.childWindowRendering) {
            m_RenderWindow.Show();
            m_RenderWindow.SetFocus();
        }

        m_MainWindow.Update();
        if (m_Config.childWindowRendering)
            m_RenderWindow.Update();
    }

    Play();
}

void GamePlayer::OnStopFullscreen() {
    Pause();

    if (StopFullscreen()) {
        LONG style = (m_Config.borderless) ? WS_POPUP : WS_POPUP | WS_CAPTION;
        RECT rc = {0, 0, m_Config.width, m_Config.height};
        ::AdjustWindowRect(&rc, style, FALSE);

        m_MainWindow.SetStyle(style);
        m_MainWindow.SetPos(HWND_NOTOPMOST, m_Config.posX, m_Config.posY, rc.right - rc.left, rc.bottom - rc.top, SWP_FRAMECHANGED);

        m_MainWindow.Show();
        m_MainWindow.SetFocus();

        if (m_Config.childWindowRendering) {
            m_RenderWindow.SetPos(nullptr, 0, 0, m_Config.width, m_Config.height, SWP_NOMOVE | SWP_NOZORDER);
            m_RenderWindow.SetFocus();
        }

        m_MainWindow.Update();
        if (m_Config.childWindowRendering)
            m_RenderWindow.Update();
    }

    ClipCursor();

    Play();
}

void GamePlayer::OnSwitchFullscreen() {
    if (m_State == eInitial)
        return;

    if (!IsRenderFullscreen())
        OnGoFullscreen();
    else
        OnStopFullscreen();
}

bool GamePlayer::FillDriverList(HWND hWnd) {
    const int drCount = m_RenderManager->GetRenderDriverCount();
    if (drCount == 0)
        return false;

    for (int i = 0; i < drCount; ++i) {
        VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(i);
#if CKVERSION == 0x13022002
        const char *driverName = drDesc->DriverName;
#else
        const char *driverName = drDesc->DriverName.CStr();
#endif
        int index = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_ADDSTRING, 0, (LPARAM) driverName);
        ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_SETITEMDATA, index, i);
        if (i == m_Config.driver)
            ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_SETCURSEL, index, 0);
    }

    return true;
}

bool GamePlayer::FillScreenModeList(HWND hWnd, int driver) {
    char buffer[256];

    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(driver);
    if (!drDesc)
        return false;

#if CKVERSION == 0x13022002
    VxDisplayMode *dm = drDesc->DisplayModes;
    if (!dm)
        return false;

    const int dmCount = drDesc->DisplayModeCount;
    if (dmCount == 0)
        return false;
#else
    XArray<VxDisplayMode> &dm = drDesc->DisplayModes;
    if (dm.Size() == 0)
        return false;

    const int dmCount = dm.Size();
    if (dmCount == 0)
        return false;
#endif

    int i = 0;
    while (i < dmCount) {
        int width = dm[i].Width;
        int height = dm[i].Height;
        while (i < dmCount && dm[i].Width == width && dm[i].Height == height) {
            if (dm[i].Bpp > 8) {
                sprintf(buffer, "%d x %d x %d x %dHz", dm[i].Width, dm[i].Height, dm[i].Bpp, dm[i].RefreshRate);
                int index = ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_ADDSTRING, 0, (LPARAM) buffer);
                ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_SETITEMDATA, index, i);
                if (i == m_Config.screenMode) {
                    ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_SETCURSEL, index, 0);
                    ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_SETTOPINDEX, index, 0);
                }
            }
            ++i;
        }
    }

    return true;
}

LRESULT GamePlayer::MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    GamePlayer &player = GetInstance();

    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return 1;

    switch (uMsg) {
        case WM_DESTROY:
            player.OnDestroy();
            return 0;

        case WM_MOVE:
            player.OnMove();
            break;

        case WM_SIZE:
            player.OnSize();
            break;

        case WM_PAINT:
            player.OnPaint();
            break;

        case WM_CLOSE:
            player.OnClose();
            return 0;

        case WM_ACTIVATEAPP:
            player.OnActivateApp(wParam == WA_ACTIVE);
            break;

        case WM_SETCURSOR:
            player.OnSetCursor();
            break;

        case WM_GETMINMAXINFO:
            player.OnGetMinMaxInfo((LPMINMAXINFO) lParam);
            break;

        case WM_SYSKEYDOWN:
            return player.OnSysKeyDown(wParam);

        case WM_LBUTTONDOWN:
            player.OnClick();
            break;

        case WM_LBUTTONDBLCLK:
            player.OnClick(true);
            break;

        case WM_COMMAND:
            return player.OnCommand(LOWORD(wParam), HIWORD(wParam));

        case WM_IME_COMPOSITION:
            if (player.OnImeComposition(hWnd, lParam) == 1)
                return 1;
            break;

        case TT_MSG_NO_GAMEINFO:
            player.OnExceptionCMO();
            break;

        case TT_MSG_CMO_RESTART:
            player.OnReturn();
            break;

        case TT_MSG_CMO_LOAD:
            player.OnLoadCMO((const char *) wParam);
            break;

        case TT_MSG_EXIT_TO_SYS:
            player.OnExitToSystem();
            break;

        case TT_MSG_EXIT_TO_TITLE:
            player.OnExitToTitle();
            break;

        case TT_MSG_SCREEN_MODE_CHG:
            return player.OnChangeScreenMode((int) lParam, (int) wParam);

        case TT_MSG_GO_FULLSCREEN:
            player.OnGoFullscreen();
            break;

        case TT_MSG_STOP_FULLSCREEN:
            player.OnStopFullscreen();
            return 1;

        default:
            break;
    }

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL GamePlayer::FullscreenSetupDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WORD wNotifyCode = HIWORD(wParam);
    int wID = LOWORD(wParam);

    GamePlayer &player = GamePlayer::GetInstance();
    GameConfig &config = player.GetConfig();

    switch (uMsg) {
        case WM_INITDIALOG: {
            if (!player.FillDriverList(hWnd))
                ::EndDialog(hWnd, IDCANCEL);
            if (!player.FillScreenModeList(hWnd, config.driver))
                ::EndDialog(hWnd, IDCANCEL);
            return TRUE;
        }

        case WM_COMMAND: {
            if (wNotifyCode == LBN_SELCHANGE && wID == IDC_LB_DRIVER) {
                int index = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETCURSEL, 0, 0);
                int driver = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETITEMDATA, index, 0);

                ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_RESETCONTENT, 0, 0);

                player.FillScreenModeList(hWnd, driver);

                return TRUE;
            } else if (wID == IDOK || wID == IDCANCEL) {
                if (wID == IDOK) {
                    int index = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETCURSEL, 0, 0);
                    if (index == LB_ERR) {
                        ::EndDialog(hWnd, IDCANCEL);
                        return TRUE;
                    }
                    config.driver = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETITEMDATA, index, 0);

                    index = ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_GETCURSEL, 0, 0);
                    if (index == LB_ERR) {
                        ::EndDialog(hWnd, IDCANCEL);
                        return TRUE;
                    }
                    config.screenMode = SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_GETITEMDATA, index, 0);

                    VxDriverDesc *drDesc = player.GetRenderManager()->GetRenderDriverDescription(config.driver);
                    if (drDesc) {
                        const VxDisplayMode &dm = drDesc->DisplayModes[config.screenMode];
                        config.width = dm.Width;
                        config.height = dm.Height;
                        config.bpp = dm.Bpp;
                    }
                }

                ::EndDialog(hWnd, wID);
                return TRUE;
            }
        }
        break;

        default:
            break;
    }
    return FALSE;
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
