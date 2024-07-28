#include "GamePlayer.h"

#include <stdio.h>
#include <string.h>

#include "Logger.h"
#include "Utils.h"
#include "Splash.h"
#include "InterfaceManager.h"

#include "resource.h"

#define ARRAY_NUM(Array) \
    (sizeof(Array) / sizeof(Array[0]))

extern bool EditScript(CKLevel *level, const CGameConfig &config);

CGamePlayer &CGamePlayer::GetInstance()
{
    static CGamePlayer player;
    return player;
}

CGamePlayer::~CGamePlayer()
{
    Exit();
}

bool CGamePlayer::Init(HINSTANCE hInstance, const CGameConfig &config)
{
    if (m_State != eInitial)
        return true;

    if (!hInstance)
        return false;

    m_hInstance = hInstance;
    m_Config = config;

    if (!InitWindow(hInstance))
    {
        CLogger::Get().Error("Failed to initialize window!");
        return false;
    }

    {
        CSplash splash(hInstance);
        splash.Show();
    }

    if (!InitEngine(m_MainWindow))
    {
        CLogger::Get().Error("Failed to initialize CK Engine!");
        return false;
    }

    if (!InitDriver())
    {
        CLogger::Get().Error("Failed to initialize Render Driver!");
        return false;
    }

    RECT rc;
    m_MainWindow.GetClientRect(&rc);
    if (rc.right - rc.left != m_Config.width || rc.bottom - rc.top != m_Config.height)
    {
        ResizeWindow();
    }

    HWND handle = (!m_Config.childWindowRendering) ? m_MainWindow.GetHandle() : m_RenderWindow.GetHandle();
    CKRECT rect = {0, 0, m_Config.width, m_Config.height};
    m_RenderContext = m_RenderManager->CreateRenderContext(handle, m_Config.driver, &rect, FALSE, m_Config.bpp);
    if (!m_RenderContext)
    {
        CLogger::Get().Error("Failed to create Render Context!");
        return false;
    }

    if (m_Config.fullscreen)
        OnGoFullscreen();

    m_MainWindow.Show();
    m_MainWindow.SetFocus();

    m_State = eReady;
    return true;
}

bool CGamePlayer::Load(const char *filename)
{
    if (m_State == eInitial)
        return false;

    if (!filename || (*filename) == '\0')
        return false;

    if (!m_CKContext)
        return false;

    CKPathManager *pm = m_CKContext->GetPathManager();
    if (!pm)
        return false;

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
    res = f->LoadFileData(array);
    if (res != CK_OK)
    {
        m_CKContext->DeleteCKFile(f);
        return false;
    }

    m_CKContext->DeleteCKFile(f);
    DeleteCKObjectArray(array);

    InterfaceManager *man = (InterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (man)
    {
        man->SetDriver(m_Config.driver);
        man->SetScreenMode(m_Config.screenMode);
        man->SetRookie(m_Config.rookie);
        man->SetTaskSwitchEnabled(true);

        if (m_GameInfo)
        {
            delete m_GameInfo;
            m_GameInfo = NULL;
        }

        m_GameInfo = new CGameInfo;
        strcpy(m_GameInfo->path, ".");
        strcpy(m_GameInfo->fileName, filename);
        man->SetGameInfo(m_GameInfo);
    }

    return FinishLoad();
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

        if (!::TranslateAccelerator(m_MainWindow.GetHandle(), m_hAccelTable, &msg))
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

void CGamePlayer::Exit()
{
    if (m_GameInfo)
    {
        delete m_GameInfo;
        m_GameInfo = NULL;
    }

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
        CKShutdown();
        m_CKContext = NULL;
    }

    // Save settings
    if (m_Config.HasPath(eConfigPath))
        m_Config.SaveToIni();
}

void CGamePlayer::Play()
{
    m_State = ePlaying;
    m_CKContext->Play();
}

void CGamePlayer::Pause()
{
    m_State = ePaused;
    m_CKContext->Pause();
}

void CGamePlayer::Reset()
{
    m_State = ePlaying;
    m_CKContext->Reset();
    m_CKContext->Play();
}

CGamePlayer::CGamePlayer()
    : m_State(eInitial),
      m_hInstance(NULL),
      m_hAccelTable(NULL),
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

void CGamePlayer::Process()
{
    m_CKContext->Process();
}

void CGamePlayer::Render()
{
    m_RenderContext->Render();
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
    if (!RegisterMainWindowClass(hInstance))
    {
        CLogger::Get().Error("Failed to register main window class!");
        return false;
    }

    if (m_Config.childWindowRendering)
    {
        if (!RegisterRenderWindowClass(hInstance))
        {
            CLogger::Get().Error("Failed to register render window class!");
            return false;
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

    if (!m_MainWindow.CreateEx(WS_EX_LEFT, TEXT("Ballance"), TEXT("Ballance"), style,
                               x, y, width, height, NULL, NULL, hInstance, NULL))
    {
        CLogger::Get().Error("Failed to create main window!");
        return false;
    }

    if (m_Config.childWindowRendering)
    {
        if (!m_RenderWindow.CreateEx(WS_EX_TOPMOST, TEXT("Ballance Render"), TEXT("Ballance Render"), WS_CHILD | WS_VISIBLE,
                                     0, 0, m_Config.width, m_Config.height, m_MainWindow.GetHandle(), NULL, hInstance, NULL))
        {
            CLogger::Get().Error("Failed to create render window!");
            return false;
        }
    }

    m_hAccelTable = ::LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCEL));
    if (!m_hAccelTable)
    {
        CLogger::Get().Error("Failed to load the accelerator table!");
        return false;
    }

    m_Config.posX = x;
    m_Config.posY = y;

    return true;
}

bool CGamePlayer::InitEngine(CWindow &mainWindow)
{
    if (CKStartUp() != CK_OK)
    {
        CLogger::Get().Error("CK Engine can not startup!");
        return false;
    }

    CKPluginManager *pluginManager = CKGetPluginManager();
    if (!InitPlugins(pluginManager))
    {
        CLogger::Get().Error("Unable to initialize plugins.");
        return false;
    }

    int renderEngine = InitRenderEngines(pluginManager);
    if (renderEngine == -1)
    {
        CLogger::Get().Error("Unable to load a RenderEngine.");
        return false;
    }

    CKERROR res = CKCreateContext(&m_CKContext, mainWindow.GetHandle(), renderEngine, 0);
    if (res != CK_OK)
    {
        CLogger::Get().Error("Unable to initialize CK Engine.");
        return false;
    }

    m_CKContext->SetVirtoolsVersion(CK_VIRTOOLS_DEV, 0x2000043);
    m_CKContext->SetInterfaceMode(FALSE, LogRedirect, NULL);

    m_RenderManager = m_CKContext->GetRenderManager();
    if (!m_RenderManager)
    {
        CLogger::Get().Error("Unable to initialize Render Manager.");
        return false;
    }

    m_RenderManager->SetRenderOptions("DisableFilter", m_Config.disableFilter);
    m_RenderManager->SetRenderOptions("DisableDithering", m_Config.disableDithering);
    m_RenderManager->SetRenderOptions("Antialias", m_Config.antialias);
    m_RenderManager->SetRenderOptions("DisableMipmap", m_Config.disableMipmap);
    m_RenderManager->SetRenderOptions("DisableSpecular", m_Config.disableSpecular);

    m_MessageManager = m_CKContext->GetMessageManager();
    if (!m_MessageManager)
    {
        CLogger::Get().Error("Unable to initialize Message Manager.");
        return false;
    }

    m_TimeManager = m_CKContext->GetTimeManager();
    if (!m_TimeManager)
    {
        CLogger::Get().Error("Unable to initialize Time Manager.");
        return false;
    }

    m_AttributeManager = m_CKContext->GetAttributeManager();
    if (!m_AttributeManager)
    {
        CLogger::Get().Error("Unable to initialize Attribute Manager.");
        return false;
    }

    m_InputManager = (CKInputManager *)m_CKContext->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!m_InputManager)
    {
        CLogger::Get().Error("Unable to initialize Input Manager.");
        return false;
    }

    CKPathManager *pm = m_CKContext->GetPathManager();
    if (!pm)
    {
        CLogger::Get().Error("Unable to initialize Path Manager.");
        return false;
    }

    XString path = m_Config.GetPath(eDataPath);
    pm->AddPath(DATA_PATH_IDX, path);

    if (!utils::DirectoryExists(m_Config.GetPath(eSoundPath)))
    {
        CLogger::Get().Error("Sounds directory is not found.");
        return false;
    }
    path = m_Config.GetPath(eSoundPath);
    pm->AddPath(SOUND_PATH_IDX, path);

    if (!utils::DirectoryExists(m_Config.GetPath(eBitmapPath)))
    {
        CLogger::Get().Error("Textures directory is not found.");
        return false;
    }
    path = m_Config.GetPath(eBitmapPath);
    pm->AddPath(BITMAP_PATH_IDX, path);

    return true;
}

bool CGamePlayer::InitDriver()
{
    int driverCount = m_RenderManager->GetRenderDriverCount();
    if (driverCount == 0)
    {
        CLogger::Get().Error("No render driver found.");
        return false;
    }

    if (m_Config.manualSetup)
        OpenSetupDialog();

    m_Config.manualSetup = false;
    bool useDefault = false;

    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Config.driver);
    if (!drDesc)
    {
        CLogger::Get().Error("Unable to find driver %d", m_Config.driver);
        m_Config.driver = 0;
        if (!OpenSetupDialog())
        {
            SetDefaultValuesForDriver();
            useDefault = true;
        }
    }

    m_Config.screenMode = FindScreenMode(m_Config.width, m_Config.height, m_Config.bpp, m_Config.driver);
    if (m_Config.screenMode == -1)
    {
        CLogger::Get().Error("Unable to find screen mode: %d x %d x %d", m_Config.width, m_Config.height, m_Config.bpp);
        if (!useDefault && !OpenSetupDialog())
        {
            SetDefaultValuesForDriver();
            m_Config.screenMode = FindScreenMode(m_Config.width, m_Config.height, m_Config.bpp, m_Config.driver);
            if (m_Config.screenMode == -1)
                return false;
        }
    }

    return true;
}

bool CGamePlayer::FinishLoad()
{
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

    if (m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID) != NULL)
    {
        if (!EditScript(level, m_Config))
        {
            CLogger::Get().Error("Failed to apply hotfixes on script!");
            return false;
        }
    }

    // Launch the default scene
    level->LaunchScene(NULL);

    // ReRegister OnClick Message in case it changed
    m_MsgClick = m_MessageManager->AddMessageType("OnClick");
    m_MsgDoubleClick = m_MessageManager->AddMessageType("OnDblClick");

    // Render the first frame
    m_RenderContext->Render();

    return true;
}

void CGamePlayer::ReportMissingGuids(CKFile *file, const char *resolvedFile)
{
    // retrieve the list of missing plugins/guids
    XClassArray<CKFilePluginDependencies> *p = file->GetMissingPlugins();
    for (CKFilePluginDependencies *it = p->Begin(); it != p->End(); it++)
    {
        int count = (*it).m_Guids.Size();
        for (int i = 0; i < count; i++)
        {
            if (!((*it).ValidGuids[i]))
            {
                if (resolvedFile)
                    CLogger::Get().Error("File Name : %s\nMissing GUIDS:\n", resolvedFile);
                CLogger::Get().Error("%x,%x\n", (*it).m_Guids[i].d1, (*it).m_Guids[i].d2);
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

    return true;
}

bool CGamePlayer::LoadManagers(CKPluginManager *pluginManager)
{
    static const char *const VirtoolsManagers[] = {
        "Dx5InputManager.dll",
        "DX7SoundManager.dll",
        "ParameterOperations.dll",
    };

    if (!pluginManager)
        return false;

    const char *path = m_Config.GetPath(eManagerPath);
    if (!utils::DirectoryExists(path))
    {
        CLogger::Get().Error("Managers directory does not exist!");
        return false;
    }

    if (m_Config.loadAllManagers)
    {
        if (pluginManager->ParsePlugins((CKSTRING)path) == 0)
        {
            CLogger::Get().Error("Managers parse error.");
            return false;
        }
    }
    else
    {
        char szPath[MAX_PATH];
        for (int i = 0; i < ARRAY_NUM(VirtoolsManagers); ++i)
        {
            utils::ConcatPath(szPath, MAX_PATH, path, VirtoolsManagers[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK)
            {
                CLogger::Get().Error("Unable to register manager: %s", VirtoolsManagers[i]);
                return false;
            }
        }
    }

    return true;
}

bool CGamePlayer::LoadBuildingBlocks(CKPluginManager *pluginManager)
{
    static const char *const VirtoolsBuildingBlocks[] = {
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

    static const char *const CustomBuildingBlocks[] = {
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
    if (!utils::DirectoryExists(path))
    {
        CLogger::Get().Error("BuildingBlocks directory does not exist!");
        return false;
    }

    if (m_Config.loadAllBuildingBlocks)
    {
        if (pluginManager->ParsePlugins((CKSTRING)path) == 0)
        {
            CLogger::Get().Error("Behaviors parse error.");
            return false;
        }
    }
    else
    {
        char szPath[MAX_PATH];
        int i;

        for (i = 0; i < ARRAY_NUM(VirtoolsBuildingBlocks); ++i)
        {
            utils::ConcatPath(szPath, MAX_PATH, path, VirtoolsBuildingBlocks[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK)
            {
                CLogger::Get().Error("Unable to register building blocks: %s", VirtoolsBuildingBlocks[i]);
                return false;
            }
        }

        for (i = 0; i < ARRAY_NUM(CustomBuildingBlocks); ++i)
        {
            utils::ConcatPath(szPath, MAX_PATH, path, CustomBuildingBlocks[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK)
            {
                CLogger::Get().Error("Unable to register building blocks: %s", CustomBuildingBlocks[i]);
                return false;
            }
        }
    }

    return true;
}

bool CGamePlayer::LoadPlugins(CKPluginManager *pluginManager)
{
    static const char *const VirtoolsPlugins[] = {
        "AVIReader.dll",
        "ImageReader.dll",
        "VirtoolsLoaderR.dll",
        "WavReader.dll",
    };

    if (!pluginManager)
        return false;

    const char *path = m_Config.GetPath(ePluginPath);
    if (!utils::DirectoryExists(path))
    {
        CLogger::Get().Error("Plugins directory does not exist!");
        return false;
    }

    if (m_Config.loadAllPlugins)
    {
        if (pluginManager->ParsePlugins((CKSTRING)path) == 0)
        {
            CLogger::Get().Error("Plugins parse error.");
            return false;
        }
    }
    else
    {
        char szPath[MAX_PATH];
        for (int i = 0; i < ARRAY_NUM(VirtoolsPlugins); ++i)
        {
            utils::ConcatPath(szPath, MAX_PATH, path, VirtoolsPlugins[i]);
            if (pluginManager->RegisterPlugin(szPath) != CK_OK)
            {
                CLogger::Get().Error("Unable to register plugin: %s", VirtoolsPlugins[i]);
                return false;
            }
        }
    }

    return true;
}

int CGamePlayer::InitRenderEngines(CKPluginManager *pluginManager)
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

void CGamePlayer::ResizeWindow()
{
    RECT rc = {0, 0, m_Config.width, m_Config.height};
    ::AdjustWindowRect(&rc, m_MainWindow.GetStyle(), FALSE);
    m_MainWindow.SetPos(NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
    if (m_Config.childWindowRendering)
        m_RenderWindow.SetPos(NULL, 0, 0, m_Config.width, m_Config.height, SWP_NOMOVE | SWP_NOZORDER);
}

int CGamePlayer::FindScreenMode(int width, int height, int bpp, int driver)
{
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(driver);
    if (!drDesc)
    {
        CLogger::Get().Error("Unable to find specified driver.");
        return false;
    }

    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;

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

    if (screenMode < 0 || screenMode >= drDesc->DisplayModeCount)
        return false;

    const VxDisplayMode &dm = drDesc->DisplayModes[screenMode];
    width = dm.Width;
    height = dm.Height;
    bpp = dm.Bpp;

    return true;
}

void CGamePlayer::SetDefaultValuesForDriver()
{
    m_Config.width = PLAYER_DEFAULT_WIDTH;
    m_Config.height = PLAYER_DEFAULT_HEIGHT;
    m_Config.bpp = PLAYER_DEFAULT_BPP;
    m_Config.driver = 0;
}

bool CGamePlayer::IsRenderFullscreen() const
{
    if (!m_RenderContext)
        return false;
    return m_RenderContext->IsFullScreen() == TRUE;
}

bool CGamePlayer::GoFullscreen()
{
    if (!m_RenderContext || IsRenderFullscreen())
        return false;

    m_Config.fullscreen = true;
    if (m_RenderContext->GoFullScreen(m_Config.width, m_Config.height, m_Config.bpp, m_Config.driver) != CK_OK)
    {
        m_Config.fullscreen = false;
        CLogger::Get().Debug("GoFullScreen Failed");
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

bool CGamePlayer::OpenSetupDialog()
{
    return ::DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_FULLSCREEN_SETUP), NULL, CGamePlayer::FullscreenSetupDlgProc, 0) == IDOK;
}

bool CGamePlayer::OpenAboutDialog()
{
    return ::DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_ABOUT), NULL, CGamePlayer::AboutDlgProc, 0) == IDOK;
}

void CGamePlayer::OnDestroy()
{
    ::PostQuitMessage(0);
}

void CGamePlayer::OnMove()
{
    if (m_State == ePlaying && !m_Config.fullscreen)
    {
        RECT rect;
        m_MainWindow.GetWindowRect(&rect);
        m_Config.posX = rect.left;
        m_Config.posY = rect.top;
    }
}

void CGamePlayer::OnPaint()
{
    if (m_RenderContext && !m_Config.fullscreen)
        Render();
}

void CGamePlayer::OnClose()
{
    m_MainWindow.Destroy();
}

void CGamePlayer::OnActivateApp(bool active)
{
    static bool wasPlaying = false;
    static bool wasFullscreen = false;
    static bool firstDeActivate = true;

    if (m_State == eInitial)
        return;

    if (!active)
    {
        if (m_CKContext)
        {
            if (firstDeActivate)
                wasPlaying = m_CKContext->IsPlaying() == TRUE;

            if (m_Config.pauseOnDeactivated)
                Pause();
            else if (!m_Config.alwaysHandleInput)
                m_InputManager->Pause(TRUE);

            if (m_RenderContext && IsRenderFullscreen())
            {
                if (firstDeActivate)
                    wasFullscreen = true;

                OnStopFullscreen();

                Pause();
                if (wasPlaying && !m_Config.pauseOnDeactivated)
                    Play();
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

        if (!m_Config.alwaysHandleInput)
            m_InputManager->Pause(FALSE);

        if (wasPlaying)
            Play();

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
        m_MainWindow.PostMsg(TT_MSG_EXIT_TO_SYS, 0, 0);
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
        m_RenderWindow.ScreenToClient(&pt);
    else
        m_MainWindow.ScreenToClient(&pt);

    CKMessageType msgType = (!dblClk) ? m_MsgClick : m_MsgDoubleClick;

    CKPOINT ckpt = {pt.x, pt.y};
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
}

int CGamePlayer::OnCommand(UINT id, UINT code)
{
    switch (id)
    {
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

void CGamePlayer::OnExceptionCMO()
{
    CLogger::Get().Error("Exception in the CMO - Abort");
    m_MainWindow.PostMsg(TT_MSG_EXIT_TO_SYS, 0, 0);
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
    return Load((const char *)filename);
}

void CGamePlayer::OnExitToSystem()
{
    bool fullscreen = m_Config.fullscreen;
    OnStopFullscreen();
    m_Config.fullscreen = fullscreen;

    m_MainWindow.PostMsg(WM_CLOSE, 0, 0);
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

    m_Config.driver = driver;
    m_Config.screenMode = screenMode;
    m_Config.width = width;
    m_Config.height = height;
    m_Config.bpp = bpp;

    InterfaceManager *im = (InterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    im->SetDriver(m_Config.driver);
    im->SetScreenMode(m_Config.screenMode);

    ResizeWindow();
    m_RenderContext->Resize();

    if (fullscreen)
        GoFullscreen();

    return 1;
}

void CGamePlayer::OnGoFullscreen()
{
    Pause();

    if (GoFullscreen())
    {
        m_MainWindow.SetStyle(WS_POPUP);
        m_MainWindow.SetPos(NULL, 0, 0, m_Config.width, m_Config.height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

        m_MainWindow.Show();
        m_MainWindow.SetFocus();

        if (m_Config.childWindowRendering)
        {
            m_RenderWindow.Show();
            m_RenderWindow.SetFocus();
        }

        m_MainWindow.Update();
        if (m_Config.childWindowRendering)
            m_RenderWindow.Update();
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

        m_MainWindow.SetStyle(style);
        m_MainWindow.SetPos(HWND_NOTOPMOST, m_Config.posX, m_Config.posY, rc.right - rc.left, rc.bottom - rc.top, SWP_FRAMECHANGED);

        m_MainWindow.Show();
        m_MainWindow.SetFocus();

        if (m_Config.childWindowRendering)
        {
            m_RenderWindow.SetPos(NULL, 0, 0, m_Config.width, m_Config.height, SWP_NOMOVE | SWP_NOZORDER);
            m_RenderWindow.SetFocus();
        }

        m_MainWindow.Update();
        if (m_Config.childWindowRendering)
            m_RenderWindow.Update();
    }

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

bool CGamePlayer::FillDriverList(HWND hWnd)
{
    const int drCount = m_RenderManager->GetRenderDriverCount();
    if (drCount == 0)
        return false;

    for (int i = 0; i < drCount; ++i)
    {
        VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(i);
        int index = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_ADDSTRING, 0, (LPARAM)drDesc->DriverName);
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

    VxDisplayMode *dm = drDesc->DisplayModes;
    if (!dm)
        return false;

    const int dmCount = drDesc->DisplayModeCount;
    if (dmCount == 0)
        return false;

    int i = 0;
    while (i < dmCount)
    {
        int width = dm[i].Width;
        int height = dm[i].Height;
        while (dm[i].Width == width && dm[i].Height == height && i < dmCount)
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

LRESULT CGamePlayer::MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CGamePlayer &player = CGamePlayer::GetInstance();

    switch (uMsg)
    {
    case WM_DESTROY:
        player.OnDestroy();
        return 0;

    case WM_MOVE:
        player.OnMove();
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
        player.OnGetMinMaxInfo((LPMINMAXINFO)lParam);
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

    case TT_MSG_NO_GAMEINFO:
        player.OnExceptionCMO();
        break;

    case TT_MSG_CMO_RESTART:
        player.OnReturn();
        break;

    case TT_MSG_CMO_LOAD:
        player.OnLoadCMO((const char *)wParam);
        break;

    case TT_MSG_EXIT_TO_SYS:
        player.OnExitToSystem();
        break;

    case TT_MSG_EXIT_TO_TITLE:
        player.OnExitToTitle();
        break;

    case TT_MSG_SCREEN_MODE_CHG:
        return player.OnChangeScreenMode((int)lParam, (int)wParam);

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

BOOL CGamePlayer::FullscreenSetupDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD wNotifyCode = HIWORD(wParam);
    int wID = LOWORD(wParam);

    CGamePlayer &player = CGamePlayer::GetInstance();
    CGameConfig &config = player.GetConfig();

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        if (!player.FillDriverList(hWnd))
            ::EndDialog(hWnd, IDCANCEL);
        if (!player.FillScreenModeList(hWnd, config.driver))
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

            player.FillScreenModeList(hWnd, driver);

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

                VxDriverDesc *drDesc = player.GetRenderManager()->GetRenderDriverDescription(config.driver);
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