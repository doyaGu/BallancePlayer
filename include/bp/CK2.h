#ifndef BP_CK2_H
#define BP_CK2_H

#include "Defines.h"
#include "GamePlayer.h"

#ifdef __cplusplus
#include "CKAll.h"
#else
typedef struct CKContext CKContext;
typedef struct CKRenderContext CKRenderContext;
typedef struct CKPluginManager CKPluginManager;
typedef struct CKAttributeManager CKAttributeManager;
typedef struct CKMessageManager CKMessageManager;
typedef struct CKTimeManager CKTimeManager;
typedef struct CKSoundManager CKSoundManager;
typedef struct CKInputManager CKInputManager;
typedef struct CKBehaviorManager CKBehaviorManager;
typedef struct CKFloorManager CKFloorManager;
typedef struct CKCollisionManager CKCollisionManager;
typedef struct CKGridManager CKGridManager;
typedef struct CKRenderManager CKRenderManager;
typedef struct CKParameterManager CKParameterManager;
typedef struct CKPathManager CKPathManager;
#endif

BP_BEGIN_CDECLS

/**
 * @brief Get the CK context of the game player.
 * @param player The game player.
 * @return The CK context.
 */
BP_EXPORT CKContext *bpGetCKContext(BpGamePlayer *player);

/**
 * @brief Get the render context of the game player.
 * @param player The game player.
 * @return The render context.
 */
BP_EXPORT CKRenderContext *bpGetRenderContext(BpGamePlayer *player);

/**
 * @brief Get the plugin manager of the game player.
 * @param player The game player.
 * @return The plugin manager.
 */
BP_EXPORT CKPluginManager *bpGetPluginManager(BpGamePlayer *player);

/**
 * @brief Get the attribute manager of the game player.
 * @param player The game player.
 * @return The attribute manager.
 */
BP_EXPORT CKAttributeManager *bpGetAttributeManager(BpGamePlayer *player);

/**
 * @brief Get the message manager of the game player.
 * @param player The game player.
 * @return The message manager.
 */
BP_EXPORT CKMessageManager *bpGetMessageManager(BpGamePlayer *player);

/**
 * @brief Get the time manager of the game player.
 * @param player The game player.
 * @return The time manager.
 */
BP_EXPORT CKTimeManager *bpGetTimeManager(BpGamePlayer *player);

/**
 * @brief Get the sound manager of the game player.
 * @param player The game player.
 * @return The sound manager.
 */
BP_EXPORT CKSoundManager *bpGetSoundManager(BpGamePlayer *player);

/**
 * @brief Get the input manager of the game player.
 * @param player The game player.
 * @return The input manager.
 */
BP_EXPORT CKInputManager *bpGetInputManager(BpGamePlayer *player);

/**
 * @brief Get the behavior manager of the game player.
 * @param player The game player.
 * @return The behavior manager.
 */
BP_EXPORT CKBehaviorManager *bpGetBehaviorManager(BpGamePlayer *player);

/**
 * @brief Get the floor manager of the game player.
 * @param player The game player.
 * @return The floor manager.
 */
BP_EXPORT CKFloorManager *bpGetFloorManager(BpGamePlayer *player);

/**
 * @brief Get the collision manager of the game player.
 * @param player The game player.
 * @return The collision manager.
 */
BP_EXPORT CKCollisionManager *bpGetCollisionManager(BpGamePlayer *player);

/**
 * @brief Get the grid manager of the game player.
 * @param player The game player.
 * @return The grid manager.
 */
BP_EXPORT CKGridManager *bpGetGridManager(BpGamePlayer *player);

/**
 * @brief Get the render manager of the game player.
 * @param player The game player.
 * @return The render manager.
 */
BP_EXPORT CKRenderManager *bpGetRenderManager(BpGamePlayer *player);

/**
 * @brief Get the parameter manager of the game player.
 * @param player The game player.
 * @return The parameter manager.
 */
BP_EXPORT CKParameterManager *bpGetParameterManager(BpGamePlayer *player);

/**
 * @brief Get the path manager of the game player.
 * @param player The game player.
 * @return The path manager.
 */
BP_EXPORT CKPathManager *bpGetPathManager(BpGamePlayer *player);

BP_END_CDECLS

#endif // BP_CK2_H
