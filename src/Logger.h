#ifndef PLAYER_LOGGER_H
#define PLAYER_LOGGER_H

#include <cstdarg>
#include <cstdio>

class Logger {
public:
    enum Level {
        LEVEL_OFF = 0,
        LEVEL_ERROR = 1,
        LEVEL_WARN = 2,
        LEVEL_INFO = 3,
        LEVEL_DEBUG = 4,
    };

    static Logger &Get();

    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;

    ~Logger();

    Logger &operator=(const Logger &) = delete;
    Logger &operator=(Logger &&) = delete;

    void Open(const char *filename, bool overwrite = true, int level = LEVEL_INFO);
    void Close();

    int GetLevel() const;
    void SetLevel(int level);

    void Debug(const char *fmt, ...);
    void Info(const char *fmt, ...);
    void Warn(const char *fmt, ...);
    void Error(const char *fmt, ...);

private:
    void Log(const char *level, const char *fmt, va_list args);

    Logger();

    int m_Level;
    bool m_ConsoleOpened;
    FILE *m_File;
};

#endif // PLAYER_LOGGER_H
