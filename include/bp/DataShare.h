#ifndef BP_DATASHARE_H
#define BP_DATASHARE_H

#include "Defines.h"

BP_BEGIN_CDECLS

typedef struct BpDataShare BpDataShare;

/**
 * @brief The callback function for data sharing.
 * @param key The key associated with the data.
 * @param data A pointer to the data.
 * @param size The size of the data.
 * @param userdata A pointer to user-defined data.
 */
typedef void(*BpDataShareCallback)(const char *key, const void *data, size_t size, void *userdata);

/**
 * @brief Get the data share object by name.
 * @param name The name of the data share object.
 * @return The data share object if found, otherwise create a new one.
 */
BP_EXPORT BpDataShare *bpGetDataShare(const char *name);

/**
 * @brief Increase the reference count of the data share object.
 * @param dataShare The data share object.
 * @return The new reference count.
 */
BP_EXPORT int bpDataShareAddRef(const BpDataShare *dataShare);

/**
 * @brief Decrease the reference count of the data share object.
 * @param dataShare The data share object.
 * @return The new reference count.
 */
BP_EXPORT int bpDataShareRelease(const BpDataShare *dataShare);

/**
 * @brief Get the name of the data share object.
 * @param dataShare The data share object.
 * @return The name of the data share object.
 */
BP_EXPORT const char *bpDataShareGetName(const BpDataShare *dataShare);

/**
 * @brief Request data associated with the specified key.
 * @param dataShare The data share object.
 * @param key The key associated with the requested data.
 * @param callback The callback function to be called when the data is available.
 * @param userdata A pointer to user-defined data to be passed to the callback function.
 */
BP_EXPORT void bpDataShareRequest(BpDataShare *dataShare, const char *key, BpDataShareCallback callback, void *userdata);

/**
 * @brief Retrieves data associated with the specified key.
 * @param dataShare The data share object.
 * @param key The key associated with the data.
 * @param size A pointer to a size_t to store the size of the retrieved data.
 * @return A pointer to the data, or nullptr if not found.
 */
BP_EXPORT const void *bpDataShareGet(const BpDataShare *dataShare, const char *key, size_t *size);

/**
 * @brief Copies data associated with the specified key into the provided buffer.
 * @param dataShare The data share object.
 * @param key The key associated with the data.
 * @param buf The buffer to copy the data into.
 * @param size The size of the buffer.
 * @return True if the data was copied successfully, false otherwise.
 */
BP_EXPORT bool bpDataShareCopy(const BpDataShare *dataShare, const char *key, void *buf, size_t size);

/**
 * @brief Sets the data associated with the specified key.
 * @param dataShare The data share object.
 * @param key The key associated with the data.
 * @param buf A pointer to the data to be set.
 * @param size The size of the data.
 * @return True if the data was set successfully, false otherwise.
 */
BP_EXPORT bool bpDataShareSet(BpDataShare *dataShare, const char *key, const void *buf, size_t size);

/**
 * @brief Adds the data associated with the specified key.
 * @param dataShare The data share object.
 * @param key The key associated with the data.
 * @param buf A pointer to the data to be set.
 * @param size The size of the data.
 * @return True if the data was added successfully, false otherwise.
 */
BP_EXPORT bool bpDataSharePut(BpDataShare *dataShare, const char *key, const void *buf, size_t size);

/**
 * @brief Removes the data associated with the specified key.
 * @param dataShare The data share object.
 * @param key The key associated with the data.
 * @return True if the data was removed successfully, false otherwise.
 */
BP_EXPORT bool bpDataShareRemove(BpDataShare *dataShare, const char *key);

/**
 * @brief Retrieves the user data associated with the specified type.
 * @param dataShare The data share object.
 * @param type The type of the user data to retrieve.
 * @return A pointer to the user data, or nullptr if not found.
 */
BP_EXPORT void *bpDataShareGetUserData(const BpDataShare *dataShare, size_t type);

/**
 * @brief Sets the user data associated with the specified type.
 * @param dataShare The data share object.
 * @param data A pointer to the user data to set.
 * @param type The type of the user data.
 * @return A pointer to the previous user data associated with the type, or nullptr if not found.
 */
BP_EXPORT void *bpDataShareSetUserData(BpDataShare *dataShare, void *data, size_t type);

BP_END_CDECLS

#endif // BP_DATASHARE_H
