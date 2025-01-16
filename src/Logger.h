#ifndef BP_LOGGER_PRIVATE_H
#define BP_LOGGER_PRIVATE_H

#include <string>
#include <unordered_map>
#include <mutex>

#include "bp/Logger.h"

#include "RefCount.h"

struct BpLogger {
    static BpLogger *GetInstance(const std::string &name);
    static BpLogger *GetDefault();
    static BpLogger *SetDefault(BpLogger *logger);

    explicit BpLogger(std::string name, BpLogLevel level = BP_LOG_INFO);

    BpLogger(const BpLogger &rhs) = delete;
    BpLogger(BpLogger &&rhs) noexcept = delete;

    ~BpLogger();

    BpLogger &operator=(const BpLogger &rhs) = delete;
    BpLogger &operator=(BpLogger &&rhs) noexcept = delete;

    int AddRef() const;
    int Release() const;

    const char *GetName() const { return m_Name.c_str(); }

    BpLogLevel GetLevel() const { return m_Level; }
    const char *GetLevelString(BpLogLevel level) const;

    void SetLevel(BpLogLevel level) {
        if (level >= BP_LOG_TRACE && level <= BP_LOG_OFF)
            m_Level = level;
    }

    void SetLock(BpLogLockFunction func, void *userdata) {
        m_LockFunction = func;
        m_Userdata = userdata;
    }

    bool AddCallback(BpLogCallback callback, void *userdata, BpLogLevel level);
    void ClearCallbacks() { m_Callbacks.clear(); }
    size_t GetCallbackCount() const { return m_Callbacks.size(); }

    void Log(BpLogLevel level, const char *format, va_list args);

    void Trace(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_TRACE, format, args);
        va_end(args);
    }

    void Debug(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_DEBUG, format, args);
        va_end(args);
    }

    void Info(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_INFO, format, args);
        va_end(args);
    }

    void Warn(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_WARN, format, args);
        va_end(args);
    }

    void Error(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_ERROR, format, args);
        va_end(args);
    }

    void Fatal(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_FATAL, format, args);
        va_end(args);
    }

private:
    struct Callback {
        BpLogCallback callback;
        void *userdata;
        BpLogLevel level;

        Callback(BpLogCallback cb, void *data, BpLogLevel lvl) : callback(cb), userdata(data), level(lvl) {}

        bool operator==(const Callback &rhs) const {
            return callback == rhs.callback &&
                   userdata == rhs.userdata &&
                   level == rhs.level;
        }

        bool operator!=(const Callback &rhs) const {
            return !(rhs == *this);
        }
    };

    void Lock() {
        if (m_LockFunction) { m_LockFunction(true, m_Userdata); }
    }

    void Unlock() {
        if (m_LockFunction) { m_LockFunction(false, m_Userdata); }
    }

    static void InitLogInfo(BpLogInfo *info, void *userdata);

    mutable RefCount m_RefCount;
    std::mutex m_Mutex;

    std::string m_Name;
    BpLogLevel m_Level;

    BpLogLockFunction m_LockFunction = nullptr;
    void *m_Userdata = nullptr;
    std::vector<Callback> m_Callbacks;

    static BpLogger *s_DefaultBpLogger;
    static std::unordered_map<std::string, BpLogger *> s_BpLoggers;
};

#endif // BP_LOGGER_PRIVATE_H
