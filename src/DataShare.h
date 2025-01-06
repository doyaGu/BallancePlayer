#ifndef BP_DATASHARE_PRIVATE_H
#define BP_DATASHARE_PRIVATE_H

#include "bp/DataShare.h"

/**
 * @brief The interface for data sharing.
 */
struct BpDataShare {
    /**
     * @brief Increase the reference count of the data share object.
     * @return The new reference count.
     */
    virtual int AddRef() const = 0;

    /**
     * @brief Decrease the reference count of the data share object.
     * @return The new reference count.
     */
    virtual int Release() const = 0;

    /**
     * @brief Gets the name of the data share object.
     * @return The name as a null-terminated string.
     */
    virtual const char *GetName() const = 0;

    /**
     * @brief Requests data associated with the specified key.
     * @param key The key associated with the requested data.
     * @param callback The callback function to be called when the data is available.
     * @param userdata A pointer to user-defined data to be passed to the callback function.
     */
    virtual void Request(const char *key, BpDataShareCallback callback, void *userdata) const = 0;

    /**
     * @brief Retrieves data associated with the specified key.
     * @param key The key associated with the data.
     * @param size A pointer to a size_t to store the size of the retrieved data.
     * @return A pointer to the data, or nullptr if not found.
     */
    virtual const void *Get(const char *key, size_t *size = nullptr) const = 0;

    /**
     * @brief Copies data associated with the specified key into the provided buffer.
     * @param key The key associated with the data.
     * @param buf The buffer to copy the data into.
     * @param size The size of the buffer.
     * @return True if the data was copied successfully, false otherwise.
     */
    virtual bool Copy(const char *key, void *buf, size_t size) const = 0;

    /**
     * @brief Sets the data associated with the specified key.
     * @param key The key associated with the data.
     * @param buf A pointer to the data to be set.
     * @param size The size of the data.
     * @return True if the data was set successfully, false otherwise.
     */
    virtual bool Set(const char *key, const void *buf, size_t size) = 0;

    /**
     * @brief Puts the data associated with the specified key. If data already exists, returns false.
     * @param key The key associated with the data.
     * @param buf A pointer to the data to be put.
     * @param size The size of the data.
     * @return True if the data was put successfully, false otherwise.
     */
    virtual bool Put(const char *key, const void *buf, size_t size) = 0;

    /**
     * @brief Removes data associated with the specified key.
     * @param key The key associated with the data to be removed.
     * @return True if the data was removed successfully, false otherwise.
     */
    virtual bool Remove(const char *key) = 0;

    /**
     * @brief Retrieves the user data associated with the specified type.
     * @param type The type of the user data to retrieve.
     * @return A pointer to the user data, or nullptr if not found.
     */
    virtual void *GetUserData(size_t type = 0) const = 0;

    /**
     * @brief Sets the user data associated with the specified type.
     * @param data A pointer to the user data to set.
     * @param type The type of the user data to set.
     * @return A pointer to the previous user data associated with the type, or nullptr if not found.
     */
    virtual void *SetUserData(void *data, size_t type = 0) = 0;

protected:
    virtual ~BpDataShare() = default;
};

#endif // BP_DATASHARE_PRIVATE_H
