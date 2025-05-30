#include "GamePlayer.h"

#include <stdio.h>
#include <string.h>

#include "Logger.h"
#include "Utils.h"
#include "InterfaceManager.h"

#include "resource.h"

#define ARRAY_NUM(Array) \
    (sizeof(Array) / sizeof(Array[0]))

extern bool EditScript(CKLevel *level, const CGameConfig &config);

CGamePlayer::CGamePlayer()
    : m_State(eInitial),
      m_hInstance(NULL),
      m_hAccelTable(NULL),
      m_MainWindow(NULL),
      m_RenderWindow(NULL),
      m_CKContext(NULL),
      m_RenderContext(NULL),
      m_RenderManager(NULL),
      m_MessageManager(NULL),
      m_TimeManager(NULL),
      m_AttributeManager(NULL),
      m_InputManager(NULL),
      m_MsgClick(-1),
      m_MsgDoubleClick(-1),
      m_GameInfo(NULL) {}

CGamePlayer::~CGamePlayer()
{
    Shutdown();
}

bool CGamePlayer::Init(const CGameConfig &config, HINSTANCE hInstance)
{
    if (m_State != eInitial)
        return true;

    m_Config = config;

    if (!hInstance)
        m_hInstance = ::GetModuleHandle(NULL);
    else
        m_hInstance = hInstance;

    if (!InitWindow(m_hInstance))
    {
        CLogger::Get().Error("Failed to initialize window!");
        ShutdownWindow();
        return false;
    }

    if (!InitEngine(m_MainWindow))
    {
        CLogger::Get().Error("Failed to initialize CK Engine!");
        return false;
    }

    if (!InitDriver())
    {
        CLogger::Get().Error("Failed to initialize Render Driver!");
        Shutdown();
        return false;
    }

    RECT rc;
    ::GetClientRect(m_MainWindow, &rc);
    if (rc.right - rc.left != m_Config.width || rc.bottom - rc.top != m_Config.height)
    {
        ResizeWindow();
    }

    HWND handle = (!m_Config.childWindowRendering) ? m_MainWindow : m_RenderWindow;
    CKRECT rect = {0, 0, m_Config.width, m_Config.height};
    m_RenderContext = m_RenderManager->CreateRenderContext(handle, m_Config.driver, &rect, FALSE, m_Config.bpp);
    if (!m_RenderContext)
    {
        CLogger::Get().Error("Failed to create Render Context!");
        return false;
    }

    CLogger::Get().Debug("Render Context created.");

    if (m_Config.fullscreen)
        OnGoFullscreen();

    ::ShowWindow(m_MainWindow, SW_SHOW);
    ::SetFocus(m_MainWindow);

    m_State = eReady;
    return true;
}

bool CGamePlayer::Load(const char *filename)
{
    if (m_State == eInitial)
    {
        CLogger::Get().Error("Player is not initialized!");
        return false;
    }

    if (!filename || (*filename) == '\0')
    {
        filename = m_Config.GetPath(eCmoPath);
        if (!filename || (*filename) == '\0')
        {
            CLogger::Get().Error("No filename specified!");
            return false;
        }
    }

    if (!m_CKContext)
    {
        CLogger::Get().Error("CKContext is not initialized!");
        return false;
    }

    CKPathManager *pm = m_CKContext->GetPathManager();
    if (!pm)
    {
        CLogger::Get().Error("Failed to get CKPathManager!");
        return false;
    }

    XString resolvedFile = filename;
    CKERROR err = pm->ResolveFileName(resolvedFile, DATA_PATH_IDX);
    if (err != CK_OK)
    {
        CLogger::Get().Error("Failed to resolve filename %s", filename);
        return false;
    }

    m_CKContext->Reset();
    m_CKContext->ClearAll();

    // Load the file and fills the array with loaded objects
    CKFile *f = m_CKContext->CreateCKFile();
    if (!f)
    {
        CLogger::Get().Error("Failed to create CKFile!");
        return false;
    }

    CKERROR res = f->OpenFile(resolvedFile.Str(), (CK_LOAD_FLAGS)(CK_LOAD_DEFAULT | CK_LOAD_CHECKDEPENDENCIES));
    if (res != CK_OK)
    {
        // something failed
        if (res == CKERR_PLUGINSMISSING)
        {
            // log the missing guids
            ReportMissingGuids(f, resolvedFile.CStr());
        }
        m_CKContext->DeleteCKFile(f);

        CLogger::Get().Error("Failed to open file: %s", resolvedFile.CStr());
        return false;
    }

    CKObjectArray *array = CreateCKObjectArray();
    if (!array)
    {
        CLogger::Get().Error("Failed to create CKObjectArray!");
        m_CKContext->DeleteCKFile(f);
        return false;
    }

    res = f->LoadFileData(array);
    if (res != CK_OK)
    {
        CLogger::Get().Error("Failed to load file: %s", resolvedFile.CStr());
        m_CKContext->DeleteCKFile(f);
        DeleteCKObjectArray(array);
        return false;
    }

    m_CKContext->DeleteCKFile(f);
    DeleteCKObjectArray(array);

    return FinishLoad(filename);
}

void CGamePlayer::Run()
{
    while (Update())
        continue;
}

bool CGamePlayer::Update()
{
    MSG msg;
    if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        if (!::TranslateAccelerator(m_MainWindow, m_hAccelTable, &msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    else
    {
        float beforeRender = 0.0f;
        float beforeProcess = 0.0f;
        m_TimeManager->GetTimeToWaitForLimits(beforeRender, beforeProcess);
        if (beforeProcess <= 0)
        {
            m_TimeManager->ResetChronos(FALSE, TRUE);
            Process();
        }
        if (beforeRender <= 0)
        {
            m_TimeManager->ResetChronos(TRUE, FALSE);
            Render();
        }
    }

    return true;
}

void CGamePlayer::Process()
{
    m_CKContext->Process();
}

void CGamePlayer::Render()
{
    m_RenderContext->Render();
}

void CGamePlayer::Shutdown()
{
    if (m_GameInfo)
    {
        delete m_GameInfo;
        m_GameInfo = NULL;
    }

    ShutdownEngine();
    ShutdownWindow();

    if (m_State != eInitial)
    {
        m_State = eInitial;
        CLogger::Get().Debug("Player shut down.");
    }

    // Save settings
    m_Config.SaveToIni();
}

void CGamePlayer::Play()
{
    m_State = ePlaying;
    m_CKContext->Play();
    CLogger::Get().Debug("Game is playing.");
}

void CGamePlayer::Pause()
{
    m_State = ePaused;
    m_CKContext->Pause();
    CLogger::Get().Debug("Game is paused.");
}

void CGamePlayer::Reset()
{
    m_State = ePlaying;
    m_CKContext->Reset();
    m_CKContext->Play();
    CLogger::Get().Debug("Game is reset.");
}

static CKERROR LogRedirect(CKUICallbackStruct &cbStruct, void *)
{
    if (cbStruct.Reason == CKUIM_OUTTOCONSOLE ||
        cbStruct.Reason == CKUIM_OUTTOINFOBAR ||
        cbStruct.Reason == CKUIM_DEBUGMESSAGESEND)
    {
        static XString text = "";
        if (text.Compare(cbStruct.ConsoleString))
        {
            CLogger::Get().Info(cbStruct.ConsoleString);
            text = cbStruct.ConsoleString;
        }
    }
    return CK_OK;
}

bool CGamePlayer::InitWindow(HINSTANCE hInstance)
{
    if (!hInstance)
        return false;

    if (!RegisterMainWindowClass(hInstance))
    {
        CLogger::Get().Error("Failed to register main window class!");
        return false;
    }

    CLogger::Get().Debug("Main window class registered.");

    if (m_Config.childWindowRendering)
    {
        if (!RegisterRenderWindowClass(hInstance))
        {
            CLogger::Get().Error("Failed to register render window class!");
            m_Config.childWindowRendering = false;
        }
        else
        {
            CLogger::Get().Debug("Render window class registered.");
        }
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

    m_MainWindow = ::CreateWindowEx(WS_EX_LEFT, TEXT("Ballance"), TEXT("Ballance"), style,
                               x, y, width, height, NULL, NULL, hInstance, this);
    if (!m_MainWindow)
    {
        CLogger::Get().Error("Failed to create main window!");
        UnregisterMainWindowClass(hInstance);
        if (m_Config.childWindowRendering)
            UnregisterRenderWindowClass(hInstance);
        return false;
    }

    CLogger::Get().Debug("Main window created.");

    if (m_Config.childWindowRendering)
    {
        m_RenderWindow = ::CreateWindowEx(WS_EX_TOPMOST, TEXT("Ballance Render"), TEXT("Ballance Render"),
                                          WS_CHILD | WS_VISIBLE, 0, 0, m_Config.width, m_Config.height, m_MainWindow,
                                          NULL, hInstance, this);
        if (!m_RenderWindow)
        {
            CLogger::Get().Error("Failed to create render window!");
            UnregisterRenderWindowClass(hInstance);
            m_Config.childWindowRendering = false;
        }
        else
        {
            CLogger::Get().Debug("Render window created.");
        }
    }

    m_hAccelTable = ::LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCEL));
    if (!m_hAccelTable)
    {
        CLogger::Get().Error("Failed to load the accelerator table!");
        ::DestroyWindow(m_MainWindow);
        if (m_Config.childWindowRendering)
            ::DestroyWindow(m_RenderWindow);
        UnregisterMainWindowClass(hInstance);
        if (m_Config.childWindowRendering)
            UnregisterRenderWindowClass(hInstance);
        return false;
    }

    m_Config.posX = x;
    m_Config.posY = y;

    return true;
}

void CGamePlayer::ShutdownWindow()
{
    if (m_hAccelTable)
    {
        ::DestroyAcceleratorTable(m_hAccelTable);
        m_hAccelTable = NULL;
    }

    if (m_Config.childWindowRendering)
        ::DestroyWindow(m_RenderWindow);
    ::DestroyWindow(m_MainWindow);

    if (m_hInstance)
    {
        if (m_Config.childWindowRendering)
            UnregisterRenderWindowClass(m_hInstance);
        UnregisterMainWindowClass(m_hInstance);
    }
}

bool CGamePlayer::InitEngine(HWND mainWindow)
{
    if (CKStartUp() != CK_OK)
    {
        CLogger::Get().Error("CK Engine can not start up!");
        return false;
    }

    CLogger::Get().Debug("CK Engine starts up successfully.");

    CKPluginManager *pluginManager = CKGetPluginManager();
    if (!InitPlugins(pluginManager))
    {
        CLogger::Get().Error("Failed to initialize plugins.");
        CKShutdown();
        return false;
    }

    int renderEngine = FindRenderEngine(pluginManager);
    if (renderEngine == -1)
    {
        CLogger::Get().Error("Failed to initialize render engine.");
        CKShutdown();
        return false;
    }

    CLogger::Get().Debug("Render Engine initialized.");
#if CKVERSION == 0x13022002
    CKERROR res = CKCreateContext(&m_CKContext, mainWindow, renderEngine, 0);
#else
    CKERROR res = CKCreateContext(&m_CKContext, mainWindow, 0);
#endif
    if (res != CK_OK)
    {
        CLogger::Get().Error("Failed to initialize CK Engine.");
        ShutdownEngine();
        return false;
    }

    CLogger::Get().Debug("CK Engine initialized.");

    m_CKContext->SetVirtoolsVersion(CK_VIRTOOLS_DEV, 0x2000043);
    m_CKContext->SetInterfaceMode(FALSE, LogRedirect, NULL);

    if (!SetupManagers())
    {
        CLogger::Get().Error("Failed to setup managers.");
        ShutdownEngine();
        return false;
    }

    if (!SetupPaths())
    {
        CLogger::Get().Error("Failed to setup paths.");
        ShutdownEngine();
        return false;
    }

    return true;
}

void CGamePlayer::ShutdownEngine()
{
    if (m_CKContext)
    {
        m_CKContext->Reset();
        m_CKContext->ClearAll();

        if (m_RenderManager && m_RenderContext)
        {
            m_RenderManager->DestroyRenderContext(m_RenderContext);
            m_RenderContext = NULL;
        }

        CKCloseContext(m_CKContext);
        m_CKContext = NULL;

        m_RenderManager = NULL;
        m_MessageManager = NULL;
        m_TimeManager = NULL;
        m_AttributeManager = NULL;
        m_InputManager = NULL;

        CKShutdown();
    }
}

bool CGamePlayer::InitDriver()
{
    int driverCount = m_RenderManager->GetRenderDriverCount();
    if (driverCount == 0)
    {
        CLogger::Get().Error("No render driver found.");
        return false;
    }

    if (driverCount == 1)
    {
        CLogger::Get().Debug("Found a render driver");
    }
    else
    {
        CLogger::Get().Debug("Found %d render drivers", driverCount);
    }

    if (m_Config.manualSetup)
    {
        OpenSetupDialog();
        m_Config.manualSetup = false;
    }

    bool tryFailed = false;

    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Config.driver);
    if (!drDesc)
    {
        CLogger::Get().Error("Unable to find driver %d", m_Config.driver);
        m_Config.driver = 0;
        tryFailed = true;
        if (!OpenSetupDialog())
        {
            SetDefaultValuesForDriver();
        }
    }

    if (tryFailed)
    {
        drDesc = m_RenderManager->GetRenderDriverDescription(m_Config.driver);
        if (!drDesc)
        {
            CLogger::Get().Error("Unable to find driver %d", m_Config.driver);
            return false;
        }
    }

    CLogger::Get().Debug("Render Driver ID: %d", m_Config.driver);
    CLogger::Get().Debug("Render Driver Name: %s", drDesc->DriverName);
    CLogger::Get().Debug("Render Driver Desc: %s", drDesc->DriverDesc);

    tryFailed = false;

    m_Config.screenMode = FindScreenMode(m_Config.width, m_Config.height, m_Config.bpp, m_Config.driver);
    if (m_Config.screenMode == -1)
    {
        CLogger::Get().Error("Unable to find screen mode: %d x %d x %d", m_Config.width, m_Config.height, m_Config.bpp);
        tryFailed = true;
        if (!OpenSetupDialog())
        {
            SetDefaultValuesForDriver();
        }
    }

    if (tryFailed)
    {
        m_Config.screenMode = FindScreenMode(m_Config.width, m_Config.height, m_Config.bpp, m_Config.driver);
        if (m_Config.screenMode == -1)
        {
            CLogger::Get().Error("Unable to find screen mode: %d x %d x %d", m_Config.width, m_Config.height, m_Config.bpp);
            return false;
        }
    }

    CLogger::Get().Debug("Screen Mode: %d x %d x %d", m_Config.width, m_Config.height, m_Config.bpp);

    return true;
}

bool CGamePlayer::FinishLoad(const char *filename)
{
    if (!filename)
        return false;

    InterfaceManager *im = (InterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (im)
    {
        im->SetDriver(m_Config.driver);
        im->SetScreenMode(m_Config.screenMode);
        im->SetRookie(m_Config.rookie);
        im->SetTaskSwitchEnabled(true);

        if (m_GameInfo)
        {
            delete m_GameInfo;
            m_GameInfo = NULL;
        }

        m_GameInfo = new CGameInfo;
        strcpy(m_GameInfo->path, ".");
        strcpy(m_GameInfo->fileName, filename);
        im->SetGameInfo(m_GameInfo);
    }

    // Retrieve the level
    CKLevel *level = m_CKContext->GetCurrentLevel();
    if (!level)
    {
        CLogger::Get().Error("Failed to retrieve the level!");
        return false;
    }

    // Add the render context to the level
    level->AddRenderContext(m_RenderContext, TRUE);

    // Take the first camera we found and attach the viewpoint to it.
    // (in case it is not set by the composition later)
    const XObjectPointerArray cams = m_CKContext->GetObjectListByType(CKCID_CAMERA, TRUE);
    if (cams.Size() != 0)
        m_RenderContext->AttachViewpointToCamera((CKCamera *)cams[0]);

    // Hide curves ?
    int curveCount = m_CKContext->GetObjectsCountByClassID(CKCID_CURVE);
    CK_ID *curve_ids = m_CKContext->GetObjectsListByClassID(CKCID_CURVE);
    for (int i = 0; i < curveCount; ++i)
    {
        CKMesh *mesh = ((CKCurve *)m_CKContext->GetObject(curve_ids[i]))->GetCurrentMesh();
        if (mesh)
            mesh->Show(CKHIDE);
    }

    if (m_Config.applyHotfix && m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID) != NULL)
    {
        if (!EditScript(level, m_Config))
        {
            CLogger::Get().Warn("Failed to apply hotfixes on script!");
        }

        CLogger::Get().Debug("Hotfixes applied on script.");
    }

    // Launch the default scene
    level->LaunchScene(NULL);

    // ReRegister OnClick Message in case it changed
    m_MsgClick = m_MessageManager->AddMessageType((CKSTRING)"OnClick");
    m_MsgDoubleClick = m_MessageManager->AddMessageType((CKSTRING)"OnDblClick");

    // Render the first frame
    m_RenderContext->Render();

    return true;
}

void CGamePlayer::ReportMissingGuids(CKFile *file, const char *resolvedFile)
{
    // retrieve the list of missing plugins/guids
    const XClassArray<CKFilePluginDependencies> *p = file->GetMissingPlugins();
    for (CKFilePluginDependencies *it = p->Begin(); it != p->End(); it++)
    {
        const int count = it->m_Guids.Size();
        for (int i = 0; i < count; i++)
        {
            if (!it->ValidGuids[i])
            {
                if (resolvedFile)
                    CLogger::Get().Error("File Name : %s\nMissing GUIDS:\n", resolvedFile);
                CLogger::Get().Error("%x,%x\n", it->m_Guids[i].d1, it->m_Guids[i].d2);
            }
        }
    }
}

bool CGamePlayer::InitPlugins(CKPluginManager *pluginManager)
{
    if (!pluginManager)
        return false;

    if (!LoadRenderEngines(pluginManager))
    {
        CLogger::Get().Error("Failed to load render engine!");
        return false;
    }

    if (!LoadManagers(pluginManager))
    {
        CLogger::Get().Error("Failed to load managers!");
        return false;
    }

    if (!LoadBuildingBlocks(pluginManager))
    {
        CLogger::Get().Error("Failed to load building blocks!");
        return false;
    }

    if (!LoadPlugins(pluginManager))
    {
        CLogger::Get().Error("Failed to load plugins!");
        return false;
    }

    return true;
}

bool CGamePlayer::LoadRenderEngines(CKPluginManager *pluginManager)
{
    if (!pluginManager)
        return false;

    const char *path = m_Config.GetPath(eRenderEnginePath);
    if (!utils::DirectoryExists(path) || pluginManager->ParsePlugins((CKSTRING)(path)) == 0)
    {
        CLogger::Get().Error("Render engine parse error.");
        return false;
    }

    CLogger::Get().Debug("Render engine loaded.");

    return true;
}

bool CGamePlayer::LoadManagers(CKPluginManager *pluginManager)
{
    if (!pluginManager)
        return false;

    const char *path = m_Config.GetPath(eManagerPath);
    if (!utils::DirectoryExists(path))
    {
        CLogger::Get().Error("Managers directory does not exist!");
        return false;
    }

    CLogger::Get().Debug("Loading managers from %s", path);

    if (pluginManager->ParsePlugins((CKSTRING)path) == 0)
    {
        CLogger::Get().Error("Managers parse error.");
        return false;
    }

    CLogger::Get().Debug("Managers loaded.");

    return true;
}

bool CGamePlayer::LoadBuildingBlocks(CKPluginManager *pluginManager)
{
    if (!pluginManager)
        return false;

    const char *path = m_Config.GetPath(eBuildingBlockPath);
    if (!utils::DirectoryExists(path))
    {
        CLogger::Get().Error("BuildingBlocks directory does not exist!");
        return false;
    }

    CLogger::Get().Debug("Loading building blocks from %s", path);

    if (pluginManager->ParsePlugins((CKSTRING)path) == 0)
    {
        CLogger::Get().Error("Behaviors parse error.");
        return false;
    }

    CLogger::Get().Debug("Building blocks loaded.");

    return true;
}

bool CGamePlayer::LoadPlugins(CKPluginManager *pluginManager)
{
    if (!pluginManager)
        return false;

    const char *path = m_Config.GetPath(ePluginPath);
    if (!utils::DirectoryExists(path))
    {
        CLogger::Get().Error("Plugins directory does not exist!");
        return false;
    }

    CLogger::Get().Debug("Loading plugins from %s", path);

    if (pluginManager->ParsePlugins((CKSTRING)path) == 0)
    {
        CLogger::Get().Error("Plugins parse error.");
        return false;
    }

    CLogger::Get().Debug("Plugins loaded.");

    return true;
}

bool CGamePlayer::UnloadPlugins(CKPluginManager *pluginManager, CK_PLUGIN_TYPE type, CKGUID guid)
{
    if (!pluginManager)
        return false;

    const int count = pluginManager->GetPluginCount(type);
    for (int i = 0; i < count; ++i)
    {
        CKPluginEntry *entry = pluginManager->GetPluginInfo(type, i);
        CKPluginInfo &info = entry->m_PluginInfo;
        if (info.m_GUID == guid)
        {
            if (pluginManager->UnLoadPluginDll(entry->m_PluginDllIndex) == CK_OK)
                return true;
            break;
        }
    }

    return false;
}

int CGamePlayer::FindRenderEngine(CKPluginManager *pluginManager)
{
    if (!pluginManager)
        return -1;

    int count = pluginManager->GetPluginCount(CKPLUGIN_RENDERENGINE_DLL);
    for (int i = 0; i < count; ++i)
    {
        CKPluginEntry *entry = pluginManager->GetPluginInfo(CKPLUGIN_RENDERENGINE_DLL, i);
        if (!entry)
            break;

        CKPluginDll *dll = pluginManager->GetPluginDllInfo(entry->m_PluginDllIndex);
        if (!dll)
            break;

        char filename[MAX_PATH];
        _splitpath(dll->m_DllFileName.Str(), NULL, NULL, filename, NULL);
        if (!_strnicmp("CK2_3D", filename, strlen(filename)))
            return i;
    }

    return -1;
}

bool CGamePlayer::SetupManagers()
{
    m_RenderManager = m_CKContext->GetRenderManager();
    if (!m_RenderManager)
    {
        CLogger::Get().Error("Unable to get Render Manager.");
        return false;
    }

    m_RenderManager->SetRenderOptions((CKSTRING)"DisablePerspectiveCorrection", m_Config.disablePerspectiveCorrection);
    m_RenderManager->SetRenderOptions((CKSTRING)"ForceLinearFog", m_Config.forceLinearFog);
    m_RenderManager->SetRenderOptions((CKSTRING)"ForceSoftware", m_Config.forceSoftware);
    m_RenderManager->SetRenderOptions((CKSTRING)"DisableFilter", m_Config.disableFilter);
    m_RenderManager->SetRenderOptions((CKSTRING)"EnsureVertexShader", m_Config.ensureVertexShader);
    m_RenderManager->SetRenderOptions((CKSTRING)"UseIndexBuffers", m_Config.useIndexBuffers);
    m_RenderManager->SetRenderOptions((CKSTRING)"DisableDithering", m_Config.disableDithering);
    m_RenderManager->SetRenderOptions((CKSTRING)"Antialias", m_Config.antialias);
    m_RenderManager->SetRenderOptions((CKSTRING)"DisableMipmap", m_Config.disableMipmap);
    m_RenderManager->SetRenderOptions((CKSTRING)"DisableSpecular", m_Config.disableSpecular);
    m_RenderManager->SetRenderOptions((CKSTRING)"EnableScreenDump", m_Config.enableScreenDump);
    m_RenderManager->SetRenderOptions((CKSTRING)"EnableDebugMode", m_Config.enableDebugMode);
    m_RenderManager->SetRenderOptions((CKSTRING)"VertexCache", m_Config.vertexCache);
    m_RenderManager->SetRenderOptions((CKSTRING)"TextureCacheManagement", m_Config.textureCacheManagement);
    m_RenderManager->SetRenderOptions((CKSTRING)"SortTransparentObjects", m_Config.sortTransparentObjects);
    if (m_Config.textureVideoFormat != UNKNOWN_PF)
        m_RenderManager->SetRenderOptions((CKSTRING)"TextureVideoFormat", m_Config.textureVideoFormat);
    if (m_Config.spriteVideoFormat != UNKNOWN_PF)
        m_RenderManager->SetRenderOptions((CKSTRING)"SpriteVideoFormat", m_Config.spriteVideoFormat);

    m_MessageManager = m_CKContext->GetMessageManager();
    if (!m_MessageManager)
    {
        CLogger::Get().Error("Unable to get Message Manager.");
        return false;
    }

    m_TimeManager = m_CKContext->GetTimeManager();
    if (!m_TimeManager)
    {
        CLogger::Get().Error("Unable to get Time Manager.");
        return false;
    }

    m_AttributeManager = m_CKContext->GetAttributeManager();
    if (!m_AttributeManager)
    {
        CLogger::Get().Error("Unable to get Attribute Manager.");
        return false;
    }

    m_InputManager = (CKInputManager *)m_CKContext->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!m_InputManager)
    {
        CLogger::Get().Error("Unable to get Input Manager.");
        return false;
    }

    return true;
}

bool CGamePlayer::SetupPaths()
{
    CKPathManager *pm = m_CKContext->GetPathManager();
    if (!pm)
    {
        CLogger::Get().Error("Unable to get Path Manager.");
        return false;
    }

    char path[MAX_PATH];
    char dir[MAX_PATH];
    ::GetCurrentDirectoryA(MAX_PATH, dir);

    if (!utils::DirectoryExists(m_Config.GetPath(eDataPath)))
    {
        CLogger::Get().Error("Data path is not found.");
        return false;
    }
    _snprintf(path, MAX_PATH, "%s\\%s", dir, m_Config.GetPath(eDataPath));
    XString dataPath = path;
    pm->AddPath(DATA_PATH_IDX, dataPath);
    CLogger::Get().Debug("Data path: %s", dataPath.CStr());

    if (!utils::DirectoryExists(m_Config.GetPath(eSoundPath)))
    {
        CLogger::Get().Error("Sounds path is not found.");
        return false;
    }
    _snprintf(path, MAX_PATH, "%s\\%s", dir, m_Config.GetPath(eSoundPath));
    XString soundPath = path;
    pm->AddPath(SOUND_PATH_IDX, soundPath);
    CLogger::Get().Debug("Sounds path: %s", soundPath.CStr());

    if (!utils::DirectoryExists(m_Config.GetPath(eBitmapPath)))
    {
        CLogger::Get().Error("Bitmap path is not found.");
        return false;
    }
    _snprintf(path, MAX_PATH, "%s\\%s", dir, m_Config.GetPath(eBitmapPath));
    XString bitmapPath = path;
    pm->AddPath(BITMAP_PATH_IDX, bitmapPath);
    CLogger::Get().Debug("Bitmap path: %s", bitmapPath.CStr());

    return true;
}

void CGamePlayer::ResizeWindow()
{
    RECT rc = {0, 0, m_Config.width, m_Config.height};
    DWORD style = ::GetWindowLong(m_MainWindow, GWL_STYLE);
    ::AdjustWindowRect(&rc, style, FALSE);
    ::SetWindowPos(m_MainWindow, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
    if (m_Config.childWindowRendering)
        ::SetWindowPos(m_RenderWindow, NULL, 0, 0, m_Config.width, m_Config.height, SWP_NOMOVE | SWP_NOZORDER);
}

int CGamePlayer::FindScreenMode(int width, int height, int bpp, int driver)
{
    if (!m_RenderManager)
    {
        CLogger::Get().Error("RenderManager is not initialized");
        return -1;
    }

    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(driver);
    if (!drDesc)
    {
        CLogger::Get().Error("Unable to find render driver %d.", driver);
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
    for (int i = 0; i < dmCount; ++i)
    {
        if (dm[i].Width == width &&
            dm[i].Height == height &&
            dm[i].Bpp == bpp)
        {
            if (dm[i].RefreshRate > refreshRate)
                refreshRate = dm[i].RefreshRate;
        }
    }

    if (refreshRate == 0)
    {
        CLogger::Get().Error("No matching refresh rate found for %d x %d x %d", width, height, bpp);
        return -1;
    }

    int screenMode = -1;
    for (int j = 0; j < dmCount; ++j)
    {
        if (dm[j].Width == width &&
            dm[j].Height == height &&
            dm[j].Bpp == bpp &&
            dm[j].RefreshRate == refreshRate)
        {
            screenMode = j;
            break;
        }
    }

    return screenMode;
}

bool CGamePlayer::GetDisplayMode(int &width, int &height, int &bpp, int driver, int screenMode)
{
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

void CGamePlayer::SetDefaultValuesForDriver()
{
    m_Config.driver = 0;
    m_Config.width = PLAYER_DEFAULT_WIDTH;
    m_Config.height = PLAYER_DEFAULT_HEIGHT;
    m_Config.bpp = PLAYER_DEFAULT_BPP;
}

bool CGamePlayer::IsRenderFullscreen() const
{
    if (!m_RenderContext)
        return false;
    return m_RenderContext->IsFullScreen() == TRUE;
}

bool CGamePlayer::GoFullscreen()
{
    if (!m_RenderContext)
        return false;

    if (IsRenderFullscreen())
        return true;

    m_Config.fullscreen = true;
    if (m_RenderContext->GoFullScreen(m_Config.width, m_Config.height, m_Config.bpp, m_Config.driver) != CK_OK)
    {
        CLogger::Get().Debug("GoFullScreen Failed");
        m_Config.fullscreen = false;
        return false;
    }

    return true;
}

bool CGamePlayer::StopFullscreen()
{
    if (!m_RenderContext || !IsRenderFullscreen())
        return false;

    if (m_RenderContext->StopFullScreen() != CK_OK)
    {
        CLogger::Get().Debug("StopFullscreen Failed");
        return false;
    }

    m_Config.fullscreen = false;
    return true;
}

bool CGamePlayer::ClipCursor()
{
    BOOL result;
    if (m_Config.clipCursor)
    {
        RECT rect;
        if (!::GetClientRect(m_MainWindow, &rect))
        {
            CLogger::Get().Error("Failed to get client rect for cursor clipping");
            return false;
        }

        POINT p1, p2;
        p1.x = rect.left;
        p1.y = rect.top;
        p2.x = rect.right;
        p2.y = rect.bottom;
        ::ClientToScreen(m_MainWindow, &p1);
        ::ClientToScreen(m_MainWindow, &p2);
        rect.left = p1.x;
        rect.top = p1.y;
        rect.right = p2.x;
        rect.bottom = p2.y;
        result = ::ClipCursor(&rect);
    }
    else
    {
        result = ::ClipCursor(NULL);
    }

    if (!result)
    {
        DWORD error = GetLastError();
        CLogger::Get().Error("ClipCursor failed with error code: %d", error);
        return false;
    }

    return true;
}

bool CGamePlayer::OpenSetupDialog()
{
    return ::DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_FULLSCREEN_SETUP), NULL, CGamePlayer::FullscreenSetupDlgProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}

bool CGamePlayer::OpenAboutDialog()
{
    return ::DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_ABOUT), NULL, CGamePlayer::AboutDlgProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}

void CGamePlayer::OnDestroy()
{
    ::ClipCursor(NULL);
    ::PostQuitMessage(0);
}

void CGamePlayer::OnMove()
{
    if (!m_Config.fullscreen)
    {
        RECT rect;
        ::GetWindowRect(m_MainWindow, &rect);
        m_Config.posX = rect.left;
        m_Config.posY = rect.top;
    }
}

void CGamePlayer::OnSize()
{
    ClipCursor();
}

void CGamePlayer::OnPaint()
{
    if (m_RenderContext && !m_Config.fullscreen)
        Render();
}

void CGamePlayer::OnClose()
{
    ::DestroyWindow(m_MainWindow);
}

void CGamePlayer::OnActivateApp(bool active)
{
    static bool wasFullscreen = false;
    static bool firstDeActivate = true;

    if (m_State == eInitial)
        return;

    if (!active)
    {
        if (m_CKContext)
        {
            if (!m_Config.alwaysHandleInput)
                m_InputManager->Pause(TRUE);

            ::ClipCursor(NULL);

            if (IsRenderFullscreen())
            {
                if (firstDeActivate)
                    wasFullscreen = true;
                OnStopFullscreen();
            }
            else if (firstDeActivate)
            {
                wasFullscreen = false;
            }
        }
        firstDeActivate = false;
        m_State = eFocusLost;
    }
    else
    {
        if (wasFullscreen && !firstDeActivate)
            OnGoFullscreen();

        ClipCursor();

        if (!m_Config.alwaysHandleInput)
            m_InputManager->Pause(FALSE);

        firstDeActivate = true;
        m_State = ePlaying;
    }
}

void CGamePlayer::OnSetCursor()
{
    if (m_State == ePaused)
        ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

void CGamePlayer::OnGetMinMaxInfo(LPMINMAXINFO lpmmi)
{
    if (lpmmi)
    {
        lpmmi->ptMinTrackSize.x = PLAYER_DEFAULT_WIDTH;
        lpmmi->ptMinTrackSize.y = PLAYER_DEFAULT_HEIGHT;
    }
}

int CGamePlayer::OnSysKeyDown(UINT uKey)
{
    // Manage system key (ALT + KEY)
    switch (uKey)
    {
    case VK_RETURN:
        // ALT + ENTER -> SwitchFullscreen
        OnSwitchFullscreen();
        break;

    case VK_F4:
        // ALT + F4 -> Quit the application
        ::PostMessage(m_MainWindow, TT_MSG_EXIT_TO_SYS, 0, 0);
        return 1;

    default:
        break;
    }
    return 0;
}

void CGamePlayer::OnClick(bool dblClk)
{
    if (!m_RenderManager)
        return;

    POINT pt;
    ::GetCursorPos(&pt);
    if (m_Config.childWindowRendering)
        ::ScreenToClient(m_RenderWindow, &pt);
    else
        ::ScreenToClient(m_MainWindow, &pt);

    CKMessageType msgType = (!dblClk) ? m_MsgClick : m_MsgDoubleClick;

#if CKVERSION == 0x13022002
    CKPOINT ckpt = {(int)pt.x, (int)pt.y};
    CKPICKRESULT res;
    CKObject *obj = m_RenderContext->Pick(ckpt, &res, FALSE);
    if (obj && CKIsChildClassOf(obj, CKCID_BEOBJECT))
        m_MessageManager->SendMessageSingle(msgType, (CKBeObject *)obj, NULL);
    if (res.Sprite)
    {
        CKObject *sprite = m_CKContext->GetObject(res.Sprite);
        if (sprite)
            m_MessageManager->SendMessageSingle(msgType, (CKBeObject *)sprite, NULL);
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

int CGamePlayer::OnCommand(UINT id, UINT code)
{
    if (id == IDM_APP_ABOUT)
    {
        OpenAboutDialog();
    }
    return 0;
}

void CGamePlayer::OnExceptionCMO()
{
    CLogger::Get().Error("Exception in the CMO - Abort");
    ::PostMessage(m_MainWindow, TT_MSG_EXIT_TO_SYS, 0, 0);
}

void CGamePlayer::OnReturn()
{
    XString filename = m_GameInfo->fileName;
    if (!Load(filename.CStr()))
    {
        OnClose();
        return;
    }

    Play();
}

bool CGamePlayer::OnLoadCMO(const char *filename)
{
    return Load(filename);
}

void CGamePlayer::OnExitToSystem()
{
    bool fullscreen = m_Config.fullscreen;
    OnStopFullscreen();
    m_Config.fullscreen = fullscreen;
    ::PostMessage(m_MainWindow, WM_CLOSE, 0, 0);
}

void CGamePlayer::OnExitToTitle()
{
}

int CGamePlayer::OnChangeScreenMode(int driver, int screenMode)
{
    int width, height, bpp;
    if (!GetDisplayMode(width, height, bpp, driver, screenMode))
    {
        CLogger::Get().Error("Failed to change screen mode.");
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

    InterfaceManager *im = (InterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (im)
    {
        im->SetDriver(m_Config.driver);
        im->SetScreenMode(m_Config.screenMode);
    }

    ResizeWindow();
    m_RenderContext->Resize();

    if (fullscreen)
        GoFullscreen();

    return 1;
}

void CGamePlayer::OnGoFullscreen()
{
    Pause();

    ::ClipCursor(NULL);

    if (GoFullscreen())
    {
        ::SetWindowLong(m_MainWindow, GWL_STYLE, WS_POPUP);
        ::SetWindowPos(m_MainWindow, NULL, 0, 0, m_Config.width, m_Config.height,
                       SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

        ::ShowWindow(m_MainWindow, SW_SHOW);
        ::SetFocus(m_MainWindow);

        if (m_Config.childWindowRendering)
        {
            ::ShowWindow(m_RenderWindow, SW_SHOW);
            ::SetFocus(m_RenderWindow);
        }

        ::UpdateWindow(m_MainWindow);
        if (m_Config.childWindowRendering)
            ::UpdateWindow(m_RenderWindow);
    }

    Play();
}

void CGamePlayer::OnStopFullscreen()
{
    Pause();

    if (StopFullscreen())
    {
        LONG style = (m_Config.borderless) ? WS_POPUP : WS_POPUP | WS_CAPTION;
        RECT rc = {0, 0, m_Config.width, m_Config.height};
        ::AdjustWindowRect(&rc, style, FALSE);

        ::SetWindowLong(m_MainWindow, GWL_STYLE, style);
        ::SetWindowPos(m_MainWindow, HWND_NOTOPMOST, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
                       rc.bottom - rc.top, SWP_FRAMECHANGED);

        ::ShowWindow(m_MainWindow, SW_SHOW);
        ::SetFocus(m_MainWindow);

        if (m_Config.childWindowRendering)
        {
            ::SetWindowPos(m_RenderWindow, NULL, 0, 0, m_Config.width, m_Config.height, SWP_NOMOVE | SWP_NOZORDER);
            ::SetFocus(m_RenderWindow);
        }

        ::UpdateWindow(m_MainWindow);
        if (m_Config.childWindowRendering)
            ::UpdateWindow(m_RenderWindow);
    }

    ClipCursor();

    Play();
}

void CGamePlayer::OnSwitchFullscreen()
{
    if (m_State == eInitial)
        return;

    if (!IsRenderFullscreen())
        OnGoFullscreen();
    else
        OnStopFullscreen();
}

LRESULT CGamePlayer::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        OnDestroy();
        return 0;

    case WM_MOVE:
        OnMove();
        break;

    case WM_SIZE:
        OnSize();
        break;

    case WM_PAINT:
        OnPaint();
        break;

    case WM_CLOSE:
        OnClose();
        return 0;

    case WM_ACTIVATEAPP:
        OnActivateApp(wParam == WA_ACTIVE);
        break;

    case WM_SETCURSOR:
        OnSetCursor();
        break;

    case WM_GETMINMAXINFO:
        OnGetMinMaxInfo((LPMINMAXINFO)lParam);
        break;

    case WM_SYSKEYDOWN:
        return OnSysKeyDown(wParam);

    case WM_LBUTTONDOWN:
        OnClick();
        break;

    case WM_LBUTTONDBLCLK:
        OnClick(true);
        break;

    case WM_COMMAND:
        return OnCommand(LOWORD(wParam), HIWORD(wParam));

    case TT_MSG_NO_GAMEINFO:
        OnExceptionCMO();
        break;

    case TT_MSG_CMO_RESTART:
        OnReturn();
        break;

    case TT_MSG_CMO_LOAD:
        OnLoadCMO((const char *)wParam);
        break;

    case TT_MSG_EXIT_TO_SYS:
        OnExitToSystem();
        break;

    case TT_MSG_EXIT_TO_TITLE:
        OnExitToTitle();
        break;

    case TT_MSG_SCREEN_MODE_CHG:
        return OnChangeScreenMode((int)lParam, (int)wParam);

    case TT_MSG_GO_FULLSCREEN:
        OnGoFullscreen();
        break;

    case TT_MSG_STOP_FULLSCREEN:
        OnStopFullscreen();
        return 1;

    default:
        break;
    }

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool CGamePlayer::FillDriverList(HWND hWnd)
{
    const int drCount = m_RenderManager->GetRenderDriverCount();
    if (drCount == 0)
        return false;

    for (int i = 0; i < drCount; ++i)
    {
        VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(i);
#if CKVERSION == 0x13022002
        const char *driverName = drDesc->DriverName;
#else
        const char *driverName = drDesc->DriverName.CStr();
#endif
        int index = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_ADDSTRING, 0, (LPARAM)driverName);
        ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_SETITEMDATA, index, i);
        if (i == m_Config.driver)
            ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_SETCURSEL, index, 0);
    }

    return true;
}

bool CGamePlayer::FillScreenModeList(HWND hWnd, int driver)
{
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
    while (i < dmCount)
    {
        int width = dm[i].Width;
        int height = dm[i].Height;
        while (i < dmCount && dm[i].Width == width && dm[i].Height == height)
        {
            if (dm[i].Bpp > 8)
            {
                sprintf(buffer, "%d x %d x %d x %dHz", dm[i].Width, dm[i].Height, dm[i].Bpp, dm[i].RefreshRate);
                int index = ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_ADDSTRING, 0, (LPARAM)buffer);
                ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_SETITEMDATA, index, i);
                if (i == m_Config.screenMode)
                {
                    ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_SETCURSEL, index, 0);
                    ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_SETTOPINDEX, index, 0);
                }
            }
            ++i;
        }
    }

    return true;
}

bool CGamePlayer::RegisterMainWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX mainWndClass;
    memset(&mainWndClass, 0, sizeof(WNDCLASSEX));

    mainWndClass.lpfnWndProc = CGamePlayer::MainWndProc;
    mainWndClass.cbSize = sizeof(WNDCLASSEX);
    mainWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    mainWndClass.cbClsExtra = 0;
    mainWndClass.cbWndExtra = 0;
    mainWndClass.hInstance = hInstance;
    mainWndClass.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLAYER));
    mainWndClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    mainWndClass.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    mainWndClass.lpszMenuName = NULL;
    mainWndClass.lpszClassName = TEXT("Ballance");
    mainWndClass.hIconSm = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLAYER));

    return ::RegisterClassEx(&mainWndClass) != 0;
}

bool CGamePlayer::RegisterRenderWindowClass(HINSTANCE hInstance)
{
    WNDCLASS renderWndClass;
    memset(&renderWndClass, 0, sizeof(WNDCLASS));

    renderWndClass.lpfnWndProc = CGamePlayer::MainWndProc;
    renderWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    renderWndClass.hInstance = hInstance;
    renderWndClass.lpszClassName = TEXT("Ballance Render");

    return ::RegisterClass(&renderWndClass) != 0;
}

bool CGamePlayer::UnregisterMainWindowClass(HINSTANCE hInstance)
{
    if (!hInstance)
        return false;
    ::UnregisterClass(TEXT("Ballance"), hInstance);
    return true;
}

bool CGamePlayer::UnregisterRenderWindowClass(HINSTANCE hInstance)
{
    if (!hInstance)
        return false;
    ::UnregisterClass(TEXT("Ballance Render"), hInstance);
    return true;
}

LRESULT CGamePlayer::MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CGamePlayer *player = NULL;
    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
        player = static_cast<CGamePlayer *>(pCreate->lpCreateParams);
        ::SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(player));
    }
    else
    {
        player = reinterpret_cast<CGamePlayer *>(::GetWindowLong(hWnd, GWL_USERDATA));
    }

    if (player) {
        return player->HandleMessage(hWnd, uMsg, wParam, lParam);
    }

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL CGamePlayer::FullscreenSetupDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD wNotifyCode = HIWORD(wParam);
    int wID = LOWORD(wParam);

    CGamePlayer *player = NULL;
    if (uMsg == WM_INITDIALOG)
    {
        player = reinterpret_cast<CGamePlayer *>(lParam);
        ::SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(player));
    }
    else
    {
        player = reinterpret_cast<CGamePlayer *>(::GetWindowLong(hWnd, GWL_USERDATA));
    }

    if (!player)
        return FALSE;

    CGameConfig &config = player->GetConfig();

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        if (!player->FillDriverList(hWnd))
            ::EndDialog(hWnd, IDCANCEL);
        if (!player->FillScreenModeList(hWnd, config.driver))
            ::EndDialog(hWnd, IDCANCEL);
        return TRUE;
    }

    case WM_COMMAND:
    {
        if (wNotifyCode == LBN_SELCHANGE && wID == IDC_LB_DRIVER)
        {
            int index = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETCURSEL, 0, 0);
            int driver = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETITEMDATA, index, 0);

            ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_RESETCONTENT, 0, 0);

            player->FillScreenModeList(hWnd, driver);

            return TRUE;
        }
        else if (wID == IDOK || wID == IDCANCEL)
        {
            if (wID == IDOK)
            {
                int index = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETCURSEL, 0, 0);
                if (index == LB_ERR)
                {
                    ::EndDialog(hWnd, IDCANCEL);
                    return TRUE;
                }
                config.driver = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETITEMDATA, index, 0);

                index = ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_GETCURSEL, 0, 0);
                if (index == LB_ERR)
                {
                    ::EndDialog(hWnd, IDCANCEL);
                    return TRUE;
                }
                config.screenMode = SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_GETITEMDATA, index, 0);

                VxDriverDesc *drDesc = player->GetRenderManager()->GetRenderDriverDescription(config.driver);
                if (drDesc)
                {
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

BOOL CGamePlayer::AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            ::EndDialog(hWnd, LOWORD(wParam));
            return TRUE;
        }
        break;

    default:
        break;
    }

    return FALSE;
}