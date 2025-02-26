#ifndef BP_GAMECONFIG_H
#define BP_GAMECONFIG_H

#include "Defines.h"

#include "Value.h"

BP_BEGIN_CDECLS

/**
 * @brief The game configuration.
 */
typedef enum BpConfigType {
    BP_CONFIG_RESERVED = 0,
    BP_CONFIG_VERBOSE,
    BP_CONFIG_LOG_MODE,
    BP_CONFIG_MANUAL_SETUP,
    BP_CONFIG_LOAD_ALL_MANAGERS,
    BP_CONFIG_LOAD_ALL_BUILDING_BLOCKS,
    BP_CONFIG_LOAD_ALL_PLUGINS,
    BP_CONFIG_DRIVER,
    BP_CONFIG_SCREEN_MODE,
    BP_CONFIG_WIDTH,
    BP_CONFIG_HEIGHT,
    BP_CONFIG_BPP,
    BP_CONFIG_FULLSCREEN,
    BP_CONFIG_DISABLE_PERSPECTIVE_CORRECTION,
    BP_CONFIG_FORCE_LINEAR_FOG,
    BP_CONFIG_FORCE_SOFTWARE,
    BP_CONFIG_DISABLE_FILTER,
    BP_CONFIG_ENSURE_VERTEX_SHADER,
    BP_CONFIG_USE_INDEX_BUFFERS,
    BP_CONFIG_DISABLE_DITHERING,
    BP_CONFIG_ANTIALIAS,
    BP_CONFIG_DISABLE_MIPMAP,
    BP_CONFIG_DISABLE_SPECULAR,
    BP_CONFIG_ENABLE_SCREEN_DUMP,
    BP_CONFIG_ENABLE_DEBUG_MODE,
    BP_CONFIG_VERTEX_CACHE,
    BP_CONFIG_TEXTURE_CACHE_MANAGEMENT,
    BP_CONFIG_SORT_TRANSPARENT_OBJECTS,
    BP_CONFIG_TEXTURE_VIDEO_FORMAT,
    BP_CONFIG_SPRITE_VIDEO_FORMAT,
    BP_CONFIG_CHILD_WINDOW_RENDERING,
    BP_CONFIG_BORDERLESS,
    BP_CONFIG_CLIP_CURSOR,
    BP_CONFIG_ALWAYS_HANDLE_INPUT,
    BP_CONFIG_X,
    BP_CONFIG_Y,
    BP_CONFIG_APPLY_HOTFIX,
    BP_CONFIG_LANG_ID,
    BP_CONFIG_SKIP_OPENING,
    BP_CONFIG_UNLOCK_FRAMERATE,
    BP_CONFIG_UNLOCK_WIDESCREEN,
    BP_CONFIG_UNLOCK_HIGH_RESOLUTION,
    BP_CONFIG_DEBUG,
    BP_CONFIG_ROOKIE,
    BP_CONFIG_COUNT,
} BpConfigType;

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
 * @brief Reset the game configuration.
 * @param config The game configuration.
 */
BP_EXPORT void bpGameConfigReset(BpGameConfig *config);

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
 * @brief Get the game configuration.
 * @param config The game configuration.
 * @param type The configuration type.
 * @return The game configuration.
 */
BP_EXPORT BpValue *bpGameConfigGetValue(BpGameConfig *config, BpConfigType type);

/**
 * @brief Check if the game path exists.
 * @param config The game configuration.
 * @param category The path category.
 * @return True if the path exists, false otherwise.
 */
BP_EXPORT bool bpHasGamePath(const BpGameConfig *config, BpPathCategory category);

/**
 * @brief Get the game path.
 * @param config The game configuration.
 * @param category The path category.
 * @return The path.
 */
BP_EXPORT const char *bpGetGamePath(const BpGameConfig *config, BpPathCategory category);

/**
 * @brief Set the game path.
 * @param config The game configuration.
 * @param category The path category.
 * @param path The path.
 */
BP_EXPORT bool bpSetGamePath(BpGameConfig *config, BpPathCategory category, const char *path);

/**
 * @brief Reset the game path.
 * @param config The game configuration.
 * @param category The path category.
 * @return True if successful, false otherwise.
 */
BP_EXPORT bool bpResetGamePath(BpGameConfig *config, BpPathCategory category);

BP_END_CDECLS

#endif // BP_GAMECONFIG_H
