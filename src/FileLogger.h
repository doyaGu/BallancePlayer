#ifndef BP_FILELOGGER_H
#define BP_FILELOGGER_H

#include <cstdio>
#include <ctime>

#include "bp/Logger.h"

class FileLogger {
public:
    FileLogger() = default;

    FileLogger(const FileLogger &) = delete;
    FileLogger(FileLogger &&) = delete;

    ~FileLogger() { Release(); }

    FileLogger &operator=(const FileLogger &) = delete;
    FileLogger &operator=(FileLogger &&) = delete;

    bool Attach(BpLogger *logger, const char *filename, bool overwrite) {
        if (!logger || m_Attached)
            return false;

        if (overwrite)
            m_File = fopen(filename, "w");
        else
            m_File = fopen(filename, "a");

        if (!m_File) {
            bpLoggerWarn(m_Logger, "Failed to open log file: %s", filename);
            return false;
        }

        bpLoggerAddCallback(logger, [](BpLogInfo *info){
            char buf[16];
            buf[strftime(buf, sizeof(buf), "%H:%M:%S", info->time)] = '\0';

            FILE *fp = (FILE *) info->userdata;
            fprintf(fp, "[%s] [%s/%s]: ", buf, bpLoggerGetName(info->self), bpGetLogLevelString(info->level));

            vfprintf(fp, info->format, info->args);
            fprintf(fp, "\n");
            fflush(fp);
        }, m_File, BP_LOG_TRACE);

        m_Logger = logger;
        m_Attached = true;
        return true;
    }

    void Release() {
        if (m_Attached) {
            if (m_File)
                fclose(m_File);
        }
    }

private:
    bool m_Attached = false;
    BpLogger *m_Logger = nullptr;
    FILE *m_File = nullptr;
};

#endif // BP_FILELOGGER_H
