#ifndef BP_UTILS_H
#define BP_UTILS_H

#include "Defines.h"

BP_BEGIN_CDECLS

/**
 * @brief Create a buffer.
 * @param size The size of the buffer.
 * @return The buffer.
 */
BP_EXPORT char *bpCreateBuffer(size_t size);

/**
 * @brief Copy a buffer.
 * @param buffer The buffer to copy.
 * @param size The size of the buffer.
 * @return The copied buffer.
 */
BP_EXPORT char *bpCopyBuffer(const char *buffer, size_t size);

/**
 * @brief Destroy a buffer.
 * @param buffer The buffer to destroy.
 */
BP_EXPORT void bpDestroyBuffer(char *buffer);

/**
 * @brief Check if a file exists.
 * @param file The file to check.
 * @return True if the file exists, false otherwise.
 */
BP_EXPORT bool bpFileOrDirectoryExists(const char *file);

/**
 * @brief Check if a directory exists.
 * @param dir The directory to check.
 * @return True if the directory exists, false otherwise.
 */
BP_EXPORT  bool bpDirectoryExists(const char *dir);

/**
 * @brief Check if a path is absolute.
 * @param path The path to check.
 * @return True if the path is absolute, false otherwise.
 */
BP_EXPORT bool bpIsAbsolutePath(const char *path);

/**
 * @brief Get the absolute path.
 * @param buffer The buffer to store the absolute path.
 * @param size The size of the buffer.
 * @param path The path to convert.
 * @param trailing True to add a trailing path separator, false otherwise.
 * @return True if the path is successfully converted, false otherwise.
 */
BP_EXPORT bool bpGetAbsolutePath(char *buffer, size_t size, const char *path, bool trailing = false);

/**
 * @brief Concatenate two paths.
 * @param buffer The buffer to store the concatenated path.
 * @param size The size of the buffer.
 * @param path1 The first path.
 * @param path2 The second path.
 * @return The concatenated path.
 */
BP_EXPORT char *bpConcatPath(char *buffer, size_t size, const char *path1, const char *path2);

/**
 * @brief Find the last path separator in a path.
 * @param path The path to search.
 * @return The last path separator in the path.
 */
BP_EXPORT const char *bpFindLastPathSeparator(const char *path);

/**
 * @brief Check if a path has a trailing path separator.
 * @param path The path to check.
 * @return True if the path has a trailing path separator, false otherwise.
 */
BP_EXPORT bool bpHasTrailingPathSeparator(const char *path);

/**
 * @brief Remove the trailing path separator from a path.
 * @param path The path to remove the trailing path separator.
 * @return True if the trailing path separator is successfully removed, false otherwise.
 */
BP_EXPORT bool bpRemoveTrailingPathSeparator(char *path);

/**
 * @brief Convert an ANSI string to a UTF-16 string.
 * @param charStr The ANSI string to convert.
 * @param wcharStr The buffer to store the UTF-16 string.
 * @param size The size of the buffer.
 * @return The number of characters converted.
 */
BP_EXPORT int bpAnsiToUtf16(const char *charStr, wchar_t *wcharStr, int size);

/**
 * @brief Convert a UTF-16 string to an ANSI string.
 * @param wcharStr The UTF-16 string to convert.
 * @param charStr The buffer to store the ANSI string.
 * @param size The size of the buffer.
 * @return The number of characters converted.
 */

BP_EXPORT int bpUtf16ToAnsi(const wchar_t *wcharStr, char *charStr, int size);

/**
 * @brief Convert a UTF-8 string to a UTF-16 string.
 * @param charStr The UTF-8 string to convert.
 * @param wcharStr The buffer to store the UTF-16 string.
 * @param size The size of the buffer.
 * @return The number of characters converted.
 */
BP_EXPORT int bpUtf16ToUtf8(const wchar_t *wcharStr, char *charStr, int size);

/**
 * @brief Convert a UTF-16 string to a UTF-8 string.
 * @param wcharStr The UTF-16 string to convert.
 * @param charStr The buffer to store the UTF-8 string.
 * @param size The size of the buffer.
 * @return The number of characters converted.
 */
BP_EXPORT int bpUtf8ToUtf16(const char *charStr, wchar_t *wcharStr, int size);

/**
 * @brief Calculate the CRC32 checksum.
 * @param key The key to calculate the checksum.
 * @param len The length of the key.
 * @param seed The seed value.
 * @param out The output buffer.
 */
BP_EXPORT void bpCRC32(const void *key, size_t len, size_t seed, void *out);

/**
 * @brief Convert a string to a pixel format.
 * @param str The string to convert.
 * @param max The maximum length of the string.
 * @return The pixel format.
 */
BP_EXPORT int bpString2PixelFormat(const char *str, size_t max);

/**
 * @brief Convert a pixel format to a string.
 * @param format The pixel format.
 * @return The string representation of the pixel format.
 */
BP_EXPORT const char *bpPixelFormat2String(int format);

BP_END_CDECLS

#endif // BP_UTILS_H
