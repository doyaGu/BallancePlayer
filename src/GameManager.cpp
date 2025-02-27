#include "GameManager.h"

#include "GamePlayer.h"

GameManager::GameManager(CKContext *context) : CKBaseManager(context, GAME_MANAGER_GUID, (CKSTRING) "Game Manager") {
    m_Player = GamePlayer::Get(context);
    assert(m_Player != nullptr);
    context->RegisterNewManager(this);
}

GameManager::~GameManager() = default;

CKERROR GameManager::OnPostSpriteRender(CKRenderContext *dev) {
#if BP_ENABLE_IMGUI
    m_Player->RenderImGui();
#endif
    return CK_OK;
}
