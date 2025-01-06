#include "Logger.h"

#include <cassert>
#include <cstdio>
#include <ctime>
#include <string>
#include <unordered_map>
#include <mutex>

#include "RefCount.h"

namespace bp {
    class Logger : public BpLogger {
    public:
        static Logger *GetInstance(const std::string &name);
        static Logger *GetDefault();
        static Logger *SetDefault(Logger *logger);

        explicit Logger(std::string name, BpLogLevel level = BP_LOG_INFO);

        Logger(const Logger &rhs) = delete;
        Logger(Logger &&rhs) noexcept = delete;

        ~Logger() override;

        Logger &operator=(const Logger &rhs) = delete;
        Logger &operator=(Logger &&rhs) noexcept = delete;

        int AddRef() const override;
        int Release() const override;

        const char *GetName() const override { return m_Name.c_str(); }

        BpLogLevel GetLevel() const override { return m_Level; }
        const char *GetLevelString(BpLogLevel level) const override;

        void SetLevel(BpLogLevel level) override {
            if (level >= BP_LOG_TRACE && level <= BP_LOG_OFF)
                m_Level = level;
        }

        void SetLock(BpLogLockFunction func, void *userdata) override {
            m_LockFunction = func;
            m_Userdata = userdata;
        }

        bool AddCallback(BpLogCallback callback, void *userdata, BpLogLevel level) override;
        void ClearCallbacks() override { m_Callbacks.clear(); }
        size_t GetCallbackCount() const { return m_Callbacks.size(); }

        void Log(BpLogLevel level, const char *format, va_list args) override;

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

        static Logger *s_DefaultLogger;
        static std::unordered_map<std::string, Logger *> s_Loggers;
    };
}

using namespace bp;

BpLogger *bpGetLogger(const char *name) {
    if (!name)
        return Logger::GetDefault();
    return Logger::GetInstance(name);
}

BpLogger *bpGetDefaultLogger() {
    return Logger::GetDefault();
}

void bpSetDefaultLogger(BpLogger *logger) {
    Logger::SetDefault((Logger *) logger);
}

int bpLoggerAddRef(BpLogger *logger) {
    if (!logger)
        return 0;
    return logger->AddRef();
}

int bpLoggerRelease(BpLogger *logger) {
    if (!logger)
        return 0;
    return logger->Release();
}

const char *bpLoggerGetName(BpLogger *logger) {
    if (!logger)
        return nullptr;
    return logger->GetName();
}

BpLogLevel bpLoggerGetLevel(BpLogger *logger) {
    if (!logger)
        return BP_LOG_OFF;
    return logger->GetLevel();
}

const char *bpLoggerGetLevelString(BpLogger *logger, BpLogLevel level) {
    if (!logger)
        return nullptr;
    return logger->GetLevelString(level);
}

void bpLoggerSetLevel(BpLogger *logger, BpLogLevel level) {
    if (!logger)
        return;
    logger->SetLevel(level);
}

void bpLoggerSetLock(BpLogger *logger, BpLogLockFunction func, void *userdata) {
    if (!logger)
        return;
    logger->SetLock(func, userdata);
}

bool bpLoggerAddCallback(BpLogger *logger, BpLogCallback callback, void *userdata, BpLogLevel level) {
    if (!logger)
        return false;
    return logger->AddCallback(callback, userdata, level);
}

void bpLoggerClearCallbacks(BpLogger *logger) {
    if (!logger)
        return;
    logger->ClearCallbacks();
}

void bpLoggerLog(BpLogger *logger, BpLogLevel level, const char *format, ...) {
    if (!logger)
        return;

    va_list args;
    va_start(args, format);
    logger->Log(level, format, args);
    va_end(args);
}

void bpLoggerLogV(BpLogger *logger, BpLogLevel level, const char *format, va_list args) {
    if (!logger)
        return;
    logger->Log(level, format, args);
}

void bpLog(BpLogLevel level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    Logger::GetDefault()->Log(level, format, args);
    va_end(args);
}

void bpLogV(BpLogLevel level, const char *format, va_list args) {
    Logger::GetDefault()->Log(level, format, args);
}

static const char *g_LevelStrings[6] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static void StdoutCallback(BpLogInfo *info) {
    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", info->time)] = '\0';

    FILE *fp = (FILE *) info->userdata;
    fprintf(fp, "[%s] [%s/%s]: ", buf, info->self->GetName(), g_LevelStrings[info->level]);

    vfprintf(fp, info->format, info->args);
    fprintf(fp, "\n");
    fflush(fp);
}

Logger *Logger::s_DefaultLogger = nullptr;
std::unordered_map<std::string, Logger *> Logger::s_Loggers;

Logger *Logger::GetInstance(const std::string &name) {
    auto it = s_Loggers.find(name);
    if (it == s_Loggers.end()) {
        return new Logger(name);
    }
    return it->second;
}

Logger *Logger::SetDefault(Logger *logger) {
    Logger *previous = s_DefaultLogger;
    if (!logger) {
        s_DefaultLogger = GetInstance(BP_DEFAULT_NAME);
    }
    s_DefaultLogger = logger;
    return previous;
}

Logger *Logger::GetDefault() {
    if (!s_DefaultLogger) {
        s_DefaultLogger = GetInstance(BP_DEFAULT_NAME);
    }
    return s_DefaultLogger;
}

Logger::Logger(std::string name, BpLogLevel level) : m_Name(std::move(name)), m_Level(level) {}

Logger::~Logger() = default;

int Logger::AddRef() const {
    return m_RefCount.AddRef();
}

int Logger::Release() const {
    int r = m_RefCount.Release();
    if (r == 0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete const_cast<Logger *>(this);
    }
    return r;
}

const char *Logger::GetLevelString(BpLogLevel level) const {
    if (level < BP_LOG_TRACE || level > BP_LOG_FATAL)
        return nullptr;
    return g_LevelStrings[level];
}

bool Logger::AddCallback(BpLogCallback callback, void *userdata, BpLogLevel level) {
    if (!callback)
        return false;

    std::lock_guard<std::mutex> lock{m_Mutex};

    Callback cb(callback, userdata, level);
    auto it = std::find(m_Callbacks.begin(), m_Callbacks.end(), cb);
    if (it != m_Callbacks.end())
        return false;

    m_Callbacks.emplace_back(cb);
    return true;
}

void Logger::Log(BpLogLevel level, const char *format, va_list args) {
    Lock();

    BpLogInfo info = {this, level, args, format, nullptr, nullptr};

    if (level >= m_Level && level < BP_LOG_OFF) {
        InitLogInfo(&info, stdout);
        StdoutCallback(&info);
    }

    for (auto it = m_Callbacks.begin(); it != m_Callbacks.end() && it->callback; it++) {
        if (level >= it->level && level < BP_LOG_OFF) {
            InitLogInfo(&info, it->userdata);
            it->callback(&info);
        }
    }

    Unlock();
}

void Logger::InitLogInfo(BpLogInfo *info, void *userdata) {
    if (!info->time) {
        time_t t = time(nullptr);
        info->time = localtime(&t);
    }
    info->userdata = userdata;
}
