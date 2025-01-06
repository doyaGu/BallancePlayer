#ifndef BP_LOGGER_PRIVATE_H
#define BP_LOGGER_PRIVATE_H

#include "bp/Logger.h"

/**
 * @interface BpLogger
 * @brief The utility interface of logger.
 * @warning Mod should have no classes implementing this interface.
 */
struct BpLogger {
    /**
     * @brief Increase the reference count of the logger object.
     * @return The new reference count.
     */
    virtual int AddRef() const = 0;

    /**
     * @brief Decrease the reference count of the logger object.
     * @return The new reference count.
     */
    virtual int Release() const = 0;

    /**
     * @brief Get the name of the logger.
     * @return The name of the logger.
     */
    virtual const char *GetName() const = 0;

    /**
     * @brief Get the current log level of the logger.
     * @return The current log level of the logger.
     */
    virtual BpLogLevel GetLevel() const = 0;

    /**
     * @brief Get the string representation of a log level.
     * @param level The log level to get the string representation of.
     * @return The string representation of the log level.
     */
    virtual const char *GetLevelString(BpLogLevel level) const = 0;

    /**
     * @brief Set the log level for the logger.
     * @param level The log level to set.
     */
    virtual void SetLevel(BpLogLevel level) = 0;

    /**
     * @brief Set the lock function and userdata for thread synchronization.
     * @param func The lock function to set.
     * @param userdata The userdata to set.
     */
    virtual void SetLock(BpLogLockFunction func, void *userdata) = 0;

    /**
     * @brief Add a log callback function with userdata and log level.
     * @param callback The log callback function to add.
     * @param fn The log callback function to add.
     * @param userdata The userdata to pass to the log callback function.
     * @param level The log level at which the callback should be triggered.
     * @return True if the callback was added successfully, false otherwise.
     */
    virtual bool AddCallback(BpLogCallback callback, void *userdata, BpLogLevel level) = 0;

    /**
     * @brief Clear all log callbacks.
     */
    virtual void ClearCallbacks() = 0;

    /**
     * @brief Log a message with the specified log level, format, and arguments.
     * @param level The log level of the message.
     * @param format The format string of the message.
     * @param args The variable arguments list for the format string.
     */
    virtual void Log(BpLogLevel level, const char *format, va_list args) = 0;

    /**
     * @brief Log a trace message with the specified format and arguments.
     * @param format The format string of the message.
     * @param ... The variable arguments list for the format string.
     */
    void Trace(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_TRACE, format, args);
        va_end(args);
    }

    /**
     * @brief Log a debug message with the specified format and arguments.
     * @param format The format string of the message.
     * @param ... The variable arguments list for the format string.
     */
    void Debug(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_DEBUG, format, args);
        va_end(args);
    }

    /**
     * @brief Log an info message with the specified format and arguments.
     * @param format The format string of the message.
     * @param ... The variable arguments list for the format string.
     */
    void Info(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_INFO, format, args);
        va_end(args);
    }

    /**
     * @brief Log a warning message with the specified format and arguments.
     * @param format The format string of the message.
     * @param ... The variable arguments list for the format string.
     */
    void Warn(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_WARN, format, args);
        va_end(args);
    }

    /**
     * @brief Log an error message with the specified format and arguments.
     * @param format The format string of the message.
     * @param ... The variable arguments list for the format string.
     */
    void Error(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_ERROR, format, args);
        va_end(args);
    }

    /**
     * @brief Log a fatal error message with the specified format and arguments.
     * @param format The format string of the message.
     * @param ... The variable arguments list for the format string.
     */
    void Fatal(const char *format, ...) {
        va_list args;
        va_start(args, format);
        Log(BP_LOG_FATAL, format, args);
        va_end(args);
    }

protected:
    virtual ~BpLogger() = default;
};

#endif // BP_LOGGER_PRIVATE_H
