#ifndef PLAYER_NEMOCONTEXT_H
#define PLAYER_NEMOCONTEXT_H

#include "CKAll.h"

class InterfaceManager;
class CNeMoContext;

typedef void (*NEMO_CALLBACK)(CNeMoContext *context, void *argument);

class CNeMoContext
{
public:
    CNeMoContext();
    ~CNeMoContext();

    CKERROR CreateContext(WIN_HANDLE mainWin);
    CKERROR CreateRenderContext(WIN_HANDLE renderWin);

    bool StartUp();
    void Shutdown();
    void Cleanup();

    void Play();
    void Debug();
    void Pause();
    void Reset();

    bool IsPlaying() const;
    bool IsDebugging() const;
    bool IsReseted() const;

    void AddPreProcessCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp = FALSE);
    void RemovePreProcessCallBack(NEMO_CALLBACK func, void *arg);

    void AddPostProcessCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp = FALSE);
    void RemovePostProcessCallBack(NEMO_CALLBACK func, void *arg);

    void AddPreRenderCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp = FALSE);
    void RemovePreRenderCallBack(NEMO_CALLBACK func, void *arg);

    void AddPostRenderCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp = FALSE);
    void RemovePostRenderCallBack(NEMO_CALLBACK func, void *arg);

    void AddDebugCallBack(NEMO_CALLBACK func, void *arg, CKBOOL temp = FALSE);
    void RemoveDebugCallBack(NEMO_CALLBACK func, void *arg);

    void Update();
    void Process();
    CKERROR Render(CK_RENDER_FLAGS flags = CK_RENDER_USECURRENTSETTINGS);

    void ClearScreen();
    void RefreshScreen();

    void SetScreen(int driver, int width, int height, int bpp, int refreshRate = 0);

    void GetResolution(int &width, int &height);

    void SetResolution(int width, int height);

    int GetWidth() const;

    void SetWidth(int width);

    int GetHeight() const;

    void SetHeight(int height);

    int GetBPP() const;

    void SetBPP(int bpp);

    int GetRefreshRate() const;

    void SetRefreshRate(int refreshRate);

    int GetDriver() const;

    void SetDriver(int driver);

    int GetScreenMode() const;

    void SetScreenMode(int screenMode);

    bool FindScreenMode();
    bool ApplyScreenMode();

    bool GoFullscreen();
    bool StopFullscreen();
    bool IsRenderFullscreen() const;

    bool IsFullscreen() const;

    void SetFullscreen(bool fullscreen);

    void SetRenderContext(CKRenderContext *renderContext);

    CKContext *GetCKContext() const;
    CKRenderContext *GetRenderContext() const;
    CKDebugContext *GetDebugContext() const;

    CKPluginManager *GetPluginManager() const;
    CKParameterManager *GetParameterManager() const;
    CKTimeManager *GetTimeManager() const;
    CKMessageManager *GetMessageManager() const;
    CKRenderManager *GetRenderManager() const;
    CKBehaviorManager *GetBehaviorManager() const;
    CKAttributeManager *GetAttributeManager() const;
    CKPathManager *GetPathManager() const;
    CKFloorManager *GetFloorManager() const;
    CKSoundManager *GetSoundManager() const;
    CKInputManager *GetInputManager() const;
    CKCollisionManager *GetCollisionManager() const;
    InterfaceManager *GetInterfaceManager() const;

    void SetInterfaceMode(bool mode = true, CKUICALLBACKFCT callback = NULL, void *data = NULL);
    bool IsInInterfaceMode();

    int GetRenderEnginePluginIdx();
    bool RegisterPlugin(const char *filename);
    bool ParsePlugins(const char *dir);

    void AddSoundPath(const char *path);
    void AddBitmapPath(const char *path);
    void AddDataPath(const char *path);

    CKERROR GetFileInfo(const char *filename, CKFileInfo *fileinfo);
    CKERROR Load(const char *filename, CKObjectArray *liste, CK_LOAD_FLAGS loadFlags = CK_LOAD_DEFAULT, CKGUID *readerGuid = NULL);
    CKERROR Save(const char *filename, CKObjectArray *liste, CKDWORD saveFlags, CKDependencies *dependencies = NULL, CKGUID *readerGuid = NULL);

    CKObject *CreateObject(CK_CLASSID cid, const char *name = NULL, CK_OBJECTCREATION_OPTIONS options = CK_OBJECTCREATION_NONAMECHECK, CK_CREATIONMODE *res = NULL);
    CKObject *CopyObject(CKObject *src, CKDependencies *dependencies = NULL, const char *appendName = NULL, CK_OBJECTCREATION_OPTIONS options = CK_OBJECTCREATION_NONAMECHECK);
    const XObjectArray &CopyObjects(const XObjectArray &srcObjects, CKDependencies *dependencies = NULL, CK_OBJECTCREATION_OPTIONS options = CK_OBJECTCREATION_NONAMECHECK, const char *appendName = NULL);

    CKObject *GetObject(CK_ID objID);
    int GetObjectCount();
    int GetObjectSize(CKObject *obj);

    CKERROR DestroyObject(CKObject *obj, CKDWORD flags = 0, CKDependencies *depoptions = NULL);
    CKERROR DestroyObject(CK_ID id, CKDWORD flags = 0, CKDependencies *depoptions = NULL);
    CKERROR DestroyObjects(CK_ID *objIds, int count, CKDWORD flags = 0, CKDependencies *depoptions = NULL);

    CKObject *GetObjectByName(const char *name, CKObject *previous = NULL);
    CKObject *GetObjectByNameAndClass(const char *name, CK_CLASSID cid, CKObject *previous = NULL);
    CKObject *GetObjectByNameAndParentClass(const char *name, CK_CLASSID pcid, CKObject *previous);
    const XObjectPointerArray &GetObjectListByType(CK_CLASSID cid, bool derived);
    CK_ID *GetObjectsListByClassID(CK_CLASSID cid);
    int GetObjectsCountByClassID(CK_CLASSID cid);

    CKDataArray *GetArrayByName(const char *name);
    CKGroup *GetGroupByName(const char *name);
    CKMaterial *GetMaterialByName(const char *name);
    CKMesh *GetMeshByName(const char *name);
    CK2dEntity *Get2dEntityByName(const char *name);
    CK3dEntity *Get3dEntityByName(const char *name);
    CK3dObject *Get3dObjectByName(const char *name);
    CKCamera *GetCameraByName(const char *name);
    CKTargetCamera *GetTargetCameraByName(const char *name);
    CKLight *GetLightByName(const char *name);
    CKTargetLight *GetTargetLightByName(const char *name);
    CKSound *GetSoundByName(const char *name);
    CKTexture *GetTextureByName(const char *name);
    CKBehavior *GetScriptByName(const char *name);

    CKLevel *GetCurrentLevel();
    CKScene *GetCurrentScene();

    CKMessageType AddMessageType(const char *msg);
    CKMessageType GetMessageByString(const char *msg);
    const char *GetMessageTypeName(CKMessageType msgType);
    CKMessage *SendMessageSingle(const char *msg, CKBeObject *dest, CKBeObject *sender = NULL);
    CKMessage *SendMessageGroup(const char *msg, CKGroup *group, CKBeObject *sender = NULL);
    CKMessage *SendMessageBroadcast(const char *msg, CK_CLASSID id = CKCID_BEOBJECT, CKBeObject *sender = NULL);

    void SetIC(CKBeObject *obj, bool hierarchy = false);
    void RestoreIC(CKBeObject *obj, bool hierarchy = false);
    void Show(CKBeObject *obj, CK_OBJECT_SHOWOPTION show, bool hierarchy = false);

    static CNeMoContext *GetInstance();

    static void RegisterInstance(CNeMoContext *nemoContext);

private:
    typedef void (CNeMoContext::*ProcessFunc)();

    struct Callback
    {
        void *callback;
        void *argument;
        CKBOOL temp;

        bool operator==(const Callback &rhs) const
        {
            return callback == rhs.callback && argument == rhs.argument;
        }

        bool operator!=(const Callback &rhs) const
        {
            return !(rhs == *this);
        }
    };

    void ProcessIdle();
    void ProcessRuntime();
    void ProcessDebug();

    void DebugEnd();

    void ExecuteCallbacks(XArray<Callback> &callbacks);
    void ClearCallbacks();

    CNeMoContext(const CNeMoContext &);
    CNeMoContext &operator=(const CNeMoContext &);

    CKContext *m_CKContext;
    CKRenderContext *m_RenderContext;
    CKDebugContext *m_DebugContext;

    CKPluginManager *m_PluginManager;
    CKParameterManager *m_ParameterManager;
    CKTimeManager *m_TimeManager;
    CKMessageManager *m_MessageManager;
    CKRenderManager *m_RenderManager;
    CKBehaviorManager *m_BehaviorManager;
    CKAttributeManager *m_AttributeManager;
    CKPathManager *m_PathManager;
    CKFloorManager *m_FloorManager;
    CKSoundManager *m_SoundManager;
    CKInputManager *m_InputManager;
    CKCollisionManager *m_CollisionManager;
    InterfaceManager *m_InterfaceManager;

    const char *m_RenderEngine;
    int m_Driver;
    int m_ScreenMode;
    VxDisplayMode m_DisplayMode;
    bool m_Fullscreen;

    bool m_DebugMode;
    bool m_InDebug;

    ProcessFunc m_ProcessFunc;
    XArray<Callback> m_PreProcessCallbacks;
    XArray<Callback> m_PostProcessCallbacks;
    XArray<Callback> m_PreRenderCallbacks;
    XArray<Callback> m_PostRenderCallbacks;
    XArray<Callback> m_DebugCallbacks;

    static CNeMoContext *s_Instance;
};

#endif /* PLAYER_NEMOCONTEXT_H */