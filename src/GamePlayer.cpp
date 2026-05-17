#include "GamePlayer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>

#ifdef BALLANCE_STATIC_MODULES
#include "StaticPlugins.h"
#endif
#include "Logger.h"
#include "PlayerSdlShortcuts.h"
#include "Utils.h"
#include "InterfaceManager.h"

#define ARRAY_NUM(Array) \
    (sizeof(Array) / sizeof(Array[0]))

extern bool EditScript(CKLevel *level, const CGameConfig &config, const char *resolvedFile);

static CKSTRING ToCKString(const char *value)
{
    return const_cast<CKSTRING>(value);
}

static int AsciiICompare(const char *lhs, const char *rhs)
{
    if (!lhs || !rhs)
        return lhs == rhs ? 0 : (lhs ? 1 : -1);
    while (*lhs && *rhs)
    {
        int lc = tolower(static_cast<unsigned char>(*lhs));
        int rc = tolower(static_cast<unsigned char>(*rhs));
        if (lc != rc)
            return lc - rc;
        ++lhs;
        ++rhs;
    }
    return tolower(static_cast<unsigned char>(*lhs)) - tolower(static_cast<unsigned char>(*rhs));
}

static bool IsDefaultRenderEngineDll(const char *dllPath)
{
    if (!dllPath || !*dllPath)
        return false;

    const char *name = utils::FindLastPathSeparator(dllPath);
    name = name ? name + 1 : dllPath;

    char filename[MAX_PATH] = {0};
    strncpy(filename, name, sizeof(filename) - 1);
    char *extension = strrchr(filename, '.');
    if (extension)
        *extension = '\0';

    name = filename;
    if (strlen(name) >= 3 &&
        tolower(static_cast<unsigned char>(name[0])) == 'l' &&
        tolower(static_cast<unsigned char>(name[1])) == 'i' &&
        tolower(static_cast<unsigned char>(name[2])) == 'b')
        name += 3;

    return AsciiICompare(name, "CK2_3D") == 0;
}

static int FindHighestRefreshMatch(const VxDisplayMode *modes, int modeCount,
                                   int width, int height, int bpp,
                                   bool exactBpp)
{
    if (!modes)
        return -1;

    int bestIndex = -1;
    int bestRefresh = -1;
    for (int i = 0; i < modeCount; ++i)
    {
        const VxDisplayMode &mode = modes[i];
        if (mode.Width != width || mode.Height != height)
            continue;

        if (exactBpp)
        {
            if (mode.Bpp != bpp)
                continue;
        }
        else if (mode.Bpp <= 8)
        {
            continue;
        }

        if (mode.RefreshRate > bestRefresh)
        {
            bestRefresh = mode.RefreshRate;
            bestIndex = i;
        }
    }
    return bestIndex;
}

static int FindBestDisplayModeIndex(const VxDisplayMode *modes, int modeCount,
                                    int width, int height, int bpp)
{
    int index = FindHighestRefreshMatch(modes, modeCount, width, height, bpp, true);
    if (index >= 0)
        return index;

    if (bpp != 32)
    {
        index = FindHighestRefreshMatch(modes, modeCount, width, height, 32, true);
        if (index >= 0)
            return index;
    }

    return FindHighestRefreshMatch(modes, modeCount, width, height, bpp, false);
}

static bool ResolveRuntimePath(char *buffer, size_t size, const char *path)
{
    if (!buffer || size == 0 || !path || !*path)
        return false;

    if (utils::IsAbsolutePath(path))
    {
        strncpy(buffer, path, size - 1);
        buffer[size - 1] = '\0';
        return true;
    }

    char currentDir[MAX_PATH];
    if (utils::GetCurrentPath(currentDir, sizeof(currentDir)) == 0)
        return false;

    return utils::ConcatPath(buffer, size, currentDir, path) != NULL;
}

static bool GetExecutableDirectory(char *buffer, size_t size)
{
    char modulePath[MAX_PATH];
    size_t length = VxGetModuleFileName(NULL, modulePath, MAX_PATH);
    return length > 0 && length < MAX_PATH && utils::GetFileDirectory(buffer, size, modulePath, true);
}

static bool ParsePluginsFromExecutableDirectory(CKPluginManager *pluginManager)
{
    char path[MAX_PATH];
    if (!pluginManager || !GetExecutableDirectory(path, sizeof(path)))
        return false;

    if (!utils::DirectoryExists(path))
        return false;

    pluginManager->ParsePlugins(ToCKString(path));
    return true;
}

static bool AddPathIfMissing(CKPathManager *pathManager, int category, const char *path)
{
    if (!pathManager || !path || !*path)
        return false;

    XString ckPath = path;
    if (pathManager->GetPathIndex(category, ckPath) >= 0)
        return true;

    return pathManager->AddPath(category, ckPath) >= 0;
}

static void AddDirectoryPathIfExists(CKPathManager *pathManager, int category, const char *basePath, const char *relativePath)
{
    char path[MAX_PATH];
    if (!utils::ConcatPath(path, sizeof(path), basePath, relativePath))
        return;
    if (utils::DirectoryExists(path))
        AddPathIfMissing(pathManager, category, path);
}

static void RegisterCompositionPaths(CKPathManager *pathManager, const char *resolvedFile)
{
    char compositionDir[MAX_PATH];
    if (!utils::GetFileDirectory(compositionDir, sizeof(compositionDir), resolvedFile, true))
        return;

    AddPathIfMissing(pathManager, DATA_PATH_IDX, compositionDir);
    AddDirectoryPathIfExists(pathManager, DATA_PATH_IDX, compositionDir, "3D Entities\\");

    AddPathIfMissing(pathManager, SOUND_PATH_IDX, compositionDir);
    AddDirectoryPathIfExists(pathManager, SOUND_PATH_IDX, compositionDir, "Sounds\\");
    AddDirectoryPathIfExists(pathManager, SOUND_PATH_IDX, compositionDir, "Sounds_low\\");

    AddPathIfMissing(pathManager, BITMAP_PATH_IDX, compositionDir);
    AddDirectoryPathIfExists(pathManager, BITMAP_PATH_IDX, compositionDir, "Textures\\");
}

static bool SetCompositionEnvironment(const char *resolvedFile)
{
    char compositionDir[MAX_PATH];
    if (!utils::GetFileDirectory(compositionDir, sizeof(compositionDir), resolvedFile, true))
        return false;

#if defined(_MSC_VER) && _MSC_VER < 1400
    char envBuf[sizeof("Gravity=") + MAX_PATH] = {0};
    _snprintf(envBuf, sizeof(envBuf) - 1, "Gravity=%s", compositionDir);
    envBuf[sizeof(envBuf) - 1] = '\0';
    if (_putenv(envBuf) != 0)
        return false;
#elif defined(_WIN32)
    if (_putenv_s("Gravity", compositionDir) != 0)
        return false;
#else
    if (setenv("Gravity", compositionDir, 1) != 0)
        return false;
#endif

    return true;
}

CGamePlayer::CGamePlayer()
    : m_State(eInitial),
      m_MainWindow(NULL),
      m_RenderWindow(NULL),
      m_SdlWindow(NULL),
      m_CKContext(NULL),
      m_RenderContext(NULL),
      m_RenderManager(NULL),
      m_MessageManager(NULL),
      m_TimeManager(NULL),
      m_AttributeManager(NULL),
      m_InputManager(NULL),
      m_MsgClick(-1),
      m_MsgDoubleClick(-1),
      m_GameInfo(NULL),
      m_AltEnterShortcutDown(false),
      m_AltF4ShortcutDown(false) {}

CGamePlayer::~CGamePlayer()
{
    Shutdown();
}

bool CGamePlayer::Init(const CGameConfig &config)
{
    return Init(config, config);
}

bool CGamePlayer::Init(const CGameConfig &runtimeConfig, const CGameConfig &persistentConfig)
{
    if (m_State != eInitial)
        return true;

    m_Config = runtimeConfig;
    m_PersistentConfig = persistentConfig;

    if (!InitWindow())
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

    CKRECT rc;
    VxGetClientRect(m_MainWindow, &rc);
    if (rc.right - rc.left != m_Config.width || rc.bottom - rc.top != m_Config.height)
    {
        ResizeWindow();
    }

    WIN_HANDLE handle = (!m_Config.childWindowRendering) ? m_MainWindow : m_RenderWindow;
    CKRECT rect = {0, 0, m_Config.width, m_Config.height};
    m_RenderContext = m_RenderManager->CreateRenderContext(handle, m_Config.driver, &rect, FALSE, m_Config.bpp);
    if (!m_RenderContext)
    {
        CLogger::Get().Error("Failed to create Render Context!");
        return false;
    }

    CLogger::Get().Debug("Render Context created.");

    if (m_Config.fullscreen)
        OnGoFullscreen(false);

    if (m_SdlWindow)
    {
        SDL_ShowWindow(m_SdlWindow);
        SDL_RaiseWindow(m_SdlWindow);
    }

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

    RegisterCompositionPaths(pm, resolvedFile.CStr());
    if (!SetCompositionEnvironment(resolvedFile.CStr()))
        CLogger::Get().Warn("Failed to set composition environment path.");

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
        if (res == CKERR_PLUGINSMISSING)
        {
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

    return FinishLoad(filename, resolvedFile.CStr());
}

void CGamePlayer::Run()
{
    while (Update())
        continue;
}

bool CGamePlayer::Update()
{
    DrainPlayerCommands();

    SDL_PumpEvents();
    SDL_Event event;
    if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_QUIT, SDL_EVENT_QUIT) > 0)
        return false;

    SDL_Event windowEvents[16];
    int windowEventCount = SDL_PeepEvents(windowEvents, 16, SDL_GETEVENT,
                                          SDL_EVENT_WINDOW_FIRST, SDL_EVENT_WINDOW_LAST);
    for (int i = 0; i < windowEventCount; ++i)
    {
        const SDL_Event &windowEvent = windowEvents[i];

        switch (windowEvent.type)
        {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            return false;
        case SDL_EVENT_WINDOW_RESIZED:
            m_Config.width = windowEvent.window.data1;
            m_Config.height = windowEvent.window.data2;
            OnSize();
            break;
        case SDL_EVENT_WINDOW_MOVED:
            m_Config.posX = windowEvent.window.data1;
            m_Config.posY = windowEvent.window.data2;
            SyncPersistentWindowPosition();
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            OnActivateApp(true);
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            OnActivateApp(false);
            break;
        default:
            break;
        }
    }

    player_sdl_shortcuts::KeyboardShortcutState shortcutState = {
        m_AltEnterShortcutDown,
        m_AltF4ShortcutDown,
    };
    switch (player_sdl_shortcuts::UpdateKeyboardShortcuts(SDL_GetKeyboardState(NULL),
                                                          SDL_GetModState(),
                                                          shortcutState))
    {
    case player_sdl_shortcuts::KeyboardShortcutToggleFullscreen:
        OnSwitchFullscreen();
        break;
    case player_sdl_shortcuts::KeyboardShortcutQuit:
        return false;
    case player_sdl_shortcuts::KeyboardShortcutNone:
        break;
    }
    m_AltEnterShortcutDown = shortcutState.altEnterDown;
    m_AltF4ShortcutDown = shortcutState.altF4Down;

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
    if (m_State != eInitial && !m_PersistentConfig.SaveToIni())
        CLogger::Get().Error("Failed to save config: %s", m_PersistentConfig.GetPath(eConfigPath));

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

static CKERROR LogRedirect(CKUICallbackStruct &cbStruct, void *userData)
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

bool CGamePlayer::InitWindow()
{
    SDL_SetMainReady();
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        CLogger::Get().Error("Failed to initialize SDL video backend: %s", SDL_GetError());
        return false;
    }

    m_Config.childWindowRendering = false;

    SDL_WindowFlags flags = SDL_WINDOW_HIDDEN;
    if (m_Config.borderless)
        flags = (SDL_WindowFlags)(flags | SDL_WINDOW_BORDERLESS);
    if (m_Config.fullscreen)
        flags = (SDL_WindowFlags)(flags | SDL_WINDOW_FULLSCREEN);

    m_SdlWindow = SDL_CreateWindow("Ballance", m_Config.width, m_Config.height, flags);
    if (!m_SdlWindow)
    {
        CLogger::Get().Error("Failed to create SDL window: %s", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return false;
    }

    int x = m_Config.posX;
    int y = m_Config.posY;
    if (x == 2147483647 || y == 2147483647)
    {
        SDL_SetWindowPosition(m_SdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_GetWindowPosition(m_SdlWindow, &x, &y);
    }
    else
    {
        SDL_SetWindowPosition(m_SdlWindow, x, y);
    }

    m_MainWindow = (WIN_HANDLE)m_SdlWindow;
    m_RenderWindow = NULL;

    m_Config.posX = x;
    m_Config.posY = y;

    return true;
}

void CGamePlayer::ShutdownWindow()
{
    if (m_Config.fullscreen)
        RestoreDisplayMode();

    if (m_SdlWindow)
    {
        SDL_DestroyWindow(m_SdlWindow);
        m_SdlWindow = NULL;
    }
    m_MainWindow = NULL;
    m_RenderWindow = NULL;

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return;
}

bool CGamePlayer::InitEngine(WIN_HANDLE mainWindow)
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
    m_CKContext->SetInterfaceMode(FALSE, LogRedirect, 0);

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
        if (OpenSetupDialog())
            SyncPersistentDisplayConfig();
        m_Config.manualSetup = false;
        m_PersistentConfig.manualSetup = false;
    }

    bool tryFailed = false;

    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Config.driver);
    if (!drDesc)
    {
        CLogger::Get().Error("Unable to find driver %d", m_Config.driver);
        m_Config.driver = 0;
        tryFailed = true;
        if (OpenSetupDialog())
        {
            SyncPersistentDisplayConfig();
        }
        else
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
        if (OpenSetupDialog())
        {
            SyncPersistentDisplayConfig();
        }
        else
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

bool CGamePlayer::FinishLoad(const char *filename, const char *resolvedFile)
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
        if (!EditScript(level, m_Config, resolvedFile))
        {
            CLogger::Get().Warn("Failed to apply hotfixes on script!");
        }

        CLogger::Get().Debug("Hotfixes applied on script.");
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

#ifdef BALLANCE_STATIC_MODULES
    if (!RegisterStaticPlugins(pluginManager))
    {
        CLogger::Get().Error("Failed to register static plugins.");
        return false;
    }

    CLogger::Get().Debug("Static plugins registered.");
    return true;
#else
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
#endif
}

bool CGamePlayer::LoadRenderEngines(CKPluginManager *pluginManager)
{
    if (!pluginManager)
        return false;

    const char *path = m_Config.GetPath(eRenderEnginePath);
    if ((!utils::DirectoryExists(path) || pluginManager->ParsePlugins(ToCKString(path)) == 0) &&
        !ParsePluginsFromExecutableDirectory(pluginManager))
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
        if (!ParsePluginsFromExecutableDirectory(pluginManager))
        {
            CLogger::Get().Error("Managers directory does not exist!");
            return false;
        }
        CLogger::Get().Debug("Managers loaded.");
        return true;
    }

    CLogger::Get().Debug("Loading managers from %s", path);

    if (pluginManager->ParsePlugins(ToCKString(path)) == 0)
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
        if (!ParsePluginsFromExecutableDirectory(pluginManager))
        {
            CLogger::Get().Error("BuildingBlocks directory does not exist!");
            return false;
        }
        CLogger::Get().Debug("Building blocks loaded.");
        return true;
    }

    CLogger::Get().Debug("Loading building blocks from %s", path);

    if (pluginManager->ParsePlugins(ToCKString(path)) == 0)
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
        if (!ParsePluginsFromExecutableDirectory(pluginManager))
        {
            CLogger::Get().Error("Plugins directory does not exist!");
            return false;
        }
        CLogger::Get().Debug("Plugins loaded.");
        return true;
    }

    CLogger::Get().Debug("Loading plugins from %s", path);

    if (pluginManager->ParsePlugins(ToCKString(path)) == 0)
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

        if (IsDefaultRenderEngineDll(dll->m_DllFileName.Str()))
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

    InterfaceManager *interfaceManager = InterfaceManager::GetManager(m_CKContext);
    if (interfaceManager)
        interfaceManager->SetPlayerCommandHandler(&CGamePlayer::HandlePlayerCommand, this);

    return true;
}

void CGamePlayer::DrainPlayerCommands()
{
    if (!m_CKContext)
        return;

    InterfaceManager *interfaceManager = InterfaceManager::GetManager(m_CKContext);
    if (!interfaceManager)
        return;

    TTPlayerCommand command;
    while (interfaceManager->PollPlayerCommand(command))
        ExecutePlayerCommand(interfaceManager, command);
}

int CGamePlayer::ExecutePlayerCommand(InterfaceManager *manager, const TTPlayerCommand &command)
{
    (void)manager;

    switch (command.type)
    {
    case TT_PLAYER_COMMAND_NO_GAMEINFO:
        OnExceptionCMO();
        return 1;

    case TT_PLAYER_COMMAND_CMO_RESTART:
        OnReturn();
        return 1;

    case TT_PLAYER_COMMAND_CMO_LOAD:
        return OnLoadCMO(command.text) ? 1 : 0;

    case TT_PLAYER_COMMAND_EXIT_TO_SYSTEM:
        OnExitToSystem();
        return 1;

    case TT_PLAYER_COMMAND_EXIT_TO_TITLE:
        OnExitToTitle();
        return 1;

    case TT_PLAYER_COMMAND_LIMIT_FPS:
        if (m_TimeManager && command.param0 > 0)
        {
            m_TimeManager->SetFrameRateLimit((float)command.param0);
            m_TimeManager->ChangeLimitOptions(CK_FRAMERATE_LIMIT);
        }
        return 1;

    case TT_PLAYER_COMMAND_SCREEN_MODE_CHANGE:
        return OnChangeScreenMode(command.param1, command.param0);

    case TT_PLAYER_COMMAND_GO_FULLSCREEN:
        OnGoFullscreen();
        return 1;

    case TT_PLAYER_COMMAND_STOP_FULLSCREEN:
        OnStopFullscreen();
        return 1;

    default:
        CLogger::Get().Warn("Unknown player command: %d", command.type);
        return 0;
    }
}

int CGamePlayer::HandlePlayerCommand(InterfaceManager *manager, const TTPlayerCommand &command, void *userData)
{
    CGamePlayer *player = (CGamePlayer *)userData;
    if (!player)
        return 0;
    return player->ExecutePlayerCommand(manager, command);
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
    if (!utils::DirectoryExists(m_Config.GetPath(eDataPath)))
    {
        CLogger::Get().Error("Data path is not found.");
        return false;
    }
    if (!ResolveRuntimePath(path, MAX_PATH, m_Config.GetPath(eDataPath)))
    {
        CLogger::Get().Error("Failed to resolve data path.");
        return false;
    }
    XString dataPath = path;
    pm->AddPath(DATA_PATH_IDX, dataPath);
    CLogger::Get().Debug("Data path: %s", dataPath.CStr());

    if (!utils::DirectoryExists(m_Config.GetPath(eSoundPath)))
    {
        CLogger::Get().Error("Sounds path is not found.");
        return false;
    }
    if (!ResolveRuntimePath(path, MAX_PATH, m_Config.GetPath(eSoundPath)))
    {
        CLogger::Get().Error("Failed to resolve sounds path.");
        return false;
    }
    XString soundPath = path;
    pm->AddPath(SOUND_PATH_IDX, soundPath);
    CLogger::Get().Debug("Sounds path: %s", soundPath.CStr());

    if (!utils::DirectoryExists(m_Config.GetPath(eBitmapPath)))
    {
        CLogger::Get().Error("Bitmap path is not found.");
        return false;
    }
    if (!ResolveRuntimePath(path, MAX_PATH, m_Config.GetPath(eBitmapPath)))
    {
        CLogger::Get().Error("Failed to resolve bitmap path.");
        return false;
    }
    XString bitmapPath = path;
    pm->AddPath(BITMAP_PATH_IDX, bitmapPath);
    CLogger::Get().Debug("Bitmap path: %s", bitmapPath.CStr());

    return true;
}

void CGamePlayer::ResizeWindow()
{
    if (m_SdlWindow)
        SDL_SetWindowSize(m_SdlWindow, m_Config.width, m_Config.height);
    return;
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
        return -1;
    }

#if CKVERSION == 0x13022002
    const VxDisplayMode *displayModes = drDesc->DisplayModes;
    const int displayModeCount = drDesc->DisplayModeCount;
#else
    XArray<VxDisplayMode> &displayModes = drDesc->DisplayModes;
    const int displayModeCount = displayModes.Size();
#endif

#if CKVERSION == 0x13022002
    int screenMode = FindBestDisplayModeIndex(displayModes, displayModeCount, width, height, bpp);
#else
    int screenMode = FindBestDisplayModeIndex(displayModes.Begin(), displayModeCount, width, height, bpp);
#endif
    if (screenMode < 0)
        CLogger::Get().Error("No matching screen mode found for %d x %d x %d", width, height, bpp);

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
    SyncPersistentDisplayConfig();
}

void CGamePlayer::SyncPersistentDisplayConfig()
{
    m_PersistentConfig.driver = m_Config.driver;
    m_PersistentConfig.width = m_Config.width;
    m_PersistentConfig.height = m_Config.height;
    m_PersistentConfig.bpp = m_Config.bpp;
}

void CGamePlayer::SyncPersistentWindowPosition()
{
    m_PersistentConfig.posX = m_Config.posX;
    m_PersistentConfig.posY = m_Config.posY;
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

    int width = m_Config.width;
    int height = m_Config.height;
    if (m_MainWindow)
    {
        CKRECT rc;
        if (VxGetClientRect(m_MainWindow, &rc) && rc.right > rc.left && rc.bottom > rc.top)
        {
            width = rc.right - rc.left;
            height = rc.bottom - rc.top;
        }
    }

    m_Config.fullscreen = true;
    if (m_RenderContext->GoFullScreen(width, height, m_Config.bpp, m_Config.driver) != CK_OK)
    {
        CLogger::Get().Debug("GoFullScreen Failed");
        m_Config.fullscreen = false;
        return false;
    }

    return true;
}

bool CGamePlayer::StopFullscreen()
{
    if (!m_RenderContext)
        return false;

    if (!IsRenderFullscreen())
    {
        m_Config.fullscreen = false;
        return true;
    }

    if (m_RenderContext->StopFullScreen() != CK_OK)
    {
        CLogger::Get().Debug("StopFullscreen Failed");
        return false;
    }

    m_Config.fullscreen = false;
    return true;
}

void CGamePlayer::SetFullscreenDisplayMode()
{
    if (m_SdlWindow)
    {
        if (!SDL_SetWindowFullscreenMode(m_SdlWindow, NULL))
            CLogger::Get().Warn("Failed to set borderless fullscreen display mode: %s", SDL_GetError());
        if (!SDL_SetWindowFullscreen(m_SdlWindow, true))
            CLogger::Get().Warn("Failed to enter fullscreen: %s", SDL_GetError());
        if (!SDL_SyncWindow(m_SdlWindow))
            CLogger::Get().Warn("Failed to sync fullscreen window: %s", SDL_GetError());
    }
    return;
}

void CGamePlayer::RestoreDisplayMode()
{
    if (m_SdlWindow)
    {
        if (!SDL_SetWindowFullscreen(m_SdlWindow, false))
            CLogger::Get().Warn("Failed to leave fullscreen: %s", SDL_GetError());
        if (!SDL_SyncWindow(m_SdlWindow))
            CLogger::Get().Warn("Failed to sync windowed mode: %s", SDL_GetError());
    }
}

void CGamePlayer::SetWindowedWindowStyle()
{
    if (m_SdlWindow)
    {
        SDL_SetWindowFullscreen(m_SdlWindow, false);
        SDL_SetWindowBordered(m_SdlWindow, !m_Config.borderless);
        SDL_SetWindowSize(m_SdlWindow, m_Config.width, m_Config.height);
        SDL_SetWindowPosition(m_SdlWindow, m_Config.posX, m_Config.posY);
        SDL_SyncWindow(m_SdlWindow);
    }
    return;
}

bool CGamePlayer::ClipCursor()
{
    if (m_SdlWindow)
        SDL_SetWindowMouseGrab(m_SdlWindow, m_Config.clipCursor ? true : false);
    return true;
}

bool CGamePlayer::ReleaseCursorClip()
{
    if (m_SdlWindow)
        SDL_SetWindowMouseGrab(m_SdlWindow, false);
    return true;
}

bool CGamePlayer::OpenSetupDialog()
{
    return false;
}

void CGamePlayer::OnDestroy()
{
}

void CGamePlayer::OnMove()
{
    if (!m_Config.fullscreen)
    {
        if (m_SdlWindow)
            SDL_GetWindowPosition(m_SdlWindow, &m_Config.posX, &m_Config.posY);
        if (m_State != eInitial)
            SyncPersistentWindowPosition();
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
    SDL_Event quitEvent;
    SDL_zero(quitEvent);
    quitEvent.type = SDL_EVENT_QUIT;
    SDL_PushEvent(&quitEvent);
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

            ReleaseCursorClip();

            if (IsRenderFullscreen())
            {
                if (firstDeActivate)
                    wasFullscreen = true;
                OnStopFullscreen(false);
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
            OnGoFullscreen(false);

        ClipCursor();

        if (!m_Config.alwaysHandleInput)
            m_InputManager->Pause(FALSE);

        firstDeActivate = true;
        m_State = ePlaying;
    }
}


void CGamePlayer::OnClick(bool dblClk)
{
    if (!m_RenderManager)
        return;

    float mouseX = 0.0f;
    float mouseY = 0.0f;
    SDL_GetMouseState(&mouseX, &mouseY);
    int pickX = static_cast<int>(mouseX);
    int pickY = static_cast<int>(mouseY);

    CKMessageType msgType = (!dblClk) ? m_MsgClick : m_MsgDoubleClick;

#if CKVERSION == 0x13022002
    CKPOINT ckpt = {pickX, pickY};
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
    CKObject *obj = m_RenderContext->Pick(pickX, pickY, &desc);
    if (obj && CKIsChildClassOf(obj, CKCID_BEOBJECT))
        m_MessageManager->SendMessageSingle(msgType, (CKBeObject *)obj);
    if (desc.Sprite)
        m_MessageManager->SendMessageSingle(msgType, (CKBeObject *)desc.Sprite);
#endif
}


void CGamePlayer::OnExceptionCMO()
{
    CLogger::Get().Error("Exception in the CMO - Abort");
    OnExitToSystem();
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
    OnStopFullscreen(false);
    m_Config.fullscreen = fullscreen;
    OnClose();
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
        OnStopFullscreen(false);

    ClipCursor();

    m_Config.driver = driver;
    m_Config.screenMode = screenMode;
    m_Config.width = width;
    m_Config.height = height;
    m_Config.bpp = bpp;
    SyncPersistentDisplayConfig();

    InterfaceManager *im = (InterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (im)
    {
        im->SetDriver(m_Config.driver);
        im->SetScreenMode(m_Config.screenMode);
    }

    ResizeWindow();
    m_RenderContext->Resize();

    if (fullscreen)
        OnGoFullscreen(false);

    return 1;
}

void CGamePlayer::OnGoFullscreen(bool persistChange)
{
    Pause();

    ReleaseCursorClip();

    m_Config.fullscreen = true;
    SetFullscreenDisplayMode();

    if (GoFullscreen())
    {
        if (persistChange)
            m_PersistentConfig.fullscreen = true;

        if (m_SdlWindow)
        {
            SDL_ShowWindow(m_SdlWindow);
            SDL_RaiseWindow(m_SdlWindow);
        }
    }
    else
    {
        RestoreDisplayMode();
        SetWindowedWindowStyle();
    }

    Play();
}

void CGamePlayer::OnStopFullscreen(bool persistChange)
{
    Pause();

    bool fullscreen = m_Config.fullscreen || IsRenderFullscreen();
    if (StopFullscreen())
    {
        if (persistChange)
            m_PersistentConfig.fullscreen = false;

        if (fullscreen)
        {
            RestoreDisplayMode();
            SetWindowedWindowStyle();
        }

        if (m_SdlWindow)
        {
            SDL_ShowWindow(m_SdlWindow);
            SDL_RaiseWindow(m_SdlWindow);
        }
    }

    ClipCursor();

    Play();
}

void CGamePlayer::OnSwitchFullscreen()
{
    if (m_State == eInitial)
        return;

    if (!IsRenderFullscreen())
        OnGoFullscreen(true);
    else
        OnStopFullscreen(true);
}
