#ifndef BP_LOGGER_H
#define BP_LOGGER_H

#include <stdarg.h>
#include <time.h>

#include "Defines.h"

BP_BEGIN_CDECLS

typedef struct BpLogger BpLogger;

/**
 * @brief Enumeration representing different log levels.
 */
typedef enum BpLogLevel {
    BP_LOG_TRACE = 0,    /**< Lowest log level for tracing detailed operations. */
    BP_LOG_DEBUG = 1,    /**< Log level for debugging information. */
    BP_LOG_INFO = 2,     /**< Log level for informational messages. */
    BP_LOG_WARN = 3,     /**< Log level for warning messages. */
    BP_LOG_ERROR = 4,    /**< Log level for error messages. */
    BP_LOG_FATAL = 5,    /**< Log level for critical/fatal errors. */
    BP_LOG_OFF = 6       /**< Highest log level, turns off logging completely. */
} BpLogLevel;

/**
 * @brief Structure representing log information.
 */
typedef struct BpLogInfo {
    BpLogger *self;     /**< Pointer to the logger itself. */
    BpLogLevel level;   /**< Log level of the message. */
    va_list args;       /**< Variable argument list for formatting log message. */
    const char *format; /**< Format string for log message. */
    struct tm *time;    /**< Pointer to a structure holding date and time information. */
    void *userdata;     /**< User-defined data associated with the log message. */
} BpLogInfo;

/**
 * @brief Function pointer type for log callbacks.
 *
 * This function is responsible for handling log messages.
 *
 * @param info Pointer to the log information structure.
 */
typedef void (*BpLogCallback)(BpLogInfo *info);

/**
 * @brief Function pointer type for log lock functions.
 *
 * This function is responsible for locking or unlocking resources used by the logging system.
 *
 * @param lock If true, lock the resources. If false, unlock the resources.
 * @param userdata User-defined data associated with the logging system.
 */
typedef void (*BpLogLockFunction)(bool lock, void *userdata);

/**
 * @brief Get the logger by name.
 * @param name The name of the logger.
 * @return The logger if found, otherwise create a new one.
 */
BP_EXPORT BpLogger *bpGetLogger(const char *name);

/**
 * @brief Get the default logger.
 * @return The default logger.
 */
BP_EXPORT BpLogger *bpGetDefaultLogger();

/**
 * @brief Set the default logger.
 * @param logger The logger to set as default.
 * @return The previous default logger.
 */
BP_EXPORT void bpSetDefaultLogger(BpLogger *logger);

/**
 * @brief Increase the reference count of the logger.
 * @param logger The logger.
 * @return The new reference count.
 */
BP_EXPORT int bpLoggerAddRef(BpLogger *logger);

/**
 * @brief Decrease the reference count of the logger.
 * @param logger The logger.
 * @return The new reference count.
 */
BP_EXPORT int bpLoggerRelease(BpLogger *logger);

/**
 * @brief Get the name of the logger.
 * @param logger The logger.
 * @return The name of the logger.
 */
BP_EXPORT const char *bpLoggerGetName(BpLogger *logger);

/**
 * @brief Get the log level of the logger.
 * @param logger The logger.
 * @return The log level of the logger.
 */
BP_EXPORT BpLogLevel bpLoggerGetLevel(BpLogger *logger);

/**
 * @brief Get the string representation of a log level.
 * @param logger The logger.
 * @param level The log level to get the string representation of.
 * @return The string representation of the log level.
 */
BP_EXPORT const char *bpLoggerGetLevelString(BpLogger *logger, BpLogLevel level);

/**
 * @brief Set the log level of the logger.
 * @param logger The logger.
 * @param level The log level to set.
 */
BP_EXPORT void bpLoggerSetLevel(BpLogger *logger, BpLogLevel level);

/**
 * @brief Set the lock function and userdata for the logger.
 * @param logger The logger.
 * @param func The lock function to set.
 * @param userdata The userdata to set.
 */
BP_EXPORT void bpLoggerSetLock(BpLogger *logger, BpLogLockFunction func, void *userdata);

/**
 * @brief Add a log callback to the logger.
 * @param logger The logger.
 * @param callback The log callback function.
 * @param userdata The userdata to pass to the callback.
 * @param level The log level to filter the messages.
 * @return True if the callback was added successfully, false otherwise.
 */
BP_EXPORT bool bpLoggerAddCallback(BpLogger *logger, BpLogCallback callback, void *userdata, BpLogLevel level);

/**
 * @brief Clear all log callbacks from the logger.
 * @param logger The logger.
 */
BP_EXPORT void bpLoggerClearCallbacks(BpLogger *logger);

/**
 * @brief Log a message with the specified log level.
 * @param logger The logger.
 * @param level The log level of the message.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLoggerLog(BpLogger *logger, BpLogLevel level, const char *format, ...);

/**
 * @brief Log a message with the specified log level and variable arguments list.
 * @param logger The logger.
 * @param level The log level of the message.
 * @param format The format string of the message.
 * @param args The variable arguments list for the format string.
 */
BP_EXPORT void bpLoggerLogV(BpLogger *logger, BpLogLevel level, const char *format, va_list args);

/**
 * @brief Log a string message.
 * @param logger The logger.
 * @param level The log level of the message.
 * @param str The string message.
 */
BP_EXPORT void bpLoggerLogString(BpLogger *logger, BpLogLevel level, const char *str);

/**
 * @brief Log a trace message with the specified format and arguments.
 * @param logger The logger.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLoggerTrace(BpLogger *logger, const char *format, ...);

/**
 * @brief Log a debug message with the specified format and arguments.
 * @param logger The logger.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLoggerDebug(BpLogger *logger, const char *format, ...);

/**
 * @brief Log an info message with the specified format and arguments.
 * @param logger The logger.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLoggerInfo(BpLogger *logger, const char *format, ...);

/**
 * @brief Log a warning message with the specified format and arguments.
 * @param logger The logger.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLoggerWarn(BpLogger *logger, const char *format, ...);

/**
 * @brief Log an error message with the specified format and arguments.
 * @param logger The logger.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLoggerError(BpLogger *logger, const char *format, ...);

/**
 * @brief Log a fatal error message with the specified format and arguments.
 * @param logger The logger.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLoggerFatal(BpLogger *logger, const char *format, ...);

/**
 * @brief Log a message with the specified log level.
 * @param level The log level of the message.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLog(BpLogLevel level, const char *format, ...);

/**
 * @brief Log a message with the specified log level and variable arguments list.
 * @param level The log level of the message.
 * @param format The format string of the message.
 * @param args The variable arguments list for the format string.
 */
BP_EXPORT void bpLogV(BpLogLevel level, const char *format, va_list args);

/**
 * @brief Log a string message.
 * @param level The log level of the message.
 * @param str The string message.
 */
BP_EXPORT void bpLogString(BpLogLevel level, const char *str);

/**
 * @brief Log a trace message with the specified format and arguments.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLogTrace(const char *format, ...);

/**
 * @brief Log a debug message with the specified format and arguments.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLogDebug(const char *format, ...);

/**
 * @brief Log an info message with the specified format and arguments.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLogInfo(const char *format, ...);

/**
 * @brief Log a warning message with the specified format and arguments.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLogWarn(const char *format, ...);

/**
 * @brief Log an error message with the specified format and arguments.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLogError(const char *format, ...);

/**
 * @brief Log a fatal error message with the specified format and arguments.
 * @param format The format string of the message.
 * @param ... The variable arguments list for the format string.
 */
BP_EXPORT void bpLogFatal(const char *format, ...);

BP_END_CDECLS

#endif // BP_LOGGER_H
