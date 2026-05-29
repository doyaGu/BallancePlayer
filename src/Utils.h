#ifndef PLAYER_UTILS_H
#define PLAYER_UTILS_H

#include <wchar.h>

#include "VxMathDefines.h"
#include "XString.h"

namespace utils
{
    bool FileOrDirectoryExists(const char *file);
    bool DirectoryExists(const char *dir);

    size_t GetCurrentPath(char *buffer, size_t size);

    bool IsAbsolutePath(const char *path);
    bool GetAbsolutePath(char *buffer, size_t size, const char *path, bool trailing = false);
    bool GetFileDirectory(char *buffer, size_t size, const char *filename, bool trailing = true);

    char *ConcatPath(char *buffer, size_t size, const char *path1, const char *path2);
    XString GetExecutableDirectory();
    XString GetFileDirectory(const char *filename, bool trailing = true);
    XString JoinPath(const char *path1, const char *path2, bool trailing = false);
    XString ResolvePathAgainstBase(const char *basePath, const char *path, bool trailing = false);
    XString WithTrailingPathSeparator(const char *path);
    XString WithoutTrailingPathSeparator(const char *path);

    const char *FindLastPathSeparator(const char *path);
    bool HasTrailingPathSeparator(const char *path);
    bool RemoveTrailingPathSeparator(char *path);

    int CharToWchar(const char *charStr, wchar_t *wcharStr, size_t size);
    int WcharToChar(const wchar_t *wcharStr, char *charStr, size_t size);

    void CRC32(const void *key, size_t len, unsigned int seed, unsigned int *out);

    VX_PIXELFORMAT String2PixelFormat(const char *str, size_t max);
    const char *PixelFormat2String(VX_PIXELFORMAT format);

    bool IniGetString(const char *section, const char *name, char *str, size_t size, const char *filename);
    bool IniGetInteger(const char *section, const char *name, int &value, const char *filename);
    bool IniGetBoolean(const char *section, const char *name, bool &value, const char *filename);
    bool IniGetPixelFormat(const char *section, const char *name, VX_PIXELFORMAT &value, const char *filename);
    bool IniSetString(const char *section, const char *name, const char *str, const char *filename);
    bool IniSetInteger(const char *section, const char *name, int value, const char *filename);
    bool IniSetBoolean(const char *section, const char *name, bool value, const char *filename);
    bool IniSetPixelFormat(const char *section, const char *name, VX_PIXELFORMAT value, const char *filename);

    // Returns the monitor bounds for the monitor nearest to the specified window.
    // Falls back to primary monitor metrics if multi-monitor APIs are unavailable.
    bool GetMonitorRectForWindow(WIN_HANDLE window, CKRECT &outRect);
}

#endif // PLAYER_UTILS_H
