#include "Logger.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

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
        ::FreeConsole();
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
    SYSTEMTIME sys;
    GetLocalTime(&sys);

    FILE *outs[] = {stdout, m_File};

    for (int i = 0; i < 2; ++i)
    {
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

CLogger::CLogger() : m_Level(LEVEL_OFF), m_ConsoleOpened(false), m_File(NULL) {}