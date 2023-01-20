#ifndef PLAYER_UTILS_H
#define PLAYER_UTILS_H

#include <wchar.h>

namespace utils
{
    bool FileOrDirectoryExists(const char *file);
    bool DirectoryExists(const char *dir);

    bool IsAbsolutePath(const char *path);
    bool GetAbsolutePath(char *buffer, size_t size, const char *path, bool trailing = false);

    char *ConcatPath(char *buffer, size_t size, const char *path1, const char *path2);

    const char *FindLastPathSeparator(const char *path);
    bool HasTrailingPathSeparator(const char *path);
    bool RemoveTrailingPathSeparator(char *path);

    int CharToWchar(const char *charStr, wchar_t *wcharStr, int size);
    int WcharToChar(const wchar_t *wcharStr, char *charStr, int size);
}

#endif // PLAYER_UTILS_H
