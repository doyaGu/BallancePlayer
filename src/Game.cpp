#include "Game.h"

#include <stdio.h>
#include <string.h>

#include "GameConfig.h"
#include "NeMoContext.h"
#include "ScriptUtils.h"
#include "Logger.h"
#include "InterfaceManager.h"

static bool SetDebugMode(CKBehavior *setDebugMode)
{
    if (!setDebugMode)
        return false;

    CKBehavior *bs = scriptutils::GetBehavior(setDebugMode, "Binary Switch");
    if (!bs)
        return false;

    CKBOOL debug = TRUE;
    scriptutils::GenerateInputParameter(setDebugMode, bs, 0, debug);

    return true;
}

static bool SetLanguage(CKBehavior *setLanguage, int langId)
{
    if (!setLanguage)
        return false;

    CKBehavior *gc = scriptutils::GetBehavior(setLanguage, "Get Cell");
    if (!gc)
        return false;

    CKBehaviorLink *linkIn0 = NULL;
    const int linkCount = setLanguage->GetSubBehaviorLinkCount();
    for (int i = 0; i < linkCount; ++i)
    {
        CKBehaviorLink *link = setLanguage->GetSubBehaviorLink(i);
        CKBehaviorIO *outIO = link->GetOutBehaviorIO();
        if (outIO->GetOwner() == gc && gc->GetInput(0) == outIO)
            linkIn0 = link;
    }
    if (!linkIn0)
        return false;

    CKBehaviorLink *linkRrOp = scriptutils::RemoveBehaviorLink(setLanguage, "TT_ReadRegistry", "Op", 0, 0);
    if (!linkRrOp)
        return false;

    CKBehavior *op2 = linkRrOp->GetOutBehaviorIO()->GetOwner();
    scriptutils::SetInputParameterValue(op2, 0, langId);
    CKDestroyObject(linkRrOp);

    linkIn0->SetOutBehaviorIO(op2->GetInput(0));

    return true;
}

static bool FillDriverList(CKDataArray *drivers)
{
    if (!drivers)
        return false;

    drivers->Clear();
    while (drivers->GetColumnCount() > 0)
        drivers->RemoveColumn(0);

    drivers->InsertColumn(-1, CKARRAYTYPE_STRING, "DriverDesc");
    drivers->InsertColumn(0, CKARRAYTYPE_STRING, "DriverName");
    drivers->InsertColumn(1, CKARRAYTYPE_INT, "DriverID");

    CNeMoContext *context = CNeMoContext::GetInstance();
    CKRenderManager *rm = context->GetRenderManager();

    const int driverCount = rm->GetRenderDriverCount();
    for (int i = 0; i < driverCount; ++i)
    {
        VxDriverDesc *drDesc = rm->GetRenderDriverDescription(i);
        drivers->InsertRow();
        drivers->SetElementStringValue(i, 0, drDesc->DriverName);
        drivers->SetElementValue(i, 1, &i, sizeof(int));
        drivers->SetElementStringValue(i, 2, drDesc->DriverDesc);
    }

    return true;
}

static bool SetDriver(CKBehavior *screenModes, int driver)
{
    if (!screenModes)
        return false;

    CKBehavior *ld = scriptutils::GetBehavior(screenModes, "TT List Driver");
    if (!ld)
        return false;

    CKParameter *paramDriver = ld->GetOutputParameter(0)->GetDestination(0);
    CKParameter *paramDriverList = ld->GetInputParameter(0)->GetDirectSource();
    CKDataArray *driverList = (CKDataArray *)paramDriverList->GetValueObject();

    CNeMoContext *context = CNeMoContext::GetInstance();
    FillDriverList(driverList);

    CKPathManager *pathManager = context->GetPathManager();
    XString path = pathManager->GetVirtoolsTemporaryFolder() + "\\drivers.cache";
    driverList->WriteElements(path.Str(), 0, driverList->GetColumnCount());

    CKBehavior *ca = scriptutils::CreateBehavior(screenModes, CKGUID(0x35c9352f, 0x7b1a193b), true); // Clear Array
    ca->GetTargetParameter()->SetDirectSource(paramDriverList);

    CKBehavior *al = scriptutils::CreateBehavior(screenModes, CKGUID(0x13bd2c64, 0x62db38e1), true); // Array Load
    al->GetTargetParameter()->SetDirectSource(paramDriverList);
    scriptutils::GenerateInputParameter(screenModes, al, 0, path.CStr());
    scriptutils::GenerateInputParameter(screenModes, al, 1, TRUE);
    scriptutils::GenerateInputParameter(screenModes, al, 2, 0);

    CKBehavior *identity = scriptutils::CreateBehavior(screenModes, CKGUID(0x15151652, 0xaeefffd5)); // Identity
    scriptutils::GenerateInputParameter(screenModes, identity, 0, driver);
    identity->GetOutputParameter(0)->AddDestination(paramDriver);

    CKBehaviorLink *linkLdSc = scriptutils::GetBehaviorLink(screenModes, ld, "Set Cell", 0, 0);
    if (!linkLdSc)
        return false;

    CKBehavior *sc = linkLdSc->GetOutBehaviorIO()->GetOwner();
    linkLdSc->SetOutBehaviorIO(ca->GetInput(0));
    scriptutils::CreateBehaviorLink(screenModes, ca, al, 0, 0);
    scriptutils::CreateBehaviorLink(screenModes, al, identity, 0, 0);
    scriptutils::CreateBehaviorLink(screenModes, identity, sc, 0, 0);

    return true;
}

static bool FillScreenModeList(int driver, CKDataArray *screenModes)
{
    if (!screenModes)
        return false;

    screenModes->Clear();
    while (screenModes->GetColumnCount() > 0)
        screenModes->RemoveColumn(0);

    screenModes->InsertColumn(-1, CKARRAYTYPE_INT, "Bpp");
    screenModes->InsertColumn(0, CKARRAYTYPE_INT, "Mode");
    screenModes->InsertColumn(1, CKARRAYTYPE_INT, "Width");
    screenModes->InsertColumn(2, CKARRAYTYPE_INT, "Height");

    CNeMoContext *context = CNeMoContext::GetInstance();
    CKRenderManager *rm = context->GetRenderManager();

    VxDriverDesc *drDesc = rm->GetRenderDriverDescription(driver);
    if (!drDesc)
        return false;

    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;
    int i = 0, row = 0;
    while (i < dmCount)
    {
        int width = dm[i].Width;
        int height = dm[i].Height;

        int maxRefreshRate = 0;
        for (int j = i; dm[j].Width == width && dm[j].Height == height && j < dmCount; ++j)
        {
            if (dm[j].RefreshRate > maxRefreshRate && dm[j].Bpp > 8)
                maxRefreshRate = dm[j].RefreshRate;
        }

        while (dm[i].Width == width && dm[i].Height == height && i < dmCount)
        {
            if (dm[i].RefreshRate == maxRefreshRate && dm[i].Bpp > 8)
            {
                screenModes->InsertRow();
                screenModes->SetElementValue(row, 0, &i, sizeof(int));
                screenModes->SetElementValue(row, 1, &dm[i].Width, sizeof(int));
                screenModes->SetElementValue(row, 2, &dm[i].Height, sizeof(int));
                screenModes->SetElementValue(row, 3, &dm[i].Bpp, sizeof(int));
                ++row;
            }
            ++i;
        }
    }

    return true;
}

static bool SetScreenMode(CKBehavior *screenModes, int screenMode)
{
    if (!screenModes)
        return false;

    CKBehavior *ls = scriptutils::GetBehavior(screenModes, "TT List ScreenModes");
    if (!ls)
        return false;

    CKParameter *paramScreenMode = ls->GetOutputParameter(0)->GetDestination(0);
    CKParameter *paramScreenModeList = ls->GetInputParameter(1)->GetDirectSource();
    CKDataArray *screenModeList = (CKDataArray *)paramScreenModeList->GetValueObject();

    CNeMoContext *context = CNeMoContext::GetInstance();
    FillScreenModeList(context->GetDriver(), screenModeList);

    CKPathManager *pathManager = context->GetPathManager();
    XString path = pathManager->GetVirtoolsTemporaryFolder() + "\\screenModes.cache";
    screenModeList->WriteElements(path.Str(), 0, screenModeList->GetColumnCount());

    CKBehavior *ca = scriptutils::CreateBehavior(screenModes, CKGUID(0x35c9352f, 0x7b1a193b), true); // Clear Array
    ca->GetTargetParameter()->SetDirectSource(paramScreenModeList);

    CKBehavior *al = scriptutils::CreateBehavior(screenModes, CKGUID(0x13bd2c64, 0x62db38e1), true); // Array Load
    al->GetTargetParameter()->SetDirectSource(paramScreenModeList);
    scriptutils::GenerateInputParameter(screenModes, al, 0, path.CStr());
    scriptutils::GenerateInputParameter(screenModes, al, 1, TRUE);
    scriptutils::GenerateInputParameter(screenModes, al, 2, 0);

    CKBehavior *identity = scriptutils::CreateBehavior(screenModes, CKGUID(0x15151652, 0xaeefffd5)); // Identity
    scriptutils::GenerateInputParameter(screenModes, identity, 0, screenMode);
    identity->GetOutputParameter(0)->AddDestination(paramScreenMode);

    CKBehaviorLink *linkLsSc = scriptutils::GetBehaviorLink(screenModes, ls, "Set Cell", 0, 0);
    if (!linkLsSc)
        return false;

    CKBehavior *sc = linkLsSc->GetOutBehaviorIO()->GetOwner();
    linkLsSc->SetOutBehaviorIO(ca->GetInput(0));
    scriptutils::CreateBehaviorLink(screenModes, ca, al, 0, 0);
    scriptutils::CreateBehaviorLink(screenModes, al, identity, 0, 0);
    scriptutils::CreateBehaviorLink(screenModes, identity, sc, 0, 0);

    return true;
}

static bool UnlockWidescreen(CKBehavior *screenModes, CKBehavior *minWidth)
{
    if (!screenModes || !minWidth)
        return false;

    CKBehavior *ic = scriptutils::GetBehavior(screenModes, "Insert Column", "ScreenModes");
    if (!ic)
        return false;

    CKBehaviorLink *linkScIc = scriptutils::RemoveBehaviorLink(screenModes, "Set Cell", ic, 0, 0);
    if (!linkScIc)
        return false;

    CKBehavior *sc = linkScIc->GetInBehaviorIO()->GetOwner();
    screenModes->RemoveSubBehaviorLink(linkScIc);
    CKDestroyObject(linkScIc);

    scriptutils::RemoveBehaviorLink(screenModes, "Remove Column", minWidth, 0, 0, true);
    scriptutils::CreateBehaviorLink(screenModes, sc, minWidth, 0, 0);

    return true;
}

static bool UnlockHighResolution(CKBehavior *screenModes, CKBehavior *bppFilter, CKBehavior *minWidth, CKBehavior *maxWidth, bool unlockWidescreen)
{
    if (!screenModes || !bppFilter || !minWidth || !maxWidth)
        return false;

    const char *inBehName = (unlockWidescreen) ? "Set Cell" : "Remove Column";
    CKBehaviorLink *linkRri = scriptutils::RemoveBehaviorLink(screenModes, inBehName, minWidth, 0, 0);
    if (!linkRri)
        return false;

    CKBehavior *inBeh = linkRri->GetInBehaviorIO()->GetOwner();
    CKDestroyObject(linkRri);

    scriptutils::RemoveBehaviorLink(screenModes, maxWidth, bppFilter, 0, 0, true);
    scriptutils::CreateBehaviorLink(screenModes, inBeh, bppFilter, 0, 0);

    return true;
}

static bool UnlockFramerate(CKBehavior *synchToScreen)
{
    if (!synchToScreen)
        return false;

    bool res = false;

    for (int i = 0; i < synchToScreen->GetSubBehaviorCount(); ++i)
    {
        CKBehavior *beh = synchToScreen->GetSubBehavior(i);
        if (strcmp(beh->GetName(), "Time Settings") == 0)
        {
            CKDWORD *frameRate = (CKDWORD *)beh->GetInputParameterReadDataPtr(0);
            if (*frameRate == 3) // Frame Rate == Limit
            {
                *frameRate = 1; // Frame Rate = Free
                res = true;
                break;
            }
        }
    }

    return res;
}

static bool SkipResolutionCheck(CKBehavior *synchToScreen)
{
    if (!synchToScreen)
        return false;

    CKBehavior *ii = scriptutils::GetBehavior(synchToScreen, "Iterator If");
    CKBehavior *delayer = scriptutils::GetBehavior(synchToScreen, "Delayer");
    if (!(ii && delayer))
        return false;

    CKBehaviorLink *linkTsIi1 = scriptutils::GetBehaviorLink(synchToScreen, "Time Settings", ii, 0, 0);
    CKBehaviorLink *linkTsIi2 = scriptutils::GetBehaviorLink(synchToScreen, "Time Settings", ii, 0, 0, linkTsIi1);
    if (!(linkTsIi1 && linkTsIi2))
        return false;

    CKBehaviorLink *linkDelayerCsm = scriptutils::RemoveBehaviorLink(synchToScreen, delayer, "TT Change ScreenMode", 0, 0);
    if (!linkDelayerCsm)
        return false;

    linkTsIi1->SetOutBehaviorIO(linkDelayerCsm->GetOutBehaviorIO());
    linkTsIi2->SetOutBehaviorIO(linkDelayerCsm->GetOutBehaviorIO());
    CKDestroyObject(linkDelayerCsm);

    return true;
}

static bool SkipOpeningAnimation(CKBehavior *defaultLevel, CKBehavior *synchToScreen)
{
    if (!defaultLevel || !synchToScreen)
        return false;

    CKBehavior *is = scriptutils::GetBehavior(defaultLevel, "Intro Start");
    CKBehavior *ie = scriptutils::GetBehavior(defaultLevel, "Intro Ende");
    CKBehavior *ml = scriptutils::GetBehavior(defaultLevel, "Main Loading");
    CKBehavior *ps = scriptutils::GetBehavior(defaultLevel, "Preload Sound");
    if (!(is && ie && ml && ps))
        return false;

    CKBehaviorLink *linkStsIs = scriptutils::GetBehaviorLink(defaultLevel, synchToScreen, is, 0, 0);
    if (!linkStsIs)
        return false;

    linkStsIs->SetOutBehaviorIO(ml->GetInput(0));

    CKBehaviorLink *linkIeAs = scriptutils::GetBehaviorLink(defaultLevel, ie, "Activate Script", 0, 0);
    if (!linkIeAs)
        return false;

    CKBehaviorLink *linkPsWfa = scriptutils::GetBehaviorLink(defaultLevel, ps, "Wait For All", 0, 0);
    if (!linkPsWfa)
        return false;

    linkPsWfa->SetOutBehaviorIO(linkIeAs->GetOutBehaviorIO());
    return true;
}

static void EditScriptDefaultLevel(CKBehavior *defaultLevel)
{
    CNeMoContext *context = CNeMoContext::GetInstance();
    const CGameConfig &config = CGameConfig::Get();

    // Set debug mode
    if (config.debug)
    {
        if (!SetDebugMode(scriptutils::GetBehavior(defaultLevel, "set DebugMode")))
            CLogger::Get().Error("Failed to set debug mode");
    }

    // Bypass "Set Language" script and set our language id
    if (!SetLanguage(scriptutils::GetBehavior(defaultLevel, "Set Language"), config.langId))
        CLogger::Get().Error("Failed to set language id");

    int i;

    CKBehavior *sm = scriptutils::GetBehavior(defaultLevel, "Screen Modes");
    if (!sm)
    {
        CLogger::Get().Error("Unable to find script Screen Modes");
        return;
    }

    if (!SetDriver(sm, context->GetDriver()))
    {
        CLogger::Get().Error("Failed to set driver");
        return;
    }

    if (!SetScreenMode(sm, context->GetScreenMode()))
    {
        CLogger::Get().Error("Failed to set screen mode");
        return;
    }

    CKBehavior *bbpFilter = NULL;
    CKBehavior *minWidth = NULL;
    CKBehavior *maxWidth = NULL;
    for (i = 0; i < sm->GetSubBehaviorCount(); ++i)
    {
        CKBehavior *beh = sm->GetSubBehavior(i);
        if (strcmp(beh->GetName(), "Remove Row If") == 0)
        {
            switch (scriptutils::GetInputParameterValue<int>(beh, 2))
            {
            case 16: // BBP filter
                bbpFilter = beh;
                break;
            case 640: // Minimum width
                minWidth = beh;
                break;
            case 1600: // Maximum width
                maxWidth = beh;
                break;
            default:
                break;
            }
        }
    }

    // Correct the bbp filter
    if (!bbpFilter)
    {
        CLogger::Get().Error("Failed to correct the bbp filter");
    }
    else
    {
        scriptutils::SetInputParameterValue(bbpFilter, 2, context->GetBPP());
    }

    // Unlock widescreen (Not 4:3)
    if (config.unlockWidescreen)
    {
        if (!UnlockWidescreen(sm, minWidth))
            CLogger::Get().Error("Failed to unlock widescreen");
    }

    // Unlock high resolution
    if (config.unlockHighResolution)
    {
        if (!UnlockHighResolution(sm, bbpFilter, minWidth, maxWidth, config.unlockWidescreen))
            CLogger::Get().Error("Failed to unlock high resolution");
    }

    CKBehavior *sts = scriptutils::GetBehavior(defaultLevel, "Synch to Screen");
    if (!sts)
    {
        CLogger::Get().Error("Unable to find script Synch to Screen");
        return;
    }

    // Unlock frame rate limitation
    if (config.unlockFramerate)
    {
        if (!UnlockFramerate(sts))
            CLogger::Get().Error("Failed to unlock frame rate limitation");
    }

    // Make it not to test 640x480 resolution
    if (!SkipResolutionCheck(sts))
        CLogger::Get().Error("Failed to bypass 640x480 resolution test");

    // Skip Opening Animation
    if (config.skipOpening)
    {
        if (!SkipOpeningAnimation(defaultLevel, sts))
            CLogger::Get().Error("Failed to skip opening animation");
    }
}

CGame::CGame()
    : m_NeMoContext(NULL),
      m_GameInfo(NULL),
      m_Level(NULL),
      m_Gameplay(NULL),
      m_InGame(false)
{
    InitEventHandler();
}

CGame::~CGame()
{
    if (m_GameInfo)
        delete m_GameInfo;
    m_GameInfo = NULL;
}

bool CGame::Load()
{
    m_NeMoContext = CNeMoContext::GetInstance();
    if (!m_NeMoContext)
    {
        CLogger::Get().Error("NeMoContext is NULL: CMO is not loaded");
        return false;
    }

    if (!m_GameInfo)
    {
        CLogger::Get().Error("gameInfo is NULL: CMO is not loaded");
        return false;
    }

    CGameConfig &config = CGameConfig::Get();

    char cmoPath[MAX_PATH];
    _snprintf(cmoPath, MAX_PATH, "%s%s\\%s", config.GetPath(eRootPath), m_GameInfo->path, m_GameInfo->fileName);
    FILE *fp = fopen(cmoPath, "r");
    if (!fp)
    {
        CLogger::Get().Error("Failed to open the cmo file");
        return false;
    }
    fclose(fp);

    m_NeMoContext->Cleanup();

    CKObjectArray *array = CreateCKObjectArray();
    if (!array)
    {
        CLogger::Get().Error("CreateCKObjectArray() Failed");
        return false;
    }

    // Load the file and fills the array with loaded objects
    if (m_NeMoContext->Load(cmoPath, array) != CK_OK)
    {
        CLogger::Get().Error("LoadFile() Failed");
        return false;
    }

    DeleteCKObjectArray(array);

    return FinishLoad();
}

bool CGame::IsInGame() const { return m_InGame; }

bool CGame::IsPaused() const { return m_Paused; }

bool CGame::IsPlaying() const { return m_InGame && !m_Paused; }

void CGame::ExitGame()
{
    SendMessageToLevel("Exit Game");
}

void CGame::PauseLevel()
{
    SendMessageToLevel("Pause Level");
}

void CGame::UnpauseLevel()
{
    SendMessageToLevel("Unpause Level");
}

void CGame::ShouldResize()
{
    SyncCamerasWithScreen();
}

void CGame::ShowCursor(bool show)
{
    if (show && !m_ShowCursor)
        SendMessageToLevel("Mouse On");
    else if (!show && m_ShowCursor)
        SendMessageToLevel("Mouse Off");
}

bool CGame::GetCursorVisibility() const { return m_ShowCursor; }

CGameInfo *CGame::NewGameInfo()
{
    m_GameInfo = new CGameInfo();
    return m_GameInfo;
}

CGameInfo *CGame::GetGameInfo() const { return m_GameInfo; }

void CGame::SetGameInfo(CGameInfo *gameInfo) { m_GameInfo = gameInfo; }

void CGame::HandleGameEvent()
{
    HandleLevelEvent();
    if (m_InGame)
        HandleGameplayEvent();
}

void CGame::HandlePostGameEvent()
{
    GAMEEVENT_CALLBACK *it = m_PostEventCallbacks.Begin();
    while (it != m_PostEventCallbacks.End())
        (*it++)(this, NULL);
    m_PostEventCallbacks.Clear();
}

static void HandleGameEvent(CNeMoContext *context, void *argument)
{
    ((CGame *)argument)->HandleGameEvent();
}

static void HandlePostGameEvent(CNeMoContext *context, void *argument)
{
    ((CGame *)argument)->HandlePostGameEvent();
}

bool CGame::FinishLoad()
{
    const CGameConfig &config = CGameConfig::Get();

    m_NeMoContext->AddPreProcessCallBack(::HandleGameEvent, this);
    m_NeMoContext->AddPostProcessCallBack(::HandlePostGameEvent, this);

    // Retrieve the level
    m_Level = m_NeMoContext->GetCurrentLevel();
    if (!m_Level)
    {
        CLogger::Get().Error("GetCurrentLevel() Failed");
        return false;
    }

    CKBehavior *defaultLevel = scriptutils::GetBehavior(m_Level->ComputeObjectList(CKCID_BEHAVIOR), "Default Level");
    if (!defaultLevel)
    {
        CLogger::Get().Error("Unable to find script Default Level");
        return false;
    }

    EditScriptDefaultLevel(defaultLevel);

    // Add the render context to the level
    CKRenderContext *dev = m_NeMoContext->GetRenderContext();
    m_Level->AddRenderContext(dev, TRUE);

    // Set the initial conditions for the level
    m_Level->LaunchScene(NULL);

    // Render the first frame
    m_NeMoContext->RefreshScreen();
    m_NeMoContext->Render();

    return true;
}

class CGameEventHandler
{
public:
    static void OnPreStartMenu(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pre Start Menu");
        game->AddPostEventCallback(OnPostStartMenu);
    }

    static void OnPostStartMenu(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Post Start Menu");
    }

    static void OnExitGame(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Exit Game");
    }

    static void OnPreLoadLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pre Load Level");
        game->AddPostEventCallback(OnPostLoadLevel);
    }

    static void OnPostLoadLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Post Load Level");
    }

    static void OnStartLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Start Level");
        if (!game->GetGameplay())
        {
            CLogger::Get().Error("Fatal Error: No Gameplay found!");
            game->ExitGame();
        }
        game->m_InGame = true;
        game->m_Paused = false;
    }

    static void OnPreResetLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pre Reset Level");
        game->AddPostEventCallback(OnPostResetLevel);
    }

    static void OnPostResetLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Post Reset Level");
    }

    static void OnPauseLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pause Level");
        game->m_Paused = true;
    }

    static void OnUnpauseLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Unpause Level");
        game->m_Paused = false;
    }

    static void OnPreExitLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pre Exit Level");
        game->AddPostEventCallback(OnPostExitLevel);
    }

    static void OnPostExitLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Post Exit Level");
        game->m_InGame = false;
    }

    static void OnPreNextLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pre Next Level");
        game->AddPostEventCallback(OnPostNextLevel);
    }

    static void OnPostNextLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Post Next Level");
    }

    static void OnDead(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Dead");
        game->m_InGame = false;
    }

    static void OnPreEndLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pre End Level");
        game->AddPostEventCallback(OnPostEndLevel);
    }

    static void OnPostEndLevel(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Post End Level");
        game->m_InGame = false;
    }

    static void OnLoaded(CGame *game, void *arg)
    {
        const char *loaded = (const char *)((CKMessage *)arg)->GetParameter(0)->GetReadDataPtr();
        CLogger::Get().Debug(loaded);

        if (strcmp(loaded, "MainLoading_Loaded") == 0)
        {
            game->SyncCamerasWithScreen();
        }
    }

    static void OnMouseOn(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Mouse On");
        game->m_ShowCursor = true;
    }

    static void OnMouseOff(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Mouse Off");
        game->m_InGame = false;
    }

    static void OnFadeIn(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Blend FadeIn");
    }

    static void OnFadeOut(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Blend FadeOut");
    }

    static void OnCounterActive(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Counter Active");
    }

    static void OnCounterInactive(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Counter Inactive");
    }

    static void OnBallNavActive(CGame *game, void *arg)
    {
        CLogger::Get().Debug("BallNav Active");
    }

    static void OnBallNavInactive(CGame *game, void *arg)
    {
        CLogger::Get().Debug("BallNav Inactive");
    }

    static void OnCamNavActive(CGame *game, void *arg)
    {
        CLogger::Get().Debug("CamNav Active");
    }

    static void OnCamNavInactive(CGame *game, void *arg)
    {
        CLogger::Get().Debug("CamNav Inactive");
    }

    static void OnBallOff(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Ball Off");
    }

    static void OnPreCheckpointReached(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pre Checkpoint Reached");
        game->AddPostEventCallback(OnPostCheckpointReached);
    }

    static void OnPostCheckpointReached(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Post Checkpoint Reached");
    }

    static void OnLevelFinish(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Level Finish");
    }

    static void OnGameOver(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Game Over");
    }

    static void OnExtraPoint(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Extra Point");
    }

    static void OnPreLifeUp(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pre Life Up");
        game->AddPostEventCallback(OnPostLifeUp);
    }

    static void OnPostLifeUp(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Post Life Up");
    }

    static void OnPreSubLife(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Pre Sub Life");
        game->AddPostEventCallback(OnPostSubLife);
    }

    static void OnPostSubLife(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Post Sub Life");
    }

    static void OnRefresh(CGame *game, void *arg)
    {
        CLogger::Get().Debug("Refresh");
    }
};

void CGame::InitEventHandler()
{
    // Level Messages
    m_LevelEventHandlers.Insert("Start Menu", CGameEventHandler::OnPreStartMenu);
    m_LevelEventHandlers.Insert("Exit Game", CGameEventHandler::OnExitGame);
    m_LevelEventHandlers.Insert("Load Level", CGameEventHandler::OnPreLoadLevel);
    m_LevelEventHandlers.Insert("Start Level", CGameEventHandler::OnStartLevel);
    m_LevelEventHandlers.Insert("Reset Level", CGameEventHandler::OnPreResetLevel);
    m_LevelEventHandlers.Insert("Pause Level", CGameEventHandler::OnPauseLevel);
    m_LevelEventHandlers.Insert("Unpause Level", CGameEventHandler::OnUnpauseLevel);
    m_LevelEventHandlers.Insert("Exit Level", CGameEventHandler::OnPreExitLevel);
    m_LevelEventHandlers.Insert("Next Level", CGameEventHandler::OnPreNextLevel);
    m_LevelEventHandlers.Insert("Dead", CGameEventHandler::OnDead);
    m_LevelEventHandlers.Insert("End Level", CGameEventHandler::OnPreEndLevel);

    m_LevelEventHandlers.Insert("Loaded", CGameEventHandler::OnLoaded);
    m_LevelEventHandlers.Insert("Mouse On", CGameEventHandler::OnMouseOn);
    m_LevelEventHandlers.Insert("Mouse Off", CGameEventHandler::OnMouseOff);
    m_LevelEventHandlers.Insert("Blend FadeIn", CGameEventHandler::OnFadeIn);
    m_LevelEventHandlers.Insert("Blend FadeOut", CGameEventHandler::OnFadeOut);

    // Gameplay Messages
    m_GameplayEventHandlers.Insert("Counter active", CGameEventHandler::OnCounterActive);
    m_GameplayEventHandlers.Insert("Counter inactive", CGameEventHandler::OnCounterInactive);

    m_GameplayEventHandlers.Insert("BallNav activate", CGameEventHandler::OnBallNavActive);
    m_GameplayEventHandlers.Insert("BallNav deactivate", CGameEventHandler::OnBallNavInactive);

    m_GameplayEventHandlers.Insert("CamNav activate", CGameEventHandler::OnCamNavActive);
    m_GameplayEventHandlers.Insert("CamNav deactivate", CGameEventHandler::OnCamNavInactive);

    m_GameplayEventHandlers.Insert("Ball Off", CGameEventHandler::OnBallOff);
    m_GameplayEventHandlers.Insert("Checkpoint reached", CGameEventHandler::OnPreCheckpointReached);
    m_GameplayEventHandlers.Insert("Level_Finish", CGameEventHandler::OnLevelFinish);
    m_GameplayEventHandlers.Insert("Game Over", CGameEventHandler::OnGameOver);

    m_GameplayEventHandlers.Insert("Extrapoint", CGameEventHandler::OnExtraPoint);
    m_GameplayEventHandlers.Insert("Life_Up", CGameEventHandler::OnPreLifeUp);
    m_GameplayEventHandlers.Insert("Sub Life", CGameEventHandler::OnPreSubLife);

    m_GameplayEventHandlers.Insert("Refresh", CGameEventHandler::OnRefresh);
}

bool CGame::GetGameplay()
{
    if (!m_Gameplay)
    {
        m_Gameplay = m_NeMoContext->GetGroupByName("All_Gameplay");
        if (!m_Gameplay)
            return false;
    }
    return true;
}

void CGame::HandleLevelEvent()
{
    if (m_Level)
    {
        int msgCount = m_Level->GetLastFrameMessageCount();
        for (int i = 0; i < msgCount; ++i)
        {
            CKMessage *msg = m_Level->GetLastFrameMessage(i);
            const char *name = m_NeMoContext->GetMessageTypeName(msg->GetMsgType());
            XHashTable<GAMEEVENT_CALLBACK, XString>::Iterator it = m_LevelEventHandlers.Find(name);
            if (it != m_LevelEventHandlers.End())
            {
                (*it)(this, (void *)msg);
            }
        }
    }
}

void CGame::HandleGameplayEvent()
{
    if (m_Gameplay)
    {
        int msgCount = m_Gameplay->GetLastFrameMessageCount();
        for (int i = 0; i < msgCount; ++i)
        {
            CKMessage *msg = m_Gameplay->GetLastFrameMessage(i);
            const char *name = m_NeMoContext->GetMessageTypeName(msg->GetMsgType());
            XHashTable<GAMEEVENT_CALLBACK, XString>::Iterator it = m_GameplayEventHandlers.Find(name);
            if (it != m_GameplayEventHandlers.End())
            {
                (*it)(this, (void *)msg);
            }
        }
    }
}

void CGame::SendMessageToLevel(const char *msg)
{
    if (m_Level)
        m_NeMoContext->SendMessageSingle(msg, m_Level);
}

void CGame::SendMessageToGameplay(const char *msg)
{
    if (m_Gameplay)
        m_NeMoContext->SendMessageSingle(msg, m_Gameplay);
}

void CGame::AddPostEventCallback(GAMEEVENT_CALLBACK callback)
{
    if (!m_PostEventCallbacks.IsHere(callback))
        m_PostEventCallbacks.PushBack(callback);
}

void CGame::SyncCamerasWithScreen()
{
    if (CGameConfig::Get().adaptiveCamera && m_NeMoContext)
    {
        CKRenderContext *dev = m_NeMoContext->GetRenderContext();
        if (!dev)
            return;

        CKCamera *cams[2] = {
            (CKCamera *)m_NeMoContext->GetObjectByNameAndClass("Cam_MenuLevel", CKCID_TARGETCAMERA),
            (CKCamera *)m_NeMoContext->GetObjectByNameAndClass("InGameCam", CKCID_TARGETCAMERA)};

        for (int i = 0; i < 2; ++i)
        {
            if (cams[i])
            {
                cams[i]->SetAspectRatio(dev->GetWidth(), dev->GetHeight());
                cams[i]->SetFov((float)(0.75 * dev->GetWidth() / dev->GetHeight()));
            }
        }
    }
}