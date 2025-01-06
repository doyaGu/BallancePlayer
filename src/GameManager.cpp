#include "GameManager.h"

#include "GamePlayer.h"

GameManager::GameManager(CKContext *context) : CKBaseManager(context, GAME_MANAGER_GUID, (CKSTRING) "Game Manager") {
    m_Player = GamePlayer::Get(context);
    assert(m_Player != nullptr);
    context->RegisterNewManager(this);
}

GameManager::~GameManager() = default;

CKERROR GameManager::PreClearAll() {
    return CKBaseManager::PreClearAll();
}

CKERROR GameManager::PostClearAll() {
    return CKBaseManager::PostClearAll();
}

CKERROR GameManager::PreProcess() {
    return CKBaseManager::PreProcess();
}

CKERROR GameManager::PostProcess() {
    return CKBaseManager::PostProcess();
}

CKERROR GameManager::SequenceAddedToScene(CKScene *scn, CK_ID *objids, int count) {
    return CKBaseManager::SequenceAddedToScene(scn, objids, count);
}

CKERROR GameManager::SequenceRemovedFromScene(CKScene *scn, CK_ID *objids, int count) {
    return CKBaseManager::SequenceRemovedFromScene(scn, objids, count);
}

CKERROR GameManager::PreLaunchScene(CKScene *OldScene, CKScene *NewScene) {
    return CKBaseManager::PreLaunchScene(OldScene, NewScene);
}

CKERROR GameManager::PostLaunchScene(CKScene *OldScene, CKScene *NewScene) {
    return CKBaseManager::PostLaunchScene(OldScene, NewScene);
}

CKERROR GameManager::OnCKInit() {
    return CKBaseManager::OnCKInit();
}

CKERROR GameManager::OnCKEnd() {
    return CKBaseManager::OnCKEnd();
}

CKERROR GameManager::OnCKReset() {
    return CKBaseManager::OnCKReset();
}

CKERROR GameManager::OnCKPostReset() {
    return CKBaseManager::OnCKPostReset();
}

CKERROR GameManager::OnCKPause() {
    return CKBaseManager::OnCKPause();
}

CKERROR GameManager::OnCKPlay() {
    return CKBaseManager::OnCKPlay();
}

CKERROR GameManager::SequenceToBeDeleted(CK_ID *objids, int count) {
    return CKBaseManager::SequenceToBeDeleted(objids, count);
}

CKERROR GameManager::SequenceDeleted(CK_ID *objids, int count) {
    return CKBaseManager::SequenceDeleted(objids, count);
}

CKERROR GameManager::PreLoad() {
    return CKBaseManager::PreLoad();
}

CKERROR GameManager::PostLoad() {
    return CKBaseManager::PostLoad();
}

CKERROR GameManager::PreSave() {
    return CKBaseManager::PreSave();
}

CKERROR GameManager::PostSave() {
    return CKBaseManager::PostSave();
}

CKERROR GameManager::OnPreCopy(CKDependenciesContext &context) {
    return CKBaseManager::OnPreCopy(context);
}

CKERROR GameManager::OnPostCopy(CKDependenciesContext &context) {
    return CKBaseManager::OnPostCopy(context);
}

CKERROR GameManager::OnPreRender(CKRenderContext *dev) {
    return CKBaseManager::OnPreRender(dev);
}

CKERROR GameManager::OnPostRender(CKRenderContext *dev) {
    return CKBaseManager::OnPostRender(dev);
}

CKERROR GameManager::OnPostSpriteRender(CKRenderContext *dev) {
#if BP_ENABLE_IMGUI
    m_Player->RenderImGui();
#endif
    return CK_OK;
}
