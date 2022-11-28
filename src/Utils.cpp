#include "Utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <ShlObj.h>

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

    bool BrowsePath(const char *title, char *path)
    {
        BROWSEINFOA bInfo;
        memset(&bInfo, 0, sizeof(BROWSEINFOA));
        bInfo.hwndOwner = ::GetForegroundWindow();
        bInfo.lpszTitle = title;
        bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_UAHINT | BIF_NONEWFOLDERBUTTON;

        LPITEMIDLIST lpDlist;
        lpDlist = ::SHBrowseForFolderA(&bInfo);
        if (!lpDlist)
            return false;

        ::SHGetPathFromIDListA(lpDlist, path);
        return true;
    }

    bool CreatePathTree(const char *path)
    {
        if (!path || strcmp(path, "") == 0)
            return false;

        const size_t sz = strlen(path);
        if (sz <= 2)
            return false;

        char *pt = new char[sz + 1];
        memcpy(pt, path, sz + 1);

        for (char *pch = &pt[3]; *pch != '\0'; ++pch)
        {
            if (*pch != '/' && *pch != '\\')
                continue;
            *pch = '\0';
            if (::GetFileAttributesA(pt) == -1)
                if (!::CreateDirectoryA(pt, NULL))
                    break;
            *pch = '\\';
        }

        delete[] pt;
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
