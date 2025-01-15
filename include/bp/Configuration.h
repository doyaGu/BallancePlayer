#ifndef BP_CONFIGURATION_H
#define BP_CONFIGURATION_H

#include "Defines.h"

BP_BEGIN_CDECLS

typedef struct BpConfig BpConfig;
typedef struct BpConfigSection BpConfigSection;
typedef struct BpConfigList BpConfigList;
typedef struct BpConfigEntry BpConfigEntry;

/**
 * @brief Enumeration of configuration types.
 *
 * This enumeration defines the available types for configuration.
 */
typedef enum BpConfigType {
    BP_CFG_TYPE_NONE, /**< No configuration type. */
    BP_CFG_TYPE_ENTRY, /**< Configuration entry type. */
    BP_CFG_TYPE_LIST, /**< Configuration list type. */
    BP_CFG_TYPE_SECTION, /**< Configuration section type. */
} BpConfigurationType;

/**
 * @brief Enumeration of configuration flags.
 *
 * This enumeration defines the available flags for configuration.
 */
typedef enum BpConfigFlag {
    BP_CFG_FLAG_NONE = 0, /**< No configuration flag. */
    BP_CFG_FLAG_READONLY = 1 << 0, /**< Read-only configuration flag. */
    BP_CFG_FLAG_DYNAMIC = 1 << 1, /**< Dynamic configuration flag. */
} BpConfigFlag;

/**
 * @brief Enumeration of configuration entry types.
 *
 * This enumeration defines the available types for configuration entries.
 */
typedef enum BpConfigEntryType {
    BP_CFG_ENTRY_NONE, /**< No entry type. */
    BP_CFG_ENTRY_BOOL, /**< Boolean entry type. */
    BP_CFG_ENTRY_UINT, /**< Unsigned integer entry type. */
    BP_CFG_ENTRY_INT, /**< Signed integer entry type. */
    BP_CFG_ENTRY_REAL, /**< Real number entry type. */
    BP_CFG_ENTRY_STR, /**< String entry type. */
} BpConfigEntryType;

/**
 * @brief Enumeration of configuration callback types.
 *
 * This enumeration defines the types of configuration callback events.
 */
typedef enum BpConfigCallbackType {
    BP_CFG_CB_ENTRY_ADD, /**< Configuration entry add event type. */
    BP_CFG_CB_ENTRY_REMOVE, /**< Configuration entry remove event type. */
    BP_CFG_CB_ENTRY_TYPE_CHANGE, /**< Configuration entry type change event type. */
    BP_CFG_CB_ENTRY_VALUE_CHANGE, /**< Configuration entry value change event type. */
    BP_CFG_CB_LIST_ADD, /**< Configuration list add event type. */
    BP_CFG_CB_LIST_REMOVE, /**< Configuration list remove event type. */
    BP_CFG_CB_SECTION_ADD, /**< Configuration section add event type. */
    BP_CFG_CB_SECTION_REMOVE, /**< Configuration section remove event type. */
    BP_CFG_CB_COUNT,
} BpConfigCallbackType;

/**
 * @brief Union for configuration data.
 */
typedef union BpConfigData {
    BpConfigEntry *entry;
    BpConfigList *list;
    BpConfigSection *section;
} BpConfigData;

/**
 * @brief Structure for configuration item.
 */
typedef struct BpConfigItem {
    BpConfigData data;
    BpConfigType type;

#ifdef __cplusplus
    BpConfigItem() : data(), type(BP_CFG_TYPE_NONE) {}
    explicit BpConfigItem(BpConfigEntry *entry) : data(), type(BP_CFG_TYPE_ENTRY) { data.entry = entry; }
    explicit BpConfigItem(BpConfigList *list) : data(), type(BP_CFG_TYPE_LIST) { data.list = list; }
    explicit BpConfigItem(BpConfigSection *section) : data(), type(BP_CFG_TYPE_SECTION) { data.section = section; }
#endif
} BpConfigItem;

/**
 * @brief Structure for configuration callback arguments.
 */
typedef struct BpConfigCallbackArgument {
    const BpConfigCallbackType type;
    const BpConfigItem item;
} BpConfigCallbackArgument;

/**
 * @brief Configuration callback function type.
 */
typedef void (*BpConfigCallback)(const BpConfigCallbackArgument *arg, void *userdata);

/**
 * @brief Get the configuration object by name.
 * @param name The name of the configuration.
 * @return The configuration object if found, otherwise create a new one.
 */
BP_EXPORT BpConfig *bpGetConfig(const char *name);

/**
 * @brief Increase the reference count of the configuration.
 * @param config The configuration.
 * @return The new reference count.
 */
BP_EXPORT int bpConfigAddRef(const BpConfig *config);

/**
 * @brief Decrease the reference count of the configuration.
 * @param config The configuration.
 * @return The new reference count.
 */
BP_EXPORT int bpConfigRelease(const BpConfig *config);

/**
 * @brief Get the name of the configuration.
 * @param config The configuration.
 * @return The name of the configuration.
 */
BP_EXPORT const char *bpConfigGetName(const BpConfig *config);

/**
 * @brief Get the number of entries in the configuration.
 * @param config The configuration.
 * @param parent The parent name of the entries.
 * @return The number of entries in the configuration.
 */
BP_EXPORT size_t bpConfigGetNumberOfEntries(const BpConfig *config, const char *parent);

/**
 * @brief Get the number of lists in the configuration.
 * @param config The configuration.
 * @param parent The parent name of the lists.
 * @return The number of lists in the configuration.
 */
BP_EXPORT size_t bpConfigGetNumberOfLists(const BpConfig *config, const char *parent);

/**
 * @brief Get the number of sections in the configuration.
 * @param config The configuration.
 * @param parent The parent name of the sections.
 * @return The number of sections in the configuration.
 */
BP_EXPORT size_t bpConfigGetNumberOfSections(const BpConfig *config, const char *parent);

/**
 * @brief Retrieves the configuration entry at the specified index.
 * @param config The configuration.
 * @param index The index of the entry to retrieve.
 * @param parent The parent name of the entry.
 * @return The configuration entry if found, nullptr otherwise.
 */
BP_EXPORT BpConfigEntry *bpConfigGetEntryByIndex(const BpConfig *config, size_t index, const char *parent);

/**
 * @brief Retrieves the configuration list at the specified index.
 * @param config The configuration.
 * @param index The index of the list to retrieve.
 * @param parent The parent name of the list.
 * @return The configuration list if found, nullptr otherwise.
 */
BP_EXPORT BpConfigList *bpConfigGetListByIndex(const BpConfig *config, size_t index, const char *parent);

/**
 * @brief Retrieves the configuration section at the specified index.
 * @param config The configuration.
 * @param index The index of the section to retrieve.
 * @param parent The parent name of the section.
 * @return The configuration section if found, nullptr otherwise.
 */
BP_EXPORT BpConfigSection *bpConfigGetSectionByIndex(const BpConfig *config, size_t index, const char *parent);

/**
 * @brief Retrieves the configuration entry with the specified name.
 * @param config The configuration.
 * @param name The name of the entry to retrieve.
 * @param parent The parent name of the entry.
 * @return The configuration entry if found, nullptr otherwise.
 */
BP_EXPORT BpConfigEntry *bpConfigGetEntry(const BpConfig *config, const char *name, const char *parent);

/**
 * @brief Retrieves the configuration list with the specified name.
 * @param config The configuration.
 * @param name The name of the list to retrieve.
 * @param parent The parent name of the list.
 * @return The configuration list if found, nullptr otherwise.
 */
BP_EXPORT BpConfigList *bpConfigGetList(const BpConfig *config, const char *name, const char *parent);

/**
 * @brief Retrieves the configuration section with the specified name.
 * @param config The configuration.
 * @param name The name of the section to retrieve.
 * @param parent The parent name of the section.
 * @return The configuration section if found, nullptr otherwise.
 */
BP_EXPORT BpConfigSection *bpConfigGetSection(const BpConfig *config, const char *name, const char *parent);

/**
 * @brief Add an entry to the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param parent The parent name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigAddEntry(BpConfig *config, const char *name, const char *parent);

/**
 * @brief Add a boolean entry to the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @param parent The parent name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigAddEntryBool(BpConfig *config, const char *name, bool value, const char *parent);

/**
 * @brief Add an unsigned integer entry to the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @param parent The parent name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigAddEntryUint32(BpConfig *config, const char *name, uint32_t value, const char *parent);

/**
 * @brief Add a signed integer entry to the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @param parent The parent name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigAddEntryInt32(BpConfig *config, const char *name, int32_t value, const char *parent);

/**
 * @brief Add an unsigned 64-bit integer entry to the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @param parent The parent name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigAddEntryUint64(BpConfig *config, const char *name, uint64_t value, const char *parent);

/**
 * @brief Add a signed 64-bit integer entry to the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @param parent The parent name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigAddEntryInt64(BpConfig *config, const char *name, int64_t value, const char *parent);

/**
 * @brief Add a floating-point entry to the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @param parent The parent name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigAddEntryFloat(BpConfig *config, const char *name, float value, const char *parent);

/**
 * @brief Add a double precision floating-point entry to the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @param parent The parent name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigAddEntryDouble(BpConfig *config, const char *name, double value, const char *parent);

/**
 * @brief Add a string entry to the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @param parent The parent name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigAddEntryString(BpConfig *config, const char *name, const char *value, const char *parent);

/**
 * @brief Add a list to the configuration.
 * @param config The configuration.
 * @param name The name of the list.
 * @param parent The parent name of the list.
 * @return The configuration list.
 */
BP_EXPORT BpConfigList *bpConfigAddList(BpConfig *config, const char *name, const char *parent);

/**
 * @brief Add a section to the configuration.
 * @param config The configuration.
 * @param name The name of the section.
 * @param parent The parent name of the section.
 * @return The configuration section.
 */
BP_EXPORT BpConfigSection *bpConfigAddSection(BpConfig *config, const char *name, const char *parent);

/**
 * @brief Remove an entry from the configuration.
 * @param config The configuration.
 * @param name The name of the entry.
 * @param parent The parent name of the entry.
 * @return True if the entry was removed, false otherwise.
 */
BP_EXPORT bool bpConfigRemoveEntry(BpConfig *config, const char *name, const char *parent);

/**
 * @brief Remove a list from the configuration.
 * @param config The configuration.
 * @param name The name of the list.
 * @param parent The parent name of the list.
 * @return True if the list was removed, false otherwise.
 */
BP_EXPORT bool bpConfigRemoveList(BpConfig *config, const char *name, const char *parent);

/**
 * @brief Remove a section from the configuration.
 * @param config The configuration.
 * @param name The name of the section.
 * @param parent The parent name of the section.
 * @return True if the section was removed, false otherwise.
 */
BP_EXPORT bool bpConfigRemoveSection(BpConfig *config, const char *name, const char *parent);

/**
 * @brief Clear the configuration.
 * @param config The configuration.
 */
BP_EXPORT void bpConfigClear(BpConfig *config);

/**
 * @brief Read the configuration from a JSON string.
 * @param config The configuration.
 * @param json The JSON string.
 * @param size The size of the JSON string.
 * @param overwrite Overwrite the existing configuration.
 * @return True if the read operation is successful, false otherwise.
 */
BP_EXPORT bool bpConfigRead(BpConfig *config, const char *json, size_t size, bool overwrite);

/**
 * @brief Write the configuration to a JSON string.
 * @param config The configuration.
 * @param size The size of the JSON string.
 * @return The JSON string if successful, nullptr otherwise.
 */
BP_EXPORT char *bpConfigWrite(BpConfig *config, size_t *size);

/**
 * @brief Free the memory allocated for the JSON string.
 * @param config The configuration.
 * @param ptr The JSON string.
 */
BP_EXPORT void bpConfigFree(const BpConfig *config, void *ptr);

/**
 * @brief Get the user data associated with the configuration.
 * @param config The configuration.
 * @param type The type of the user data.
 * @return The user data if found, nullptr otherwise.
 */
BP_EXPORT void *bpConfigGetUserData(const BpConfig *config, size_t type);

/**
 * @brief Set the user data associated with the configuration.
 * @param config The configuration.
 * @param data The user data to set.
 * @param type The type of the user data.
 * @return A pointer to the previous user data associated with the type, or nullptr if not found.
 */
BP_EXPORT void *bpConfigSetUserData(BpConfig *config, void *data, size_t type);

/**
 * @brief Increase the reference count of the configuration section.
 * @param section The configuration section.
 * @return The new reference count.
 */
BP_EXPORT int bpConfigSectionAddRef(const BpConfigSection *section);

/**
 * @brief Decrease the reference count of the configuration section.
 * @param section The configuration section.
 * @return The new reference count.
 */
BP_EXPORT int bpConfigSectionRelease(const BpConfigSection *section);

/**
 * @brief Get the name of the configuration section.
 * @param section The configuration section.
 * @return The name of the configuration section.
 */
BP_EXPORT const char *bpConfigSectionGetName(const BpConfigSection *section);

/**
 * @brief Get the parent configuration section of the section.
 * @param section The configuration section.
 * @return The parent configuration section of the section.
 */
BP_EXPORT BpConfigSection *bpConfigSectionGetParent(const BpConfigSection *section);

/**
 * @brief Get the flags of the configuration section.
 * @param section The configuration section.
 * @return The flags of the configuration section.
 */
BP_EXPORT uint32_t bpConfigSectionGetFlags(const BpConfigSection *section);

/**
 * @brief Set the flags of the configuration section.
 * @param section The configuration section.
 * @param flags The flags to set.
 */
BP_EXPORT void bpConfigSectionSetFlags(BpConfigSection *section, uint32_t flags);

/**
 * @brief Get the number of entries in the configuration section.
 * @param section The configuration section.
 * @return The number of entries in the configuration section.
 */
BP_EXPORT size_t bpConfigSectionGetNumberOfEntries(const BpConfigSection *section);

/**
 * @brief Get the number of lists in the configuration section.
 * @param section The configuration section.
 * @return The number of lists in the configuration section.
 */
BP_EXPORT size_t bpConfigSectionGetNumberOfLists(const BpConfigSection *section);

/**
 * @brief Get the number of subsections in the configuration section.
 * @param section The configuration section.
 * @return The number of subsections in the configuration section.
 */
BP_EXPORT size_t bpConfigSectionGetNumberOfSections(const BpConfigSection *section);

/**
 * @brief Retrieves the configuration entry at the specified index.
 * @param section The configuration section.
 * @param index The index of the entry to retrieve.
 * @return The configuration entry if found, nullptr otherwise.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionGetEntryByIndex(const BpConfigSection *section, size_t index);

/**
 * @brief Retrieves the configuration list at the specified index.
 * @param section The configuration section.
 * @param index The index of the list to retrieve.
 * @return The configuration list if found, nullptr otherwise.
 */
BP_EXPORT BpConfigList *bpConfigSectionGetListByIndex(const BpConfigSection *section, size_t index);

/**
 * @brief Retrieves the configuration section at the specified index.
 * @param section The configuration section.
 * @param index The index of the section to retrieve.
 * @return The configuration section if found, nullptr otherwise.
 */
BP_EXPORT BpConfigSection *bpConfigSectionGetSectionByIndex(const BpConfigSection *section, size_t index);

/**
 * @brief Retrieves the configuration entry with the specified name.
 * @param section The configuration section.
 * @param name The name of the entry to retrieve.
 * @return The configuration entry if found, nullptr otherwise.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionGetEntry(const BpConfigSection *section, const char *name);

/**
 * @brief Retrieves the configuration list with the specified name.
 * @param section The configuration section.
 * @param name The name of the list to retrieve.
 * @return The configuration list if found, nullptr otherwise.
 */
BP_EXPORT BpConfigList *bpConfigSectionGetList(const BpConfigSection *section, const char *name);

/**
 * @brief Retrieves the configuration section with the specified name.
 * @param section The configuration section.
 * @param name The name of the section to retrieve.
 * @return The configuration section if found, nullptr otherwise.
 */
BP_EXPORT BpConfigSection *bpConfigSectionGetSection(const BpConfigSection *section, const char *name);

/**
 * @brief Add an entry to the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionAddEntry(BpConfigSection *section, const char *name);

/**
 * @brief Add a boolean entry to the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionAddEntryBool(BpConfigSection *section, const char *name, bool value);

/**
 * @brief Add an unsigned integer entry to the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionAddEntryUint32(BpConfigSection *section, const char *name, uint32_t value);

/**
 * @brief Add a signed integer entry to the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionAddEntryInt32(BpConfigSection *section, const char *name, int32_t value);

/**
 * @brief Add an unsigned 64-bit integer entry to the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionAddEntryUint64(BpConfigSection *section, const char *name, uint64_t value);

/**
 * @brief Add a signed 64-bit integer entry to the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionAddEntryInt64(BpConfigSection *section, const char *name, int64_t value);

/**
 * @brief Add a floating-point entry to the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionAddEntryFloat(BpConfigSection *section, const char *name, float value);

/**
 * @brief Add a double precision floating-point entry to the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionAddEntryDouble(BpConfigSection *section, const char *name, double value);

/**
 * @brief Add a string entry to the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @param value The default value for the entry.
 * @return The configuration entry.
 */
BP_EXPORT BpConfigEntry *bpConfigSectionAddEntryString(BpConfigSection *section, const char *name, const char *value);

/**
 * @brief Add a list to the configuration section.
 * @param section The configuration section.
 * @param name The name of the list.
 * @return The configuration list.
 */
BP_EXPORT BpConfigList *bpConfigSectionAddList(BpConfigSection *section, const char *name);

/**
 * @brief Add a section to the configuration section.
 * @param section The configuration section.
 * @param name The name of the section.
 * @return The configuration section.
 */
BP_EXPORT BpConfigSection *bpConfigSectionAddSection(BpConfigSection *section, const char *name);

/**
 * @brief Remove an entry from the configuration section.
 * @param section The configuration section.
 * @param name The name of the entry.
 * @return True if the entry was removed, false otherwise.
 */
BP_EXPORT bool bpConfigSectionRemoveEntry(BpConfigSection *section, const char *name);

/**
 * @brief Remove a list from the configuration section.
 * @param section The configuration section.
 * @param name The name of the list.
 * @return True if the list was removed, false otherwise.
 */
BP_EXPORT bool bpConfigSectionRemoveList(BpConfigSection *section, const char *name);

/**
 * @brief Remove a section from the configuration section.
 * @param section The configuration section.
 * @param name The name of the section.
 * @return True if the section was removed, false otherwise.
 */
BP_EXPORT bool bpConfigSectionRemoveSection(BpConfigSection *section, const char *name);

/**
 * @brief Clear the configuration section.
 * @param section The configuration section.
 */
BP_EXPORT void bpConfigSectionClear(BpConfigSection *section);

/**
 * @brief Add a callback to the configuration section.
 * @param section The configuration section.
 * @param type The type of the configuration callback.
 * @param callback The callback function.
 * @param userdata The user data to pass to the callback.
 * @return True if the callback is successfully added, false otherwise.
 */
BP_EXPORT bool bpConfigSectionAddCallback(BpConfigSection *section, BpConfigCallbackType type, BpConfigCallback callback, void *userdata);

/**
 * @brief Remove a callback from the configuration section.
 * @param section The configuration section.
 * @param type The type of the configuration callback.
 * @param callback The callback function.
 * @param userdata The user data to pass to the callback.
 * @return True if the callback is successfully removed, false otherwise.
 */
BP_EXPORT bool bpConfigSectionRemoveCallback(BpConfigSection *section, BpConfigCallbackType type, BpConfigCallback callback, void *userdata);

/**
 * @brief Clear all callbacks of the specified type from the configuration section.
 * @param section The configuration section.
 * @param type The type of the configuration callback.
 */
BP_EXPORT void bpConfigSectionClearCallbacks(BpConfigSection *section, BpConfigCallbackType type);

/**
 * @brief Enable or disable callbacks of the specified type in the configuration section.
 * @param section The configuration section.
 * @param type The type of the configuration callback.
 * @param enable True to enable the callbacks, false to disable.
 */
BP_EXPORT void bpConfigSectionEnableCallbacks(BpConfigSection *section, BpConfigCallbackType type, bool enable);

/**
 * @brief Increase the reference count of the configuration list.
 * @param list The configuration list.
 * @return The new reference count.
 */
BP_EXPORT int bpConfigListAddRef(const BpConfigList *list);

/**
 * @brief Decrease the reference count of the configuration list.
 * @param list The configuration list.
 * @return The new reference count.
 */
BP_EXPORT int bpConfigListRelease(const BpConfigList *list);

/**
 * @brief Get the name of the configuration list.
 * @param list The configuration list.
 * @return The name of the configuration list.
 */
BP_EXPORT const char *bpConfigListGetName(const BpConfigList *list);

/**
 * @brief Get the parent configuration section of the list.
 * @param list The configuration list.
 * @return The parent configuration section of the list.
 */
BP_EXPORT BpConfigSection *bpConfigListGetParent(const BpConfigList *list);

/**
 * @brief Get the flags of the configuration list.
 * @param list The configuration list.
 * @return The flags of the configuration list.
 */
BP_EXPORT uint32_t bpConfigListGetFlags(const BpConfigList *list);

/**
 * @brief Set the flags of the configuration list.
 * @param list The configuration list.
 * @param flags The flags to set.
 */
BP_EXPORT void bpConfigListSetFlags(BpConfigList *list, uint32_t flags);

/**
 * @brief Get the number of values in the configuration list.
 * @param list The configuration list.
 * @return The number of values in the configuration list.
 */
BP_EXPORT size_t bpConfigListGetNumberOfValues(const BpConfigList *list);

/**
 * @brief Get the type of the configuration entry.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The type of the configuration entry.
 */
BP_EXPORT BpConfigEntryType bpConfigListGetType(const BpConfigList *list, size_t index);

/**
 * @brief Get the size of the configuration entry.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The size of the configuration entry.
 */
BP_EXPORT size_t bpConfigListGetSize(const BpConfigList *list, size_t index);

/**
 * @brief Get a boolean value from the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The boolean value.
 */
BP_EXPORT bool bpConfigListGetBool(const BpConfigList *list, size_t index);

/**
 * @brief Get an unsigned integer value from the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The unsigned integer value.
 */
BP_EXPORT uint32_t bpConfigListGetUint32(const BpConfigList *list, size_t index);

/**
 * @brief Get a signed integer value from the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The signed integer value.
 */
BP_EXPORT int32_t bpConfigListGetInt32(const BpConfigList *list, size_t index);

/**
 * @brief Get an unsigned 64-bit integer value from the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The unsigned 64-bit integer value.
 */
BP_EXPORT uint64_t bpConfigListGetUint64(const BpConfigList *list, size_t index);

/**
 * @brief Get a signed 64-bit integer value from the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The signed 64-bit integer value.
 */
BP_EXPORT int64_t bpConfigListGetInt64(const BpConfigList *list, size_t index);

/**
 * @brief Get a floating-point value from the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The floating-point value.
 */
BP_EXPORT float bpConfigListGetFloat(const BpConfigList *list, size_t index);

/**
 * @brief Get a double precision floating-point value from the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The double precision floating-point value.
 */
BP_EXPORT double bpConfigListGetDouble(const BpConfigList *list, size_t index);

/**
 * @brief Get a string value from the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @return The string value.
 */
BP_EXPORT const char *bpConfigListGetString(const BpConfigList *list, size_t index);

/**
 * @brief Set a boolean value in the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @param value The boolean value.
 */
BP_EXPORT void bpConfigListSetBool(BpConfigList *list, size_t index, bool value);

/**
 * @brief Set an unsigned integer value in the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @param value The unsigned integer value.
 */
BP_EXPORT void bpConfigListSetUint32(BpConfigList *list, size_t index, uint32_t value);

/**
 * @brief Set a signed integer value in the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @param value The signed integer value.
 */
BP_EXPORT void bpConfigListSetInt32(BpConfigList *list, size_t index, int32_t value);

/**
 * @brief Set an unsigned 64-bit integer value in the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @param value The unsigned 64-bit integer value.
 */
BP_EXPORT void bpConfigListSetUint64(BpConfigList *list, size_t index, uint64_t value);

/**
 * @brief Set a signed 64-bit integer value in the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @param value The signed 64-bit integer value.
 */
BP_EXPORT void bpConfigListSetInt64(BpConfigList *list, size_t index, int64_t value);

/**
 * @brief Set a floating-point value in the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @param value The floating-point value.
 */
BP_EXPORT void bpConfigListSetFloat(BpConfigList *list, size_t index, float value);

/**
 * @brief Set a double precision floating-point value in the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @param value The double precision floating-point value.
 */
BP_EXPORT void bpConfigListSetDouble(BpConfigList *list, size_t index, double value);

/**
 * @brief Set a string value in the configuration list.
 * @param list The configuration list.
 * @param index The index of the value.
 * @param value The string value.
 */
BP_EXPORT void bpConfigListSetString(BpConfigList *list, size_t index, const char *value);

/**
 * @brief Insert a boolean value into the configuration list.
 * @param list The configuration list.
 * @param index The index to insert the value.
 * @param value The boolean value.
 */
BP_EXPORT void bpConfigListInsertBool(BpConfigList *list, size_t index, bool value);

/**
 * @brief Insert an unsigned integer value into the configuration list.
 * @param list The configuration list.
 * @param index The index to insert the value.
 * @param value The unsigned integer value.
 */
BP_EXPORT void bpConfigListInsertUint32(BpConfigList *list, size_t index, uint32_t value);

/**
 * @brief Insert a signed integer value into the configuration list.
 * @param list The configuration list.
 * @param index The index to insert the value.
 * @param value The signed integer value.
 */
BP_EXPORT void bpConfigListInsertInt32(BpConfigList *list, size_t index, int32_t value);

/**
 * @brief Insert an unsigned 64-bit integer value into the configuration list.
 * @param list The configuration list.
 * @param index The index to insert the value.
 * @param value The unsigned 64-bit integer value.
 */
BP_EXPORT void bpConfigListInsertUint64(BpConfigList *list, size_t index, uint64_t value);

/**
 * @brief Insert a signed 64-bit integer value into the configuration list.
 * @param list The configuration list.
 * @param index The index to insert the value.
 * @param value The signed 64-bit integer value.
 */
BP_EXPORT void bpConfigListInsertInt64(BpConfigList *list, size_t index, int64_t value);

/**
 * @brief Insert a floating-point value into the configuration list.
 * @param list The configuration list.
 * @param index The index to insert the value.
 * @param value The floating-point value.
 */
BP_EXPORT void bpConfigListInsertFloat(BpConfigList *list, size_t index, float value);

/**
 * @brief Insert a double precision floating-point value into the configuration list.
 * @param list The configuration list.
 * @param index The index to insert the value.
 * @param value The double precision floating-point value.
 */
BP_EXPORT void bpConfigListInsertDouble(BpConfigList *list, size_t index, double value);

/**
 * @brief Insert a string value into the configuration list.
 * @param list The configuration list.
 * @param index The index to insert the value.
 * @param value The string value.
 */
BP_EXPORT void bpConfigListInsertString(BpConfigList *list, size_t index, const char *value);

/**
 * @brief Append a boolean value to the configuration list.
 * @param list The configuration list.
 * @param value The boolean value.
 */
BP_EXPORT void bpConfigListAppendBool(BpConfigList *list, bool value);

/**
 * @brief Append an unsigned integer value to the configuration list.
 * @param list The configuration list.
 * @param value The unsigned integer value.
 */
BP_EXPORT void bpConfigListAppendUint32(BpConfigList *list, uint32_t value);

/**
 * @brief Append a signed integer value to the configuration list.
 * @param list The configuration list.
 * @param value The signed integer value.
 */
BP_EXPORT void bpConfigListAppendInt32(BpConfigList *list, int32_t value);

/**
 * @brief Append an unsigned 64-bit integer value to the configuration list.
 * @param list The configuration list.
 * @param value The unsigned 64-bit integer value.
 */
BP_EXPORT void bpConfigListAppendUint64(BpConfigList *list, uint64_t value);

/**
 * @brief Append a signed 64-bit integer value to the configuration list.
 * @param list The configuration list.
 * @param value The signed 64-bit integer value.
 */
BP_EXPORT void bpConfigListAppendInt64(BpConfigList *list, int64_t value);

/**
 * @brief Append a floating-point value to the configuration list.
 * @param list The configuration list.
 * @param value The floating-point value.
 */
BP_EXPORT void bpConfigListAppendFloat(BpConfigList *list, float value);

/**
 * @brief Append a double precision floating-point value to the configuration list.
 * @param list The configuration list.
 * @param value The double precision floating-point value.
 */
BP_EXPORT void bpConfigListAppendDouble(BpConfigList *list, double value);

/**
 * @brief Append a string value to the configuration list.
 * @param list The configuration list.
 * @param value The string value.
 */
BP_EXPORT void bpConfigListAppendString(BpConfigList *list, const char *value);

/**
 * @brief Remove a value from the configuration list.
 * @param list The configuration list.
 * @param index The index of the value to remove.
 * @return True if the value was removed, false otherwise.
 */
BP_EXPORT bool bpConfigListRemove(BpConfigList *list, size_t index);

/**
 * @brief Clear the configuration list.
 * @param list The configuration list.
 */
BP_EXPORT void bpConfigListClear(BpConfigList *list);

/**
 * @brief Resize the configuration list.
 * @param list The configuration list.
 * @param size The new size of the list.
 */
BP_EXPORT void bpConfigListResize(BpConfigList *list, size_t size);

/**
 * @brief Reserve memory for the configuration list.
 * @param list The configuration list.
 * @param size The size to reserve.
 */
BP_EXPORT void bpConfigListReserve(BpConfigList *list, size_t size);

/**
 * @brief Increase the reference count of the configuration entry.
 * @param entry The configuration entry.
 * @return The new reference count.
 */
BP_EXPORT int bpConfigEntryAddRef(const BpConfigEntry *entry);

/**
 * @brief Decrease the reference count of the configuration entry.
 * @param entry The configuration entry.
 * @return The new reference count.
 */
BP_EXPORT int bpConfigEntryRelease(const BpConfigEntry *entry);

/**
 * @brief Get the name of the configuration entry.
 * @param entry The configuration entry.
 * @return The name of the configuration entry.
 */
BP_EXPORT const char *bpConfigEntryGetName(const BpConfigEntry *entry);

/**
 * @brief Get the parent configuration section of the entry.
 * @param entry The configuration entry.
 * @return The parent configuration section of the entry.
 */
BP_EXPORT BpConfigSection *bpConfigEntryGetParent(const BpConfigEntry *entry);

/**
 * @brief Get the flags of the configuration entry.
 * @param entry The configuration entry.
 * @return The flags of the configuration entry.
 */
BP_EXPORT uint32_t bpConfigEntryGetFlags(const BpConfigEntry *entry);

/**
 * @brief Set the flags of the configuration entry.
 * @param entry The configuration entry.
 * @param flags The flags to set.
 */
BP_EXPORT void bpConfigEntrySetFlags(BpConfigEntry *entry, uint32_t flags);

/**
 * @brief Get the type of the configuration entry.
 * @param entry The configuration entry.
 * @return The type of the configuration entry.
 */
BP_EXPORT BpConfigEntryType bpConfigEntryGetType(const BpConfigEntry *entry);

/**
 * @brief Get the size of the configuration entry.
 * @param entry The configuration entry.
 * @return The size of the configuration entry.
 */
BP_EXPORT size_t bpConfigEntryGetSize(const BpConfigEntry *entry);

/**
 * @brief Get the boolean value of the configuration entry.
 * @param entry The configuration entry.
 * @return The boolean value of the configuration entry.
 */
BP_EXPORT bool bpConfigEntryGetBool(const BpConfigEntry *entry);

/**
 * @brief Get the unsigned integer value of the configuration entry.
 * @param entry The configuration entry.
 * @return The unsigned integer value of the configuration entry.
 */
BP_EXPORT uint32_t bpConfigEntryGetUint32(const BpConfigEntry *entry);

/**
 * @brief Get the signed integer value of the configuration entry.
 * @param entry The configuration entry.
 * @return The signed integer value of the configuration entry.
 */
BP_EXPORT int32_t bpConfigEntryGetInt32(const BpConfigEntry *entry);

/**
 * @brief Get the unsigned 64-bit integer value of the configuration entry.
 * @param entry The configuration entry.
 * @return The unsigned 64-bit integer value of the configuration entry.
 */
BP_EXPORT uint64_t bpConfigEntryGetUint64(const BpConfigEntry *entry);

/**
 * @brief Get the signed 64-bit integer value of the configuration entry.
 * @param entry The configuration entry.
 * @return The signed 64-bit integer value of the configuration entry.
 */
BP_EXPORT int64_t bpConfigEntryGetInt64(const BpConfigEntry *entry);

/**
 * @brief Get the floating-point value of the configuration entry.
 * @param entry The configuration entry.
 * @return The floating-point value of the configuration entry.
 */
BP_EXPORT float bpConfigEntryGetFloat(const BpConfigEntry *entry);

/**
 * @brief Get the double precision floating-point value of the configuration entry.
 * @param entry The configuration entry.
 * @return The double precision floating-point value of the configuration entry.
 */
BP_EXPORT double bpConfigEntryGetDouble(const BpConfigEntry *entry);

/**
 * @brief Get the string value of the configuration entry.
 * @param entry The configuration entry.
 * @return The string value of the configuration entry.
 */
BP_EXPORT const char *bpConfigEntryGetString(const BpConfigEntry *entry);

/**
 * @brief Get the hash value of the configuration entry.
 * @param entry The configuration entry.
 * @return The hash value of the configuration entry.
 */
BP_EXPORT size_t bpConfigEntryGetHash(const BpConfigEntry *entry);

/**
 * @brief Set a boolean value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The boolean value.
 */
BP_EXPORT void bpConfigEntrySetBool(BpConfigEntry *entry, bool value);

/**
 * @brief Set an unsigned integer value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The unsigned integer value.
 */
BP_EXPORT void bpConfigEntrySetUint32(BpConfigEntry *entry, uint32_t value);

/**
 * @brief Set a signed integer value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The signed integer value.
 */
BP_EXPORT void bpConfigEntrySetInt32(BpConfigEntry *entry, int32_t value);

/**
 * @brief Set an unsigned 64-bit integer value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The unsigned 64-bit integer value.
 */
BP_EXPORT void bpConfigEntrySetUint64(BpConfigEntry *entry, uint64_t value);

/**
 * @brief Set a signed 64-bit integer value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The signed 64-bit integer value.
 */
BP_EXPORT void bpConfigEntrySetInt64(BpConfigEntry *entry, int64_t value);

/**
 * @brief Set a floating-point value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The floating-point value.
 */
BP_EXPORT void bpConfigEntrySetFloat(BpConfigEntry *entry, float value);

/**
 * @brief Set a double precision floating-point value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The double precision floating-point value.
 */
BP_EXPORT void bpConfigEntrySetDouble(BpConfigEntry *entry, double value);

/**
 * @brief Set a string value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The string value.
 */
BP_EXPORT void bpConfigEntrySetString(BpConfigEntry *entry, const char *value);

/**
 * @brief Set the default boolean value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The default boolean value.
 */
BP_EXPORT void bpConfigEntrySetDefaultBool(BpConfigEntry *entry, bool value);

/**
 * @brief Set the default unsigned integer value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The default unsigned integer value.
 */
BP_EXPORT void bpConfigEntrySetDefaultUint32(BpConfigEntry *entry, uint32_t value);

/**
 * @brief Set the default signed integer value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The default signed integer value.
 */
BP_EXPORT void bpConfigEntrySetDefaultInt32(BpConfigEntry *entry, int32_t value);

/**
 * @brief Set the default unsigned 64-bit integer value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The default unsigned 64-bit integer value.
 */
BP_EXPORT void bpConfigEntrySetDefaultUint64(BpConfigEntry *entry, uint64_t value);

/**
 * @brief Set the default signed 64-bit integer value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The default signed 64-bit integer value.
 */
BP_EXPORT void bpConfigEntrySetDefaultInt64(BpConfigEntry *entry, int64_t value);

/**
 * @brief Set the default floating-point value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The default floating-point value.
 */
BP_EXPORT void bpConfigEntrySetDefaultFloat(BpConfigEntry *entry, float value);

/**
 * @brief Set the default double precision floating-point value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The default double precision floating-point value.
 */
BP_EXPORT void bpConfigEntrySetDefaultDouble(BpConfigEntry *entry, double value);

/**
 * @brief Set the default string value in the configuration entry.
 * @param entry The configuration entry.
 * @param value The default string value.
 */
BP_EXPORT void bpConfigEntrySetDefaultString(BpConfigEntry *entry, const char *value);

/**
 * @brief Copy the value of the source configuration entry to the destination configuration entry.
 * @param entry The destination configuration entry.
 * @param src The source configuration entry.
 */
BP_EXPORT void bpConfigEntryCopyValue(BpConfigEntry *entry, BpConfigEntry *src);

/**
 * @brief Clear the configuration entry.
 * @param entry The configuration entry.
 */
BP_EXPORT void bpConfigEntryClear(BpConfigEntry *entry);

BP_END_CDECLS

#endif // BP_CONFIGURATION_H
