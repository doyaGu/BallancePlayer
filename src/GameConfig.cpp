#include "GameConfig.h"

#include "Utils.h"

#include <ctype.h>
#include <stdio.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

static const char *const DefaultPaths[] = {
#define X_PATH(category, defaultPath, cliLong, validateDir) defaultPath,
    GAMECONFIG_PATH_FIELDS
#undef X_PATH
};

static bool GetLastWriteTime(const char *filename, FILETIME &outTime)
{
    WIN32_FIND_DATAA findData;
    HANDLE handle = ::FindFirstFileA(filename, &findData);
    if (handle == INVALID_HANDLE_VALUE)
        return false;
    outTime = findData.ftLastWriteTime;
    ::FindClose(handle);
    return true;
}

static bool SameFileTimeComponents(const FILETIME &timeValue, DWORD low, DWORD high)
{
    return (timeValue.dwLowDateTime == low) && (timeValue.dwHighDateTime == high);
}

static bool SamePathIgnoreCase(const char *lhs, const char *rhs)
{
    if (!lhs || !rhs)
        return false;
    while (*lhs && *rhs)
    {
        if (toupper(static_cast<unsigned char>(*lhs++)) != toupper(static_cast<unsigned char>(*rhs++)))
            return false;
    }
    return *lhs == '\0' && *rhs == '\0';
}

static bool ResolveAbsolutePath(const char *path, std::string &outPath)
{
    char buffer[MAX_PATH];
    if (!utils::GetAbsolutePath(buffer, MAX_PATH, path, false))
        return false;
    outPath = buffer;
    return true;
}

static bool ResolveDefaultConfigPath(std::string &outPath)
{
    char modulePath[MAX_PATH];
    DWORD length = ::GetModuleFileNameA(NULL, modulePath, MAX_PATH);
    if (length > 0 && length < MAX_PATH)
    {
        char dir[MAX_PATH];
        if (utils::GetFileDirectory(dir, MAX_PATH, modulePath, true))
        {
            char path[MAX_PATH];
            utils::ConcatPath(path, MAX_PATH, dir, DefaultPaths[eConfigPath]);
            outPath = path;
            return true;
        }
    }
    return ResolveAbsolutePath(DefaultPaths[eConfigPath], outPath);
}

static bool GetDefaultRootPath(char *buffer, size_t size)
{
    if (!buffer || size == 0)
        return false;

    char currentDir[MAX_PATH];
    char path[MAX_PATH];
    if (utils::GetCurrentPath(currentDir, sizeof(currentDir)) == 0)
        return false;

    if (utils::ConcatPath(path, sizeof(path), currentDir, DefaultPaths[eCmoPath]) &&
        utils::FileOrDirectoryExists(path))
    {
        strncpy(buffer, ".\\", size - 1);
        buffer[size - 1] = '\0';
        return true;
    }

    strncpy(buffer, DefaultPaths[eRootPath], size - 1);
    buffer[size - 1] = '\0';
    return true;
}

static std::string JoinConfigPath(const char *basePath, const char *relativePath)
{
    if (!basePath || basePath[0] == '\0')
        return relativePath ? relativePath : "";

    std::string result = basePath;
    while (!result.empty() && (result[result.size() - 1] == '\\' || result[result.size() - 1] == '/'))
        result.erase(result.size() - 1);

    result += "\\";
    if (relativePath)
        result += relativePath;
    return result;
}

static bool ResolveConfigPath(const char *requestedPath, const char *storedPath,
                              std::string &outPath, bool *shouldUpdateStoredPath)
{
    if (!requestedPath)
        return false;

    outPath.erase();
    bool updateStoredPath = false;
    const char *pathToResolve = requestedPath;
    if (requestedPath[0] == '\0')
    {
        if (!storedPath || storedPath[0] == '\0')
        {
            if (!ResolveDefaultConfigPath(outPath))
                return false;
            updateStoredPath = true;
        }
        else
        {
            pathToResolve = storedPath;
            updateStoredPath = true;
        }
    }

    if (outPath.empty() && !ResolveAbsolutePath(pathToResolve, outPath))
        return false;

    if (!updateStoredPath && storedPath && storedPath[0] != '\0')
    {
        std::string storedResolvedPath;
        updateStoredPath = ResolveAbsolutePath(storedPath, storedResolvedPath) &&
                           SamePathIgnoreCase(outPath.c_str(), storedResolvedPath.c_str());
    }

    if (shouldUpdateStoredPath)
        *shouldUpdateStoredPath = updateStoredPath;
    return true;
}

static std::string SerializeValue(int value)
{
    char buffer[64];
    sprintf(buffer, "%d", value);
    return buffer;
}

static std::string SerializeValue(bool value)
{
    return value ? "1" : "0";
}

static std::string SerializeValue(VX_PIXELFORMAT value)
{
    return utils::PixelFormat2String(value);
}

static bool ReadFieldValue(const char *section, const char *key, bool &value, const char *filename)
{
    return utils::IniGetBoolean(section, key, value, filename);
}

static bool ReadFieldValue(const char *section, const char *key, int &value, const char *filename)
{
    return utils::IniGetInteger(section, key, value, filename);
}

static bool ReadFieldValue(const char *section, const char *key, VX_PIXELFORMAT &value, const char *filename)
{
    return utils::IniGetPixelFormat(section, key, value, filename);
}

template <typename TValue>
static void LoadFieldValue(const char *section, const char *key, TValue &member, const char *filename)
{
    TValue value = member;
    if (ReadFieldValue(section, key, value, filename))
        member = value;
}

CGameConfig::CGameConfig()
{
    // Auto-generated initialization from master list
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) member = def;
    #define X_INT(sec,key,member,def,cliLong,cliShort)  member = def;
    #define X_PF(sec,key,member,def,cliLong,cliShort)   member = def;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    // Non-INI members
    screenMode = -1;

    ResetPath();
    ResetFieldSnapshots();
}

CGameConfig &CGameConfig::operator=(const CGameConfig &config)
{
    if (this == &config)
        return *this;

    // Auto-generated copying from master list
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) member = config.member;
    #define X_INT(sec,key,member,def,cliLong,cliShort)  member = config.member;
    #define X_PF(sec,key,member,def,cliLong,cliShort)   member = config.member;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    // Non-INI members
    screenMode = config.screenMode;

    // Copy paths
    int i;
    for (i = 0; i < ePathCategoryCount; ++i)
        m_Paths[i] = config.m_Paths[i];

    // Copy field snapshots
    for (i = 0; i < eLoadedSentinel; ++i)
        m_FieldSnapshots[i] = config.m_FieldSnapshots[i];

    m_ConfigTimestampLow = config.m_ConfigTimestampLow;
    m_ConfigTimestampHigh = config.m_ConfigTimestampHigh;
    m_ConfigTimestampValid = config.m_ConfigTimestampValid;
    m_LastConfigAbsolutePath = config.m_LastConfigAbsolutePath;

    return *this;
}

bool CGameConfig::HasPath(PathCategory category) const
{
    if (category < 0 || category >= ePathCategoryCount)
        return false;
    return m_Paths[category].size() != 0;
}

const char *CGameConfig::GetPath(PathCategory category) const
{
    if (category < 0 || category >= ePathCategoryCount)
        return NULL;
    return m_Paths[category].c_str();
}

void CGameConfig::SetPath(PathCategory category, const char *path)
{
    if (category < 0 || category >= ePathCategoryCount || !path)
        return;
    m_Paths[category] = path;
}

bool CGameConfig::ResetPath(PathCategory category)
{
    if (category < 0 || category > ePathCategoryCount)
        return false;

    if (category == ePathCategoryCount)
    {
        char rootPath[MAX_PATH];
        if (!GetDefaultRootPath(rootPath, sizeof(rootPath)))
        {
            strncpy(rootPath, DefaultPaths[eRootPath], sizeof(rootPath));
            rootPath[sizeof(rootPath) - 1] = '\0';
        }

        // Reset all paths
        for (int i = 0; i < ePathCategoryCount; ++i)
        {
            if (i < eRootPath)
            {
                SetPath((PathCategory)i, DefaultPaths[i]);
            }
            else if (i == eRootPath)
            {
                SetPath((PathCategory)i, rootPath);
            }
            else
            {
                SetPath((PathCategory)i, JoinConfigPath(GetPath(eRootPath), DefaultPaths[i]).c_str());
            }
        }
    }
    else
    {
        // Reset specific path
        if (category < eRootPath)
        {
            SetPath(category, DefaultPaths[category]);
        }
        else if (category == eRootPath)
        {
            char rootPath[MAX_PATH];
            if (!GetDefaultRootPath(rootPath, sizeof(rootPath)))
            {
                strncpy(rootPath, DefaultPaths[eRootPath], sizeof(rootPath));
                rootPath[sizeof(rootPath) - 1] = '\0';
            }
            SetPath(category, rootPath);
        }
        else
        {
            SetPath(category, JoinConfigPath(GetPath(eRootPath), DefaultPaths[category]).c_str());
        }
    }

    return true;
}

bool CGameConfig::EnsureConfigPath()
{
    std::string path;
    if (!ResolveConfigPath("", m_Paths[eConfigPath].c_str(), path, NULL))
        return false;

    SetPath(eConfigPath, path.c_str());
    return true;
}

void CGameConfig::LoadFromIni(const char *filename)
{
    std::string path;
    bool updateStoredPath = false;
    if (!ResolveConfigPath(filename, m_Paths[eConfigPath].c_str(), path, &updateStoredPath))
        return;

    filename = path.c_str();
    if (!utils::FileOrDirectoryExists(filename))
        return;

    if (updateStoredPath)
        SetPath(eConfigPath, filename);

    ResetFieldSnapshots();

    FILETIME fileTime;
    if (GetLastWriteTime(filename, fileTime))
    {
        m_ConfigTimestampLow = fileTime.dwLowDateTime;
        m_ConfigTimestampHigh = fileTime.dwHighDateTime;
        m_ConfigTimestampValid = true;
    }

    SetLastConfigAbsolutePath(filename);

    // Auto-generated loading from master list
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        LoadFieldValue(sec, key, member, filename);

    #define X_INT(sec,key,member,def,cliLong,cliShort) \
        LoadFieldValue(sec, key, member, filename);

    #define X_PF(sec,key,member,def,cliLong,cliShort) \
        LoadFieldValue(sec, key, member, filename);

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    CaptureFieldValuesAsLoaded();
}

bool CGameConfig::SaveToIni(const char *filename)
{
    std::string path;
    bool updateStoredPath = false;
    if (!ResolveConfigPath(filename, m_Paths[eConfigPath].c_str(), path, &updateStoredPath))
        return false;

    filename = path.c_str();
    if (updateStoredPath)
        SetPath(eConfigPath, filename);

    bool shouldMerge = false;
    FILETIME fileTime;
    if (m_ConfigTimestampValid && IsSameConfigPath(filename) && GetLastWriteTime(filename, fileTime))
    {
        if (!SameFileTimeComponents(fileTime, m_ConfigTimestampLow, m_ConfigTimestampHigh))
            shouldMerge = true;
    }

    if (shouldMerge)
        MergeExternalChanges(filename);

    bool ok = true;

    // Auto-generated saving from master list
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        ok = utils::IniSetBoolean(sec, key, member, filename) && ok;
    #define X_INT(sec,key,member,def,cliLong,cliShort) \
        ok = utils::IniSetInteger(sec, key, member, filename) && ok;
    #define X_PF(sec,key,member,def,cliLong,cliShort) \
        ok = utils::IniSetPixelFormat(sec, key, member, filename) && ok;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    if (!ok)
        return false;

    CaptureFieldValuesAsLoaded();

    if (GetLastWriteTime(filename, fileTime))
    {
        m_ConfigTimestampLow = fileTime.dwLowDateTime;
        m_ConfigTimestampHigh = fileTime.dwHighDateTime;
        m_ConfigTimestampValid = true;
    }
    else
    {
        m_ConfigTimestampValid = false;
    }

    SetLastConfigAbsolutePath(filename);
    return true;
}

void CGameConfig::ResetFieldSnapshots()
{
    for (int i = 0; i < eLoadedSentinel; ++i)
    {
        m_FieldSnapshots[i].hasLoadedValue = false;
        m_FieldSnapshots[i].loadedValue.erase();
    }

    m_ConfigTimestampLow = 0;
    m_ConfigTimestampHigh = 0;
    m_ConfigTimestampValid = false;
    m_LastConfigAbsolutePath.erase();
}

void CGameConfig::StoreLoadedValue(int index, const std::string &value)
{
    if (index < 0 || index >= eLoadedSentinel)
        return;
    m_FieldSnapshots[index].hasLoadedValue = true;
    m_FieldSnapshots[index].loadedValue = value;
}

bool CGameConfig::CanAcceptExternalChange(int index, const std::string &currentValue) const
{
    if (index < 0 || index >= eLoadedSentinel)
        return false;
    if (!m_FieldSnapshots[index].hasLoadedValue)
        return true;
    return m_FieldSnapshots[index].loadedValue == currentValue;
}

bool CGameConfig::TryAcceptExternalValue(int index, const std::string &currentValue, const std::string &externalValue)
{
    if (!CanAcceptExternalChange(index, currentValue))
        return false;
    StoreLoadedValue(index, externalValue);
    return true;
}

void CGameConfig::CaptureFieldValuesAsLoaded()
{
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        CaptureFieldValueAsLoaded(eLoadedBool_##member, member);

    #define X_INT(sec,key,member,def,cliLong,cliShort) \
        CaptureFieldValueAsLoaded(eLoadedInt_##member, member);

    #define X_PF(sec,key,member,def,cliLong,cliShort) \
        CaptureFieldValueAsLoaded(eLoadedPixel_##member, member);

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_PF
}

template <typename TValue>
void CGameConfig::CaptureFieldValueAsLoaded(int index, const TValue &value)
{
    StoreLoadedValue(index, SerializeValue(value));
}

void CGameConfig::MergeExternalFieldValue(const char *filename, const char *section, const char *key,
                                          bool &member, int index)
{
    bool externalValue;
    if (!ReadFieldValue(section, key, externalValue, filename))
        return;

    if (TryAcceptExternalValue(index, SerializeValue(member), SerializeValue(externalValue)))
        member = externalValue;
}

void CGameConfig::MergeExternalFieldValue(const char *filename, const char *section, const char *key,
                                          int &member, int index)
{
    int externalValue;
    if (!ReadFieldValue(section, key, externalValue, filename))
        return;

    if (TryAcceptExternalValue(index, SerializeValue(member), SerializeValue(externalValue)))
        member = externalValue;
}

void CGameConfig::MergeExternalFieldValue(const char *filename, const char *section, const char *key,
                                          VX_PIXELFORMAT &member, int index)
{
    VX_PIXELFORMAT externalValue;
    if (!ReadFieldValue(section, key, externalValue, filename))
        return;

    if (TryAcceptExternalValue(index, SerializeValue(member), SerializeValue(externalValue)))
        member = externalValue;
}

void CGameConfig::MergeExternalChanges(const char *filename)
{
    // Auto-generated merging from master list
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        MergeExternalFieldValue(filename, sec, key, member, eLoadedBool_##member);

    #define X_INT(sec,key,member,def,cliLong,cliShort) \
        MergeExternalFieldValue(filename, sec, key, member, eLoadedInt_##member);

    #define X_PF(sec,key,member,def,cliLong,cliShort) \
        MergeExternalFieldValue(filename, sec, key, member, eLoadedPixel_##member);

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_PF
}

void CGameConfig::SetLastConfigAbsolutePath(const char *path)
{
    if (path && path[0] != '\0')
        m_LastConfigAbsolutePath = path;
    else
        m_LastConfigAbsolutePath.erase();
}

bool CGameConfig::IsSameConfigPath(const char *path) const
{
    if (!path || m_LastConfigAbsolutePath.size() == 0)
        return false;

    return SamePathIgnoreCase(m_LastConfigAbsolutePath.c_str(), path);
}
