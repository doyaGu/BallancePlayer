#include "GameManager.h"

#include "GamePlayer.h"

GameManager::GameManager(CKContext *context) : CKBaseManager(context, GAME_MANAGER_GUID, (CKSTRING) "Game Manager") {
    m_Player = GamePlayer::Get(context);
    assert(m_Player != nullptr);
    context->RegisterNewManager(this);
}

GameManager::~GameManager() = default;

CKERROR GameManager::PreClearAll() {
    return CK_OK;
}

CKERROR GameManager::PostClearAll() {
    return CK_OK;
}

CKERROR GameManager::PreProcess() {
    return CK_OK;
}

CKERROR GameManager::PostProcess() {
    return CK_OK;
}

CKERROR GameManager::SequenceAddedToScene(CKScene *scn, CK_ID *objids, int count) {
    return CK_OK;
}

CKERROR GameManager::SequenceRemovedFromScene(CKScene *scn, CK_ID *objids, int count) {
    return CK_OK;
}

CKERROR GameManager::PreLaunchScene(CKScene *OldScene, CKScene *NewScene) {
    return CK_OK;
}

CKERROR GameManager::PostLaunchScene(CKScene *OldScene, CKScene *NewScene) {
    return CK_OK;
}

CKERROR GameManager::OnCKInit() {
    return CK_OK;
}

CKERROR GameManager::OnCKEnd() {
    return CK_OK;
}

CKERROR GameManager::OnCKReset() {
    return CK_OK;
}

CKERROR GameManager::OnCKPostReset() {
    return CK_OK;
}

CKERROR GameManager::OnCKPause() {
    return CK_OK;
}

CKERROR GameManager::OnCKPlay() {
    return CK_OK;
}

CKERROR GameManager::SequenceToBeDeleted(CK_ID *objids, int count) {
    return CK_OK;
}

CKERROR GameManager::SequenceDeleted(CK_ID *objids, int count) {
    return CK_OK;
}

CKERROR GameManager::PreLoad() {
    return CK_OK;
}

CKERROR GameManager::PostLoad() {
    return CK_OK;
}

CKERROR GameManager::PreSave() {
    return CK_OK;
}

CKERROR GameManager::PostSave() {
    return CK_OK;
}

CKERROR GameManager::OnPreCopy(CKDependenciesContext &context) {
    return CK_OK;
}

CKERROR GameManager::OnPostCopy(CKDependenciesContext &context) {
    return CK_OK;
}

CKERROR GameManager::OnPreRender(CKRenderContext *dev) {
    return CK_OK;
}

CKERROR GameManager::OnPostRender(CKRenderContext *dev) {
    return CK_OK;
}

CKERROR GameManager::OnPostSpriteRender(CKRenderContext *dev) {
#if BP_ENABLE_IMGUI
    m_Player->RenderImGui();
#endif
    return CK_OK;
}
