#include "GameConfig.h"

#include "Utils.h"

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
    "Player.ini",
    "Player.log",
    "base.cmo",
    "..\\",
    "Plugins\\",
    "RenderEngines\\",
    "Managers\\",
    "BuildingBlocks\\",
    "Sounds\\",
    "Textures\\",
    "",
};

CGameConfig::CGameConfig()
{
    // Auto-generated initialization from master list
    #define X_BOOL(sec,key,member,def) member = def;
    #define X_INT(sec,key,member,def)  member = def;
    #define X_PF(sec,key,member,def)   member = def;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    // Non-INI members
    screenMode = -1;
    debug = false;
    rookie = false;

    ResetPath();
    ResetLoadedSnapshots();
}

CGameConfig &CGameConfig::operator=(const CGameConfig &config)
{
    if (this == &config)
        return *this;

    // Auto-generated copying from master list
    #define X_BOOL(sec,key,member,def) member = config.member;
    #define X_INT(sec,key,member,def)  member = config.member;
    #define X_PF(sec,key,member,def)   member = config.member;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    // Non-INI members
    screenMode = config.screenMode;
    debug = config.debug;
    rookie = config.rookie;

    // Copy paths
    int i;
    for (i = 0; i < ePathCategoryCount; ++i)
        m_Paths[i] = config.m_Paths[i];

    // Copy loaded snapshots
    for (i = 0; i < eLoadedSentinel; ++i)
    {
        m_LoadedInts[i] = config.m_LoadedInts[i];
        m_LoadedBools[i] = config.m_LoadedBools[i];
        m_LoadedPixels[i] = config.m_LoadedPixels[i];
    }

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

    if (filename[0] == '\0')
    {
        if (m_Paths[eConfigPath].size() == 0 || !utils::FileOrDirectoryExists(m_Paths[eConfigPath].c_str()))
            return;
        filename = m_Paths[eConfigPath].c_str();
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename))
    {
        utils::GetCurrentPath(path, MAX_PATH);
        utils::ConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }

    ResetLoadedSnapshots();

    FILETIME fileTime;
    if (GetLastWriteTime(filename, fileTime))
    {
        m_ConfigTimestampLow = fileTime.dwLowDateTime;
        m_ConfigTimestampHigh = fileTime.dwHighDateTime;
        m_ConfigTimestampValid = true;
    }

    SetLastConfigAbsolutePath(filename);

    // Auto-generated loading from master list
    #define X_BOOL(sec,key,member,def) \
        do { \
            bool tmp = member; \
            if (utils::IniGetBoolean(sec, key, tmp, filename)) { \
                member = tmp; \
                StoreLoadedBool(eLoadedBool_##member, tmp); \
            } \
        } while(0);

    #define X_INT(sec,key,member,def) \
        do { \
            int tmp = member; \
            if (utils::IniGetInteger(sec, key, tmp, filename)) { \
                member = tmp; \
                StoreLoadedInt(eLoadedInt_##member, tmp); \
            } \
        } while(0);

    #define X_PF(sec,key,member,def) \
        do { \
            VX_PIXELFORMAT tmp = member; \
            if (utils::IniGetPixelFormat(sec, key, tmp, filename)) { \
                member = tmp; \
                StoreLoadedPixel(eLoadedPixel_##member, tmp); \
            } \
        } while(0);

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_PF
}

void CGameConfig::SaveToIni(const char *filename)
{
    if (!filename)
        return;

    if (filename[0] == '\0')
    {
        if (m_Paths[eConfigPath].size() == 0)
            return;
        filename = m_Paths[eConfigPath].c_str();
    }

    char path[MAX_PATH];
    if (!utils::IsAbsolutePath(filename))
    {
        utils::GetCurrentPath(path, MAX_PATH);
        utils::ConcatPath(path, MAX_PATH, path, filename);
        filename = path;
    }

    bool shouldMerge = false;
    FILETIME fileTime;
    if (m_ConfigTimestampValid && IsSameConfigPath(filename) && GetLastWriteTime(filename, fileTime))
    {
        if (!SameFileTimeComponents(fileTime, m_ConfigTimestampLow, m_ConfigTimestampHigh))
            shouldMerge = true;
    }

    if (shouldMerge)
        MergeExternalChanges(filename);

    // Auto-generated saving from master list
    #define X_BOOL(sec,key,member,def) utils::IniSetBoolean(sec, key, member, filename);
    #define X_INT(sec,key,member,def)  utils::IniSetInteger(sec, key, member, filename);
    #define X_PF(sec,key,member,def)   utils::IniSetPixelFormat(sec, key, member, filename);
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_PF

    CaptureCurrentValuesAsLoaded();

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
}

void CGameConfig::ResetLoadedSnapshots()
{
    for (int i = 0; i < eLoadedSentinel; ++i)
    {
        m_LoadedInts[i].hasValue = false;
        m_LoadedInts[i].value = 0;
        m_LoadedBools[i].hasValue = false;
        m_LoadedBools[i].value = false;
        m_LoadedPixels[i].hasValue = false;
        m_LoadedPixels[i].value = UNKNOWN_PF;
    }

    m_ConfigTimestampLow = 0;
    m_ConfigTimestampHigh = 0;
    m_ConfigTimestampValid = false;
    m_LastConfigAbsolutePath.erase();
}

void CGameConfig::StoreLoadedInt(int index, int value)
{
    if (index < 0 || index >= eLoadedSentinel)
        return;
    m_LoadedInts[index].hasValue = true;
    m_LoadedInts[index].value = value;
}

void CGameConfig::StoreLoadedBool(int index, bool value)
{
    if (index < 0 || index >= eLoadedSentinel)
        return;
    m_LoadedBools[index].hasValue = true;
    m_LoadedBools[index].value = value;
}

void CGameConfig::StoreLoadedPixel(int index, VX_PIXELFORMAT value)
{
    if (index < 0 || index >= eLoadedSentinel)
        return;
    m_LoadedPixels[index].hasValue = true;
    m_LoadedPixels[index].value = value;
}

bool CGameConfig::ShouldOverrideInt(int index, int newValue) const
{
    if (index < 0 || index >= eLoadedSentinel)
        return false;
    if (!m_LoadedInts[index].hasValue)
        return true;
    // Return true if the current in-memory value is unchanged from what we loaded
    // This means we should accept the external change
    return m_LoadedInts[index].value == newValue;
}

bool CGameConfig::ShouldOverrideBool(int index, bool newValue) const
{
    if (index < 0 || index >= eLoadedSentinel)
        return false;
    if (!m_LoadedBools[index].hasValue)
        return true;
    // Return true if the current in-memory value is unchanged from what we loaded
    // This means we should accept the external change
    return m_LoadedBools[index].value == newValue;
}

bool CGameConfig::ShouldOverridePixel(int index, VX_PIXELFORMAT newValue) const
{
    if (index < 0 || index >= eLoadedSentinel)
        return false;
    if (!m_LoadedPixels[index].hasValue)
        return true;
    // Return true if the current in-memory value is unchanged from what we loaded
    // This means we should accept the external change
    return m_LoadedPixels[index].value == newValue;
}

void CGameConfig::CaptureCurrentValuesAsLoaded()
{
    // Auto-generated snapshot capture from master list
    #define X_BOOL(sec,key,member,def) \
        m_LoadedBools[eLoadedBool_##member].hasValue = true; \
        m_LoadedBools[eLoadedBool_##member].value = member;

    #define X_INT(sec,key,member,def) \
        m_LoadedInts[eLoadedInt_##member].hasValue = true; \
        m_LoadedInts[eLoadedInt_##member].value = member;

    #define X_PF(sec,key,member,def) \
        m_LoadedPixels[eLoadedPixel_##member].hasValue = true; \
        m_LoadedPixels[eLoadedPixel_##member].value = member;

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_PF
}

void CGameConfig::MergeExternalChanges(const char *filename)
{
    // Auto-generated merging from master list
    #define X_BOOL(sec,key,member,def) \
        do { \
            bool tmp; \
            if (utils::IniGetBoolean(sec, key, tmp, filename)) { \
                if (ShouldOverrideBool(eLoadedBool_##member, member)) \
                    member = tmp; \
            } \
        } while(0);

    #define X_INT(sec,key,member,def) \
        do { \
            int tmp; \
            if (utils::IniGetInteger(sec, key, tmp, filename)) { \
                if (ShouldOverrideInt(eLoadedInt_##member, member)) \
                    member = tmp; \
            } \
        } while(0);

    #define X_PF(sec,key,member,def) \
        do { \
            VX_PIXELFORMAT tmp; \
            if (utils::IniGetPixelFormat(sec, key, tmp, filename)) { \
                if (ShouldOverridePixel(eLoadedPixel_##member, member)) \
                    member = tmp; \
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