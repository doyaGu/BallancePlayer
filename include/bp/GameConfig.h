#ifndef BP_GAMECONFIG_H
#define BP_GAMECONFIG_H

#include "Defines.h"
#include "Configuration.h"

BP_BEGIN_CDECLS

/**
 * @brief The path category.
 */
typedef enum BpPathCategory {
    BP_PATH_CONFIG = 0,
    BP_PATH_LOG,
    BP_PATH_CMO,
    BP_PATH_ROOT,
    BP_PATH_PLUGINS,
    BP_PATH_RENDER_ENGINES,
    BP_PATH_MANAGERS,
    BP_PATH_BUILDING_BLOCKS,
    BP_PATH_SOUNDS,
    BP_PATH_TEXTURES,
    BP_PATH_DATA,
    BP_PATH_SCRIPTS,
    BP_PATH_CATEGORY_COUNT,
} BpPathCategory;

/**
 * @brief The log mode.
 */
typedef enum BpLogMode {
    BP_LOG_MODE_APPEND = 0,
    BP_LOG_MODE_OVERWRITE,
} BpLogMode;

typedef struct BpGameConfig BpGameConfig;

/**
 * @brief Create a new game configuration.
 * @param name The name of the game configuration.
 * @return The game configuration.
 */
BP_EXPORT BpGameConfig *bpCreateGameConfig(const char *name);

/**
 * @brief Destroy the game configuration.
 * @param config The game configuration.
 */
BP_EXPORT void bpDestroyGameConfig(BpGameConfig *config);

/**
 * @brief Get the game configuration.
 * @param config The game configuration.
 * @return The configuration.
 */
BP_EXPORT BpConfig *bpGameConfigGet(const BpGameConfig *config);

/**
 * @brief Check if the game configuration is initialized.
 * @param config The game configuration.
 * @return True if initialized, false otherwise.
 */
BP_EXPORT bool bpGameConfigIsInitialized(const BpGameConfig *config);

/**
 * @brief Initialize the game configuration.
 * @param config The game configuration.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpGameConfigInit(BpGameConfig *config);

/**
 * @brief Release the game configuration.
 * @param config The game configuration.
 */
BP_EXPORT void bpGameConfigRelease(BpGameConfig *config);

/**
 * @brief Read the game configuration from JSON.
 * @param config The game configuration.
 * @param json The JSON string.
 * @param size The size of the JSON string.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpGameConfigRead(BpGameConfig *config, const char *json, size_t size);

/**
 * @brief Write the game configuration to JSON.
 * @param config The game configuration.
 * @return The JSON string.
 */
BP_EXPORT char *bpGameConfigWrite(const BpGameConfig *config);

/**
 * @brief Free the JSON string.
 * @param config The game configuration.
 * @param json The JSON string.
 */
BP_EXPORT void bpGameConfigFree(const BpGameConfig *config, char *json);

/**
 * @brief Load the game configuration from a file.
 * @param config The game configuration.
 * @param filename The filename.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpGameConfigLoad(BpGameConfig *config, const char *filename);

/**
 * @brief Save the game configuration to a file.
 * @param config The game configuration.
 * @param filename The filename.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpGameConfigSave(const BpGameConfig *config, const char *filename);

/**
 * @brief Set the game path.
 * @param config The game configuration.
 * @param category The path category.
 * @param path The path.
 */
BP_EXPORT bool bpSetGamePath(BpGameConfig *config, BpPathCategory category, const char *path);

/**
 * @brief Get the game path.
 * @param config The game configuration.
 * @param category The path category.
 * @return The path.
 */
BP_EXPORT const char *bpGetGamePath(const BpGameConfig *config, BpPathCategory category);

/**
 * @brief Check if the game path exists.
 * @param config The game configuration.
 * @param category The path category.
 * @return True if the path exists, false otherwise.
 */
BP_EXPORT bool bpHasGamePath(const BpGameConfig *config, BpPathCategory category);

/**
 * @brief Reset the game path.
 * @param config The game configuration.
 * @param category The path category.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpResetGamePath(BpGameConfig *config, BpPathCategory category);

BP_END_CDECLS

#endif // BP_GAMECONFIG_H
