#ifndef BP_DATASHARE_PRIVATE_H
#define BP_DATASHARE_PRIVATE_H

#include <mutex>
#include <string>
#include <unordered_map>

#include "bp/DataShare.h"

#include "DataBox.h"
#include "RefCount.h"

struct BpDataShare final {
    static BpDataShare *GetInstance(const std::string &name);

    explicit BpDataShare(std::string name);

    BpDataShare(const BpDataShare &rhs) = delete;
    BpDataShare(BpDataShare &&rhs) noexcept = delete;

    ~BpDataShare();

    BpDataShare &operator=(const BpDataShare &rhs) = delete;
    BpDataShare &operator=(BpDataShare &&rhs) noexcept = delete;

    int AddRef() const;
    int Release() const;

    const char *GetName() const;

    void Request(const char *key, BpDataShareCallback callback, void *userdata) const;

    const void *Get(const char *key, size_t *size) const;
    bool Copy(const char *key, void *buf, size_t size) const;
    bool Set(const char *key, const void *buf, size_t size);
    bool Put(const char *key, const void *buf, size_t size);
    bool Remove(const char *key);

    void *GetUserData(size_t type) const;
    void *SetUserData(void *data, size_t type);

private:
    struct Callback {
        BpDataShareCallback callback;
        void *userdata;

        Callback(BpDataShareCallback cb, void *data) : callback(cb), userdata(data) {}

        bool operator==(const Callback &rhs) const {
            return callback == rhs.callback && userdata == rhs.userdata;
        }

        bool operator!=(const Callback &rhs) const {
            return !(rhs == *this);
        }
    };

    bool AddCallbacks(const char *key, BpDataShareCallback callback, void *userdata) const;
    void TriggerCallbacks(const char *key, const void *data, size_t size) const;

    static bool ValidateKey(const char *key);

    std::string m_Name;
    mutable RefCount m_RefCount;
    mutable std::mutex m_RWLock;
    std::unordered_map<std::string, std::vector<uint8_t>> m_DataMap;
    mutable std::unordered_map<std::string, std::vector<Callback>> m_CallbackMap;
    DataBox m_UserData;

    static std::mutex s_MapMutex;
    static std::unordered_map<std::string, BpDataShare *> s_DataShares;
};

#endif // BP_DATASHARE_PRIVATE_H
