#include "DataShare.h"

#include <utility>

BpDataShare *bpGetDataShare(const char *name) {
    if (!name)
        name = BP_DEFAULT_NAME;
    return BpDataShare::GetInstance(name);
}

int bpDataShareAddRef(const BpDataShare *dataShare) {
    if (!dataShare)
        return 0;
    return dataShare->AddRef();
}

int bpDataShareRelease(const BpDataShare *dataShare) {
    if (!dataShare)
        return 0;
    return dataShare->Release();
}

const char *bpDataShareGetName(const BpDataShare *dataShare) {
    if (!dataShare)
        return nullptr;
    return dataShare->GetName();
}

void bpDataShareRequest(BpDataShare *dataShare, const char *key, BpDataShareCallback callback, void *userdata) {
    if (!dataShare || !key || !callback)
        return;
    dataShare->Request(key, callback, userdata);
}

const void *bpDataShareGet(const BpDataShare *dataShare, const char *key, size_t *size) {
    if (!dataShare || !key)
        return nullptr;
    return dataShare->Get(key, size);
}

bool bpDataShareCopy(const BpDataShare *dataShare, const char *key, void *buf, size_t size) {
    if (!dataShare || !key || !buf || size == 0)
        return false;
    return dataShare->Copy(key, buf, size);
}

bool bpDataShareSet(BpDataShare *dataShare, const char *key, const void *buf, size_t size) {
    if (!dataShare || !key || !buf || size == 0)
        return false;
    return dataShare->Set(key, buf, size);
}

bool bpDataSharePut(BpDataShare *dataShare, const char *key, const void *buf, size_t size) {
    if (!dataShare || !key || !buf || size == 0)
        return false;
    return dataShare->Put(key, buf, size);
}

bool bpDataShareRemove(BpDataShare *dataShare, const char *key) {
    if (!dataShare || !key)
        return false;
    return dataShare->Remove(key);
}

void *bpDataShareGetUserData(const BpDataShare *dataShare, size_t type) {
    if (!dataShare)
        return nullptr;
    return dataShare->GetUserData(type);
}

void *bpDataShareSetUserData(BpDataShare *dataShare, void *data, size_t type) {
    if (!dataShare)
        return nullptr;
    return dataShare->SetUserData(data, type);
}

std::mutex BpDataShare::s_MapMutex;
std::unordered_map<std::string, BpDataShare *> BpDataShare::s_DataShares;

BpDataShare *BpDataShare::GetInstance(const std::string &name) {
    auto it = s_DataShares.find(name);
    if (it == s_DataShares.end()) {
        return new BpDataShare(name);
    }
    return it->second;
}

BpDataShare::BpDataShare(std::string name) : m_Name(std::move(name)) {
    std::lock_guard<std::mutex> lock{s_MapMutex};
    s_DataShares[m_Name] = this;
}

BpDataShare::~BpDataShare() {
    std::lock_guard<std::mutex> lock{s_MapMutex};
    s_DataShares.erase(m_Name);
}

int BpDataShare::AddRef() const {
    return m_RefCount.AddRef();
}

int BpDataShare::Release() const {
    int r = m_RefCount.Release();
    if (r == 0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete const_cast<BpDataShare *>(this);
    }
    return r;
}

const char *BpDataShare::GetName() const {
    return m_Name.c_str();
}

void BpDataShare::Request(const char *key, BpDataShareCallback callback, void *userdata) const {
    if (!ValidateKey(key))
        return;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto it = m_DataMap.find(key);
    if (it == m_DataMap.end()) {
        AddCallbacks(key, callback, userdata);
    } else {
        auto &data = it->second;
        TriggerCallbacks(key, data.data(), data.size());
    }
}

const void *BpDataShare::Get(const char *key, size_t *size) const {
    if (!ValidateKey(key))
        return nullptr;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto it = m_DataMap.find(key);
    if (it == m_DataMap.end())
        return nullptr;

    auto &data = it->second;
    if (size)
        *size = data.size();
    return data.data();
}

bool BpDataShare::Copy(const char *key, void *buf, size_t size) const {
    if (!buf || size == 0)
        return false;

    if (!ValidateKey(key))
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto it = m_DataMap.find(key);
    if (it == m_DataMap.end())
        return false;

    auto &data = it->second;
    memcpy(buf, data.data(), std::max(data.size(), size));
    return true;
}

bool BpDataShare::Set(const char *key, const void *buf, size_t size) {
    if (!ValidateKey(key))
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto it = m_DataMap.find(key);
    if (it != m_DataMap.end()) {
        auto &data = it->second;
        data.resize(size);
        memcpy(data.data(), buf, size);
    } else {
        auto result = m_DataMap.emplace(key, std::vector<uint8_t>());
        if (!result.second)
            return false;
        it = result.first;
        auto &data = it->second;
        data.resize(size);
        memcpy(data.data(), buf, size);
    }

    auto &data = it->second;
    TriggerCallbacks(key, data.data(), data.size());
    return true;
}

bool BpDataShare::Put(const char *key, const void *buf, size_t size) {
    if (!ValidateKey(key))
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto it = m_DataMap.find(key);
    if (it != m_DataMap.end())
        return false;

    auto result = m_DataMap.emplace(key, std::vector<uint8_t>());
    if (!result.second)
        return false;
    it = result.first;
    auto &data = it->second;
    data.resize(size);
    memcpy(data.data(), buf, size);
    TriggerCallbacks(key, data.data(), data.size());
    return true;
}

bool BpDataShare::Remove(const char *key) {
    if (!ValidateKey(key))
        return false;

    std::lock_guard<std::mutex> guard(m_RWLock);

    auto it = m_DataMap.find(key);
    if (it == m_DataMap.end())
        return false;

    m_DataMap.erase(it);
    return true;
}

void *BpDataShare::GetUserData(size_t type) const {
    return m_UserData.GetData(type);
}

void *BpDataShare::SetUserData(void *data, size_t type) {
    return m_UserData.SetData(data, type);
}

bool BpDataShare::AddCallbacks(const char *key, BpDataShareCallback callback, void *userdata) const {
    auto it = m_CallbackMap.find(key);
    if (it == m_CallbackMap.end()) return false;
    it->second.emplace_back(callback, userdata);
    return true;
}

void BpDataShare::TriggerCallbacks(const char *key, const void *data, size_t size) const {
    auto it = m_CallbackMap.find(key);
    if (it != m_CallbackMap.end()) {
        for (auto &cb : it->second) {
            cb.callback(key, data, size, cb.userdata);
        }
        m_CallbackMap.erase(it);
    }
}

bool BpDataShare::ValidateKey(const char *key) {
    if (!key || key[0] == '\0')
        return false;
    return true;
}