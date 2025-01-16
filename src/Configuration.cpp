#include "Configuration.h"

#include <cassert>
#include <utility>

BpConfig *bpGetConfig(const char *name) {
    if (!name)
        name = BP_DEFAULT_NAME;
    return BpConfig::GetInstance(name);
}

int bpConfigAddRef(const BpConfig *config) {
    if (!config)
        return 0;
    return config->AddRef();
}

int bpConfigRelease(const BpConfig *config) {
    if (!config)
        return 0;
    return config->Release();
}

const char *bpConfigGetName(const BpConfig *config) {
    if (!config)
        return nullptr;
    return config->GetName();
}

size_t bpConfigGetNumberOfEntries(const BpConfig *config, const char *parent) {
    if (!config)
        return 0;
    return config->GetNumberOfEntries(parent);
}

size_t bpConfigGetNumberOfLists(const BpConfig *config, const char *parent) {
    if (!config)
        return 0;
    return config->GetNumberOfLists(parent);
}

size_t bpConfigGetNumberOfSections(const BpConfig *config, const char *parent) {
    if (!config)
        return 0;
    return config->GetNumberOfSections(parent);
}

BpConfigEntry *bpConfigGetEntryByIndex(const BpConfig *config, size_t index, const char *parent) {
    if (!config)
        return nullptr;
    return config->GetEntry(index, parent);
}

BpConfigList *bpConfigGetListByIndex(const BpConfig *config, size_t index, const char *parent) {
    if (!config)
        return nullptr;
    return config->GetList(index, parent);
}

BpConfigSection *bpConfigGetSectionByIndex(const BpConfig *config, size_t index, const char *parent) {
    if (!config)
        return nullptr;
    return config->GetSection(index, parent);
}

BpConfigEntry *bpConfigGetEntry(const BpConfig *config, const char *name, const char *parent) {
    if (!config)
        return nullptr;
    return config->GetEntry(name, parent);
}

BpConfigList *bpConfigGetList(const BpConfig *config, const char *name, const char *parent) {
    if (!config)
        return nullptr;
    return config->GetList(name, parent);
}

BpConfigSection *bpConfigGetSection(const BpConfig *config, const char *name, const char *parent) {
    if (!config)
        return nullptr;
    return config->GetSection(name, parent);
}

BpConfigEntry *bpConfigAddEntry(BpConfig *config, const char *name, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddEntry(name, parent);
}

BpConfigEntry *bpConfigAddEntryBool(BpConfig *config, const char *name, bool value, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddEntryBool(name, value, parent);
}

BpConfigEntry *bpConfigAddEntryUint32(BpConfig *config, const char *name, uint32_t value, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddEntryUint32(name, value, parent);
}

BpConfigEntry *bpConfigAddEntryInt32(BpConfig *config, const char *name, int32_t value, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddEntryInt32(name, value, parent);
}

BpConfigEntry *bpConfigAddEntryUint64(BpConfig *config, const char *name, uint64_t value, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddEntryUint64(name, value, parent);
}

BpConfigEntry *bpConfigAddEntryInt64(BpConfig *config, const char *name, int64_t value, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddEntryInt64(name, value, parent);
}

BpConfigEntry *bpConfigAddEntryFloat(BpConfig *config, const char *name, float value, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddEntryFloat(name, value, parent);
}

BpConfigEntry *bpConfigAddEntryDouble(BpConfig *config, const char *name, double value, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddEntryDouble(name, value, parent);
}

BpConfigEntry *bpConfigAddEntryString(BpConfig *config, const char *name, const char *value, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddEntryString(name, value, parent);
}

BpConfigList *bpConfigAddList(BpConfig *config, const char *name, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddList(name, parent);
}

BpConfigSection *bpConfigAddSection(BpConfig *config, const char *name, const char *parent) {
    if (!config)
        return nullptr;
    return config->AddSection(name, parent);
}

bool bpConfigRemoveEntry(BpConfig *config, const char *name, const char *parent) {
    if (!config)
        return false;
    return config->RemoveEntry(name, parent);
}

bool bpConfigRemoveList(BpConfig *config, const char *name, const char *parent) {
    if (!config)
        return false;
    return config->RemoveList(name, parent);
}

bool bpConfigRemoveSection(BpConfig *config, const char *name, const char *parent) {
    if (!config)
        return false;
    return config->RemoveSection(name, parent);
}

void bpConfigClear(BpConfig *config) {
    if (!config)
        return;
    config->Clear();
}

bool bpConfigRead(BpConfig *config, const char *json, size_t size, bool overwrite) {
    if (!config || !json || size == 0)
        return false;
    return config->Read(json, size, overwrite);
}

char *bpConfigWrite(BpConfig *config, size_t *size) {
    if (!config || !size)
        return nullptr;
    return config->Write(size);
}

void bpConfigFree(BpConfig *config, char *ptr) {
    if (!config || !ptr)
        return;
    config->Free(ptr);
}

void *bpConfigGetUserData(const BpConfig *config, size_t type) {
    if (!config)
        return nullptr;
    return config->GetUserData(type);
}

void *bpConfigSetUserData(BpConfig *config, void *data, size_t type) {
    if (!config)
        return nullptr;
    return config->SetUserData(data, type);
}

int bpConfigSectionAddRef(const BpConfigSection *section) {
    if (!section)
        return 0;
    return section->AddRef();
}

int bpConfigSectionRelease(const BpConfigSection *section) {
    if (!section)
        return 0;
    return section->Release();
}

const char *bpConfigSectionGetName(const BpConfigSection *section) {
    if (!section)
        return nullptr;
    return section->GetName();
}

BpConfigSection *bpConfigSectionGetParent(const BpConfigSection *section) {
    if (!section)
        return nullptr;
    return section->GetParent();
}

uint32_t bpConfigSectionGetFlags(const BpConfigSection *section) {
    if (!section)
        return 0;
    return section->GetFlags();
}

void bpConfigSectionSetFlags(BpConfigSection *section, uint32_t flags) {
    if (!section)
        return;
    section->SetFlags(flags);
}

size_t bpConfigSectionGetNumberOfEntries(const BpConfigSection *section) {
    if (!section)
        return 0;
    return section->GetNumberOfEntries();
}

size_t bpConfigSectionGetNumberOfLists(const BpConfigSection *section) {
    if (!section)
        return 0;
    return section->GetNumberOfLists();
}

size_t bpConfigSectionGetNumberOfSections(const BpConfigSection *section) {
    if (!section)
        return 0;
    return section->GetNumberOfSections();
}

BpConfigEntry *bpConfigSectionGetEntryByIndex(const BpConfigSection *section, size_t index) {
    if (!section)
        return nullptr;
    return section->GetEntry(index);
}

BpConfigList *bpConfigSectionGetListByIndex(const BpConfigSection *section, size_t index) {
    if (!section)
        return nullptr;
    return section->GetList(index);
}

BpConfigSection *bpConfigSectionGetSectionByIndex(const BpConfigSection *section, size_t index) {
    if (!section)
        return nullptr;
    return section->GetSection(index);
}

BpConfigEntry *bpConfigSectionGetEntry(const BpConfigSection *section, const char *name) {
    if (!section)
        return nullptr;
    return section->GetEntry(name);
}

BpConfigList *bpConfigSectionGetList(const BpConfigSection *section, const char *name) {
    if (!section)
        return nullptr;
    return section->GetList(name);
}

BpConfigSection *bpConfigSectionGetSection(const BpConfigSection *section, const char *name) {
    if (!section)
        return nullptr;
    return section->GetSection(name);
}

BpConfigEntry *bpConfigSectionAddEntry(BpConfigSection *section, const char *name) {
    if (!section)
        return nullptr;
    return section->AddEntry(name);
}

BpConfigEntry *bpConfigSectionAddEntryBool(BpConfigSection *section, const char *name, bool value) {
    if (!section)
        return nullptr;
    return section->AddEntryBool(name, value);
}

BpConfigEntry *bpConfigSectionAddEntryUint32(BpConfigSection *section, const char *name, uint32_t value) {
    if (!section)
        return nullptr;
    return section->AddEntryUint32(name, value);
}

BpConfigEntry *bpConfigSectionAddEntryInt32(BpConfigSection *section, const char *name, int32_t value) {
    if (!section)
        return nullptr;
    return section->AddEntryInt32(name, value);
}

BpConfigEntry *bpConfigSectionAddEntryUint64(BpConfigSection *section, const char *name, uint64_t value) {
    if (!section)
        return nullptr;
    return section->AddEntryUint64(name, value);
}

BpConfigEntry *bpConfigSectionAddEntryInt64(BpConfigSection *section, const char *name, int64_t value) {
    if (!section)
        return nullptr;
    return section->AddEntryInt64(name, value);
}

BpConfigEntry *bpConfigSectionAddEntryFloat(BpConfigSection *section, const char *name, float value) {
    if (!section)
        return nullptr;
    return section->AddEntryFloat(name, value);
}

BpConfigEntry *bpConfigSectionAddEntryDouble(BpConfigSection *section, const char *name, double value) {
    if (!section)
        return nullptr;
    return section->AddEntryDouble(name, value);
}

BpConfigEntry *bpConfigSectionAddEntryString(BpConfigSection *section, const char *name, const char *value) {
    if (!section)
        return nullptr;
    return section->AddEntryString(name, value);
}

BpConfigList *bpConfigSectionAddList(BpConfigSection *section, const char *name) {
    if (!section)
        return nullptr;
    return section->AddList(name);
}

BpConfigSection *bpConfigSectionAddSection(BpConfigSection *section, const char *name) {
    if (!section)
        return nullptr;
    return section->AddSection(name);
}

bool bpConfigSectionRemoveEntry(BpConfigSection *section, const char *name) {
    if (!section)
        return false;
    return section->RemoveEntry(name);
}

bool bpConfigSectionRemoveList(BpConfigSection *section, const char *name) {
    if (!section)
        return false;
    return section->RemoveList(name);
}

bool bpConfigSectionRemoveSection(BpConfigSection *section, const char *name) {
    if (!section)
        return false;
    return section->RemoveSection(name);
}

void bpConfigSectionClear(BpConfigSection *section) {
    if (!section)
        return;
    section->Clear();
}

bool bpConfigSectionAddCallback(BpConfigSection *section, BpConfigCallbackType type, BpConfigCallback callback, void *data) {
    if (!section || !callback)
        return false;
    return section->AddCallback(type, callback, data);
}

bool bpConfigSectionRemoveCallback(BpConfigSection *section, BpConfigCallbackType type, BpConfigCallback callback, void *data) {
    if (!section || !callback)
        return false;
    return section->RemoveCallback(type, callback, data);
}

void bpConfigSectionClearCallbacks(BpConfigSection *section, BpConfigCallbackType type) {
    if (!section)
        return;
    section->ClearCallbacks(type);
}

void bpConfigSectionEnableCallbacks(BpConfigSection *section, BpConfigCallbackType type, bool enable) {
    if (!section)
        return;
    section->EnableCallbacks(type, enable);
}

int bpConfigListAddRef(const BpConfigList *list) {
    if (!list)
        return 0;
    return list->AddRef();
}

int bpConfigListRelease(const BpConfigList *list) {
    if (!list)
        return 0;
    return list->Release();
}

const char *bpConfigListGetName(const BpConfigList *list) {
    if (!list)
        return nullptr;
    return list->GetName();
}

BpConfigSection *bpConfigListGetParent(const BpConfigList *list) {
    if (!list)
        return nullptr;
    return list->GetParent();
}

uint32_t bpConfigListGetFlags(const BpConfigList *list) {
    if (!list)
        return 0;
    return list->GetFlags();
}

void bpConfigListSetFlags(BpConfigList *list, uint32_t flags) {
    if (!list)
        return;
    list->SetFlags(flags);
}

size_t bpConfigListGetNumberOfValues(const BpConfigList *list) {
    if (!list)
        return 0;
    return list->GetNumberOfValues();
}

BpConfigEntryType bpConfigListGetType(const BpConfigList *list, size_t index) {
    if (!list)
        return BP_CFG_ENTRY_NONE;
    return list->GetType(index);
}

size_t bpConfigListGetSize(const BpConfigList *list, size_t index) {
    if (!list)
        return 0;
    return list->GetSize(index);
}

bool bpConfigListGetBool(const BpConfigList *list, size_t index) {
    if (!list)
        return false;
    return list->GetBool(index);
}

uint32_t bpConfigListGetUint32(const BpConfigList *list, size_t index) {
    if (!list)
        return 0;
    return list->GetUint32(index);
}

int32_t bpConfigListGetInt32(const BpConfigList *list, size_t index) {
    if (!list)
        return 0;
    return list->GetInt32(index);
}

uint64_t bpConfigListGetUint64(const BpConfigList *list, size_t index) {
    if (!list)
        return 0;
    return list->GetUint64(index);
}

int64_t bpConfigListGetInt64(const BpConfigList *list, size_t index) {
    if (!list)
        return 0;
    return list->GetInt64(index);
}

float bpConfigListGetFloat(const BpConfigList *list, size_t index) {
    if (!list)
        return 0.0f;
    return list->GetFloat(index);
}

double bpConfigListGetDouble(const BpConfigList *list, size_t index) {
    if (!list)
        return 0.0;
    return list->GetDouble(index);
}

const char *bpConfigListGetString(const BpConfigList *list, size_t index) {
    if (!list)
        return nullptr;
    return list->GetString(index);
}

void bpConfigListSetBool(BpConfigList *list, size_t index, bool value) {
    if (!list)
        return;
    list->SetBool(index, value);
}

void bpConfigListSetUint32(BpConfigList *list, size_t index, uint32_t value) {
    if (!list)
        return;
    list->SetUint32(index, value);
}

void bpConfigListSetInt32(BpConfigList *list, size_t index, int32_t value) {
    if (!list)
        return;
    list->SetInt32(index, value);
}

void bpConfigListSetUint64(BpConfigList *list, size_t index, uint64_t value) {
    if (!list)
        return;
    list->SetUint64(index, value);
}

void bpConfigListSetInt64(BpConfigList *list, size_t index, int64_t value) {
    if (!list)
        return;
    list->SetInt64(index, value);
}

void bpConfigListSetFloat(BpConfigList *list, size_t index, float value) {
    if (!list)
        return;
    list->SetFloat(index, value);
}

void bpConfigListSetDouble(BpConfigList *list, size_t index, double value) {
    if (!list)
        return;
    list->SetDouble(index, value);
}

void bpConfigListSetString(BpConfigList *list, size_t index, const char *value) {
    if (!list)
        return;
    list->SetString(index, value);
}

void bpConfigListInsertBool(BpConfigList *list, size_t index, bool value) {
    if (!list)
        return;
    list->InsertBool(index, value);
}

void bpConfigListInsertUint32(BpConfigList *list, size_t index, uint32_t value) {
    if (!list)
        return;
    list->InsertUint32(index, value);
}

void bpConfigListInsertInt32(BpConfigList *list, size_t index, int32_t value) {
    if (!list)
        return;
    list->InsertInt32(index, value);
}

void bpConfigListInsertUint64(BpConfigList *list, size_t index, uint64_t value) {
    if (!list)
        return;
    list->InsertUint64(index, value);
}

void bpConfigListInsertInt64(BpConfigList *list, size_t index, int64_t value) {
    if (!list)
        return;
    list->InsertInt64(index, value);
}

void bpConfigListInsertFloat(BpConfigList *list, size_t index, float value) {
    if (!list)
        return;
    list->InsertFloat(index, value);
}

void bpConfigListInsertDouble(BpConfigList *list, size_t index, double value) {
    if (!list)
        return;
    list->InsertDouble(index, value);
}

void bpConfigListInsertString(BpConfigList *list, size_t index, const char *value) {
    if (!list)
        return;
    list->InsertString(index, value);
}

void bpConfigListAppendBool(BpConfigList *list, bool value) {
    if (!list)
        return;
    list->AppendBool(value);
}

void bpConfigListAppendUint32(BpConfigList *list, uint32_t value) {
    if (!list)
        return;
    list->AppendUint32(value);
}

void bpConfigListAppendInt32(BpConfigList *list, int32_t value) {
    if (!list)
        return;
    list->AppendInt32(value);
}

void bpConfigListAppendUint64(BpConfigList *list, uint64_t value) {
    if (!list)
        return;
    list->AppendUint64(value);
}

void bpConfigListAppendInt64(BpConfigList *list, int64_t value) {
    if (!list)
        return;
    list->AppendInt64(value);
}

void bpConfigListAppendFloat(BpConfigList *list, float value) {
    if (!list)
        return;
    list->AppendFloat(value);
}

void bpConfigListAppendDouble(BpConfigList *list, double value) {
    if (!list)
        return;
    list->AppendDouble(value);
}

void bpConfigListAppendString(BpConfigList *list, const char *value) {
    if (!list)
        return;
    list->AppendString(value);
}

bool bpConfigListRemove(BpConfigList *list, size_t index) {
    if (!list)
        return false;
    return list->Remove(index);
}

void bpConfigListClear(BpConfigList *list) {
    if (!list)
        return;
    list->Clear();
}

void bpConfigListResize(BpConfigList *list, size_t size) {
    if (!list)
        return;
    list->Resize(size);
}

void bpConfigListReserve(BpConfigList *list, size_t size) {
    if (!list)
        return;
    list->Reserve(size);
}

int bpConfigEntryAddRef(const BpConfigEntry *entry) {
    if (!entry)
        return 0;
    return entry->AddRef();
}

int bpConfigEntryRelease(const BpConfigEntry *entry) {
    if (!entry)
        return 0;
    return entry->Release();
}

const char *bpConfigEntryGetName(const BpConfigEntry *entry) {
    if (!entry)
        return nullptr;
    return entry->GetName();
}

BpConfigSection *bpConfigEntryGetParent(const BpConfigEntry *entry) {
    if (!entry)
        return nullptr;
    return entry->GetParent();
}

uint32_t bpConfigEntryGetFlags(const BpConfigEntry *entry) {
    if (!entry)
        return 0;
    return entry->GetFlags();
}

void bpConfigEntrySetFlags(BpConfigEntry *entry, uint32_t flags) {
    if (!entry)
        return;
    entry->SetFlags(flags);
}

BpConfigEntryType bpConfigEntryGetType(const BpConfigEntry *entry) {
    if (!entry)
        return BP_CFG_ENTRY_NONE;
    return entry->GetType();
}

size_t bpConfigEntryGetSize(const BpConfigEntry *entry) {
    if (!entry)
        return 0;
    return entry->GetSize();
}

bool bpConfigEntryGetBool(const BpConfigEntry *entry) {
    if (!entry)
        return false;
    return entry->GetBool();
}

uint32_t bpConfigEntryGetUint32(const BpConfigEntry *entry) {
    if (!entry)
        return 0;
    return entry->GetUint32();
}

int32_t bpConfigEntryGetInt32(const BpConfigEntry *entry) {
    if (!entry)
        return 0;
    return entry->GetInt32();
}

uint64_t bpConfigEntryGetUint64(const BpConfigEntry *entry) {
    if (!entry)
        return 0;
    return entry->GetUint64();
}

int64_t bpConfigEntryGetInt64(const BpConfigEntry *entry) {
    if (!entry)
        return 0;
    return entry->GetInt64();
}

float bpConfigEntryGetFloat(const BpConfigEntry *entry) {
    if (!entry)
        return 0.0f;
    return entry->GetFloat();
}

double bpConfigEntryGetDouble(const BpConfigEntry *entry) {
    if (!entry)
        return 0.0;
    return entry->GetDouble();
}

const char *bpConfigEntryGetString(const BpConfigEntry *entry) {
    if (!entry)
        return nullptr;
    return entry->GetString();
}

size_t bpConfigEntryGetHash(const BpConfigEntry *entry) {
    if (!entry)
        return 0;
    return entry->GetHash();
}

void bpConfigEntrySetBool(BpConfigEntry *entry, bool value) {
    if (!entry)
        return;
    entry->SetBool(value);
}

void bpConfigEntrySetUint32(BpConfigEntry *entry, uint32_t value) {
    if (!entry)
        return;
    entry->SetUint32(value);
}

void bpConfigEntrySetInt32(BpConfigEntry *entry, int32_t value) {
    if (!entry)
        return;
    entry->SetInt32(value);
}

void bpConfigEntrySetUint64(BpConfigEntry *entry, uint64_t value) {
    if (!entry)
        return;
    entry->SetUint64(value);
}

void bpConfigEntrySetInt64(BpConfigEntry *entry, int64_t value) {
    if (!entry)
        return;
    entry->SetInt64(value);
}

void bpConfigEntrySetFloat(BpConfigEntry *entry, float value) {
    if (!entry)
        return;
    entry->SetFloat(value);
}

void bpConfigEntrySetDouble(BpConfigEntry *entry, double value) {
    if (!entry)
        return;
    entry->SetDouble(value);
}

void bpConfigEntrySetString(BpConfigEntry *entry, const char *value) {
    if (!entry)
        return;
    entry->SetString(value);
}

void bpConfigEntrySetDefaultBool(BpConfigEntry *entry, bool value) {
    if (!entry)
        return;
    entry->SetDefaultBool(value);
}

void bpConfigEntrySetDefaultUint32(BpConfigEntry *entry, uint32_t value) {
    if (!entry)
        return;
    entry->SetDefaultUint32(value);
}

void bpConfigEntrySetDefaultInt32(BpConfigEntry *entry, int32_t value) {
    if (!entry)
        return;
    entry->SetDefaultInt32(value);
}

void bpConfigEntrySetDefaultUint64(BpConfigEntry *entry, uint64_t value) {
    if (!entry)
        return;
    entry->SetDefaultUint64(value);
}

void bpConfigEntrySetDefaultInt64(BpConfigEntry *entry, int64_t value) {
    if (!entry)
        return;
    entry->SetDefaultInt64(value);
}

void bpConfigEntrySetDefaultFloat(BpConfigEntry *entry, float value) {
    if (!entry)
        return;
    entry->SetDefaultFloat(value);
}

void bpConfigEntrySetDefaultDouble(BpConfigEntry *entry, double value) {
    if (!entry)
        return;
    entry->SetDefaultDouble(value);
}

void bpConfigEntrySetDefaultString(BpConfigEntry *entry, const char *value) {
    if (!entry)
        return;
    entry->SetDefaultString(value);
}

void bpConfigEntryCopyValue(BpConfigEntry *entry, BpConfigEntry *source) {
    if (!entry || !source)
        return;
    entry->CopyValue(source);
}

void bpConfigEntryClear(BpConfigEntry *entry) {
    if (!entry)
        return;
    entry->Clear();
}

std::mutex BpConfig::s_MapMutex;
std::unordered_map<std::string, BpConfig *> BpConfig::s_BpConfigs;

BpConfigEntryType GetValueType(const Variant &value) {
    switch (value.GetType()) {
        case VAR_TYPE_BOOL:
            return BP_CFG_ENTRY_BOOL;
        case VAR_TYPE_NUM:
            switch (value.GetSubtype()) {
                case VAR_SUBTYPE_UINT64:
                case VAR_SUBTYPE_UINT32:
                    return BP_CFG_ENTRY_UINT;
                case VAR_SUBTYPE_INT64:
                case VAR_SUBTYPE_INT32:
                    return BP_CFG_ENTRY_INT;
                case VAR_SUBTYPE_FLOAT64:
                case VAR_SUBTYPE_FLOAT32:
                    return BP_CFG_ENTRY_REAL;
                default:
                    break;
            }
        case VAR_TYPE_STR:
            return BP_CFG_ENTRY_STR;
        default:
            break;
    }
    return BP_CFG_ENTRY_NONE;
}

BpConfig *BpConfig::GetInstance(const std::string &name) {
    auto it = s_BpConfigs.find(name);
    if (it == s_BpConfigs.end()) {
        return new BpConfig(name);
    }
    return it->second;
}

BpConfig::BpConfig(std::string name)
    : m_Name(std::move(name)), m_Root(new BpConfigSection(nullptr, "root")) {
    std::lock_guard<std::mutex> lock{s_MapMutex};
    s_BpConfigs[m_Name] = this;
}

BpConfig::~BpConfig() {
    Clear();
    m_Root->Release();

    std::lock_guard<std::mutex> lock{s_MapMutex};
    s_BpConfigs.erase(m_Name);
}

int BpConfig::AddRef() const {
    return m_RefCount.AddRef();
}

int BpConfig::Release() const {
    int r = m_RefCount.Release();
    if (r == 0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete const_cast<BpConfig *>(this);
    }
    return r;
}

size_t BpConfig::GetNumberOfEntries(const char *parent) const {
    if (!parent)
        return m_Root->GetNumberOfEntries();
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->GetNumberOfEntries();
    return 0;
}

size_t BpConfig::GetNumberOfLists(const char *parent) const {
    if (!parent)
        return m_Root->GetNumberOfLists();
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->GetNumberOfLists();
    return 0;
}

size_t BpConfig::GetNumberOfSections(const char *parent) const {
    if (!parent)
        return m_Root->GetNumberOfSections();
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->GetNumberOfSections();
    return 0;
}

BpConfigEntry *BpConfig::GetEntry(size_t index, const char *parent) const {
    if (!parent)
        return m_Root->GetEntry(index);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->GetEntry(index);
    return nullptr;
}

BpConfigList * BpConfig::GetList(size_t index, const char *parent) const {
    if (!parent)
        return m_Root->GetList(index);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->GetList(index);
    return nullptr;
}

BpConfigSection *BpConfig::GetSection(size_t index, const char *parent) const {
    if (!parent)
        return m_Root->GetSection(index);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->GetSection(index);
    return nullptr;
}

BpConfigEntry *BpConfig::GetEntry(const char *name, const char *parent) const {
    if (!parent)
        return m_Root->GetEntry(name);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->GetEntry(name);
    return nullptr;
}

BpConfigList * BpConfig::GetList(const char *name, const char *parent) const {
    if (!parent)
        return m_Root->GetList(name);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->GetList(name);
    return nullptr;
}

BpConfigSection *BpConfig::GetSection(const char *name, const char *parent) const {
    if (!parent)
        return m_Root->GetSection(name);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->GetSection(name);
    return nullptr;
}

BpConfigEntry *BpConfig::AddEntry(const char *name, const char *parent) {
    if (parent == nullptr)
        return m_Root->AddEntry(name);
    auto *section = m_Root->GetSection(parent);
    if (section || (section = CreateSection(m_Root, parent)))
        return section->AddEntry(name);
    return nullptr;
}

BpConfigEntry *BpConfig::AddEntryBool(const char *name, bool value, const char *parent) {
    if (parent == nullptr)
        return m_Root->AddEntryBool(name, value);
    auto *section = m_Root->GetSection(parent);
    if (section || (section = CreateSection(m_Root, parent)))
        return section->AddEntryBool(name, value);
    return nullptr;
}

BpConfigEntry *BpConfig::AddEntryUint32(const char *name, uint32_t value, const char *parent) {
    if (parent == nullptr)
        return m_Root->AddEntryUint32(name, value);
    auto *section = m_Root->GetSection(parent);
    if (section || (section = CreateSection(m_Root, parent)))
        return section->AddEntryUint32(name, value);
    return nullptr;
}

BpConfigEntry *BpConfig::AddEntryInt32(const char *name, int32_t value, const char *parent) {
    if (parent == nullptr)
        return m_Root->AddEntryInt32(name, value);
    auto *section = m_Root->GetSection(parent);
    if (section || (section = CreateSection(m_Root, parent)))
        return section->AddEntryInt32(name, value);
    return nullptr;
}

BpConfigEntry *BpConfig::AddEntryUint64(const char *name, uint64_t value, const char *parent) {
    if (parent == nullptr)
        return m_Root->AddEntryUint64(name, value);
    auto *section = m_Root->GetSection(parent);
    if (section || (section = CreateSection(m_Root, parent)))
        return section->AddEntryUint64(name, value);
    return nullptr;
}

BpConfigEntry *BpConfig::AddEntryInt64(const char *name, int64_t value, const char *parent) {
    if (parent == nullptr)
        return m_Root->AddEntryInt64(name, value);
    auto *section = m_Root->GetSection(parent);
    if (section || (section = CreateSection(m_Root, parent)))
        return section->AddEntryInt64(name, value);
    return nullptr;
}

BpConfigEntry *BpConfig::AddEntryFloat(const char *name, float value, const char *parent) {
    if (parent == nullptr)
        return m_Root->AddEntryFloat(name, value);
    auto *section = m_Root->GetSection(parent);
    if (section || (section = CreateSection(m_Root, parent)))
        return section->AddEntryFloat(name, value);
    return nullptr;
}

BpConfigEntry *BpConfig::AddEntryDouble(const char *name, double value, const char *parent) {
    if (parent == nullptr)
        return m_Root->AddEntryDouble(name, value);
    auto *section = m_Root->GetSection(parent);
    if (section || (section = CreateSection(m_Root, parent)))
        return section->AddEntryDouble(name, value);
    return nullptr;
}

BpConfigEntry *BpConfig::AddEntryString(const char *name, const char *value, const char *parent) {
    if (parent == nullptr)
        return m_Root->AddEntryString(name, value);
    auto *section = m_Root->GetSection(parent);
    if (section || (section = CreateSection(m_Root, parent)))
        return section->AddEntryString(name, value);
    return nullptr;
}

BpConfigList *BpConfig::AddList(const char *name, const char *parent) {
    if (!parent)
        return m_Root->AddList(name);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->AddList(name);
    return nullptr;
}

BpConfigSection *BpConfig::AddSection(const char *name, const char *parent) {
    if (!parent)
        return m_Root->AddSection(name);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->AddSection(name);
    return nullptr;
}

bool BpConfig::RemoveEntry(const char *name, const char *parent) {
    if (!parent)
        return m_Root->RemoveEntry(name);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->RemoveEntry(name);
    return false;
}

bool BpConfig::RemoveList(const char *name, const char *parent) {
    if (!parent)
        return m_Root->RemoveList(name);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->RemoveList(name);
    return false;
}

bool BpConfig::RemoveSection(const char *name, const char *parent) {
    if (!parent)
        return m_Root->RemoveSection(name);
    auto *section = m_Root->GetSection(parent);
    if (section)
        return section->RemoveSection(name);
    return false;
}

void BpConfig::Clear() {
    m_Root->Clear();
}

bool BpConfig::Read(const char *json, size_t len, bool overwrite) {
    yyjson_read_flag flag = YYJSON_READ_STOP_WHEN_DONE |
                            YYJSON_READ_ALLOW_COMMENTS |
                            YYJSON_READ_ALLOW_INF_AND_NAN;
    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_opts(const_cast<char *>(json), len, flag, nullptr, &err);
    if (!doc)
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    yyjson_val *obj = yyjson_doc_get_root(doc);
    ConvertObjectToSection(obj, m_Root, overwrite);
    yyjson_doc_free(doc);

    return true;
}

char *BpConfig::Write(size_t *len) {
    yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
    if (!doc) {
        *len = 0;
        return nullptr;
    }

    yyjson_mut_val *root = nullptr;
    if (m_Root->GetNumberOfEntries() != 0 || m_Root->GetNumberOfSections() != 0) {
        root = m_Root->ToJsonObject(doc);
    }

    if (!root || yyjson_mut_obj_size(root) == 0) {
        *len = 0;
        return nullptr;
    }
    yyjson_mut_doc_set_root(doc, root);

    yyjson_write_flag flg = YYJSON_WRITE_PRETTY | YYJSON_WRITE_ESCAPE_UNICODE;
    yyjson_write_err err;
    char *json = yyjson_mut_write_opts(doc, flg, nullptr, len, &err);
    return json;
}

void BpConfig::Free(void *ptr) const {
    free(ptr);
}

void *BpConfig::GetUserData(size_t type) const {
    return m_UserData.GetData(type);
}

void *BpConfig::SetUserData(void *data, size_t type) {
    return m_UserData.SetData(data, type);
}

void BpConfig::ConvertObjectToSection(yyjson_val *obj, BpConfigSection *section, bool overwrite) {
    if (!yyjson_is_obj(obj))
        return;

    const bool readonly = section->IsReadOnly();
    if (readonly)
        section->SetFlags(section->GetFlags() & ~BP_CFG_FLAG_READONLY);

    yyjson_val *key;
    yyjson_obj_iter iter;
    yyjson_obj_iter_init(obj, &iter);
    while ((key = yyjson_obj_iter_next(&iter))) {
        yyjson_val *val = yyjson_obj_iter_get_val(key);
        switch (yyjson_get_tag(val)) {
            case YYJSON_TYPE_OBJ | YYJSON_SUBTYPE_NONE:
                ConvertObjectToSection(val, (BpConfigSection *) section->AddSection(yyjson_get_str(key)), overwrite);
                break;
            case YYJSON_TYPE_BOOL | YYJSON_SUBTYPE_TRUE:
            case YYJSON_TYPE_BOOL | YYJSON_SUBTYPE_FALSE: {
                auto *entry = (BpConfigEntry *) section->GetEntry(yyjson_get_str(key));
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntryBool(yyjson_get_str(key), yyjson_get_bool(val));
                }
            }
                break;
            case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT: {
                auto *entry = (BpConfigEntry *) section->GetEntry(yyjson_get_str(key));
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    auto value = yyjson_get_uint(val);
                    if (value <= INT64_MAX)
                        section->AddEntryInt64(yyjson_get_str(key), static_cast<int64_t>(value));
                    else
                        section->AddEntryUint64(yyjson_get_str(key), value);
                }
            }
                break;
            case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT: {
                auto *entry = (BpConfigEntry *) section->GetEntry(yyjson_get_str(key));
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntryInt64(yyjson_get_str(key), yyjson_get_sint(val));
                }
            }
                break;
            case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL: {
                auto *entry = (BpConfigEntry *) section->GetEntry(yyjson_get_str(key));
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntryDouble(yyjson_get_str(key), yyjson_get_real(val));
                }
            }
                break;
            case YYJSON_TYPE_STR | YYJSON_SUBTYPE_NONE: {
                auto *entry = (BpConfigEntry *) section->GetEntry(yyjson_get_str(key));
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntryString(yyjson_get_str(key), yyjson_get_str(val));
                }
            }
                break;
            case YYJSON_TYPE_NULL | YYJSON_SUBTYPE_NONE: {
                auto *entry = (BpConfigEntry *) section->GetEntry(yyjson_get_str(key));
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntry(yyjson_get_str(key));
                }
            }
                break;
            case YYJSON_TYPE_ARR | YYJSON_SUBTYPE_NONE:
                ConvertArrayToList(val, (BpConfigList *) section->AddList(yyjson_get_str(key)), overwrite);
                break;
            default:
                break;
        }
    }

    if (readonly)
        section->SetFlags(section->GetFlags() | BP_CFG_FLAG_READONLY);
}

void BpConfig::ConvertArrayToSection(yyjson_val *arr, BpConfigSection *section, bool overwrite) {
    if (!yyjson_is_arr(arr))
        return;

    const bool readonly = section->IsReadOnly();
    if (readonly)
        section->SetFlags(section->GetFlags() & ~BP_CFG_FLAG_READONLY);

    size_t idx = 0;
    yyjson_val *val;
    yyjson_arr_iter iter = yyjson_arr_iter_with(arr);
    while ((val = yyjson_arr_iter_next(&iter))) {
        char buf[32];
        snprintf(buf, 32, "%u", idx);
        switch (yyjson_get_tag(val)) {
            case YYJSON_TYPE_OBJ | YYJSON_SUBTYPE_NONE:
                ConvertObjectToSection(val, (BpConfigSection *) section->AddSection(buf), overwrite);
                break;
            case YYJSON_TYPE_BOOL | YYJSON_SUBTYPE_TRUE:
            case YYJSON_TYPE_BOOL | YYJSON_SUBTYPE_FALSE: {
                auto *entry = (BpConfigEntry *) section->GetEntry(buf);
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntryBool(buf, yyjson_get_bool(val));
                }
            }
                break;
            case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT: {
                auto *entry = (BpConfigEntry *) section->GetEntry(buf);
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    auto value = yyjson_get_uint(val);
                    if (value <= INT64_MAX)
                        section->AddEntryInt64(buf, static_cast<int64_t>(value));
                    else
                        section->AddEntryUint64(buf, value);
                }
            }
                break;
            case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT: {
                auto *entry = (BpConfigEntry *) section->GetEntry(buf);
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntryInt64(buf, yyjson_get_sint(val));
                }
            }
                break;
            case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL: {
                auto *entry = (BpConfigEntry *) section->GetEntry(buf);
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntryDouble(buf, yyjson_get_real(val));
                }
            }
                break;
            case YYJSON_TYPE_STR | YYJSON_SUBTYPE_NONE: {
                auto *entry = (BpConfigEntry *) section->GetEntry(buf);
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntryString(buf, yyjson_get_str(val));
                }
            }
                break;
            case YYJSON_TYPE_NULL | YYJSON_SUBTYPE_NONE: {
                auto *entry = (BpConfigEntry *) section->GetEntry(buf);
                if (entry) {
                    if (overwrite)
                        entry->FromJsonValue(val);
                } else {
                    section->AddEntry(buf);
                }
            }
                break;
            case YYJSON_TYPE_ARR | YYJSON_SUBTYPE_NONE:
                ConvertArrayToSection(val, (BpConfigSection *) section->AddSection(buf), overwrite);
                break;
            default:
                break;
        }
        ++idx;
    }

    if (readonly)
        section->SetFlags(section->GetFlags() | BP_CFG_FLAG_READONLY);
}

void BpConfig::ConvertArrayToList(yyjson_val *arr, BpConfigList *list, bool overwrite) {
    if (!yyjson_is_arr(arr))
        return;

    const bool readonly = list->IsReadOnly();
    if (readonly)
        list->SetFlags(list->GetFlags() & ~BP_CFG_FLAG_READONLY);

    if (overwrite)
        list->Clear();

    size_t idx = 0;
    yyjson_val *val;
    yyjson_arr_iter iter = yyjson_arr_iter_with(arr);
    while ((val = yyjson_arr_iter_next(&iter))) {
        char buf[32];
        snprintf(buf, 32, "%u", idx);
        switch (yyjson_get_tag(val)) {
                break;
            case YYJSON_TYPE_BOOL | YYJSON_SUBTYPE_TRUE:
            case YYJSON_TYPE_BOOL | YYJSON_SUBTYPE_FALSE: {
                list->AppendBool(yyjson_get_bool(val));
            }
                break;
            case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT: {
                list->AppendUint64(yyjson_get_uint(val));
            }
                break;
            case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT: {
                list->AppendInt64(yyjson_get_sint(val));
            }
                break;
            case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL: {
                list->AppendDouble(yyjson_get_real(val));
            }
                break;
            case YYJSON_TYPE_STR | YYJSON_SUBTYPE_NONE: {
                list->AppendString(yyjson_get_str(val));
            }
                break;
            case YYJSON_TYPE_NULL | YYJSON_SUBTYPE_NONE: {
                list->AppendUint64(0);
            }
                break;
            case YYJSON_TYPE_OBJ | YYJSON_SUBTYPE_NONE: // Skip object
            case YYJSON_TYPE_ARR | YYJSON_SUBTYPE_NONE: // Skip array
            default:
                break;
        }
        ++idx;
    }

    if (readonly)
        list->SetFlags(list->GetFlags() | BP_CFG_FLAG_READONLY);
}

BpConfigSection *BpConfig::CreateSection(BpConfigSection *root, const char *name) {
    return (BpConfigSection *) root->AddSection(name);
}

BpConfigSection *BpConfig::GetSection(BpConfigSection *root, const char *name) {
    return (BpConfigSection *) root->GetSection(name);
}

BpConfigSection::BpConfigSection(BpConfigSection *parent, const char *name) : m_Parent(parent), m_Name(name) {
    assert(name != nullptr);
}

BpConfigSection::~BpConfigSection() {
    Clear();
    if (m_Parent) {
        m_Parent->RemoveSection(m_Name.c_str());
    }
}

int BpConfigSection::AddRef() const {
    return m_RefCount.AddRef();
}

int BpConfigSection::Release() const {
    int r = m_RefCount.Release();
    if (r == 0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete const_cast<BpConfigSection *>(this);
    }
    return r;
}

size_t BpConfigSection::GetNumberOfEntries() const {
    return m_Entries.size();
}

size_t BpConfigSection::GetNumberOfLists() const {
    return m_Lists.size();
}

size_t BpConfigSection::GetNumberOfSections() const {
    return m_Sections.size();
}

BpConfigEntry *BpConfigSection::GetEntry(size_t index) const {
    if (index >= m_Entries.size())
        return nullptr;

    return m_Entries[index];
}

BpConfigList * BpConfigSection::GetList(size_t index) const {
    if (index >= m_Lists.size())
        return nullptr;

    return m_Lists[index];
}

BpConfigSection *BpConfigSection::GetSection(size_t index) const {
    if (index >= m_Sections.size())
        return nullptr;

    return m_Sections[index];
}

BpConfigEntry *BpConfigSection::GetEntry(const char *name) const {
    if (!name)
        return nullptr;

    auto it = m_EntryMap.find(name);
    if (it == m_EntryMap.end())
        return nullptr;
    return it->second;
}

BpConfigList * BpConfigSection::GetList(const char *name) const {
    if (!name)
        return nullptr;

    auto it = m_ListMap.find(name);
    if (it == m_ListMap.end())
        return nullptr;
    return it->second;
}

BpConfigSection *BpConfigSection::GetSection(const char *name) const {
    if (!name)
        return nullptr;

    auto it = m_SectionMap.find(name);
    if (it == m_SectionMap.end())
        return nullptr;
    return it->second;
}

BpConfigEntry *BpConfigSection::AddEntry(const char *name) {
    if (!name)
        return nullptr;

    auto *entry = (BpConfigEntry *) GetEntry(name);
    if (entry)
        return entry;

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    entry = new BpConfigEntry(this, name);
    m_Items.emplace_back(entry);
    m_Entries.emplace_back(entry);
    m_EntryMap[entry->GetName()] = entry;

    InvokeCallbacks(BP_CFG_CB_ENTRY_ADD, BpConfigItem(entry));
    return entry;
}

BpConfigEntry *BpConfigSection::AddEntryBool(const char *name, bool value) {
    if (!name)
        return nullptr;

    auto *entry = (BpConfigEntry *) GetEntry(name);
    if (entry) {
        entry->SetDefaultBool(value);
        return entry;
    }

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    entry = new BpConfigEntry(this, name, value);
    m_Items.emplace_back(entry);
    m_Entries.emplace_back(entry);
    m_EntryMap[entry->GetName()] = entry;

    InvokeCallbacks(BP_CFG_CB_ENTRY_ADD, BpConfigItem(entry));
    return entry;
}

BpConfigEntry *BpConfigSection::AddEntryUint32(const char *name, uint32_t value) {
    if (!name)
        return nullptr;

    auto *entry = (BpConfigEntry *) GetEntry(name);
    if (entry) {
        entry->SetDefaultUint32(value);
        return entry;
    }

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    entry = new BpConfigEntry(this, name, value);
    m_Items.emplace_back(entry);
    m_Entries.emplace_back(entry);
    m_EntryMap[entry->GetName()] = entry;

    InvokeCallbacks(BP_CFG_CB_ENTRY_ADD, BpConfigItem(entry));
    return entry;
}

BpConfigEntry *BpConfigSection::AddEntryInt32(const char *name, int32_t value) {
    if (!name)
        return nullptr;

    auto *entry = (BpConfigEntry *) GetEntry(name);
    if (entry) {
        entry->SetDefaultInt32(value);
        return entry;
    }

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    entry = new BpConfigEntry(this, name, value);
    m_Items.emplace_back(entry);
    m_Entries.emplace_back(entry);
    m_EntryMap[entry->GetName()] = entry;

    InvokeCallbacks(BP_CFG_CB_ENTRY_ADD, BpConfigItem(entry));
    return entry;
}

BpConfigEntry *BpConfigSection::AddEntryUint64(const char *name, uint64_t value) {
    if (!name)
        return nullptr;

    auto *entry = (BpConfigEntry *) GetEntry(name);
    if (entry) {
        entry->SetDefaultUint64(value);
        return entry;
    }

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    entry = new BpConfigEntry(this, name, value);
    m_Items.emplace_back(entry);
    m_Entries.emplace_back(entry);
    m_EntryMap[entry->GetName()] = entry;

    InvokeCallbacks(BP_CFG_CB_ENTRY_ADD, BpConfigItem(entry));
    return entry;
}

BpConfigEntry *BpConfigSection::AddEntryInt64(const char *name, int64_t value) {
    if (!name)
        return nullptr;

    auto *entry = (BpConfigEntry *) GetEntry(name);
    if (entry) {
        entry->SetDefaultInt64(value);
        return entry;
    }

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    entry = new BpConfigEntry(this, name, value);
    m_Items.emplace_back(entry);
    m_Entries.emplace_back(entry);
    m_EntryMap[entry->GetName()] = entry;

    InvokeCallbacks(BP_CFG_CB_ENTRY_ADD, BpConfigItem(entry));
    return entry;
}

BpConfigEntry *BpConfigSection::AddEntryFloat(const char *name, float value) {
    if (!name)
        return nullptr;

    auto *entry = (BpConfigEntry *) GetEntry(name);
    if (entry) {
        entry->SetDefaultFloat(value);
        return entry;
    }

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    entry = new BpConfigEntry(this, name, value);
    m_Items.emplace_back(entry);
    m_Entries.emplace_back(entry);
    m_EntryMap[entry->GetName()] = entry;

    InvokeCallbacks(BP_CFG_CB_ENTRY_ADD, BpConfigItem(entry));
    return entry;
}

BpConfigEntry *BpConfigSection::AddEntryDouble(const char *name, double value) {
    if (!name)
        return nullptr;

    auto *entry = (BpConfigEntry *) GetEntry(name);
    if (entry) {
        entry->SetDefaultDouble(value);
        return entry;
    }

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    entry = new BpConfigEntry(this, name, value);
    m_Items.emplace_back(entry);
    m_Entries.emplace_back(entry);
    m_EntryMap[entry->GetName()] = entry;

    InvokeCallbacks(BP_CFG_CB_ENTRY_ADD, BpConfigItem(entry));
    return entry;
}

BpConfigEntry *BpConfigSection::AddEntryString(const char *name, const char *value) {
    if (!name)
        return nullptr;

    if (!value)
        value = "";

    auto *entry = (BpConfigEntry *) GetEntry(name);
    if (entry) {
        entry->SetDefaultString(value);
        return entry;
    }

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    entry = new BpConfigEntry(this, name, value);
    m_Items.emplace_back(entry);
    m_Entries.emplace_back(entry);
    m_EntryMap[entry->GetName()] = entry;

    InvokeCallbacks(BP_CFG_CB_ENTRY_ADD, BpConfigItem(entry));
    return entry;
}

BpConfigList * BpConfigSection::AddList(const char *name) {
    if (!name)
        return nullptr;

    auto *list = (BpConfigList *) GetList(name);
    if (list)
        return list;

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    list = new BpConfigList(this, name);
    m_Items.emplace_back(list);
    m_Lists.emplace_back(list);
    m_ListMap[list->GetName()] = list;

    InvokeCallbacks(BP_CFG_CB_LIST_ADD, BpConfigItem(list));
    return list;
}

BpConfigSection *BpConfigSection::AddSection(const char *name) {
    if (!name)
        return nullptr;

    auto *section = (BpConfigSection *) GetSection(name);
    if (section)
        return section;

    if (IsReadOnly())
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    section = new BpConfigSection(this, name);
    m_Items.emplace_back(section);
    m_Sections.emplace_back(section);
    m_SectionMap[section->GetName()] = section;

    InvokeCallbacks(BP_CFG_CB_SECTION_ADD, BpConfigItem(section));
    return section;
}

bool BpConfigSection::RemoveEntry(const char *name) {
    if (!name)
        return false;

    if (IsReadOnly())
        return false;

    auto it = m_EntryMap.find(name);
    if (it == m_EntryMap.end())
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto *entry = it->second;
    auto del = std::remove_if(m_Items.begin(), m_Items.end(),
                              [entry](const BpConfigItem &e) { return e.data.entry == entry; });
    m_Items.erase(del, m_Items.end());
    m_Entries.erase(std::remove(m_Entries.begin(), m_Entries.end(), entry), m_Entries.end());
    m_EntryMap.erase(it);

    InvokeCallbacks(BP_CFG_CB_ENTRY_REMOVE, BpConfigItem(entry));

    if (entry->Release() != 0) {
        entry->SetParent(nullptr);
    }
    return true;
}

bool BpConfigSection::RemoveList(const char *name) {
    if (!name)
        return false;

    if (IsReadOnly())
        return false;

    auto it = m_ListMap.find(name);
    if (it == m_ListMap.end())
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto *list = it->second;
    auto del = std::remove_if(m_Items.begin(), m_Items.end(),
                              [list](const BpConfigItem &e) { return e.data.list == list; });
    m_Items.erase(del, m_Items.end());
    m_Lists.erase(std::remove(m_Lists.begin(), m_Lists.end(), list), m_Lists.end());
    m_ListMap.erase(it);

    if (list->Release() != 0) {
        list->SetParent(nullptr);
    }

    InvokeCallbacks(BP_CFG_CB_LIST_REMOVE, BpConfigItem(list));
    return true;
}

bool BpConfigSection::RemoveSection(const char *name) {
    if (!name)
        return false;

    if (IsReadOnly())
        return false;

    auto it = m_SectionMap.find(name);
    if (it == m_SectionMap.end())
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto *section = it->second;
    auto del = std::remove_if(m_Items.begin(), m_Items.end(),
                              [section](const BpConfigItem &e) { return e.data.section == section; });
    m_Items.erase(del, m_Items.end());
    m_Sections.erase(std::remove(m_Sections.begin(), m_Sections.end(), section), m_Sections.end());
    m_SectionMap.erase(it);

    if (section->Release() != 0) {
        section->SetParent(nullptr);
    }

    InvokeCallbacks(BP_CFG_CB_SECTION_REMOVE, BpConfigItem(section));
    return true;
}

void BpConfigSection::Clear() {
    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    for (auto &pair: m_EntryMap) {
        auto *entry = pair.second;
        if (entry->Release() != 0) {
            entry->SetParent(nullptr);
        }
    }
    m_EntryMap.clear();
    m_Entries.clear();

    for (auto &pair: m_SectionMap) {
        auto *section = pair.second;
        if (section->Release() != 0) {
            section->SetParent(nullptr);
        }
    }
    m_SectionMap.clear();
    m_Sections.clear();

    m_Items.clear();
}

yyjson_mut_val *BpConfigSection::ToJsonKey(yyjson_mut_doc *doc) {
    if (!doc)
        return nullptr;
    return yyjson_mut_str(doc, m_Name.c_str());
}

yyjson_mut_val *BpConfigSection::ToJsonObject(yyjson_mut_doc *doc) {
    if (!doc)
        return nullptr;

    yyjson_mut_val *obj = yyjson_mut_obj(doc);
    if (!obj)
        return nullptr;

    for (auto &e: m_Items) {
        switch (e.type) {
            case BP_CFG_TYPE_ENTRY: {
                auto *entry = (BpConfigEntry *) e.data.entry;
                if (entry && !entry->IsDynamic()) {
                    yyjson_mut_val *key = entry->ToJsonKey(doc);
                    yyjson_mut_val *val = entry->ToJsonValue(doc);
                    if (val)
                        yyjson_mut_obj_add(obj, key, val);
                }
            }
            break;
            case BP_CFG_TYPE_LIST: {
                auto *list = (BpConfigList *) e.data.list;
                if (list && !list->IsDynamic()) {
                    yyjson_mut_val *key = list->ToJsonKey(doc);
                    yyjson_mut_val *val = list->ToJsonArray(doc);
                    if (val)
                        yyjson_mut_obj_add(obj, key, val);
                }
            }
            case BP_CFG_TYPE_SECTION: {
                auto *section = (BpConfigSection *) e.data.section;
                if (section && !section->IsDynamic()) {
                    yyjson_mut_val *key = section->ToJsonKey(doc);
                    yyjson_mut_val *val = section->ToJsonObject(doc);
                    if (val)
                        yyjson_mut_obj_add(obj, key, val);
                }
            }
            break;
            default:
                break;
        }
    }

    return obj;
}

bool BpConfigSection::AddCallback(BpConfigCallbackType type, BpConfigCallback callback, void *arg) {
    if (type < 0 || type >= BP_CFG_CB_COUNT)
        return false;

    if (!callback)
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto &callbacks = m_Callbacks[type];
    Callback cb(callback, arg);
    auto it = std::find(callbacks.begin(), callbacks.end(), cb);
    if (it != callbacks.end())
        return false;

    callbacks.emplace_back(cb);
    return true;
}

bool BpConfigSection::RemoveCallback(BpConfigCallbackType type, BpConfigCallback callback, void *arg) {
    if (type < 0 || type >= BP_CFG_CB_COUNT)
        return false;

    if (!callback)
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto &callbacks = m_Callbacks[type];
    Callback cb(callback, arg);
    auto it = std::find(callbacks.begin(), callbacks.end(), cb);
    if (it == callbacks.end())
        return false;

    callbacks.erase(it);
    return true;
}

void BpConfigSection::ClearCallbacks(BpConfigCallbackType type) {
    std::lock_guard<std::mutex> guard(m_RWLock);

    if (type >= 0 && type < BP_CFG_CB_COUNT)
        m_Callbacks[type].clear();
}

void BpConfigSection::EnableCallbacks(BpConfigCallbackType type, bool enable) {
    std::lock_guard<std::mutex> guard(m_RWLock);

    if (type >= 0 && type < BP_CFG_CB_COUNT)
        m_CallbacksEnabled[type] = enable;
}

void BpConfigSection::InvokeCallbacks(BpConfigCallbackType type, const BpConfigItem &item) const {
    if (type >= 0 && type < BP_CFG_CB_COUNT && m_CallbacksEnabled[type]) {
        const BpConfigCallbackArgument arg = {type, item};
        for (const auto &cb: m_Callbacks[type]) {
            cb.callback(&arg, cb.userdata);
        }
    }
}

BpConfigList::BpConfigList(BpConfigSection *parent, const char *name) : m_Parent(parent), m_Name(name) {
    assert(name != nullptr);
}

BpConfigList::~BpConfigList() {
    if (m_Parent) {
        m_Parent->RemoveEntry(m_Name.c_str());
    }
}

int BpConfigList::AddRef() const {
    return m_RefCount.AddRef();
}

int BpConfigList::Release() const {
    int r = m_RefCount.Release();
    if (r == 0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete const_cast<BpConfigList *>(this);
    }
    return r;
}

size_t BpConfigList::GetNumberOfValues() const {
    return m_Values.size();
}

BpConfigEntryType BpConfigList::GetType(size_t index) const {
    if (index >= m_Values.size())
        return BP_CFG_ENTRY_NONE;
    return GetValueType(m_Values[index]);
}

size_t BpConfigList::GetSize(size_t index) const {
    if (index >= m_Values.size())
        return 0;
    return m_Values[index].GetSize();
}

bool BpConfigList::GetBool(size_t index) const {
    if (index >= m_Values.size())
        return false;
    return m_Values[index].GetBool();
}

uint32_t BpConfigList::GetUint32(size_t index) const {
    if (index >= m_Values.size())
        return 0;
    return m_Values[index].GetUint32();
}

int32_t BpConfigList::GetInt32(size_t index) const {
    if (index >= m_Values.size())
        return 0;
    return m_Values[index].GetInt32();
}

uint64_t BpConfigList::GetUint64(size_t index) const {
    if (index >= m_Values.size())
        return 0;
    return m_Values[index].GetUint64();
}

int64_t BpConfigList::GetInt64(size_t index) const {
    if (index >= m_Values.size())
        return 0;
    return m_Values[index].GetInt64();
}

float BpConfigList::GetFloat(size_t index) const {
    if (index >= m_Values.size())
        return 0;
    return m_Values[index].GetFloat32();
}

double BpConfigList::GetDouble(size_t index) const {
    if (index >= m_Values.size())
        return 0;
    return m_Values[index].GetFloat64();
}

const char *BpConfigList::GetString(size_t index) const {
    if (index >= m_Values.size())
        return nullptr;
    return m_Values[index].GetString();
}

void BpConfigList::SetBool(size_t index, bool value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values[index] = value;
}

void BpConfigList::SetUint32(size_t index, uint32_t value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values[index] = value;
}

void BpConfigList::SetInt32(size_t index, int32_t value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values[index] = value;
}

void BpConfigList::SetUint64(size_t index, uint64_t value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values[index] = value;
}

void BpConfigList::SetInt64(size_t index, int64_t value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values[index] = value;
}

void BpConfigList::SetFloat(size_t index, float value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values[index] = value;
}

void BpConfigList::SetDouble(size_t index, double value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values[index] = value;
}

void BpConfigList::SetString(size_t index, const char *value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values[index] = value;
}

void BpConfigList::InsertBool(size_t index, bool value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values.insert(m_Values.begin() + static_cast<int>(index), value);
}

void BpConfigList::InsertUint32(size_t index, uint32_t value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values.insert(m_Values.begin() + static_cast<int>(index), value);
}

void BpConfigList::InsertInt32(size_t index, int32_t value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values.insert(m_Values.begin() + static_cast<int>(index), value);
}

void BpConfigList::InsertUint64(size_t index, uint64_t value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values.insert(m_Values.begin() + static_cast<int>(index), value);
}

void BpConfigList::InsertInt64(size_t index, int64_t value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values.insert(m_Values.begin() + static_cast<int>(index), value);
}

void BpConfigList::InsertFloat(size_t index, float value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values.insert(m_Values.begin() + static_cast<int>(index), value);
}

void BpConfigList::InsertDouble(size_t index, double value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values.insert(m_Values.begin() + static_cast<int>(index), value);
}

void BpConfigList::InsertString(size_t index, const char *value) {
    if (IsReadOnly())
        return;

    if (index < m_Values.size())
        m_Values.insert(m_Values.begin() + static_cast<int>(index), value);
}

void BpConfigList::AppendBool(bool value) {
    if (IsReadOnly())
        return;

    m_Values.emplace_back(value);
}

void BpConfigList::AppendUint32(uint32_t value) {
    if (IsReadOnly())
        return;

    m_Values.emplace_back(value);
}

void BpConfigList::AppendInt32(int32_t value) {
    if (IsReadOnly())
        return;

    m_Values.emplace_back(value);
}

void BpConfigList::AppendUint64(uint64_t value) {
    if (IsReadOnly())
        return;

    m_Values.emplace_back(value);
}

void BpConfigList::AppendInt64(int64_t value) {
    if (IsReadOnly())
        return;

    m_Values.emplace_back(value);
}

void BpConfigList::AppendFloat(float value) {
    if (IsReadOnly())
        return;

    m_Values.emplace_back(value);
}

void BpConfigList::AppendDouble(double value) {
    if (IsReadOnly())
        return;

    m_Values.emplace_back(value);
}

void BpConfigList::AppendString(const char *value) {
    if (IsReadOnly())
        return;

    m_Values.emplace_back(value);
}

bool BpConfigList::Remove(size_t index) {
    if (IsReadOnly())
        return false;

    if (index == -1) {
        m_Values.pop_back();
        return true;
    }

    if (index < m_Values.size()) {
        m_Values.erase(m_Values.begin() + static_cast<int>(index));
        return true;
    }

    return false;
}

void BpConfigList::Clear() {
    if (IsReadOnly())
        return;

    m_Values.clear();
}

void BpConfigList::Resize(size_t size) {
    if (IsReadOnly())
        return;

    m_Values.resize(size);
}

void BpConfigList::Reserve(size_t size) {
    if (IsReadOnly())
        return;

    m_Values.reserve(size);
}

yyjson_mut_val *BpConfigList::ToJsonKey(yyjson_mut_doc *doc) {
    if (!doc)
        return nullptr;
    return yyjson_mut_str(doc, m_Name.c_str());
}

yyjson_mut_val *BpConfigList::ToJsonArray(yyjson_mut_doc *doc) {
    if (!doc)
        return nullptr;

    yyjson_mut_val *arr = yyjson_mut_arr(doc);

    for (const auto &value : m_Values) {
        switch (GetValueType(value)) {
            case BP_CFG_ENTRY_NONE:
                yyjson_mut_arr_add_null(doc, arr);
            case BP_CFG_ENTRY_BOOL:
                yyjson_mut_arr_add_bool(doc, arr, value.GetBool());
            case BP_CFG_ENTRY_UINT:
                yyjson_mut_arr_add_uint(doc, arr, value.GetUint64());
            case BP_CFG_ENTRY_INT:
                yyjson_mut_arr_add_int(doc, arr, value.GetInt64());
            case BP_CFG_ENTRY_REAL:
                yyjson_mut_arr_add_real(doc, arr, value.GetFloat64());
            case BP_CFG_ENTRY_STR:
                yyjson_mut_arr_add_str(doc, arr, value.GetString());
            default:
                return nullptr;
        }
    }

    return arr;
}

BpConfigEntry::BpConfigEntry(BpConfigSection *parent, const char *name)
    : m_Parent(parent), m_Name(name) {
    assert(name != nullptr);
}

BpConfigEntry::BpConfigEntry(BpConfigSection *parent, const char *name, bool value)
    : m_Parent(parent), m_Name(name), m_Value(value) {
    assert(name != nullptr);
}

BpConfigEntry::BpConfigEntry(BpConfigSection *parent, const char *name, uint32_t value)
    : m_Parent(parent), m_Name(name), m_Value(static_cast<uint64_t>(value)) {
    assert(name != nullptr);
}

BpConfigEntry::BpConfigEntry(BpConfigSection *parent, const char *name, int32_t value)
    : m_Parent(parent), m_Name(name), m_Value(static_cast<int64_t>(value)) {
    assert(name != nullptr);
}

BpConfigEntry::BpConfigEntry(BpConfigSection *parent, const char *name, uint64_t value)
    : m_Parent(parent), m_Name(name), m_Value(value) {
    assert(name != nullptr);
}

BpConfigEntry::BpConfigEntry(BpConfigSection *parent, const char *name, int64_t value)
    : m_Parent(parent), m_Name(name), m_Value(value) {
    assert(name != nullptr);
}

BpConfigEntry::BpConfigEntry(BpConfigSection *parent, const char *name, float value)
    : m_Parent(parent), m_Name(name), m_Value(static_cast<double>(value)) {
    assert(name != nullptr);
}

BpConfigEntry::BpConfigEntry(BpConfigSection *parent, const char *name, double value)
    : m_Parent(parent), m_Name(name), m_Value(value) {
    assert(name != nullptr);
}

BpConfigEntry::BpConfigEntry(BpConfigSection *parent, const char *name, const char *value)
    : m_Parent(parent), m_Name(name), m_Value(value) {
    assert(name != nullptr);
}

BpConfigEntry::~BpConfigEntry() {
    if (m_Parent) {
        m_Parent->RemoveEntry(m_Name.c_str());
    }
}

int BpConfigEntry::AddRef() const {
    return m_RefCount.AddRef();
}

int BpConfigEntry::Release() const {
    int r = m_RefCount.Release();
    if (r == 0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete const_cast<BpConfigEntry *>(this);
    }
    return r;
}

BpConfigEntryType BpConfigEntry::GetType() const {
    return GetValueType(m_Value);
}

size_t BpConfigEntry::GetSize() const {
    return m_Value.GetSize();
}

void BpConfigEntry::SetBool(bool value) {
    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    bool v = false;
    bool t = false;

    if (GetType() != BP_CFG_ENTRY_BOOL) {
        t = true;
    }

    if (m_Value != value) {
        v = true;
    }

    if (t || v) {
        m_Value = value;
    }

    InvokeCallbacks(t, v);
}

void BpConfigEntry::SetUint32(uint32_t value) {
    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    bool v = false;
    bool t = false;

    if (GetType() != BP_CFG_ENTRY_UINT) {
        t = true;
    }

    if (m_Value != value) {
        v = true;
    }

    if (t || v) {
        m_Value = value;
    }

    InvokeCallbacks(t, v);
}

void BpConfigEntry::SetInt32(int32_t value) {
    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    bool v = false;
    bool t = false;

    if (GetType() != BP_CFG_ENTRY_INT) {
        t = true;
    }

    if (m_Value != value) {
        v = true;
    }

    if (t || v) {
        m_Value = value;
    }

    InvokeCallbacks(t, v);
}

void BpConfigEntry::SetUint64(uint64_t value) {
    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    bool v = false;
    bool t = false;

    if (GetType() != BP_CFG_ENTRY_UINT) {
        t = true;
    }

    if (m_Value != value) {
        v = true;
    }

    if (t || v) {
        m_Value = value;
    }

    InvokeCallbacks(t, v);
}

void BpConfigEntry::SetInt64(int64_t value) {
    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    bool v = false;
    bool t = false;

    if (GetType() != BP_CFG_ENTRY_INT) {
        t = true;
    }

    if (m_Value != value) {
        v = true;
    }

    if (t || v) {
        m_Value = value;
    }

    InvokeCallbacks(t, v);
}

void BpConfigEntry::SetFloat(float value) {
    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    bool v = false;
    bool t = false;

    if (GetType() != BP_CFG_ENTRY_REAL) {
        t = true;
    }

    if (m_Value != value) {
        v = true;
    }

    if (t || v) {
        m_Value = value;
    }

    InvokeCallbacks(t, v);
}

void BpConfigEntry::SetDouble(double value) {
    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    bool v = false;
    bool t = false;

    if (GetType() != BP_CFG_ENTRY_REAL) {
        t = true;
    }

    if (m_Value != value) {
        v = true;
    }

    if (t || v) {
        m_Value = value;
    }

    InvokeCallbacks(t, v);
}

// BKDR hash
static inline size_t HashString(const char *str) {
    size_t hash = 0;
    if (str) {
        while (char ch = *str++) {
            hash = hash * 131 + ch;
        }
    }
    return hash;
}

void BpConfigEntry::SetString(const char *value) {
    if (!value)
        return;

    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    bool v = false;
    bool t = false;

    if (GetType() != BP_CFG_ENTRY_REAL) {
        t = true;
    }

    if (m_Value != value) {
        v = true;
    }

    if (t || v) {
        m_Value = value;
        m_Hash = HashString(value);
    }

    InvokeCallbacks(t, v);
}

void BpConfigEntry::SetDefaultBool(bool value) {
    std::lock_guard<std::mutex> guard(m_RWLock);
    if (GetType() != BP_CFG_ENTRY_BOOL) {
        m_Value = value;
    }
}

void BpConfigEntry::SetDefaultUint32(uint32_t value) {
    std::lock_guard<std::mutex> guard(m_RWLock);
    if (GetType() != BP_CFG_ENTRY_UINT) {
        m_Value = value;
    }
}

void BpConfigEntry::SetDefaultInt32(int32_t value) {
    std::lock_guard<std::mutex> guard(m_RWLock);
    if (GetType() != BP_CFG_ENTRY_INT) {
        m_Value = value;
    }
}

void BpConfigEntry::SetDefaultUint64(uint64_t value) {
    std::lock_guard<std::mutex> guard(m_RWLock);
    if (GetType() != BP_CFG_ENTRY_UINT) {
        m_Value = value;
    }
}

void BpConfigEntry::SetDefaultInt64(int64_t value) {
    std::lock_guard<std::mutex> guard(m_RWLock);
    if (GetType() != BP_CFG_ENTRY_INT) {
        m_Value = value;
    }
}

void BpConfigEntry::SetDefaultFloat(float value) {
    std::lock_guard<std::mutex> guard(m_RWLock);
    if (GetType() != BP_CFG_ENTRY_REAL) {
        m_Value = value;
    }
}

void BpConfigEntry::SetDefaultDouble(double value) {
    std::lock_guard<std::mutex> guard(m_RWLock);
    if (GetType() != BP_CFG_ENTRY_REAL) {
        m_Value = value;
    }
}

void BpConfigEntry::SetDefaultString(const char *value) {
    std::lock_guard<std::mutex> guard(m_RWLock);
    if (value && GetType() != BP_CFG_ENTRY_STR) {
        m_Value = value;
    }
}

void BpConfigEntry::CopyValue(BpConfigEntry *entry) {
    if (IsReadOnly())
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);
    switch (entry->GetType()) {
        case BP_CFG_ENTRY_BOOL:
            SetBool(entry->GetBool());
            break;
        case BP_CFG_ENTRY_UINT:
            if (GetSize() == sizeof(uint32_t))
                SetUint32(entry->GetUint32());
            else
                SetUint64(entry->GetUint64());
            break;
        case BP_CFG_ENTRY_INT:
            if (GetSize() == sizeof(int32_t))
                SetInt32(entry->GetInt32());
            else
                SetInt64(entry->GetInt64());
            break;
        case BP_CFG_ENTRY_REAL:
            if (GetSize() == sizeof(float))
                SetFloat(entry->GetFloat());
            else
                SetDouble(entry->GetDouble());
            break;
        case BP_CFG_ENTRY_STR:
            SetString(entry->GetString());
            break;
        default:
            break;
    }
}

void BpConfigEntry::Clear() {
    if (IsReadOnly())
        return;

    m_Value.Clear();
}

yyjson_mut_val *BpConfigEntry::ToJsonKey(yyjson_mut_doc *doc) {
    if (!doc)
        return nullptr;
    return yyjson_mut_str(doc, m_Name.c_str());
}

yyjson_mut_val *BpConfigEntry::ToJsonValue(yyjson_mut_doc *doc) {
    if (!doc)
        return nullptr;

    switch (GetType()) {
        case BP_CFG_ENTRY_NONE:
            return yyjson_mut_null(doc);
        case BP_CFG_ENTRY_BOOL:
            return yyjson_mut_bool(doc, GetBool());
        case BP_CFG_ENTRY_UINT:
            return yyjson_mut_uint(doc, GetUint64());
        case BP_CFG_ENTRY_INT:
            return yyjson_mut_int(doc, GetInt64());
        case BP_CFG_ENTRY_REAL:
            return yyjson_mut_real(doc, GetDouble());
        case BP_CFG_ENTRY_STR:
            return yyjson_mut_str(doc, GetString());
        default:
            return nullptr;
    }
}

void BpConfigEntry::FromJsonValue(yyjson_val *val) {
    switch (yyjson_get_tag(val)) {
        case YYJSON_TYPE_BOOL | YYJSON_SUBTYPE_TRUE:
        case YYJSON_TYPE_BOOL | YYJSON_SUBTYPE_FALSE: {
            auto value = yyjson_get_bool(val);
            switch (GetType()) {
                case BP_CFG_ENTRY_NONE:
                case BP_CFG_ENTRY_BOOL:
                    SetBool(value);
                    break;
                case BP_CFG_ENTRY_UINT:
                    SetUint64(value);
                    break;
                case BP_CFG_ENTRY_INT:
                    SetInt64(value);
                    break;
                case BP_CFG_ENTRY_REAL:
                    SetDouble(value);
                    break;
                case BP_CFG_ENTRY_STR:
                    SetString(value ? "true" : "false");
                    break;
                default:
                    break;
            }
        }
            break;
        case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT: {
            auto value = yyjson_get_uint(val);
            switch (GetType()) {
                case BP_CFG_ENTRY_NONE:
                    if (value <= INT64_MAX)
                        SetInt64(static_cast<int64_t>(value));
                    else
                        SetUint64(value);
                break;
                case BP_CFG_ENTRY_BOOL:
                    SetBool(value);
                    break;
                case BP_CFG_ENTRY_UINT:
                    SetUint64(value);
                    break;
                case BP_CFG_ENTRY_INT:
                    SetInt64(static_cast<int64_t>(value));
                    break;
                case BP_CFG_ENTRY_REAL:
                    SetDouble(static_cast<double>(value));
                    break;
                case BP_CFG_ENTRY_STR: {
                    auto str = std::to_string(value);
                    SetString(str.c_str());
                }
                    break;
                default:
                    break;
            }
        }
            break;
        case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT: {
            auto value = yyjson_get_int(val);
            switch (GetType()) {
                case BP_CFG_ENTRY_BOOL:
                    SetBool(value);
                    break;
                case BP_CFG_ENTRY_UINT:
                    SetUint64(value);
                    break;
                case BP_CFG_ENTRY_NONE:
                case BP_CFG_ENTRY_INT:
                    SetInt64(value);
                    break;
                case BP_CFG_ENTRY_REAL:
                    SetDouble(value);
                    break;
                case BP_CFG_ENTRY_STR: {
                    auto str = std::to_string(value);
                    SetString(str.c_str());
                }
                    break;
                default:
                    break;
            }
        }
            break;
        case YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL: {
            auto value = yyjson_get_real(val);
            switch (GetType()) {
            case BP_CFG_ENTRY_BOOL:
                SetBool(std::fabs(value) < std::numeric_limits<double>::epsilon());
                break;
            case BP_CFG_ENTRY_UINT:
                SetUint64(static_cast<uint64_t>(value));
                break;
            case BP_CFG_ENTRY_INT:
                SetInt64(static_cast<int64_t>(value));
                break;
            case BP_CFG_ENTRY_NONE:
            case BP_CFG_ENTRY_REAL:
                SetDouble(value);
                break;
            case BP_CFG_ENTRY_STR: {
                auto str = std::to_string(value);
                SetString(str.c_str());
            }
                break;
            default:
                break;
            }
        }
            break;
        case YYJSON_TYPE_STR | YYJSON_SUBTYPE_NONE: {
            auto str = yyjson_get_str(val);
            switch (GetType()) {
            case BP_CFG_ENTRY_BOOL:
                SetBool(std::strcmp(str, "true") == 0 ||
                        std::strcmp(str, "yes") == 0 ||
                        std::strcmp(str, "on") == 0 ||
                        std::strcmp(str, "1") == 0);
                break;
            case BP_CFG_ENTRY_UINT:
                SetUint64(std::strtoull(str, nullptr, 10));
                break;
            case BP_CFG_ENTRY_INT:
                SetInt64(std::strtoll(str, nullptr, 10));
                break;
            case BP_CFG_ENTRY_REAL:
                SetDouble(std::strtod(str, nullptr));
                break;
            case BP_CFG_ENTRY_NONE:
            case BP_CFG_ENTRY_STR:
                SetString(str);
                break;
            default:
                break;
            }
        }
            break;
        case YYJSON_TYPE_NULL | YYJSON_SUBTYPE_NONE:
            Clear();
            break;
        default:
            break;
    }
}

void BpConfigEntry::InvokeCallbacks(bool typeChanged, bool valueChanged) {
    if (!m_Parent)
        return;

    if (typeChanged) {
        m_Parent->InvokeCallbacks(BP_CFG_CB_ENTRY_TYPE_CHANGE, BpConfigItem(this));
    }

    if (valueChanged) {
        m_Parent->InvokeCallbacks(BP_CFG_CB_ENTRY_VALUE_CHANGE, BpConfigItem(this));
    }
}
