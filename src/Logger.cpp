#include "Logger.h"

#include <cassert>
#include <cstdio>
#include <ctime>

BpLogger *bpGetLogger(const char *name) {
    if (!name)
        return BpLogger::GetDefault();
    return BpLogger::GetInstance(name);
}

BpLogger *bpGetDefaultLogger() {
    return BpLogger::GetDefault();
}

void bpSetDefaultBpLogger(BpLogger *logger) {
    BpLogger::SetDefault(logger);
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

const char *bpGetLogLevelString(BpLogLevel level) {
    return BpLogger::GetLevelString(level);
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

void bpLoggerLogString(BpLogger *logger, BpLogLevel level, const char *str) {
    if (!logger || !str)
        return;
    bpLoggerLog(logger, level, "%s", str);
}

void bpLoggerTrace(BpLogger *logger, const char *format, ...) {
    if (!logger)
        return;

    va_list args;
    va_start(args, format);
    logger->Log(BP_LOG_TRACE, format, args);
    va_end(args);
}

void bpLoggerDebug(BpLogger *logger, const char *format, ...) {
    if (!logger)
        return;

    va_list args;
    va_start(args, format);
    logger->Log(BP_LOG_DEBUG, format, args);
    va_end(args);
}

void bpLoggerInfo(BpLogger *logger, const char *format, ...) {
    if (!logger)
        return;

    va_list args;
    va_start(args, format);
    logger->Log(BP_LOG_INFO, format, args);
    va_end(args);
}

void bpLoggerWarn(BpLogger *logger, const char *format, ...) {
    if (!logger)
        return;

    va_list args;
    va_start(args, format);
    logger->Log(BP_LOG_WARN, format, args);
    va_end(args);
}

void bpLoggerError(BpLogger *logger, const char *format, ...) {
    if (!logger)
        return;

    va_list args;
    va_start(args, format);
    logger->Log(BP_LOG_ERROR, format, args);
    va_end(args);
}

void bpLoggerFatal(BpLogger *logger, const char *format, ...) {
    if (!logger)
        return;

    va_list args;
    va_start(args, format);
    logger->Log(BP_LOG_FATAL, format, args);
}

void bpLog(BpLogLevel level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    BpLogger::GetDefault()->Log(level, format, args);
    va_end(args);
}

void bpLogV(BpLogLevel level, const char *format, va_list args) {
    BpLogger::GetDefault()->Log(level, format, args);
}

void bpLogString(BpLogLevel level, const char *str) {
    if (!str)
        return;
    bpLog(level, "%s", str);
}

void bpLogTrace(const char *format, ...) {
    va_list args;
    va_start(args, format);
    BpLogger::GetDefault()->Log(BP_LOG_TRACE, format, args);
    va_end(args);
}

void bpLogDebug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    BpLogger::GetDefault()->Log(BP_LOG_DEBUG, format, args);
    va_end(args);
}

void bpLogInfo(const char *format, ...) {
    va_list args;
    va_start(args, format);
    BpLogger::GetDefault()->Log(BP_LOG_INFO, format, args);
    va_end(args);
}

void bpLogWarn(const char *format, ...) {
    va_list args;
    va_start(args, format);
    BpLogger::GetDefault()->Log(BP_LOG_WARN, format, args);
    va_end(args);
}

void bpLogError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    BpLogger::GetDefault()->Log(BP_LOG_ERROR, format, args);
    va_end(args);
}

void bpLogFatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    BpLogger::GetDefault()->Log(BP_LOG_FATAL, format, args);
    va_end(args);
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

BpLogger *BpLogger::s_DefaultBpLogger = nullptr;
std::unordered_map<std::string, BpLogger *> BpLogger::s_BpLoggers;

BpLogger *BpLogger::GetInstance(const std::string &name) {
    auto it = s_BpLoggers.find(name);
    if (it == s_BpLoggers.end()) {
        return new BpLogger(name);
    }
    return it->second;
}

BpLogger *BpLogger::SetDefault(BpLogger *logger) {
    BpLogger *previous = s_DefaultBpLogger;
    if (!logger) {
        s_DefaultBpLogger = GetInstance(BP_DEFAULT_NAME);
    }
    s_DefaultBpLogger = logger;
    return previous;
}

BpLogger *BpLogger::GetDefault() {
    if (!s_DefaultBpLogger) {
        s_DefaultBpLogger = GetInstance(BP_DEFAULT_NAME);
    }
    return s_DefaultBpLogger;
}

BpLogger::BpLogger(std::string name, BpLogLevel level) : m_Name(std::move(name)), m_Level(level) {}

BpLogger::~BpLogger() = default;

int BpLogger::AddRef() const {
    return m_RefCount.AddRef();
}

int BpLogger::Release() const {
    const int r = m_RefCount.Release();
    if (r == 0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        delete const_cast<BpLogger *>(this);
    }
    return r;
}

bool BpLogger::AddCallback(BpLogCallback callback, void *userdata, BpLogLevel level) {
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

void BpLogger::ClearCallbacks() {
    std::lock_guard<std::mutex> lock{m_Mutex};
    m_Callbacks.clear();
}

size_t BpLogger::GetCallbackCount() const {
    std::lock_guard<std::mutex> lock{m_Mutex};
    return m_Callbacks.size();
}

void BpLogger::Log(BpLogLevel level, const char *format, va_list args) {
    Lock();

    BpLogInfo info = {this, level, args, format, nullptr, nullptr};

    if (level >= m_Level && level < BP_LOG_OFF) {
        InitLogInfo(&info, stdout);
        StdoutCallback(&info);
    }

    for (auto it = m_Callbacks.begin(); it != m_Callbacks.end() && it->callback; it++) {
        if (level >= m_Level && level < BP_LOG_OFF) {
            InitLogInfo(&info, it->userdata);
            it->callback(&info);
        }
    }

    Unlock();
}

const char *BpLogger::GetLevelString(BpLogLevel level) {
    if (level < BP_LOG_TRACE || level > BP_LOG_FATAL)
        return nullptr;
    return g_LevelStrings[level];
}

void BpLogger::InitLogInfo(BpLogInfo *info, void *userdata) {
    if (!info->time) {
        time_t t = time(nullptr);
        info->time = localtime(&t);
    }
    info->userdata = userdata;
}
