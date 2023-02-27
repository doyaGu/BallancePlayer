#include "NeMoContext.h"

#include <stdlib.h>
#include <string.h>

#include "Logger.h"
#include "InterfaceManager.h"

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

CNeMoContext *CNeMoContext::s_Instance = NULL;

CNeMoContext::CNeMoContext()
    : m_CKContext(NULL),
      m_RenderContext(NULL),
      m_PluginManager(NULL),
      m_ParameterManager(NULL),
      m_TimeManager(NULL),
      m_MessageManager(NULL),
      m_RenderManager(NULL),
      m_BehaviorManager(NULL),
      m_AttributeManager(NULL),
      m_PathManager(NULL),
      m_FloorManager(NULL),
      m_SoundManager(NULL),
      m_InputManager(NULL),
      m_CollisionManager(NULL),
      m_InterfaceManager(NULL),
      m_RenderEngine("CK2_3D"),
      m_Driver(0),
      m_ScreenMode(-1),
      m_DisplayMode(),
      m_Fullscreen(false),
      m_DebugMode(false),
      m_InDebug(false),
      m_DebugContext(NULL),
      m_ProcessFunc(&CNeMoContext::ProcessIdle)
{
    RegisterInstance(this);
}

CNeMoContext::~CNeMoContext()
{
    Shutdown();
}

CKERROR CNeMoContext::CreateContext(WIN_HANDLE mainWin)
{
    if (!mainWin)
        return CKERR_INVALIDPARAMETER;

    int iRenderEngine = GetRenderEnginePluginIdx();
    if (iRenderEngine == -1)
    {
        CLogger::Get().Error("Found no render-engine! (Driver? Critical!!!)");
        return CKERR_NORENDERENGINE;
    }

    CKERROR res = CKCreateContext(&m_CKContext, mainWin, iRenderEngine, 0);
    if (res != CK_OK)
    {
        if (res == CKERR_NODLLFOUND)
        {
            CLogger::Get().Error("Render engine is not found");
            return CKERR_NODLLFOUND;
        }

        CLogger::Get().Error("Unable to load render engine");
        return CKERR_NORENDERENGINE;
    }

    m_CKContext->SetVirtoolsVersion(CK_VIRTOOLS_DEV, 0x2000043);

    m_ParameterManager = m_CKContext->GetParameterManager();
    if (!m_ParameterManager)
    {
        CLogger::Get().Error("No Parameter Manager found");
        return CKERR_NODLLFOUND;
    }

    m_TimeManager = m_CKContext->GetTimeManager();
    if (!m_TimeManager)
    {
        CLogger::Get().Error("No Time Manager found");
        return CKERR_NODLLFOUND;
    }

    m_MessageManager = m_CKContext->GetMessageManager();
    if (!m_MessageManager)
    {
        CLogger::Get().Error("No Message Manager found");
        return CKERR_NODLLFOUND;
    }

    m_RenderManager = m_CKContext->GetRenderManager();
    if (!m_RenderManager)
    {
        CLogger::Get().Error("No Render Manager found");
        return CKERR_NODLLFOUND;
    }

    m_BehaviorManager = m_CKContext->GetBehaviorManager();
    if (!m_BehaviorManager)
    {
        CLogger::Get().Error("No Behavior Manager found");
        return CKERR_NODLLFOUND;
    }

    m_AttributeManager = m_CKContext->GetAttributeManager();
    if (!m_AttributeManager)
    {
        CLogger::Get().Error("No Attribute Manager found");
        return CKERR_NODLLFOUND;
    }

    m_PathManager = m_CKContext->GetPathManager();
    if (!m_PathManager)
    {
        CLogger::Get().Error("No Attribute Manager found");
        return CKERR_NODLLFOUND;
    }

    m_FloorManager = (CKFloorManager *)m_CKContext->GetManagerByGuid(FLOOR_MANAGER_GUID);
    if (!m_FloorManager)
    {
        CLogger::Get().Error("No Floor Manager found");
        return CKERR_NODLLFOUND;
    }

    m_SoundManager = (CKSoundManager *)m_CKContext->GetManagerByGuid(SOUND_MANAGER_GUID);
    if (!m_SoundManager)
    {
        CLogger::Get().Error("No Sound Manager found");
        return CKERR_NODLLFOUND;
    }

    m_InputManager = (CKInputManager *)m_CKContext->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!m_InputManager)
    {
        CLogger::Get().Error("No Input Manager found");
        return CKERR_NODLLFOUND;
    }

    m_CollisionManager = (CKCollisionManager *)m_CKContext->GetManagerByGuid(COLLISION_MANAGER_GUID);
    if (!m_CollisionManager)
    {
        CLogger::Get().Error("No Collision Manager found");
        return CKERR_NODLLFOUND;
    }

    m_InterfaceManager = (InterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (!m_InterfaceManager)
    {
        CLogger::Get().Error("No Interface Manager found");
        return CKERR_NODLLFOUND;
    }

    return CK_OK;
}

CKERROR CNeMoContext::CreateRenderContext(WIN_HANDLE renderWin)
{
    if (!renderWin)
        return CKERR_INVALIDPARAMETER;

    CKRenderManager *rm = m_CKContext->GetRenderManager();
    CKRECT rect = {0, 0, m_DisplayMode.Width, m_DisplayMode.Height};
    m_RenderContext = rm->CreateRenderContext(renderWin, m_Driver, &rect, FALSE, m_DisplayMode.Bpp);
    if (!m_RenderContext)
        return CKERR_INVALIDPARAMETER;

    return CK_OK;
}

bool CNeMoContext::StartUp()
{
    if (CKStartUp() != CK_OK)
    {
        CLogger::Get().Error("Virtools Engine cannot startup!");
        return false;
    }
    m_PluginManager = CKGetPluginManager();
    return true;
}

void CNeMoContext::Shutdown()
{
    if (!m_CKContext)
        return;

    Reset();
    Cleanup();

    if (m_RenderManager && m_RenderContext)
        m_RenderManager->DestroyRenderContext(m_RenderContext);
    m_RenderContext = NULL;

    CKCloseContext(m_CKContext);
    CKShutdown();
    m_CKContext = NULL;
}

void CNeMoContext::Cleanup()
{
    XASSERT(m_CKContext);
    m_CKContext->ClearAll();
    ClearCallbacks();
}

void CNeMoContext::Play()
{
    XASSERT(m_CKContext);
    if (m_DebugMode)
    {
        DebugEnd();
        m_DebugMode = false;
    }
    m_ProcessFunc = &CNeMoContext::ProcessRuntime;
    m_CKContext->Play();
}

void CNeMoContext::Debug()
{
    XASSERT(m_CKContext);
    m_DebugMode = true;
    m_ProcessFunc = &CNeMoContext::ProcessDebug;
    m_CKContext->Play();
}

void CNeMoContext::Pause()
{
    XASSERT(m_CKContext);
    m_CKContext->Pause();
}

void CNeMoContext::Reset()
{
    XASSERT(m_CKContext);
    m_CKContext->Reset();
}

bool CNeMoContext::IsPlaying() const
{
    XASSERT(m_CKContext);
    return m_CKContext->IsPlaying() == TRUE;
}

bool CNeMoContext::IsDebugging() const
{
    return m_DebugMode;
}

bool CNeMoContext::IsReseted() const
{
    XASSERT(m_CKContext);
    return m_CKContext->IsReseted() == TRUE;
}

void CNeMoContext::AddPreProcessCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp)
{
    Callback cb = {(void *)func, arg, temp};
    m_PreProcessCallbacks.PushBack(cb);
}

void CNeMoContext::RemovePreProcessCallBack(NEMO_CALLBACK func, void *arg)
{
    Callback cb = {(void *)func, arg};
    m_PreProcessCallbacks.Remove(cb);
}

void CNeMoContext::AddPostProcessCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp)
{
    Callback cb = {(void *)func, arg, temp};
    m_PostProcessCallbacks.PushBack(cb);
}

void CNeMoContext::RemovePostProcessCallBack(NEMO_CALLBACK func, void *arg)
{
    Callback cb = {(void *)func, arg};
    m_PostProcessCallbacks.Remove(cb);
}

void CNeMoContext::AddPreRenderCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp)
{
    Callback cb = {(void *)func, arg, temp};
    m_PreRenderCallbacks.PushBack(cb);
}

void CNeMoContext::RemovePreRenderCallBack(NEMO_CALLBACK func, void *arg)
{
    Callback cb = {(void *)func, arg};
    m_PreRenderCallbacks.Remove(cb);
}

void CNeMoContext::AddPostRenderCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp)
{
    Callback cb = {(void *)func, arg, temp};
    m_PostRenderCallbacks.PushBack(cb);
}

void CNeMoContext::RemovePostRenderCallBack(NEMO_CALLBACK func, void *arg)
{
    Callback cb = {(void *)func, arg};
    m_PostRenderCallbacks.Remove(cb);
}

void CNeMoContext::AddDebugCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp)
{
    Callback cb = {(void *)func, arg, temp};
    m_DebugCallbacks.PushBack(cb);
}

void CNeMoContext::RemoveDebugCallBack(NEMO_CALLBACK func, void *arg)
{
    Callback cb = {(void *)func, arg};
    m_DebugCallbacks.Remove(cb);
}

void CNeMoContext::Update()
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

void CNeMoContext::Process()
{
    ExecuteCallbacks(m_PreProcessCallbacks);
    (this->*m_ProcessFunc)();
    ExecuteCallbacks(m_PostProcessCallbacks);
}

CKERROR CNeMoContext::Render(CK_RENDER_FLAGS flags)
{
    ExecuteCallbacks(m_PreRenderCallbacks);
    CKERROR res = m_RenderContext->Render(flags);
    ExecuteCallbacks(m_PostRenderCallbacks);
    return res;
}

void CNeMoContext::ClearScreen()
{
    if (m_RenderContext)
    {
        m_RenderContext->Clear();
        m_RenderContext->Clear(CK_RENDER_BACKGROUNDSPRITES);
        m_RenderContext->Clear(CK_RENDER_FOREGROUNDSPRITES);
        m_RenderContext->Clear(CK_RENDER_USECAMERARATIO);
        m_RenderContext->Clear(CK_RENDER_CLEARZ);
        m_RenderContext->Clear(CK_RENDER_CLEARBACK);
        m_RenderContext->Clear(CK_RENDER_DOBACKTOFRONT);
        m_RenderContext->Clear(CK_RENDER_DEFAULTSETTINGS);
        m_RenderContext->Clear(CK_RENDER_CLEARVIEWPORT);
        m_RenderContext->Clear(CK_RENDER_FOREGROUNDSPRITES);
        m_RenderContext->Clear(CK_RENDER_USECAMERARATIO);
        m_RenderContext->Clear(CK_RENDER_WAITVBL);
        m_RenderContext->Clear(CK_RENDER_PLAYERCONTEXT);
    }
}

void CNeMoContext::RefreshScreen()
{
    if (m_RenderContext)
    {
        m_RenderContext->Clear();
        m_RenderContext->SetClearBackground();
        m_RenderContext->BackToFront();
        m_RenderContext->SetClearBackground();
        m_RenderContext->Clear();
    }
}

void CNeMoContext::SetScreen(int driver, int width, int height, int bpp, int refreshRate)
{
    m_Driver = driver;
    m_DisplayMode.Width = width;
    m_DisplayMode.Height = height;
    m_DisplayMode.Bpp = bpp;
    m_DisplayMode.RefreshRate = refreshRate;
}

void CNeMoContext::GetResolution(int &width, int &height)
{
    width = m_DisplayMode.Width;
    height = m_DisplayMode.Height;
}

void CNeMoContext::SetResolution(int width, int height)
{
    m_DisplayMode.Width = width;
    m_DisplayMode.Height = height;
}

int CNeMoContext::GetWidth() const
{
    return m_DisplayMode.Width;
}

void CNeMoContext::SetWidth(int width)
{
    m_DisplayMode.Width = width;
}

int CNeMoContext::GetHeight() const
{
    return m_DisplayMode.Height;
}

void CNeMoContext::SetHeight(int height)
{
    m_DisplayMode.Height = height;
}

int CNeMoContext::GetBPP() const
{
    return m_DisplayMode.Bpp;
}

void CNeMoContext::SetBPP(int bpp)
{
    m_DisplayMode.Bpp = bpp;
}

int CNeMoContext::GetRefreshRate() const
{
    return m_DisplayMode.RefreshRate;
}

void CNeMoContext::SetRefreshRate(int refreshRate)
{
    m_DisplayMode.RefreshRate = refreshRate;
}

int CNeMoContext::GetDriver() const
{
    return m_Driver;
}

void CNeMoContext::SetDriver(int driver)
{
    m_Driver = driver;
}

int CNeMoContext::GetScreenMode() const
{
    return m_ScreenMode;
}

void CNeMoContext::SetScreenMode(int screenMode)
{
    m_ScreenMode = screenMode;
}

bool CNeMoContext::FindScreenMode()
{
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Driver);
    if (!drDesc)
    {
        CLogger::Get().Error("Unable to find specified driver");
        return false;
    }

    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;

    m_DisplayMode.RefreshRate = 0;
    for (int i = 0; i < dmCount; ++i)
    {
        if (dm[i].Width == m_DisplayMode.Width &&
            dm[i].Height == m_DisplayMode.Height &&
            dm[i].Bpp == m_DisplayMode.Bpp)
        {
            if (dm[i].RefreshRate > m_DisplayMode.RefreshRate)
                m_DisplayMode.RefreshRate = dm[i].RefreshRate;
        }
    }

    m_ScreenMode = -1;
    for (int j = 0; j < dmCount; ++j)
    {
        if (dm[j].Width == m_DisplayMode.Width &&
            dm[j].Height == m_DisplayMode.Height &&
            dm[j].Bpp == m_DisplayMode.Bpp &&
            dm[j].RefreshRate == m_DisplayMode.RefreshRate)
        {
            m_ScreenMode = j;
            break;
        }
    }

    return m_ScreenMode >= 0;
}

bool CNeMoContext::ApplyScreenMode()
{
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Driver);
    if (!drDesc)
        return false;

    if (m_ScreenMode < 0 || m_ScreenMode >= drDesc->DisplayModeCount)
        return false;

    m_DisplayMode = drDesc->DisplayModes[m_ScreenMode];
    return true;
}

bool CNeMoContext::IsFullscreen() const
{
    return m_Fullscreen;
}

void CNeMoContext::SetFullscreen(bool fullscreen)
{
    m_Fullscreen = fullscreen;
}

bool CNeMoContext::IsRenderFullscreen() const
{
    XASSERT(m_RenderContext);
    return m_RenderContext->IsFullScreen() == TRUE;
}

bool CNeMoContext::GoFullscreen()
{
    if (!m_RenderContext || IsRenderFullscreen())
        return false;

    if (m_RenderContext->GoFullScreen(m_DisplayMode.Width, m_DisplayMode.Height, m_DisplayMode.Bpp, m_Driver) != CK_OK)
        return false;

    m_Fullscreen = true;
    return true;
}

bool CNeMoContext::StopFullscreen()
{
    if (!m_RenderContext || !IsRenderFullscreen())
        return false;

    m_RenderContext->StopFullScreen();

    m_Fullscreen = false;
    return true;
}

CKContext *CNeMoContext::GetCKContext() const
{
    return m_CKContext;
}

CKRenderContext *CNeMoContext::GetRenderContext() const
{
    return m_RenderContext;
}

CKDebugContext *CNeMoContext::GetDebugContext() const
{
    return m_DebugContext;
}

CKPluginManager *CNeMoContext::GetPluginManager() const
{
    return m_PluginManager;
}

CKParameterManager *CNeMoContext::GetParameterManager() const
{
    return m_ParameterManager;
}

CKTimeManager *CNeMoContext::GetTimeManager() const
{
    return m_TimeManager;
}

CKMessageManager *CNeMoContext::GetMessageManager() const
{
    return m_MessageManager;
}

CKRenderManager *CNeMoContext::GetRenderManager() const
{
    return m_RenderManager;
}

CKBehaviorManager *CNeMoContext::GetBehaviorManager() const
{
    return m_BehaviorManager;
}

CKAttributeManager *CNeMoContext::GetAttributeManager() const
{
    return m_AttributeManager;
}

CKPathManager *CNeMoContext::GetPathManager() const
{
    return m_PathManager;
}

CKFloorManager *CNeMoContext::GetFloorManager() const
{
    return m_FloorManager;
}

CKSoundManager *CNeMoContext::GetSoundManager() const
{
    return m_SoundManager;
}

CKInputManager *CNeMoContext::GetInputManager() const
{
    return m_InputManager;
}

CKCollisionManager *CNeMoContext::GetCollisionManager() const
{
    return m_CollisionManager;
}

InterfaceManager *CNeMoContext::GetInterfaceManager() const
{
    return m_InterfaceManager;
}

void CNeMoContext::SetInterfaceMode(bool mode, CKUICALLBACKFCT callback, void *data)
{
    XASSERT(m_CKContext);
    m_CKContext->SetInterfaceMode(mode, callback, data);
}

bool CNeMoContext::IsInInterfaceMode()
{
    XASSERT(m_CKContext);
    return m_CKContext->IsInInterfaceMode() == TRUE;
}

int CNeMoContext::GetRenderEnginePluginIdx()
{
    if (!m_RenderEngine)
        return -1;

    const int pluginCount = m_PluginManager->GetPluginCount(CKPLUGIN_RENDERENGINE_DLL);
    for (int i = 0; i < pluginCount; ++i)
    {
        CKPluginEntry *entry = m_PluginManager->GetPluginInfo(CKPLUGIN_RENDERENGINE_DLL, i);
        if (!entry)
            break;

        CKPluginDll *dll = m_PluginManager->GetPluginDllInfo(entry->m_PluginDllIndex);
        if (!dll)
            break;

        char *dllname = dll->m_DllFileName.Str();
        if (!dllname)
            break;

        char filename[512];
        _splitpath(dllname, NULL, NULL, filename, NULL);
        if (!_strnicmp(m_RenderEngine, filename, strlen(filename)))
            return i;
    }

    return -1;
}

bool CNeMoContext::RegisterPlugin(const char *filename)
{
    if (!m_PluginManager)
        return false;
    return m_PluginManager->RegisterPlugin((CKSTRING)filename) == CK_OK;
}

bool CNeMoContext::ParsePlugins(const char *dir)
{
    if (!m_PluginManager)
        return false;
    return m_PluginManager->ParsePlugins((CKSTRING)dir) != 0;
}

void CNeMoContext::AddSoundPath(const char *path)
{
    if (!path)
        return;
    XString str = path;
    m_CKContext->GetPathManager()->AddPath(SOUND_PATH_IDX, str);
}

void CNeMoContext::AddBitmapPath(const char *path)
{
    if (!path)
        return;
    XString str = path;
    m_CKContext->GetPathManager()->AddPath(BITMAP_PATH_IDX, str);
}

void CNeMoContext::AddDataPath(const char *path)
{
    if (!path)
        return;
    XString str = path;
    m_CKContext->GetPathManager()->AddPath(DATA_PATH_IDX, str);
}

CKFile *CNeMoContext::CreateCKFile()
{
    XASSERT(m_CKContext);
    return m_CKContext->CreateCKFile();
}

CKERROR CNeMoContext::DeleteCKFile(CKFile *file)
{
    XASSERT(m_CKContext);
    return m_CKContext->DeleteCKFile(file);
}

CKERROR CNeMoContext::GetFileInfo(const char *filename, CKFileInfo *fileinfo)
{
    XASSERT(m_CKContext);
    return m_CKContext->GetFileInfo((CKSTRING)filename, fileinfo);
}

CKERROR CNeMoContext::Load(const char *filename, CKObjectArray *liste, CK_LOAD_FLAGS loadFlags, CKGUID *readerGuid)
{
    XASSERT(m_CKContext);
    return m_CKContext->Load((CKSTRING)filename, liste, loadFlags, readerGuid);
}

CKERROR CNeMoContext::Save(const char *filename, CKObjectArray *liste, CKDWORD saveFlags, CKDependencies *dependencies, CKGUID *readerGuid)
{
    XASSERT(m_CKContext);
    return m_CKContext->Save((CKSTRING)filename, liste, saveFlags, dependencies, readerGuid);
}

CKObject *CNeMoContext::CreateObject(CK_CLASSID cid, const char *name, CK_OBJECTCREATION_OPTIONS options, CK_CREATIONMODE *res)
{
    XASSERT(m_CKContext);
    return m_CKContext->CreateObject(cid, (CKSTRING)name, options, res);
}

CKObject *CNeMoContext::CopyObject(CKObject *src, CKDependencies *dependencies, const char *appendName, CK_OBJECTCREATION_OPTIONS options)
{
    XASSERT(m_CKContext);
    return m_CKContext->CopyObject(src, dependencies, (CKSTRING)appendName, options);
}

const XObjectArray &CNeMoContext::CopyObjects(const XObjectArray &srcObjects, CKDependencies *dependencies, CK_OBJECTCREATION_OPTIONS options, const char *appendName)
{
    XASSERT(m_CKContext);
    return m_CKContext->CopyObjects(srcObjects, dependencies, options, (CKSTRING)appendName);
}

CKObject *CNeMoContext::GetObject(CK_ID objID)
{
    XASSERT(m_CKContext);
    return m_CKContext->GetObject(objID);
}

int CNeMoContext::GetObjectCount()
{
    XASSERT(m_CKContext);
    return m_CKContext->GetObjectCount();
}

int CNeMoContext::GetObjectSize(CKObject *obj)
{
    XASSERT(m_CKContext);
    return m_CKContext->GetObjectSize(obj);
}

CKERROR CNeMoContext::DestroyObject(CKObject *obj, CKDWORD flags, CKDependencies *depoptions)
{
    XASSERT(m_CKContext);
    return m_CKContext->DestroyObject(obj, flags, depoptions);
}

CKERROR CNeMoContext::DestroyObject(CK_ID id, CKDWORD flags, CKDependencies *depoptions)
{
    XASSERT(m_CKContext);
    return m_CKContext->DestroyObject(id, flags, depoptions);
}

CKERROR CNeMoContext::DestroyObjects(CK_ID *objIds, int count, CKDWORD flags, CKDependencies *depoptions)
{
    XASSERT(m_CKContext);
    return m_CKContext->DestroyObjects(objIds, count, flags, depoptions);
}

CKObject *CNeMoContext::GetObjectByName(const char *name, CKObject *previous)
{
    XASSERT(m_CKContext);
    return m_CKContext->GetObjectByName((CKSTRING)name, previous);
}

CKObject *CNeMoContext::GetObjectByNameAndClass(const char *name, CK_CLASSID cid, CKObject *previous)
{
    XASSERT(m_CKContext);
    return m_CKContext->GetObjectByNameAndClass((CKSTRING)name, cid, previous);
}

CKObject *CNeMoContext::GetObjectByNameAndParentClass(const char *name, CK_CLASSID pcid, CKObject *previous)
{
    XASSERT(m_CKContext);
    return m_CKContext->GetObjectByNameAndClass((CKSTRING)name, pcid, previous);
}

const XObjectPointerArray &CNeMoContext::GetObjectListByType(CK_CLASSID cid, bool derived)
{
    XASSERT(m_CKContext);
    return m_CKContext->GetObjectListByType(cid, derived);
}

CK_ID *CNeMoContext::GetObjectsListByClassID(CK_CLASSID cid)
{
    XASSERT(m_CKContext);
    return m_CKContext->GetObjectsListByClassID(cid);
}

int CNeMoContext::GetObjectsCountByClassID(CK_CLASSID cid)
{
    XASSERT(m_CKContext);
    return m_CKContext->GetObjectsCountByClassID(cid);
}

CKDataArray *CNeMoContext::GetArrayByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKDataArray *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_DATAARRAY);
}

CKGroup *CNeMoContext::GetGroupByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKGroup *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_GROUP);
}

CKMaterial *CNeMoContext::GetMaterialByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKMaterial *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_MATERIAL);
}

CKMesh *CNeMoContext::GetMeshByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKMesh *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_MESH);
}

CK2dEntity *CNeMoContext::Get2dEntityByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CK2dEntity *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_2DENTITY);
}

CK3dEntity *CNeMoContext::Get3dEntityByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CK3dEntity *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_3DENTITY);
}

CK3dObject *CNeMoContext::Get3dObjectByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CK3dObject *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_3DOBJECT);
}

CKCamera *CNeMoContext::GetCameraByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKCamera *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_CAMERA);
}

CKTargetCamera *CNeMoContext::GetTargetCameraByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKTargetCamera *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_TARGETCAMERA);
}

CKLight *CNeMoContext::GetLightByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKLight *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_LIGHT);
}

CKTargetLight *CNeMoContext::GetTargetLightByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKTargetLight *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_TARGETLIGHT);
}

CKSound *CNeMoContext::GetSoundByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKSound *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_SOUND);
}

CKTexture *CNeMoContext::GetTextureByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKTexture *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_TEXTURE);
}

CKBehavior *CNeMoContext::GetScriptByName(const char *name)
{
    XASSERT(m_CKContext);
    return (CKBehavior *)m_CKContext->GetObjectByNameAndClass((CKSTRING)name, CKCID_BEHAVIOR);
}

CKLevel *CNeMoContext::GetCurrentLevel()
{
    XASSERT(m_CKContext);
    return m_CKContext->GetCurrentLevel();
}

CKScene *CNeMoContext::GetCurrentScene()
{
    XASSERT(m_CKContext);
    return m_CKContext->GetCurrentScene();
}

CKMessageType CNeMoContext::AddMessageType(const char *msg)
{
    XASSERT(m_MessageManager);
    return m_MessageManager->AddMessageType((CKSTRING)msg);
}

CKMessageType CNeMoContext::GetMessageByString(const char *msg)
{
    XASSERT(m_MessageManager);
    for (int i = 0; i < m_MessageManager->GetMessageTypeCount(); i++)
        if (!strcmp(m_MessageManager->GetMessageTypeName(i), msg))
            return i;
    return -1;
}

const char *CNeMoContext::GetMessageTypeName(CKMessageType msgType)
{
    XASSERT(m_MessageManager);
    return m_MessageManager->GetMessageTypeName(msgType);
}

CKMessage *CNeMoContext::SendMessageSingle(const char *msg, CKBeObject *dest, CKBeObject *sender)
{
    XASSERT(m_MessageManager);
    CKMessageType msgType = GetMessageByString(msg);
    return m_MessageManager->SendMessageSingle(msgType, dest, sender);
}

CKMessage *CNeMoContext::SendMessageGroup(const char *msg, CKGroup *group, CKBeObject *sender)
{
    XASSERT(m_MessageManager);
    CKMessageType msgType = GetMessageByString(msg);
    return m_MessageManager->SendMessageSingle(msgType, group, sender);
}

CKMessage *CNeMoContext::SendMessageBroadcast(const char *msg, CK_CLASSID id, CKBeObject *sender)
{
    XASSERT(m_MessageManager);
    CKMessageType msgType = GetMessageByString(msg);
    return m_MessageManager->SendMessageBroadcast(msgType, id, sender);
}

void CNeMoContext::SetIC(CKBeObject *obj, bool hierarchy)
{
    GetCurrentScene()->SetObjectInitialValue(obj, CKSaveObjectState(obj));
    if (hierarchy)
    {
        if (CKIsChildClassOf(obj, CKCID_2DENTITY))
        {
            CK2dEntity *entity = (CK2dEntity *)obj;
            for (int i = 0; i < entity->GetChildrenCount(); i++)
                SetIC(entity->GetChild(i), true);
        }
        if (CKIsChildClassOf(obj, CKCID_3DENTITY))
        {
            CK3dEntity *entity = (CK3dEntity *)obj;
            for (int i = 0; i < entity->GetChildrenCount(); i++)
                SetIC(entity->GetChild(i), true);
        }
    }
}

void CNeMoContext::RestoreIC(CKBeObject *obj, bool hierarchy)
{
    CKStateChunk *chunk = GetCurrentScene()->GetObjectInitialValue(obj);
    if (chunk)
        CKReadObjectState(obj, chunk);

    if (hierarchy)
    {
        if (CKIsChildClassOf(obj, CKCID_2DENTITY))
        {
            CK2dEntity *entity = (CK2dEntity *)obj;
            for (int i = 0; i < entity->GetChildrenCount(); i++)
                RestoreIC(entity->GetChild(i), true);
        }
        if (CKIsChildClassOf(obj, CKCID_3DENTITY))
        {
            CK3dEntity *entity = (CK3dEntity *)obj;
            for (int i = 0; i < entity->GetChildrenCount(); i++)
                RestoreIC(entity->GetChild(i), true);
        }
    }
}

void CNeMoContext::Show(CKBeObject *obj, CK_OBJECT_SHOWOPTION show, bool hierarchy)
{
    obj->Show(show);
    if (hierarchy)
    {
        if (CKIsChildClassOf(obj, CKCID_2DENTITY))
        {
            CK2dEntity *entity = (CK2dEntity *)obj;
            for (int i = 0; i < entity->GetChildrenCount(); i++)
                Show(entity->GetChild(i), show, true);
        }
        if (CKIsChildClassOf(obj, CKCID_3DENTITY))
        {
            CK3dEntity *entity = (CK3dEntity *)obj;
            for (int i = 0; i < entity->GetChildrenCount(); i++)
                Show(entity->GetChild(i), show, true);
        }
    }
}

CNeMoContext *CNeMoContext::GetInstance()
{
    return s_Instance;
}

void CNeMoContext::RegisterInstance(CNeMoContext *nemoContext)
{
    s_Instance = nemoContext;
}

void CNeMoContext::ProcessIdle()
{
    // Do nothing
}

void CNeMoContext::ProcessRuntime()
{
    m_CKContext->Process();
}

void CNeMoContext::ProcessDebug()
{
    if (!m_InDebug)
    {
        m_CKContext->ProcessDebugStart(m_TimeManager->GetLastDeltaTime());
        m_InDebug = true;
    }

    for (m_InDebug = (m_CKContext->ProcessDebugStep() == TRUE);
         m_InDebug;
         m_InDebug = (m_CKContext->ProcessDebugStep() == TRUE))
    {
        m_DebugContext = m_CKContext->GetDebugContext();
        ExecuteCallbacks(m_DebugCallbacks);
        m_DebugContext = NULL;
    }

    if (!m_InDebug)
    {
        m_CKContext->ProcessDebugEnd();
    }
}

void CNeMoContext::ExecuteCallbacks(XArray<Callback> &callbacks)
{
    Callback *cb = callbacks.Begin();
    while (cb != callbacks.End())
    {
        ((NEMO_CALLBACK)cb->callback)(this, cb->argument);
        if (cb->temp)
            callbacks.Remove(cb);
        ++cb;
    }
}

void CNeMoContext::ClearCallbacks()
{
    m_PreProcessCallbacks.Clear();
    m_PostProcessCallbacks.Clear();
    m_PreRenderCallbacks.Clear();
    m_PostRenderCallbacks.Clear();
    m_DebugCallbacks.Clear();
}

void CNeMoContext::DebugEnd()
{
    while (m_CKContext->ProcessDebugStep() == TRUE)
        continue;
    m_InDebug = false;
    m_CKContext->ProcessDebugEnd();
}