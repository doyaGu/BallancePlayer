#include "GamePlayer.h"

#include <stdio.h>
#include <string.h>

#include "GameConfig.h"
#include "Splash.h"
#include "Logger.h"
#include "Utils.h"
#include "InterfaceManager.h"
#include "resource.h"

#define ARRAY_NUM(Array) \
    (sizeof(Array) / sizeof(Array[0]))

CGamePlayer *CGamePlayer::s_Instance = NULL;

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

static bool ClipMouse(bool enable)
{
    CNeMoContext *context = CNeMoContext::GetInstance();
    CKRenderContext *rc = context->GetRenderContext();
    if (!rc)
        return false;

    if (!enable)
        return ::ClipCursor(NULL) == TRUE; // Disable the clipping

    // Retrieve the render window rectangle
    VxRect r;
    rc->GetWindowRect(r, TRUE);

    RECT rect;
    rect.top = (LONG)r.top;
    rect.left = (LONG)r.left;
    rect.bottom = (LONG)r.bottom;
    rect.right = (LONG)r.right;

    // To clip the mouse in it.
    return ::ClipCursor(&rect) == TRUE;
}

static BOOL FillScreenModeList(HWND hWnd)
{
    char buffer[256];

    CNeMoContext *context = CNeMoContext::GetInstance();
    CKRenderManager *rm = context->GetRenderManager();
    if (!rm)
        return FALSE;

    VxDriverDesc *drDesc = rm->GetRenderDriverDescription(context->GetDriver());
    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;
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
                if (i == context->GetScreenMode())
                {
                    ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_SETCURSEL, index, 0);
                    ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_SETTOPINDEX, index, 0);
                }
            }
            ++i;
        }
    }

    return TRUE;
}

static BOOL OnInitDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!FillScreenModeList(hWnd))
        EndDialog(hWnd, IDCANCEL);

    CNeMoContext *context = CNeMoContext::GetInstance();
    CKRenderManager *rm = context->GetRenderManager();
    if (!rm)
        return FALSE;

    const int drCount = rm->GetRenderDriverCount();
    for (int i = 0; i < drCount; ++i)
    {
        VxDriverDesc *drDesc = rm->GetRenderDriverDescription(i);
        int index = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_ADDSTRING, 0, (LPARAM)drDesc->DriverName);
        ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_SETITEMDATA, index, i);
        if (i == context->GetDriver())
            ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_SETCURSEL, index, 0);
    }

    return TRUE;
}

static BOOL CALLBACK FullscreenSetupProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD wNotifyCode = HIWORD(wParam);
    int wID = LOWORD(wParam);

    switch (uMsg)
    {
    case WM_INITDIALOG:
        return OnInitDialog(hWnd, uMsg, wParam, lParam);
    case WM_COMMAND:
    {
        CNeMoContext *context = CNeMoContext::GetInstance();
        if (wNotifyCode == LBN_SELCHANGE && wID == IDC_LB_DRIVER)
        {
            int index = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETCURSEL, 0, 0);
            int driver = ::SendDlgItemMessage(hWnd, IDC_LB_DRIVER, LB_GETITEMDATA, index, 0);
            context->SetDriver(driver);

            ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_RESETCONTENT, 0, 0);
            if (!FillScreenModeList(hWnd))
                ::EndDialog(hWnd, FALSE);
            return TRUE;
        }
        else if (wID == IDOK || wID == IDCANCEL)
        {
            int index = ::SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_GETCURSEL, 0, 0);
            int screenMode = SendDlgItemMessage(hWnd, IDC_LB_SCREEN_MODE, LB_GETITEMDATA, index, 0);
            context->SetScreenMode(screenMode);

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

static BOOL CALLBACK AboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CGamePlayer *player = CGamePlayer::GetInstance();
    assert(player != NULL);

    switch (uMsg)
    {
    case WM_DESTROY:
        player->OnDestroy();
        break;

    case WM_MOVE:
        player->OnMove();
        break;

    case WM_SIZE:
        player->OnSized();
        break;

    case WM_PAINT:
        player->OnPaint();
        break;

    case WM_CLOSE:
        player->OnClose();
        return 0;

    case WM_ACTIVATEAPP:
        player->OnActivateApp(hWnd, uMsg, wParam, lParam);
        break;

    case WM_SETCURSOR:
        player->OnSetCursor();
        break;

    case WM_GETMINMAXINFO:
        player->OnGetMinMaxInfo((LPMINMAXINFO)lParam);
        break;

    case WM_SYSKEYDOWN:
        return player->OnSysKeyDown(wParam);

    case WM_COMMAND:
        return player->OnCommand(LOWORD(wParam), HIWORD(wParam));

    case TT_MSG_NO_GAMEINFO:
        player->OnExceptionCMO(wParam, lParam);
        break;

    case TT_MSG_CMO_RESTART:
        player->OnReturn(wParam, lParam);
        break;

    case TT_MSG_CMO_LOAD:
        player->OnLoadCMO(wParam, lParam);
        break;

    case TT_MSG_EXIT_TO_SYS:
        player->OnExitToSystem(wParam, lParam);
        break;

    case TT_MSG_EXIT_TO_TITLE:
        player->OnExitToTitle(wParam, lParam);
        break;

    case TT_MSG_SCREEN_MODE_CHG:
        return player->OnChangeScreenMode(wParam, lParam);

    case TT_MSG_GO_FULLSCREEN:
        player->OnGoFullscreen();
        break;

    case TT_MSG_STOP_FULLSCREEN:
        player->OnStopFullscreen();
        return 1;

    default:
        break;
    }

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

CGamePlayer::CGamePlayer()
    : m_State(eInitial),
      m_NeMoContext(NULL),
      m_WinContext(NULL),
      m_Game(NULL),
      m_hMutex(NULL) {}

CGamePlayer::~CGamePlayer()
{
    Exit();
}

bool CGamePlayer::Init(HINSTANCE hInstance, HANDLE hMutex)
{
    if (m_State != eInitial)
        return true;

    Register(this);
    m_hMutex = hMutex;

    m_NeMoContext = new CNeMoContext;
    m_WinContext = new CWinContext;
    m_Game = new CGame;

    CGameConfig &config = CGameConfig::Get();

    m_WinContext->SetMainSize(config.width, config.height);
    {
        int styleFlags = 0;
        if (config.fullscreen)
            styleFlags |= WINDOW_STYLE_FULLSCREEN;
        if (config.borderless)
            styleFlags |= WINDOW_STYLE_BORDERLESS;
        if (config.resizable)
            styleFlags |= WINDOW_STYLE_RESIZABLE;
        m_WinContext->SetMainStyle(styleFlags, WINDOW_STYLE_USECURRENTSETTINGS);
    }

    // Keep the last position before creating the windows.
    int x = config.posX;
    int y = config.posY;

    if (!m_WinContext->Init(hInstance, WndProc))
    {
        CLogger::Get().Error("WinContext Initialization Failed!");
        return false;
    }

    m_WinContext->SetMainSize(config.width, config.height);
    m_WinContext->SetRenderSize(config.width, config.height);
    if (!config.fullscreen && x != 2147483647 && y != 2147483647)
        m_WinContext->SetPosition(x, y);

    switch (InitEngine())
    {
    case CK_OK:
        RedirectLog();
        break;
    case CKERR_NODLLFOUND:
        ::MessageBox(NULL, TEXT("Some required plugin dlls are not found!"), TEXT("Error"), MB_OK);
        return false;
    case CKERR_NORENDERENGINE:
        ::MessageBox(NULL, TEXT("No Render Engine Found!"), TEXT("Error"), MB_OK);
        return false;
    case CKERR_INVALIDPARAMETER:
        if (!ReInitEngine())
        {
            CLogger::Get().Error("Failed to reinitialize engine!");
            return false;
        }
        break;
    default:
        return false;
    }

    InterfaceManager *im = m_NeMoContext->GetInterfaceManager();
    im->SetDriver(m_NeMoContext->GetDriver());
    im->SetScreenMode(m_NeMoContext->GetScreenMode());
    im->SetRookie(config.rookie);
    im->SetTaskSwitchEnabled(true);

    m_State = eReady;
    return true;
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

        if (!m_WinContext->TranslateAccelerators(&msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    else if (!m_WinContext->IsIconic())
    {
        m_NeMoContext->Update();
    }

    return true;
}

void CGamePlayer::Exit()
{
    if (m_State == eInitial)
        return;

    delete m_Game;
    m_Game = NULL;

    delete m_WinContext;
    m_WinContext = NULL;

    m_NeMoContext->Shutdown();
    delete m_NeMoContext;
    m_NeMoContext = NULL;

    if (m_hMutex)
        ::CloseHandle(m_hMutex);
    m_hMutex = NULL;

    m_State = eInitial;
}

bool CGamePlayer::Load(const char *filename)
{
    if (m_State == eInitial)
        return false;

    CGameConfig &config = CGameConfig::Get();

    m_NeMoContext->AddDataPath(config.GetPath(eDataPath));

    if (!utils::DirectoryExists(config.GetPath(eSoundPath)))
    {
        CLogger::Get().Error("No Sounds directory");
        return false;
    }
    m_NeMoContext->AddSoundPath(config.GetPath(eSoundPath));

    if (!utils::DirectoryExists(config.GetPath(eBitmapPath)))
    {
        CLogger::Get().Error("No Textures directory");
        return false;
    }
    m_NeMoContext->AddBitmapPath(config.GetPath(eBitmapPath));

    CGameInfo *gameInfo = m_Game->NewGameInfo();
    strcpy(gameInfo->path, ".");
    strcpy(gameInfo->fileName, filename);

    InterfaceManager *im = m_NeMoContext->GetInterfaceManager();
    im->SetGameInfo(m_Game->GetGameInfo());

    Pause();

    if (!m_Game->Load())
        return false;

    Play();

    return true;
}

void CGamePlayer::Play()
{
    m_State = ePlaying;
    if (!m_NeMoContext->IsDebugging())
        m_NeMoContext->Play();
    else
        m_NeMoContext->Debug();
}

void CGamePlayer::Pause()
{
    m_State = ePaused;
    m_NeMoContext->Pause();
}

void CGamePlayer::Reset()
{
    m_State = ePlaying;
    m_NeMoContext->Reset();
    m_NeMoContext->Play();
}

void CGamePlayer::OnDestroy()
{
    ::PostQuitMessage(0);
}

void CGamePlayer::OnMove()
{
    CGameConfig &config = CGameConfig::Get();
    if (!config.fullscreen)
        m_WinContext->GetPosition(config.posX, config.posY);
}

void CGamePlayer::OnSized()
{
    CKRenderContext *rc = m_NeMoContext->GetRenderContext();
    if (rc)
    {
        int w, h;
        m_WinContext->GetMainSize(w, h);
        m_WinContext->SetRenderSize(w, h);
        rc->Resize();
    }
}

void CGamePlayer::OnPaint()
{
    if (m_NeMoContext->GetRenderContext() && !m_NeMoContext->IsRenderFullscreen())
        m_NeMoContext->Render();
}

void CGamePlayer::OnClose()
{
    m_NeMoContext->Cleanup();
    m_NeMoContext->RefreshScreen();
    m_NeMoContext->Shutdown();

    m_WinContext->DestroyWindows();
}

void CGamePlayer::OnActivateApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static bool wasPlaying = false;
    static bool wasFullscreen = false;
    static bool firstDeActivate = true;

    if (m_State == eInitial)
        return;

    CGameConfig &config = CGameConfig::Get();

    if (wParam != WA_ACTIVE)
    {
        if (m_NeMoContext->GetCKContext())
        {
            if (firstDeActivate)
                wasPlaying = m_NeMoContext->IsPlaying();

            if (config.pauseOnDeactivated)
                Pause();
            else if (!config.alwaysHandleInput)
                m_NeMoContext->GetInputManager()->Pause(TRUE);

            if (config.clipMouse)
                ClipMouse(false);

            if (m_NeMoContext->GetRenderContext() && m_NeMoContext->IsRenderFullscreen())
            {
                if (firstDeActivate)
                    wasFullscreen = true;

                Pause();
                OnStopFullscreen();
                if (wasPlaying && !config.pauseOnDeactivated)
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

        if (config.clipMouse)
            ClipMouse(true);

        if (!config.alwaysHandleInput)
            m_NeMoContext->GetInputManager()->Pause(FALSE);

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
        lpmmi->ptMinTrackSize.x = 400;
        lpmmi->ptMinTrackSize.y = 200;
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
        m_WinContext->PostMessageToMainWindow(TT_MSG_EXIT_TO_SYS, 0, 0);
        return 1;

    case VK_F6:
        // ALT + F6 -> Start debugging
        m_NeMoContext->Pause();
        m_NeMoContext->Debug();
        m_WinContext->SetWindowText(TEXT("Ballance [Debug]"));
        break;

    case VK_F9:
        // ALT + F9 -> Stop debugging
        m_NeMoContext->Pause();
        m_NeMoContext->Play();
        m_WinContext->SetWindowText();
        break;

    default:
        break;
    }
    return 0;
}

int CGamePlayer::OnCommand(UINT id, UINT code)
{
    switch (id)
    {
    case IDM_APP_ABOUT:
        Pause();
        ::DialogBoxParam(m_WinContext->GetAppInstance(), MAKEINTRESOURCE(IDD_ABOUT), NULL, AboutProc, 0);
        Play();
        break;

    default:
        break;
    }
    return 0;
}

void CGamePlayer::OnExceptionCMO(WPARAM wParam, LPARAM lParam)
{
    CLogger::Get().Error("Exception in the CMO - Abort");
    m_WinContext->PostMessageToMainWindow(TT_MSG_EXIT_TO_SYS, 0, 0);
}

void CGamePlayer::OnReturn(WPARAM wParam, LPARAM lParam)
{
    InterfaceManager *im = m_NeMoContext->GetInterfaceManager();
    im->SetGameInfo(m_Game->GetGameInfo());

    if (!m_Game->Load())
    {
        OnDestroy();
        return;
    }
    Play();
}

bool CGamePlayer::OnLoadCMO(WPARAM wParam, LPARAM lParam)
{
    return Load((const char *)wParam);
}

void CGamePlayer::OnExitToSystem(WPARAM wParam, LPARAM lParam)
{
    CGameConfig &config = CGameConfig::Get();
    bool fullscreen = config.fullscreen;
    OnStopFullscreen();
    config.fullscreen = fullscreen;

    m_WinContext->PostMessageToMainWindow(WM_CLOSE, 0, 0);
}

void CGamePlayer::OnExitToTitle(WPARAM wParam, LPARAM lParam)
{
}

int CGamePlayer::OnChangeScreenMode(WPARAM wParam, LPARAM lParam)
{
    int driverBefore = m_NeMoContext->GetDriver();
    int screenModeBefore = m_NeMoContext->GetScreenMode();
    m_NeMoContext->SetDriver((int)lParam);
    m_NeMoContext->SetScreenMode((int)wParam);
    if (!m_NeMoContext->ApplyScreenMode())
    {
        m_NeMoContext->SetDriver(driverBefore);
        m_NeMoContext->SetScreenMode(screenModeBefore);

        CLogger::Get().Error("Failed to change screen mode");
        return 0;
    }

    if (m_NeMoContext->IsRenderFullscreen())
    {
        m_NeMoContext->StopFullscreen();
        m_NeMoContext->GoFullscreen();
    }

    int w, h;
    m_NeMoContext->GetResolution(w, h);
    m_WinContext->SetMainSize(w, h);
    m_WinContext->SetRenderSize(w, h);
    m_NeMoContext->GetRenderContext()->Resize();

    InterfaceManager *im = m_NeMoContext->GetInterfaceManager();
    im->SetDriver(m_NeMoContext->GetDriver());
    im->SetScreenMode(m_NeMoContext->GetScreenMode());

    CGameConfig &config = CGameConfig::Get();
    config.driver = m_NeMoContext->GetDriver();
    m_NeMoContext->GetResolution(config.width, config.height);
    config.bpp = m_NeMoContext->GetBPP();

    if (config.clipMouse)
        ClipMouse(true);

    return 1;
}

void CGamePlayer::OnGoFullscreen()
{
    CGameConfig &config = CGameConfig::Get();

    if (config.clipMouse)
        ClipMouse(false);

    Pause();

    config.fullscreen = true;
    m_NeMoContext->GoFullscreen();

    m_WinContext->SetMainStyle(WINDOW_STYLE_FULLSCREEN, WINDOW_STYLE_USECURRENTSETTINGS);
    m_WinContext->ShowMainWindow();
    m_WinContext->FocusMainWindow();
    m_WinContext->ShowRenderWindow();
    m_WinContext->FocusRenderWindow();
    m_WinContext->UpdateWindows();

    Play();
}

void CGamePlayer::OnStopFullscreen()
{
    CGameConfig &config = CGameConfig::Get();

    Pause();

    m_NeMoContext->StopFullscreen();
    config.fullscreen = false;

    m_WinContext->SetPosition(config.posX, config.posY);
    m_WinContext->SetMainStyle(WINDOW_STYLE_USECURRENTSETTINGS, WINDOW_STYLE_FULLSCREEN);
    m_WinContext->ShowMainWindow();
    m_WinContext->FocusMainWindow();
    m_WinContext->ShowRenderWindow();
    m_WinContext->FocusRenderWindow();
    m_WinContext->UpdateWindows();

    if (config.clipMouse)
        ClipMouse(true);

    Play();
}

void CGamePlayer::OnSwitchFullscreen()
{
    if (m_State == eInitial)
        return;

    if (!m_NeMoContext->IsRenderFullscreen())
        OnGoFullscreen();
    else
        OnStopFullscreen();
}

void CGamePlayer::Register(CGamePlayer *player)
{
    s_Instance = player;
}

CGamePlayer *CGamePlayer::GetInstance()
{
    return s_Instance;
}

int CGamePlayer::InitEngine()
{
    {
        CSplash splash(m_WinContext->GetAppInstance());
        splash.Show();
    }

    if (!m_NeMoContext->StartUp())
        return CKERR_INVALIDPARAMETER;

    if (!LoadRenderEngine())
        return CKERR_INVALIDPARAMETER;

    if (!LoadManagers())
        return CKERR_INVALIDPARAMETER;

    if (!LoadBuildingBlocks())
        return CKERR_INVALIDPARAMETER;

    if (!LoadPlugins())
        return CKERR_INVALIDPARAMETER;

    CGameConfig &config = CGameConfig::Get();

    if (!(config.manualSetup && ::DialogBoxParam(m_WinContext->GetAppInstance(), MAKEINTRESOURCE(IDD_FULLSCREEN_SETUP), NULL, FullscreenSetupProc, 0) == IDOK))
        m_NeMoContext->SetScreen(config.fullscreen, config.driver, config.width, config.height, config.bpp);

    CKERROR err = m_NeMoContext->CreateContext(m_WinContext->GetMainWindow());
    if (err != CK_OK)
    {
        CLogger::Get().Error("Failed to create virtools context");
        return err;
    }

    CKRenderManager *rm = m_NeMoContext->GetRenderManager();
    rm->SetRenderOptions("DisableFilter", config.disableFilter);
    rm->SetRenderOptions("DisableDithering", config.disableDithering);
    rm->SetRenderOptions("Antialias", config.antialias);
    rm->SetRenderOptions("DisableMipmap", config.disableMipmap);
    rm->SetRenderOptions("DisableSpecular", config.disableSpecular);

    if (!m_NeMoContext->FindScreenMode())
    {
        CLogger::Get().Error("Found no capable screen mode");
        return CKERR_INVALIDPARAMETER;
    }

    err = m_NeMoContext->CreateRenderContext(m_WinContext->GetRenderWindow());
    if (err != CK_OK)
    {
        CLogger::Get().Error("Failed to create render context");
        return err;
    }

    if (config.fullscreen)
        OnGoFullscreen();
    else
        OnStopFullscreen();

    m_NeMoContext->ClearScreen();
    m_NeMoContext->RefreshScreen();

    return CK_OK;
}

bool CGamePlayer::ReInitEngine()
{
    CLogger::Get().Warn("Failed to initialize engine, retrying");

    if (!m_NeMoContext->GetCKContext())
        return false;

    if (::DialogBoxParam(m_WinContext->GetAppInstance(), MAKEINTRESOURCE(IDD_FULLSCREEN_SETUP), NULL, FullscreenSetupProc, 0) != IDOK)
        return false;

    if (!m_NeMoContext->ApplyScreenMode())
        return false;

    CGameConfig &config = CGameConfig::Get();
    config.driver = m_NeMoContext->GetDriver();
    m_NeMoContext->GetResolution(config.width, config.height);
    config.bpp = m_NeMoContext->GetBPP();

    m_WinContext->SetMainSize(config.width, config.height);
    m_WinContext->SetRenderSize(config.width, config.height);
    if (!config.fullscreen)
        m_WinContext->SetPosition(config.posX, config.posY);

    CKERROR err = m_NeMoContext->CreateRenderContext(m_WinContext->GetRenderWindow());
    if (err != CK_OK)
    {
        CLogger::Get().Error("Cannot recreate render context");
        return false;
    }

    if (config.fullscreen)
        OnGoFullscreen();
    else
        OnStopFullscreen();

    m_NeMoContext->ClearScreen();
    m_NeMoContext->RefreshScreen();

    return true;
}

bool CGamePlayer::LoadRenderEngine()
{
    CGameConfig &config = CGameConfig::Get();
    const char *path = config.GetPath(eRenderEnginePath);
    if (!utils::DirectoryExists(path) || !m_NeMoContext->ParsePlugins(path))
    {
        CLogger::Get().Error("Render engine parse error");
        return false;
    }

    return true;
}

bool CGamePlayer::LoadManagers()
{
    static const char *const VirtoolsManagers[] = {
        "Dx5InputManager.dll",
        "DX7SoundManager.dll",
        "ParameterOperations.dll",
    };

    CGameConfig &config = CGameConfig::Get();
    const char *path = config.GetPath(eManagerPath);
    if (!utils::DirectoryExists(path))
    {
        CLogger::Get().Error("Managers directory does not exist!");
        return false;
    }

    if (config.loadAllManagers)
    {
        if (!m_NeMoContext->ParsePlugins(path))
        {
            CLogger::Get().Error("Managers parse error");
            return false;
        }
    }
    else
    {
        char szPath[MAX_PATH];
        for (int i = 0; i < ARRAY_NUM(VirtoolsManagers); ++i)
        {
            utils::ConcatPath(szPath, MAX_PATH, path, VirtoolsManagers[i]);
            if (!m_NeMoContext->RegisterPlugin(szPath))
            {
                CLogger::Get().Error("Unable to register manager: %s", VirtoolsManagers[i]);
                return false;
            }
        }
    }

    return true;
}

bool CGamePlayer::LoadBuildingBlocks()
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

    CGameConfig &config = CGameConfig::Get();
    const char *path = config.GetPath(eBuildingBlockPath);
    if (!utils::DirectoryExists(path))
    {
        CLogger::Get().Error("BuildingBlocks directory does not exist!");
        return false;
    }

    if (config.loadAllBuildingBlocks)
    {
        if (!m_NeMoContext->ParsePlugins(path))
        {
            CLogger::Get().Error("Behaviors parse error");
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
            if (!m_NeMoContext->RegisterPlugin(szPath))
            {
                CLogger::Get().Error("Unable to register building blocks: %s", VirtoolsBuildingBlocks[i]);
                return false;
            }
        }

        for (i = 0; i < ARRAY_NUM(CustomBuildingBlocks); ++i)
        {
            utils::ConcatPath(szPath, MAX_PATH, path, CustomBuildingBlocks[i]);
            if (!m_NeMoContext->RegisterPlugin(szPath))
            {
                CLogger::Get().Error("Unable to register building blocks: %s", CustomBuildingBlocks[i]);
                return false;
            }
        }
    }

    return true;
}

bool CGamePlayer::LoadPlugins()
{
    static const char *const VirtoolsPlugins[] = {
        "AVIReader.dll",
        "ImageReader.dll",
        "VirtoolsLoaderR.dll",
        "WavReader.dll",
    };

    CGameConfig &config = CGameConfig::Get();
    const char *path = config.GetPath(ePluginPath);
    if (!utils::DirectoryExists(path))
    {
        CLogger::Get().Error("Plugins directory does not exist!");
        return false;
    }

    if (config.loadAllPlugins)
    {
        if (!m_NeMoContext->ParsePlugins(path))
        {
            CLogger::Get().Error("Plugins parse error");
            return false;
        }
    }
    else
    {
        char szPath[MAX_PATH];
        for (int i = 0; i < ARRAY_NUM(VirtoolsPlugins); ++i)
        {
            utils::ConcatPath(szPath, MAX_PATH, path, VirtoolsPlugins[i]);
            if (!m_NeMoContext->RegisterPlugin(szPath))
            {
                CLogger::Get().Error("Unable to register plugin: %s", VirtoolsPlugins[i]);
                return false;
            }
        }
    }

    return true;
}

void CGamePlayer::RedirectLog()
{
    m_NeMoContext->SetInterfaceMode(FALSE, LogRedirect, NULL);
}