#include "Utils.h"

#include <stdio.h>
#include <string.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

namespace utils
{
    bool FileOrDirectoryExists(const char *file)
    {
        if (!file || strcmp(file, "") == 0)
            return false;
        const DWORD attributes = ::GetFileAttributesA(file);
        return attributes != INVALID_FILE_ATTRIBUTES;
    }

    bool DirectoryExists(const char *dir)
    {
        if (!dir || strcmp(dir, "") == 0)
            return false;
        const DWORD attributes = ::GetFileAttributesA(dir);
        return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool IsAbsolutePath(const char *path)
    {
        if (!path || strcmp(path, "") == 0)
            return false;

        if (strlen(path) < 2 || !isalpha(path[0]) || path[1] != ':')
            return false;

        return true;
    }

    bool GetAbsolutePath(char *buffer, size_t size, const char *path, bool trailing)
    {
        if (!path || strcmp(path, "") == 0)
            return false;

        if (!buffer)
            return false;

        size_t len;
        if (IsAbsolutePath(path))
        {
            len = strlen(path);
            strncpy(buffer, path, size);
        }
        else
        {
            size_t n = ::GetCurrentDirectoryA(size, buffer);
            n = size - 1 - n;
            strncat(buffer, "\\", n);
            --n;
            strncat(buffer, path, n);
            len = strlen(path);
        }

        if (trailing && !HasTrailingPathSeparator(path))
        {
            if (size > len + 2)
            {
                buffer[len] = '\\';
                buffer[len + 1] = '\0';
            }
        }
        else if (!trailing && HasTrailingPathSeparator(path))
        {
            buffer[len - 1] = '\0';
        }
        return true;
    }

    char *ConcatPath(char *buffer, size_t size, const char *path1, const char *path2)
    {
        if (!buffer)
            return NULL;

        if (!path1 || strcmp(path1, "") == 0)
        {
            strncpy(buffer, path2, size);
        }
        else if (path2)
        {
            strncpy(buffer, path1, size);
            RemoveTrailingPathSeparator(buffer);
            size_t len2 = strlen(path2);
            size_t n = size - 1 - len2;
            strncat(buffer, "\\", n);
            --n;
            strncat(buffer, path2, n);
        }

        return buffer;
    }

    const char *FindLastPathSeparator(const char *path)
    {
        if (!path || strcmp(path, "") == 0)
            return NULL;

        const char *const lastSep = strrchr(path, '\\');
        const char *const lastAltSep = strrchr(path, '/');
        return (lastAltSep && (!lastSep || lastAltSep > lastSep)) ? lastAltSep : lastSep;
    }

    bool HasTrailingPathSeparator(const char *path)
    {
        if (!path || strcmp(path, "") == 0)
            return false;

        if (path[strlen(path) - 1] == '\\')
            return true;

        return false;
    }

    bool RemoveTrailingPathSeparator(char *path)
    {
        if (!HasTrailingPathSeparator(path))
            return false;
        path[strlen(path) - 1] = '\0';
        return true;
    }

    int CharToWchar(const char *charStr, wchar_t *wcharStr, int size)
    {
        return ::MultiByteToWideChar(CP_ACP, 0, charStr, -1, wcharStr, size);
    }

    int WcharToChar(const wchar_t *wcharStr, char *charStr, int size)
    {
        return ::WideCharToMultiByte(CP_ACP, 0, wcharStr, -1, charStr, size, NULL, NULL);
    }
}
