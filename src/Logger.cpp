#include "Logger.h"

#include <string.h>
#include <time.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
#define PLAYER_VA_COPY(dest, src) ((dest) = (src))
#else
#define PLAYER_VA_COPY(dest, src) va_copy(dest, src)
#endif

CLogger &CLogger::Get()
{
    static CLogger logger;
    return logger;
}

CLogger::~CLogger()
{
    Close();
}

void CLogger::Open(const char *filename, bool overwrite, int level)
{
    m_Level = level;

    if (m_File)
        return;

    if (overwrite)
        m_File = fopen(filename, "w");
    else
        m_File = fopen(filename, "a");
}

void CLogger::Close()
{
    if (m_ConsoleOpened)
    {
#ifdef _WIN32
        ::FreeConsole();
#endif
        m_ConsoleOpened = false;
    }

    if (m_File)
    {
        fclose(m_File);
        m_File = NULL;
    }
}

void CLogger::OpenConsole(bool opened)
{
#ifdef _WIN32
    if (opened)
    {
        ::AllocConsole();
        freopen("CONOUT$", "w", stdout);
        m_ConsoleOpened = true;
    }
    else
    {
        freopen("CON", "w", stdout);
        ::FreeConsole();
        m_ConsoleOpened = false;
    }
#else
    m_ConsoleOpened = opened;
#endif
}

void CLogger::Debug(const char *fmt, ...)
{
    if (m_Level >= LEVEL_DEBUG)
    {
        va_list args;
        va_start(args, fmt);
        Log("DEBUG", fmt, args);
        va_end(args);
    }
}

void CLogger::Info(const char *fmt, ...)
{
    if (m_Level >= LEVEL_INFO)
    {
        va_list args;
        va_start(args, fmt);
        Log("INFO", fmt, args);
        va_end(args);
    }
}

void CLogger::Warn(const char *fmt, ...)
{
    if (m_Level >= LEVEL_WARN)
    {
        va_list args;
        va_start(args, fmt);
        Log("WARN", fmt, args);
        va_end(args);
    }
}

void CLogger::Error(const char *fmt, ...)
{
    if (m_Level >= LEVEL_ERROR)
    {
        va_list args;
        va_start(args, fmt);
        Log("ERROR", fmt, args);
        va_end(args);
    }
}

void CLogger::Log(const char *level, const char *fmt, va_list args)
{
#ifdef _WIN32
    SYSTEMTIME sys;
    GetLocalTime(&sys);
#else
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    struct tm sys;
#if defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    localtime_r(&ts.tv_sec, &sys);
#else
    struct tm *local = localtime(&ts.tv_sec);
    if (local)
        sys = *local;
    else
        memset(&sys, 0, sizeof(sys));
#endif
#endif

    FILE *outs[] = {stdout, m_File};

    for (int i = 0; i < 2; ++i)
    {
        FILE *out = outs[i];
        if (!out)
            continue;

#ifdef _WIN32
        fprintf(out, "[%02d/%02d/%d %02d:%02d:%02d.%03d] ",
                sys.wMonth, sys.wDay, sys.wYear,
                sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
#else
        fprintf(out, "[%02d/%02d/%d %02d:%02d:%02d.%03ld] ",
                sys.tm_mon + 1, sys.tm_mday, sys.tm_year + 1900,
                sys.tm_hour, sys.tm_min, sys.tm_sec, ts.tv_nsec / 1000000L);
#endif
        fprintf(out, "[%s]: ", level);
        va_list argsCopy;
        PLAYER_VA_COPY(argsCopy, args);
        vfprintf(out, fmt, argsCopy);
        va_end(argsCopy);
        fputc('\n', out);
        fflush(out);
    }
}

CLogger::CLogger() : m_Level(LEVEL_OFF), m_ConsoleOpened(false), m_File(NULL) {}
