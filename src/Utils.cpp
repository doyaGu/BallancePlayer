#include "bp/Utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

#include "VxMathDefines.h"

char *bpCreateBuffer(size_t size) {
    if (size == 0)
        return nullptr;

    try {
        return new char[size](); // Zero-initialize buffer
    } catch (const std::bad_alloc &) {
        return nullptr; // Handle allocation failure
    }
}

char *bpCopyBuffer(const char *buffer, size_t size) {
    if (!buffer || size == 0)
        return nullptr;

    try {
        char *newBuffer = new char[size];
        memcpy(newBuffer, buffer, size);
        return newBuffer;
    } catch (const std::bad_alloc &) {
        return nullptr; // Handle allocation failure
    }
}

void bpDestroyBuffer(char *buffer) {
    if (buffer)
        delete[] buffer;
}

bool bpGetCurrentDirectory(char *buffer, size_t size) {
    if (!buffer || size == 0)
        return false;
    return ::GetCurrentDirectoryA(size, buffer) != 0;
}

bool bpFileOrDirectoryExists(const char *file) {
    if (!file || file[0] == '\0')
        return false;
    const DWORD attributes = ::GetFileAttributesA(file);
    return attributes != INVALID_FILE_ATTRIBUTES;
}

bool bpDirectoryExists(const char *dir) {
    if (!dir || dir[0] == '\0')
        return false;
    const DWORD attributes = ::GetFileAttributesA(dir);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool bpIsAbsolutePath(const char *path) {
    if (!path || path[0] == '\0')
        return false;

    if (strlen(path) < 2 || !isalpha(path[0]) || path[1] != ':')
        return false;

    return true;
}

bool bpGetAbsolutePath(char *buffer, size_t size, const char *path, bool trailing) {
    if (!path || path[0] == '\0')
        return false;

    if (!buffer)
        return false;

    size_t len;
    if (bpIsAbsolutePath(path)) {
        len = strlen(path);
        strncpy(buffer, path, size);
    } else {
        size_t n = ::GetCurrentDirectoryA(size, buffer);
        n = size - 1 - n;
        strncat(buffer, "\\", n);
        --n;
        strncat(buffer, path, n);
        len = strlen(path);
    }

    if (trailing && !bpHasTrailingPathSeparator(path)) {
        if (size > len + 2) {
            buffer[len] = '\\';
            buffer[len + 1] = '\0';
        }
    } else if (!trailing && bpHasTrailingPathSeparator(path)) {
        buffer[len - 1] = '\0';
    }
    return true;
}

char *bpConcatPath(char *buffer, size_t size, const char *path1, const char *path2) {
    if (!buffer || size == 0)
        return nullptr;

    if (!path1 || path1[0] == '\0') {
        if (path2) {
            strncpy(buffer, path2, size - 1);
            buffer[size - 1] = '\0'; // Ensure null termination
        } else {
            buffer[0] = '\0';
        }
        return buffer;
    }

    if (!path2 || path2[0] == '\0') {
        strncpy(buffer, path1, size - 1);
        buffer[size - 1] = '\0';
        return buffer;
    }

    // Copy first path
    size_t path1_len = strlen(path1);
    if (path1_len >= size - 1) {
        // First path too long, truncate
        strncpy(buffer, path1, size - 2);
        buffer[size - 2] = '\0';
        path1_len = size - 2;
    } else {
        strcpy(buffer, path1);
    }

    // Check if first path has separator
    bool needsSeparator = true;
    if (path1_len > 0) {
#ifdef _WIN32
        needsSeparator = (path1[path1_len - 1] != '\\' && path1[path1_len - 1] != '/');
#else
        needsSeparator = (path1[path1_len - 1] != '/');
#endif
    }

    // Add separator if needed
    if (needsSeparator) {
        if (path1_len >= size - 2) {
            return buffer; // No room for separator and path2
        }
#ifdef _WIN32
        buffer[path1_len] = '\\';
#else
        buffer[path1_len] = '/';
#endif
        buffer[path1_len + 1] = '\0';
        path1_len++;
    }

    // Add second path
    size_t remaining = size - path1_len - 1;
    if (remaining > 0) {
        strncat(buffer + path1_len, path2, remaining);
        buffer[size - 1] = '\0'; // Ensure null termination
    }

    return buffer;
}

const char *bpFindLastPathSeparator(const char *path) {
    if (!path || path[0] == '\0')
        return nullptr;

    const char *lastSlash = strrchr(path, '/');

#ifdef _WIN32
    const char *lastBackslash = strrchr(path, '\\');

    // Return the one that appears later in the string
    if (!lastSlash) return lastBackslash;
    if (!lastBackslash) return lastSlash;

    return (lastSlash > lastBackslash) ? lastSlash : lastBackslash;
#else
    return lastSlash;
#endif
}

bool bpHasTrailingPathSeparator(const char *path) {
    if (!path || path[0] == '\0')
        return false;

    size_t len = strlen(path);
    if (len == 0)
        return false;

#ifdef _WIN32
    return (path[len - 1] == '\\' || path[len - 1] == '/');
#else
    return (path[len - 1] == '/');
#endif
}

bool bpRemoveTrailingPathSeparator(char *path) {
    if (!bpHasTrailingPathSeparator(path))
        return false;
    path[strlen(path) - 1] = '\0';
    return true;
}

int bpAnsiToUtf16(const char *charStr, wchar_t *wcharStr, int size) {
    return ::MultiByteToWideChar(CP_ACP, 0, charStr, -1, wcharStr, size);
}

int bpUtf16ToAnsi(const wchar_t *wcharStr, char *charStr, int size) {
    return ::WideCharToMultiByte(CP_ACP, 0, wcharStr, -1, charStr, size, nullptr, nullptr);
}

int bpUtf16ToUtf8(const wchar_t *wcharStr, char *charStr, int size) {
    return ::WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, charStr, size, nullptr, nullptr);
}

int bpUtf8ToUtf16(const char *charStr, wchar_t *wcharStr, int size) {
    return ::MultiByteToWideChar(CP_UTF8, 0, charStr, -1, wcharStr, size);
}

/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

// Table of CRC-32's of all single-byte values (made by make_crc_table)
static const size_t crc_table[256] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
    0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
    0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
    0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
    0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
    0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
    0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
    0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
    0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
    0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
    0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
    0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
    0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
    0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
    0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
    0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
    0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
    0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
    0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
    0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
    0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
    0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
    0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
    0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
    0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
    0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
    0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
    0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
    0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
    0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
    0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
    0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
    0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
    0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
    0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
    0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
    0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
    0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
    0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
    0x2d02ef8dL
};

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8)
#define DO2(buf) DO1(buf); DO1(buf)
#define DO4(buf) DO2(buf); DO2(buf)
#define DO8(buf) DO4(buf); DO4(buf)

void bpCRC32(const void *key, size_t len, size_t seed, void *out) {
    unsigned char *buf = (unsigned char *) key;
    size_t crc = seed ^ 0xffffffffL;

    while (len >= 8) {
        DO8(buf);
        len -= 8;
    }

    while (len--) {
        DO1(buf);
    }

    crc ^= 0xffffffffL;

    *(size_t *) out = crc;
}

int bpString2PixelFormat(const char *str) {
    if (!str || str[0] == '\0')
        return UNKNOWN_PF;

    size_t max = strlen(str);

    int format = UNKNOWN_PF;
    if (strncmp(str, "_32_ARGB8888", max) == 0)
        format = _32_ARGB8888;
    else if (strncmp(str, "_32_RGB888", max) == 0)
        format = _32_RGB888;
    else if (strncmp(str, "_24_RGB888", max) == 0)
        format = _24_RGB888;
    else if (strncmp(str, "_16_RGB565", max) == 0)
        format = _16_RGB565;
    else if (strncmp(str, "_16_RGB555", max) == 0)
        format = _16_RGB555;
    else if (strncmp(str, "_16_ARGB1555", max) == 0)
        format = _16_ARGB1555;
    else if (strncmp(str, "_16_ARGB4444", max) == 0)
        format = _16_ARGB4444;
    else if (strncmp(str, "_8_RGB332", max) == 0)
        format = _8_RGB332;
    else if (strncmp(str, "_8_ARGB2222", max) == 0)
        format = _8_ARGB2222;
    else if (strncmp(str, "_32_ABGR8888", max) == 0)
        format = _32_ABGR8888;
    else if (strncmp(str, "_32_RGBA8888", max) == 0)
        format = _32_RGBA8888;
    else if (strncmp(str, "_32_BGRA8888", max) == 0)
        format = _32_BGRA8888;
    else if (strncmp(str, "_32_BGR888", max) == 0)
        format = _32_BGR888;
    else if (strncmp(str, "_24_BGR888", max) == 0)
        format = _24_BGR888;
    else if (strncmp(str, "_16_BGR565", max) == 0)
        format = _16_BGR565;
    else if (strncmp(str, "_16_BGR555", max) == 0)
        format = _16_BGR555;
    else if (strncmp(str, "_16_ABGR1555", max) == 0)
        format = _16_ABGR1555;
    else if (strncmp(str, "_16_ABGR4444", max) == 0)
        format = _16_ABGR4444;
    else if (strncmp(str, "_DXT1", max) == 0)
        format = _DXT1;
    else if (strncmp(str, "_DXT2", max) == 0)
        format = _DXT2;
    else if (strncmp(str, "_DXT3", max) == 0)
        format = _DXT3;
    else if (strncmp(str, "_DXT4", max) == 0)
        format = _DXT4;
    else if (strncmp(str, "_DXT5", max) == 0)
        format = _DXT5;
    else if (strncmp(str, "_16_V8U8", max) == 0)
        format = _16_V8U8;
    else if (strncmp(str, "_32_V16U16", max) == 0)
        format = _32_V16U16;
    else if (strncmp(str, "_16_L6V5U5", max) == 0)
        format = _16_L6V5U5;
    else if (strncmp(str, "_32_X8L8V8U8", max) == 0)
        format = _32_X8L8V8U8;
    else if (strncmp(str, "_8_ABGR8888_CLUT", max) == 0)
        format = _8_ABGR8888_CLUT;
    else if (strncmp(str, "_8_ARGB8888_CLUT", max) == 0)
        format = _8_ARGB8888_CLUT;
    else if (strncmp(str, "_4_ABGR8888_CLUT", max) == 0)
        format = _4_ABGR8888_CLUT;
    else if (strncmp(str, "_4_ARGB8888_CLUT", max) == 0)
        format = _4_ARGB8888_CLUT;
    return format;
}

const char *bpPixelFormat2String(int format) {
    const char *str;
    switch (format) {
    case _32_ARGB8888:
        str = "_32_ARGB8888";
        break;
    case _32_RGB888:
        str = "_32_RGB888";
        break;
    case _24_RGB888:
        str = "_24_RGB888";
        break;
    case _16_RGB565:
        str = "_16_RGB565";
        break;
    case _16_RGB555:
        str = "_16_RGB555";
        break;
    case _16_ARGB1555:
        str = "_16_ARGB1555";
        break;
    case _16_ARGB4444:
        str = "_16_ARGB4444";
        break;
    case _8_RGB332:
        str = "_8_RGB332";
        break;
    case _8_ARGB2222:
        str = "_8_ARGB2222";
        break;
    case _32_ABGR8888:
        str = "_32_ABGR8888";
        break;
    case _32_RGBA8888:
        str = "_32_RGBA8888";
        break;
    case _32_BGRA8888:
        str = "_32_BGRA8888";
        break;
    case _32_BGR888:
        str = "_32_BGR888";
        break;
    case _24_BGR888:
        str = "_24_BGR888";
        break;
    case _16_BGR565:
        str = "_16_BGR565";
        break;
    case _16_BGR555:
        str = "_16_BGR555";
        break;
    case _16_ABGR1555:
        str = "_16_ABGR1555";
        break;
    case _16_ABGR4444:
        str = "_16_ABGR4444";
        break;
    case _DXT1:
        str = "_DXT1";
        break;
    case _DXT2:
        str = "_DXT2";
        break;
    case _DXT3:
        str = "_DXT3";
        break;
    case _DXT4:
        str = "_DXT4";
        break;
    case _DXT5:
        str = "_DXT5";
        break;
    case _16_V8U8:
        str = "_16_V8U8";
        break;
    case _32_V16U16:
        str = "_32_V16U16";
        break;
    case _16_L6V5U5:
        str = "_16_L6V5U5";
        break;
    case _32_X8L8V8U8:
        str = "_32_X8L8V8U8";
        break;
    case _8_ABGR8888_CLUT:
        str = "_8_ABGR8888_CLUT";
        break;
    case _8_ARGB8888_CLUT:
        str = "_8_ARGB8888_CLUT";
        break;
    case _4_ABGR8888_CLUT:
        str = "_4_ABGR8888_CLUT";
        break;
    case _4_ARGB8888_CLUT:
        str = "_4_ARGB8888_CLUT";
        break;
    default:
        str = "UNKNOWN_PF";
        break;
    }

    return str;
}

bool bpIniGetString(const char *section, const char *name, char *str, size_t size, const char *filename) {
    if (!section || !name || !str || !filename || size <= 0) {
        return false;
    }

    DWORD result = ::GetPrivateProfileStringA(section, name, "", str, size, filename);
    return (result > 0 || GetLastError() == ERROR_SUCCESS);
}

bool bpIniSetString(const char *section, const char *name, const char *str, const char *filename) {
    if (!section || !name || !str || !filename) {
        return false;
    }

    return ::WritePrivateProfileStringA(section, name, str, filename) != 0;
}

bool bpIniGetBoolean(const char *section, const char *name, bool *value, const char *filename) {
    if (!section || !name || !filename) {
        return false;
    }

    const DWORD val = ::GetPrivateProfileIntA(section, name, -1, filename);
    if (val == -1 && GetLastError() != ERROR_SUCCESS) {
        return false;
    }

    char buf[8] = {0};
    if (bpIniGetString(section, name, buf, sizeof(buf), filename)) {
        if (_stricmp(buf, "true") == 0 || _stricmp(buf, "yes") == 0 || _stricmp(buf, "on") == 0) {
            if (value)
                *value = true;
            return true;
        }
        if (_stricmp(buf, "false") == 0 || _stricmp(buf, "no") == 0 || _stricmp(buf, "off") == 0) {
            if (value)
                *value = false;
            return true;
        }
    }

    if (value)
        *value = (val == 1);
    return true;
}

bool bpIniSetBoolean(const char *section, const char *name, bool value, const char *filename) {
    if (!section || !name || !filename) {
        return false;
    }

    const char *buf = (value) ? "1" : "0";
    return bpIniSetString(section, name, buf, filename);
}

bool bpIniGetInteger(const char *section, const char *name, int *value, const char *filename) {
    if (!section || !name || !filename) {
        return false;
    }

    char buf[512] = {0};
    if (!bpIniGetString(section, name, buf, sizeof(buf), filename) || buf[0] == '\0') {
        return false;
    }

    char *endptr = nullptr;
    errno = 0;
    long val = strtol(buf, &endptr, 10);

    // Check for conversion errors
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) ||
        (errno != 0 && val == 0) ||
        (endptr == buf) ||
        (*endptr != '\0')) {
        return false;
    }

    if (value)
        *value = static_cast<int>(val);
    return true;
}

bool bpIniSetInteger(const char *section, const char *name, int value, const char *filename) {
    if (!section || !name || !filename) {
        return false;
    }

    char buf[64] = {0};
    int result = snprintf(buf, sizeof(buf), "%d", value);
    if (result < 0 || result >= static_cast<int>(sizeof(buf))) {
        return false;
    }

    return bpIniSetString(section, name, buf, filename);
}

bool bpIniGetFloat(const char *section, const char *name, float *value, const char *filename) {
    if (!section || !name || !filename) {
        return false;
    }

    char buf[512] = {0};
    if (!bpIniGetString(section, name, buf, sizeof(buf), filename) || buf[0] == '\0') {
        return false;
    }

    char *endptr = nullptr;
    errno = 0;
    float val = strtof(buf, &endptr);

    // Check for conversion errors
    if ((errno == ERANGE) ||
        (errno != 0 && val == 0) ||
        (endptr == buf) ||
        (*endptr != '\0')) {
        return false;
    }

    if (value)
        *value = val;
    return true;
}

bool bpIniSetFloat(const char *section, const char *name, float value, int precision, const char *filename) {
    if (!section || !name || !filename || precision < 0 || precision > 20) {
        return false;
    }

    char buf[64] = {0};
    char format[16] = {0};

    // Create format string with specified precision
    snprintf(format, sizeof(format), "%%.%df", precision);

    int result = snprintf(buf, sizeof(buf), format, value);
    if (result < 0 || result >= static_cast<int>(sizeof(buf))) {
        return false;
    }

    return bpIniSetString(section, name, buf, filename);
}

bool bpIniGetPixelFormat(const char *section, const char *name, int *value, const char *filename) {
    if (!section || !name || !filename) {
        return false;
    }

    char buf[32] = {};
    if (!bpIniGetString(section, name, buf, sizeof(buf), filename) || buf[0] == '\0') {
        return false;
    }

    const int format = bpString2PixelFormat(buf);
    if (value)
        *value = format;
    return true;
}

bool bpIniSetPixelFormat(const char *section, const char *name, int value, const char *filename) {
    if (!section || !name || !filename) {
        return false;
    }

    const char *formatStr = bpPixelFormat2String(value);
    if (!formatStr) {
        return false;
    }

    return bpIniSetString(section, name, formatStr, filename);
}
