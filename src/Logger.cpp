#include "Logger.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

Logger &Logger::Get() {
    static Logger logger;
    return logger;
}

Logger::~Logger() {
    Close();
}

void Logger::Open(const char *filename, bool overwrite, int level) {
    m_Level = level;

    if (m_File)
        return;

    if (overwrite)
        m_File = fopen(filename, "w");
    else
        m_File = fopen(filename, "a");
}

void Logger::Close() {
    if (m_ConsoleOpened) {
        ::FreeConsole();
    }

    if (m_File)
        fclose(m_File);
}

int Logger::GetLevel() const {
    return m_Level;
}

void Logger::SetLevel(int level) {
    m_Level = level;
}

void Logger::Debug(const char *fmt, ...) {
    if (m_Level >= LEVEL_DEBUG) {
        va_list args;
        va_start(args, fmt);
        Log("DEBUG", fmt, args);
        va_end(args);
    }
}

void Logger::Info(const char *fmt, ...) {
    if (m_Level >= LEVEL_INFO) {
        va_list args;
        va_start(args, fmt);
        Log("INFO", fmt, args);
        va_end(args);
    }
}

void Logger::Warn(const char *fmt, ...) {
    if (m_Level >= LEVEL_WARN) {
        va_list args;
        va_start(args, fmt);
        Log("WARN", fmt, args);
        va_end(args);
    }
}

void Logger::Error(const char *fmt, ...) {
    if (m_Level >= LEVEL_ERROR) {
        va_list args;
        va_start(args, fmt);
        Log("ERROR", fmt, args);
        va_end(args);
    }
}

void Logger::Log(const char *level, const char *fmt, va_list args) {
    SYSTEMTIME sys;
    GetLocalTime(&sys);

    FILE *outs[] = {stdout, m_File};

    for (int i = 0; i < 2; ++i) {
        FILE *out = outs[i];
        if (!out)
            continue;

        fprintf(out, "[%02d/%02d/%d %02d:%02d:%02d.%03d] ",
                sys.wMonth, sys.wDay, sys.wYear,
                sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
        fprintf(out, "[%s]: ", level);
        vfprintf(out, fmt, args);
        fputc('\n', out);
        fflush(out);
    }
}

Logger::Logger() : m_Level(LEVEL_OFF), m_ConsoleOpened(false), m_File(nullptr) {}
