#ifndef BP_GAMEPLAYER_H
#define BP_GAMEPLAYER_H

#include "Defines.h"
#include "Logger.h"
#include "GameConfig.h"

BP_BEGIN_CDECLS

typedef struct BpGamePlayer BpGamePlayer;

/**
 * @brief The state of the game player.
 */
typedef enum BpGamePlayerState {
    BP_PLAYER_INITIAL = 0,
    BP_PLAYER_READY,
    BP_PLAYER_PLAYING,
    BP_PLAYER_PAUSED,
    BP_PLAYER_FOCUS_LOST,
} BpGamePlayerState;

/**
 * @brief Create a new game player.
 * @return The game player.
 */
BP_EXPORT BpGamePlayer *bpCreateGamePlayer();

/**
 * @brief Destroy the game player.
 * @param player The game player.
 */
BP_EXPORT void bpDestroyGamePlayer(BpGamePlayer *player);

/**
 * @brief Get the game player by ID.
 * @param id The ID of the game player.
 * @return The game player.
 */
BP_EXPORT BpGamePlayer *bpGetGamePlayerById(int id);

/**
 * @brief Get the ID of the game player.
 * @param player The game player.
 * @return The ID of the game player.
 */
BP_EXPORT int bpGamePlayerGetId(const BpGamePlayer *player);

/**
 * @brief Get the state of the game player.
 * @param player The game player.
 * @return The state of the game player.
 */
BP_EXPORT BpGamePlayerState bpGamePlayerGetState(const BpGamePlayer *player);

/**
 * @brief Get the logger of the game player.
 * @param player The game player.
 * @return The logger of the game player.
 */
BP_EXPORT BpLogger *bpGamePlayerGetLogger(const BpGamePlayer *player);

/**
 * @brief Get the configuration of the game player.
 * @param player The game player.
 * @return The configuration of the game player.
 */
BP_EXPORT BpGameConfig *bpGamePlayerGetConfig(const BpGamePlayer *player);

/**
 * @brief Get the main window of the game player.
 * @param player The game player.
 * @return The main window handle of the game player.
 */
BP_EXPORT void *bpGamePlayerGetMainWindow(const BpGamePlayer *player);

/**
 * @brief Get the render window of the game player.
 * @param player The game player.
 * @return The render window handle of the game player.
 */
BP_EXPORT void *bpGamePlayerGetRenderWindow(const BpGamePlayer *player);

/**
 * @brief Initialize the game.
 * @param player The game player.
 * @param hInstance The instance handle.
 * @param config The game configuration.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpGamePlayerInit(BpGamePlayer *player, void *hInstance, const BpGameConfig *config);

/**
 * @brief Load the game.
 * @param player The game player.
 * @param filename The filename to load.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpGamePlayerLoad(BpGamePlayer *player, const char *filename);

/**
 * @brief Run the game.
 * @param player The game player.
 */
BP_EXPORT void bpGamePlayerRun(BpGamePlayer *player);

/**
 * @brief Update the game.
 * @param player The game player.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpGamePlayerUpdate(BpGamePlayer *player);

/**
 * @brief Process the game.
 * @param player The game player.
 */
BP_EXPORT void bpGamePlayerProcess(BpGamePlayer *player);

/**
 * @brief Render the game.
 * @param player The game player.
 */
BP_EXPORT void bpGamePlayerRender(BpGamePlayer *player);

/**
 * @brief Shutdown the game.
 * @param player The game player.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpGamePlayerShutdown(BpGamePlayer *player);

/**
 * @brief Play the game.
 * @param player The game player.
 */
BP_EXPORT void bpGamePlayerPlay(BpGamePlayer *player);

/**
 * @brief Pause the game.
 * @param player The game player.
 */
BP_EXPORT void bpGamePlayerPause(BpGamePlayer *player);

/**
 * @brief Reset the game.
 * @param player The game player.
 */
BP_EXPORT void bpGamePlayerReset(BpGamePlayer *player);

BP_END_CDECLS

#endif // BP_GAMEPLAYER_H
