#ifndef BP_CONFIGURATION_PRIVATE_H
#define BP_CONFIGURATION_PRIVATE_H

#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

#include <yyjson.h>

#include "bp/Configuration.h"

#include "DataBox.h"
#include "RefCount.h"
#include "Variant.h"

struct BpConfigSection;
struct BpConfigList;
struct BpConfigEntry;

struct BpConfig {
    static BpConfig *GetInstance(const std::string &name);

    explicit BpConfig(std::string name);

    BpConfig(const BpConfig &rhs) = delete;
    BpConfig(BpConfig &&rhs) noexcept = delete;

    ~BpConfig();

    BpConfig &operator=(const BpConfig &rhs) = delete;
    BpConfig &operator=(BpConfig &&rhs) noexcept = delete;

    int AddRef() const;
    int Release() const;

    const char *GetName() const { return m_Name.c_str(); }

    size_t GetNumberOfEntries(const char *parent) const;
    size_t GetNumberOfLists(const char *parent) const;
    size_t GetNumberOfSections(const char *parent) const;

    BpConfigEntry *GetEntry(size_t index, const char *parent) const;
    BpConfigList *GetList(size_t index, const char *parent) const;
    BpConfigSection *GetSection(size_t index, const char *parent) const;

    BpConfigEntry *GetEntry(const char *name, const char *parent) const;
    BpConfigList *GetList(const char *name, const char *parent) const;
    BpConfigSection *GetSection(const char *name, const char *parent) const;

    BpConfigEntry *AddEntry(const char *name, const char *parent);
    BpConfigEntry *AddEntryBool(const char *name, bool value, const char *parent);
    BpConfigEntry *AddEntryUint32(const char *name, uint32_t value, const char *parent);
    BpConfigEntry *AddEntryInt32(const char *name, int32_t value, const char *parent);
    BpConfigEntry *AddEntryUint64(const char *name, uint64_t value, const char *parent);
    BpConfigEntry *AddEntryInt64(const char *name, int64_t value, const char *parent);
    BpConfigEntry *AddEntryFloat(const char *name, float value, const char *parent);
    BpConfigEntry *AddEntryDouble(const char *name, double value, const char *parent);
    BpConfigEntry *AddEntryString(const char *name, const char *value, const char *parent);
    BpConfigList *AddList(const char *name, const char *parent);
    BpConfigSection *AddSection(const char *name, const char *parent);

    bool RemoveEntry(const char *name, const char *parent);
    bool RemoveList(const char *name, const char *parent);
    bool RemoveSection(const char *name, const char *parent);

    void Clear();

    bool Read(const char *json, size_t len, bool overwrite);
    char *Write(size_t *len);

    void Free(void *ptr) const;

    void *GetUserData(size_t type) const;
    void *SetUserData(void *data, size_t type);

private:
    void ConvertObjectToSection(yyjson_val *obj, BpConfigSection *section, bool overwrite);
    void ConvertArrayToSection(yyjson_val *arr, BpConfigSection *section, bool overwrite);
    void ConvertArrayToList(yyjson_val *arr, BpConfigList *list, bool overwrite);

    static BpConfigSection *CreateSection(BpConfigSection *root, const char *name);
    static BpConfigSection *GetSection(BpConfigSection *root, const char *name);

    mutable RefCount m_RefCount;
    mutable std::mutex m_RWLock;
    std::string m_Name;
    BpConfigSection *m_Root;
    DataBox m_UserData;

    static std::mutex s_MapMutex;
    static std::unordered_map<std::string, BpConfig *> s_BpConfigs;
};

struct BpConfigSection {
    BpConfigSection(BpConfigSection *parent, const char *name);

    BpConfigSection(const BpConfigSection &rhs) = delete;
    BpConfigSection(BpConfigSection &&rhs) noexcept = delete;

    ~BpConfigSection();

    BpConfigSection &operator=(const BpConfigSection &rhs) = delete;
    BpConfigSection &operator=(BpConfigSection &&rhs) noexcept = delete;

    int AddRef() const;
    int Release() const;

    const char *GetName() const { return m_Name.c_str(); }

    BpConfigSection *GetParent() const { return m_Parent; }
    void SetParent(BpConfigSection *parent) { m_Parent = parent; }

    uint32_t GetFlags() const { return m_Flags; }
    void SetFlags(uint32_t flags) { m_Flags = flags; }

    bool IsReadOnly() const { return (m_Flags & BP_CFG_FLAG_READONLY) != 0; }
    bool IsDynamic() const { return (m_Flags & BP_CFG_FLAG_DYNAMIC) != 0; }

    size_t GetNumberOfEntries() const;
    size_t GetNumberOfLists() const;
    size_t GetNumberOfSections() const;

    BpConfigEntry *GetEntry(size_t index) const;
    BpConfigList *GetList(size_t index) const;
    BpConfigSection *GetSection(size_t index) const;

    BpConfigEntry *GetEntry(const char *name) const;
    BpConfigList *GetList(const char *name) const;
    BpConfigSection *GetSection(const char *name) const;

    BpConfigEntry *AddEntry(const char *name);
    BpConfigEntry *AddEntryBool(const char *name, bool value);
    BpConfigEntry *AddEntryUint32(const char *name, uint32_t value);
    BpConfigEntry *AddEntryInt32(const char *name, int32_t value);
    BpConfigEntry *AddEntryUint64(const char *name, uint64_t value);
    BpConfigEntry *AddEntryInt64(const char *name, int64_t value);
    BpConfigEntry *AddEntryFloat(const char *name, float value);
    BpConfigEntry *AddEntryDouble(const char *name, double value);
    BpConfigEntry *AddEntryString(const char *name, const char *value);
    BpConfigList *AddList(const char *name);
    BpConfigSection *AddSection(const char *name);

    bool RemoveEntry(const char *name);
    bool RemoveList(const char *name);
    bool RemoveSection(const char *name);

    void Clear();

    yyjson_mut_val *ToJsonKey(yyjson_mut_doc *doc);
    yyjson_mut_val *ToJsonObject(yyjson_mut_doc *doc);

    bool AddCallback(BpConfigCallbackType type, BpConfigCallback callback, void *arg);
    bool RemoveCallback(BpConfigCallbackType type, BpConfigCallback callback, void *arg);
    void ClearCallbacks(BpConfigCallbackType type);
    void EnableCallbacks(BpConfigCallbackType type, bool enable);
    void InvokeCallbacks(BpConfigCallbackType type, const BpConfigItem &item) const;

private:
    struct Callback {
        BpConfigCallback callback;
        void *userdata;

        Callback(BpConfigCallback cb, void *data) : callback(cb), userdata(data) {}

        bool operator==(const Callback &rhs) const { return callback == rhs.callback && userdata == rhs.userdata; }
        bool operator!=(const Callback &rhs) const { return !(rhs == *this); }
    };

    mutable RefCount m_RefCount;
    mutable std::mutex m_RWLock;
    BpConfigSection *m_Parent;
    std::string m_Name;
    uint32_t m_Flags = 0;
    std::vector<BpConfigItem> m_Items;
    std::vector<BpConfigEntry *> m_Entries;
    std::vector<BpConfigList *> m_Lists;
    std::vector<BpConfigSection *> m_Sections;
    std::unordered_map<std::string, BpConfigEntry *> m_EntryMap;
    std::unordered_map<std::string, BpConfigList *> m_ListMap;
    std::unordered_map<std::string, BpConfigSection *> m_SectionMap;
    std::vector<Callback> m_Callbacks[BP_CFG_CB_COUNT] = {};
    bool m_CallbacksEnabled[BP_CFG_CB_COUNT] = {false};
};

struct BpConfigList {
    BpConfigList(BpConfigSection *parent, const char *name);

    BpConfigList(const BpConfigList &rhs) = delete;
    BpConfigList(BpConfigList &&rhs) noexcept = delete;

    ~BpConfigList();

    BpConfigList &operator=(const BpConfigList &rhs) = delete;
    BpConfigList &operator=(BpConfigList &&rhs) noexcept = delete;

    int AddRef() const;
    int Release() const;

    const char *GetName() const { return m_Name.c_str(); }

    BpConfigSection *GetParent() const { return m_Parent; }
    void SetParent(BpConfigSection *parent) { m_Parent = parent; }

    uint32_t GetFlags() const { return m_Flags; }
    void SetFlags(uint32_t flags) { m_Flags = flags; }

    bool IsReadOnly() const { return (m_Flags & BP_CFG_FLAG_READONLY) != 0; }
    bool IsDynamic() const { return (m_Flags & BP_CFG_FLAG_DYNAMIC) != 0; }

    size_t GetNumberOfValues() const;

    BpConfigEntryType GetType(size_t index) const;
    size_t GetSize(size_t index) const;

    bool GetBool(size_t index) const;
    uint32_t GetUint32(size_t index) const;
    int32_t GetInt32(size_t index) const;
    uint64_t GetUint64(size_t index) const;
    int64_t GetInt64(size_t index) const;
    float GetFloat(size_t index) const;
    double GetDouble(size_t index) const;
    const char *GetString(size_t index) const;

    void *GetValue(size_t index) { return &m_Values[index].GetValue(); }

    void SetBool(size_t index, bool value);
    void SetUint32(size_t index, uint32_t value);
    void SetInt32(size_t index, int32_t value);
    void SetUint64(size_t index, uint64_t value);
    void SetInt64(size_t index, int64_t value);
    void SetFloat(size_t index, float value);
    void SetDouble(size_t index, double value);
    void SetString(size_t index, const char *value);

    void InsertBool(size_t index, bool value);
    void InsertUint32(size_t index, uint32_t value);
    void InsertInt32(size_t index, int32_t value);
    void InsertUint64(size_t index, uint64_t value);
    void InsertInt64(size_t index, int64_t value);
    void InsertFloat(size_t index, float value);
    void InsertDouble(size_t index, double value);
    void InsertString(size_t index, const char *value);

    void AppendBool(bool value);
    void AppendUint32(uint32_t value);
    void AppendInt32(int32_t value);
    void AppendUint64(uint64_t value);
    void AppendInt64(int64_t value);
    void AppendFloat(float value);
    void AppendDouble(double value);
    void AppendString(const char *value);

    bool Remove(size_t index);

    void Clear();

    void Resize(size_t size);
    void Reserve(size_t size);

    yyjson_mut_val *ToJsonKey(yyjson_mut_doc *doc);
    yyjson_mut_val *ToJsonArray(yyjson_mut_doc *doc);

private:
    mutable RefCount m_RefCount;
    mutable std::mutex m_RWLock;
    BpConfigSection *m_Parent;
    std::string m_Name;
    uint32_t m_Flags = 0;
    std::vector<Variant> m_Values;
};

struct BpConfigEntry {
    BpConfigEntry(BpConfigSection *parent, const char *name);
    BpConfigEntry(BpConfigSection *parent, const char *name, bool value);
    BpConfigEntry(BpConfigSection *parent, const char *name, uint32_t value);
    BpConfigEntry(BpConfigSection *parent, const char *name, int32_t value);
    BpConfigEntry(BpConfigSection *parent, const char *name, uint64_t value);
    BpConfigEntry(BpConfigSection *parent, const char *name, int64_t value);
    BpConfigEntry(BpConfigSection *parent, const char *name, float value);
    BpConfigEntry(BpConfigSection *parent, const char *name, double value);
    BpConfigEntry(BpConfigSection *parent, const char *name, const char *value);

    BpConfigEntry(const BpConfigEntry &rhs) = delete;
    BpConfigEntry(BpConfigEntry &&rhs) noexcept = delete;

    ~BpConfigEntry();

    BpConfigEntry &operator=(const BpConfigEntry &rhs) = delete;
    BpConfigEntry &operator=(BpConfigEntry &&rhs) noexcept = delete;

    int AddRef() const;
    int Release() const;

    const char *GetName() const { return m_Name.c_str(); }

    BpConfigSection *GetParent() const { return m_Parent; }
    void SetParent(BpConfigSection *parent) { m_Parent = parent; }

    uint32_t GetFlags() const { return m_Flags; }
    void SetFlags(uint32_t flags) { m_Flags = flags; }

    bool IsReadOnly() const { return (m_Flags & BP_CFG_FLAG_READONLY) != 0; }
    bool IsDynamic() const { return (m_Flags & BP_CFG_FLAG_DYNAMIC) != 0; }

    BpConfigEntryType GetType() const;
    size_t GetSize() const;

    bool IsNull() const { return m_Value.GetType() == VAR_TYPE_NONE; }

    bool GetBool() const { return m_Value.GetBool(); }
    uint32_t GetUint32() const { return static_cast<uint32_t>(m_Value.GetUint64()); }
    int32_t GetInt32() const { return static_cast<int32_t>(m_Value.GetInt64()); }
    uint64_t GetUint64() const { return m_Value.GetUint64(); }
    int64_t GetInt64() const { return m_Value.GetInt64(); }
    float GetFloat() const { return static_cast<float>(m_Value.GetFloat64()); }
    double GetDouble() const { return m_Value.GetFloat64(); }
    const char *GetString() const { return m_Value.GetString(); }
    size_t GetHash() const { return m_Value.IsString() ? m_Hash : m_Value.GetUint32(); }

    void *GetValue() { return &m_Value.GetValue(); }

    void SetBool(bool value);
    void SetUint32(uint32_t value);
    void SetInt32(int32_t value);
    void SetUint64(uint64_t value);
    void SetInt64(int64_t value);
    void SetFloat(float value);
    void SetDouble(double value);
    void SetString(const char *value);

    void SetDefaultBool(bool value);
    void SetDefaultUint32(uint32_t value);
    void SetDefaultInt32(int32_t value);
    void SetDefaultUint64(uint64_t value);
    void SetDefaultInt64(int64_t value);
    void SetDefaultFloat(float value);
    void SetDefaultDouble(double value);
    void SetDefaultString(const char *value);

    void CopyValue(BpConfigEntry *entry);

    void Clear();

    yyjson_mut_val *ToJsonKey(yyjson_mut_doc *doc);
    yyjson_mut_val *ToJsonValue(yyjson_mut_doc *doc);
    void FromJsonValue(yyjson_val *val);

private:
    void InvokeCallbacks(bool typeChanged, bool valueChanged);

    mutable RefCount m_RefCount;
    mutable std::mutex m_RWLock;
    BpConfigSection *m_Parent;
    std::string m_Name;
    uint32_t m_Flags = 0;
    Variant m_Value;
    size_t m_Hash = 0;
};

#endif // BP_CONFIGURATION_PRIVATE_H