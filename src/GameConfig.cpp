#include "GameConfig.h"

#include "Utils.h"

#include <ctype.h>
#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

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

static const char *const DefaultPaths[] = {
#define X_PATH(category, defaultPath, cliLong, validateDir) defaultPath,
    GAMECONFIG_PATH_FIELDS
#undef X_PATH
};

static std::string SerializeInt(int value)
{
    char buffer[64];
    sprintf(buffer, "%d", value);
    return buffer;
}

static std::string SerializeBool(bool value)
{
    return value ? "1" : "0";
}

static std::string SerializePixel(VX_PIXELFORMAT value)
{
    return utils::PixelFormat2String(value);
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
        // Reset all paths
        for (int i = 0; i < ePathCategoryCount; ++i)
        {
            if (i < ePluginPath)
            {
                SetPath((PathCategory)i, DefaultPaths[i]);
            }
            else
            {
                char szPath[MAX_PATH];
                utils::ConcatPath(szPath, MAX_PATH, GetPath(eRootPath), DefaultPaths[i]);
                SetPath((PathCategory)i, szPath);
            }
        }
    }
    else
    {
        // Reset specific path
        if (category < ePluginPath)
        {
            SetPath(category, DefaultPaths[category]);
        }
        else
        {
            char szPath[MAX_PATH];
            utils::ConcatPath(szPath, MAX_PATH, GetPath(eRootPath), DefaultPaths[category]);
            SetPath(category, szPath);
        }
    }

    return true;
}

void CGameConfig::LoadFromIni(const char *filename)
{
    if (!filename)
        return;

    bool usingConfigPath = false;
    if (filename[0] == '\0')
    {
        if (m_Paths[eConfigPath].size() == 0 || !utils::FileOrDirectoryExists(m_Paths[eConfigPath].c_str()))
            return;
        filename = m_Paths[eConfigPath].c_str();
        usingConfigPath = true;
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename))
    {
        utils::GetCurrentPath(path, MAX_PATH);
        utils::ConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }
    if (usingConfigPath)
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
        do { \
            bool tmp = member; \
            if (utils::IniGetBoolean(sec, key, tmp, filename)) { \
                member = tmp; \
            } \
        } while(0);

    #define X_INT(sec,key,member,def,cliLong,cliShort) \
        do { \
            int tmp = member; \
            if (utils::IniGetInteger(sec, key, tmp, filename)) { \
                member = tmp; \
            } \
        } while(0);

    #define X_PF(sec,key,member,def,cliLong,cliShort) \
        do { \
            VX_PIXELFORMAT tmp = member; \
            if (utils::IniGetPixelFormat(sec, key, tmp, filename)) { \
                member = tmp; \
            } \
        } while(0);

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    CaptureCurrentValuesAsLoaded();
}

bool CGameConfig::SaveToIni(const char *filename)
{
    if (!filename)
        return false;

    bool usingConfigPath = false;
    if (filename[0] == '\0')
    {
        if (m_Paths[eConfigPath].size() == 0)
            return false;
        filename = m_Paths[eConfigPath].c_str();
        usingConfigPath = true;
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename))
    {
        utils::GetCurrentPath(path, MAX_PATH);
        utils::ConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }
    if (usingConfigPath)
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

    CapturePersistedValuesAsLoaded();

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

void CGameConfig::CapturePersistedValuesAsLoaded()
{
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        StoreLoadedValue(eLoadedBool_##member, SerializeBool(member));

    #define X_INT(sec,key,member,def,cliLong,cliShort) \
        StoreLoadedValue(eLoadedInt_##member, SerializeInt(member));

    #define X_PF(sec,key,member,def,cliLong,cliShort) \
        StoreLoadedValue(eLoadedPixel_##member, SerializePixel(member));

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_PF
}

void CGameConfig::CaptureCurrentValuesAsLoaded()
{
    // Auto-generated snapshot capture from master list
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        StoreLoadedValue(eLoadedBool_##member, SerializeBool(member));

    #define X_INT(sec,key,member,def,cliLong,cliShort) \
        StoreLoadedValue(eLoadedInt_##member, SerializeInt(member));

    #define X_PF(sec,key,member,def,cliLong,cliShort) \
        StoreLoadedValue(eLoadedPixel_##member, SerializePixel(member));

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_PF
}

void CGameConfig::MergeExternalChanges(const char *filename)
{
    // Auto-generated merging from master list
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        do { \
            bool tmp; \
            if (utils::IniGetBoolean(sec, key, tmp, filename)) { \
                std::string currentValue = SerializeBool(member); \
                std::string externalValue = SerializeBool(tmp); \
                if (CanAcceptExternalChange(eLoadedBool_##member, currentValue)) { \
                    member = tmp; \
                    StoreLoadedValue(eLoadedBool_##member, externalValue); \
                } \
            } \
        } while(0);

    #define X_INT(sec,key,member,def,cliLong,cliShort) \
        do { \
            int tmp; \
            if (utils::IniGetInteger(sec, key, tmp, filename)) { \
                std::string currentValue = SerializeInt(member); \
                std::string externalValue = SerializeInt(tmp); \
                if (CanAcceptExternalChange(eLoadedInt_##member, currentValue)) { \
                    member = tmp; \
                    StoreLoadedValue(eLoadedInt_##member, externalValue); \
                } \
            } \
        } while(0);

    #define X_PF(sec,key,member,def,cliLong,cliShort) \
        do { \
            VX_PIXELFORMAT tmp; \
            if (utils::IniGetPixelFormat(sec, key, tmp, filename)) { \
                std::string currentValue = SerializePixel(member); \
                std::string externalValue = SerializePixel(tmp); \
                if (CanAcceptExternalChange(eLoadedPixel_##member, currentValue)) { \
                    member = tmp; \
                    StoreLoadedValue(eLoadedPixel_##member, externalValue); \
                } \
            } \
        } while(0);

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
    if (!path)
        return false;
    if (m_LastConfigAbsolutePath.size() == 0)
        return false;

    const char *stored = m_LastConfigAbsolutePath.c_str();
    while (*stored && *path)
    {
        unsigned char c1 = static_cast<unsigned char>(*stored);
        unsigned char c2 = static_cast<unsigned char>(*path);
        if (toupper(c1) != toupper(c2))
            return false;
        ++stored;
        ++path;
    }

    return (*stored == '\0') && (*path == '\0');
}
