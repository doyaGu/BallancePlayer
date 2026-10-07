#include "GameConfig.h"

#include "Utils.h"
#include "VxWindowFunctions.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

static const char *const DefaultPaths[] = {
#define X_PATH(category, defaultPath, cliLong, validateDir) defaultPath,
    GAMECONFIG_PATH_FIELDS
#undef X_PATH
};

struct ConfigFileTime
{
    unsigned long low;
    unsigned long high;
};

static bool GetLastWriteTime(const char *filename, ConfigFileTime &outTime)
{
#if defined(_WIN32)
    WIN32_FIND_DATAA findData;
    HANDLE handle = ::FindFirstFileA(filename, &findData);
    if (handle == INVALID_HANDLE_VALUE)
        return false;
    outTime.low = findData.ftLastWriteTime.dwLowDateTime;
    outTime.high = findData.ftLastWriteTime.dwHighDateTime;
    ::FindClose(handle);
    return true;
#else
    struct stat st;
    if (stat(filename, &st) != 0)
        return false;
    // Nanoseconds: whole seconds would miss an external change made in the
    // second the file was read.
#if defined(__APPLE__)
    const struct timespec &mtime = st.st_mtimespec;
#else
    const struct timespec &mtime = st.st_mtim;
#endif
    unsigned long long value = static_cast<unsigned long long>(mtime.tv_sec) * 1000000000ull +
                               static_cast<unsigned long long>(mtime.tv_nsec);
    outTime.low = static_cast<unsigned long>(value & 0xffffffffu);
    outTime.high = static_cast<unsigned long>((value >> 32) & 0xffffffffu);
    return true;
#endif
}

static bool SameFileTimeComponents(const ConfigFileTime &timeValue, unsigned long low, unsigned long high)
{
    return (timeValue.low == low) && (timeValue.high == high);
}

static bool SamePathIgnoreCase(const char *lhs, const char *rhs)
{
    if (!lhs || !rhs)
        return false;
#if defined(_WIN32)
    while (*lhs && *rhs)
    {
        if (toupper(static_cast<unsigned char>(*lhs++)) != toupper(static_cast<unsigned char>(*rhs++)))
            return false;
    }
    return *lhs == '\0' && *rhs == '\0';
#else
    return strcmp(lhs, rhs) == 0;
#endif
}

static bool ResolveAbsolutePath(const char *path, const char *basePath, XString &outPath, bool trailing = false)
{
    outPath = utils::ResolvePathAgainstBase(basePath, path, trailing);
    if (outPath.IsEmpty())
        return false;
    return true;
}

static bool ResolveDefaultConfigPath(const char *basePath, XString &outPath)
{
    return ResolveAbsolutePath(DefaultPaths[eConfigPath], basePath, outPath);
}

static XString GetDefaultRootPath(const char *basePath)
{
    XString cmoPath = utils::ResolvePathAgainstBase(basePath, DefaultPaths[eCmoPath], false);
    if (!cmoPath.IsEmpty() && utils::FileOrDirectoryExists(cmoPath.CStr()))
        return utils::WithTrailingPathSeparator(".");
    return utils::WithTrailingPathSeparator(DefaultPaths[eRootPath]);
}

static XString JoinConfigPath(const char *basePath, const char *relativePath)
{
    return utils::JoinPath(basePath, relativePath, true);
}

// An explicit path, the stored path or the default path, in that order. The
// resolved path becomes the stored path so later saves target the same file.
static bool ResolveConfigPath(const char *requestedPath, const char *storedPath,
                              const char *basePath, XString &outPath)
{
    if (!requestedPath)
        return false;

    outPath.Clear();
    if (requestedPath[0] != '\0')
        return ResolveAbsolutePath(requestedPath, basePath, outPath);
    if (storedPath && storedPath[0] != '\0')
        return ResolveAbsolutePath(storedPath, basePath, outPath);
    return ResolveDefaultConfigPath(basePath, outPath);
}

static XString SerializeValue(int value)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return buffer;
}

static XString SerializeValue(bool value)
{
    return value ? "1" : "0";
}

static XString SerializeValue(const XString &value)
{
    return value;
}

static bool ReadFieldValue(const char *section, const char *key, XString &value, const char *filename)
{
    char text[128] = {};
    if (!utils::IniGetString(section, key, text, sizeof(text), filename))
        return false;
    value = text;
    return true;
}

static bool ReadFieldValue(const char *section, const char *key, bool &value, const char *filename)
{
    return utils::IniGetBoolean(section, key, value, filename);
}

static bool ReadFieldValue(const char *section, const char *key, int &value, const char *filename)
{
    return utils::IniGetInteger(section, key, value, filename);
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
    #define X_STRING(sec,key,member,def,cliLong,cliShort)   member = def;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_STRING

    // Non-INI members
    screenMode = -1;

    m_RuntimeBasePath = utils::GetExecutableDirectory();
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
    #define X_STRING(sec,key,member,def,cliLong,cliShort)   member = config.member;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_STRING

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
    m_RuntimeBasePath = config.m_RuntimeBasePath;

    return *this;
}

bool CGameConfig::HasPath(PathCategory category) const
{
    if (category < 0 || category >= ePathCategoryCount)
        return false;
    return !m_Paths[category].IsEmpty();
}

const char *CGameConfig::GetPath(PathCategory category) const
{
    if (category < 0 || category >= ePathCategoryCount)
        return NULL;
    return m_Paths[category].CStr();
}

void CGameConfig::SetPath(PathCategory category, const char *path)
{
    if (category < 0 || category >= ePathCategoryCount || !path)
        return;
    m_Paths[category] = path;
}

void CGameConfig::SetRuntimeBasePath(const char *path)
{
    m_RuntimeBasePath = path && path[0] != '\0' ? utils::WithTrailingPathSeparator(path) : utils::GetExecutableDirectory();
    ResetPath();
}

const char *CGameConfig::GetRuntimeBasePath() const
{
    return m_RuntimeBasePath.CStr();
}

bool CGameConfig::ResolvePath(PathCategory category, XString &outPath, bool trailing) const
{
    outPath.Clear();
    if (category < 0 || category >= ePathCategoryCount)
        return false;
    outPath = utils::ResolvePathAgainstBase(m_RuntimeBasePath.CStr(), GetPath(category), trailing);
    return !outPath.IsEmpty();
}

bool CGameConfig::ResetPath(PathCategory category)
{
    if (category < 0 || category > ePathCategoryCount)
        return false;

    if (category == ePathCategoryCount)
    {
        XString rootPath = GetDefaultRootPath(m_RuntimeBasePath.CStr());

        // Reset all paths
        for (int i = 0; i < ePathCategoryCount; ++i)
        {
            if (i < eRootPath)
            {
                SetPath((PathCategory)i, DefaultPaths[i]);
            }
            else if (i == eRootPath)
            {
                SetPath((PathCategory)i, rootPath.CStr());
            }
            else
            {
                SetPath((PathCategory)i, JoinConfigPath(GetPath(eRootPath), DefaultPaths[i]).CStr());
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
            XString rootPath = GetDefaultRootPath(m_RuntimeBasePath.CStr());
            SetPath(category, rootPath.CStr());
        }
        else
        {
            SetPath(category, JoinConfigPath(GetPath(eRootPath), DefaultPaths[category]).CStr());
        }
    }

    return true;
}

bool CGameConfig::EnsureConfigPath()
{
    XString path;
    if (!ResolveConfigPath("", m_Paths[eConfigPath].CStr(), m_RuntimeBasePath.CStr(), path))
        return false;

    SetPath(eConfigPath, path.CStr());
    return true;
}

void CGameConfig::LoadFromIni(const char *filename)
{
    XString path;
    if (!ResolveConfigPath(filename, m_Paths[eConfigPath].CStr(), m_RuntimeBasePath.CStr(), path))
        return;

    filename = path.CStr();
    if (!utils::FileOrDirectoryExists(filename))
        return;

    SetPath(eConfigPath, filename);

    ResetFieldSnapshots();

    ConfigFileTime fileTime;
    if (GetLastWriteTime(filename, fileTime))
    {
        m_ConfigTimestampLow = fileTime.low;
        m_ConfigTimestampHigh = fileTime.high;
        m_ConfigTimestampValid = true;
    }

    SetLastConfigAbsolutePath(filename);

    // A legacy numeric selector is explicit, even when it selects driver zero.
    // Files without either selector retain the build's default rasterizer.
    XString configuredRasterizer;
    int configuredDriver = 0;
    if (!ReadFieldValue("Graphics", "Rasterizer", configuredRasterizer, filename) &&
        ReadFieldValue("Graphics", "Driver", configuredDriver, filename))
        rasterizer.Clear();

    // Auto-generated loading from master list
    #define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        LoadFieldValue(sec, key, member, filename);

    #define X_INT(sec,key,member,def,cliLong,cliShort) \
        LoadFieldValue(sec, key, member, filename);

    #define X_STRING(sec,key,member,def,cliLong,cliShort) \
        LoadFieldValue(sec, key, member, filename);

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_STRING

    CaptureFieldValuesAsLoaded();

    // The retired bgfx rasterizer selects the build's default; saving the
    // configuration replaces it.
    if (rasterizer.Compare("bgfx") == 0)
        rasterizer = PLAYER_DEFAULT_RASTERIZER;
}

bool CGameConfig::SaveToIni(const char *filename)
{
    XString path;
    if (!ResolveConfigPath(filename, m_Paths[eConfigPath].CStr(), m_RuntimeBasePath.CStr(), path))
        return false;

    filename = path.CStr();
    SetPath(eConfigPath, filename);

    bool shouldMerge = false;
    ConfigFileTime fileTime;
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
    #define X_STRING(sec,key,member,def,cliLong,cliShort) \
        ok = utils::IniSetString(sec, key, member.CStr(), filename) && ok;
        GAMECONFIG_FIELDS
    #undef X_BOOL
    #undef X_INT
    #undef X_STRING

    if (!ok)
        return false;

    CaptureFieldValuesAsLoaded();

    if (GetLastWriteTime(filename, fileTime))
    {
        m_ConfigTimestampLow = fileTime.low;
        m_ConfigTimestampHigh = fileTime.high;
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
        m_FieldSnapshots[i].loadedValue.Clear();
    }

    m_ConfigTimestampLow = 0;
    m_ConfigTimestampHigh = 0;
    m_ConfigTimestampValid = false;
    m_LastConfigAbsolutePath.Clear();
}

void CGameConfig::StoreLoadedValue(int index, const XString &value)
{
    if (index < 0 || index >= eLoadedSentinel)
        return;
    m_FieldSnapshots[index].hasLoadedValue = true;
    m_FieldSnapshots[index].loadedValue = value;
}

bool CGameConfig::CanAcceptExternalChange(int index, const XString &currentValue) const
{
    if (index < 0 || index >= eLoadedSentinel)
        return false;
    if (!m_FieldSnapshots[index].hasLoadedValue)
        return true;
    return m_FieldSnapshots[index].loadedValue.Compare(currentValue) == 0;
}

bool CGameConfig::TryAcceptExternalValue(int index, const XString &currentValue, const XString &externalValue)
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

    #define X_STRING(sec,key,member,def,cliLong,cliShort) \
        CaptureFieldValueAsLoaded(eLoadedString_##member, member);

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_STRING
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
                                          XString &member, int index)
{
    XString externalValue;
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

    #define X_STRING(sec,key,member,def,cliLong,cliShort) \
        MergeExternalFieldValue(filename, sec, key, member, eLoadedString_##member);

        GAMECONFIG_FIELDS

    #undef X_BOOL
    #undef X_INT
    #undef X_STRING
}

void CGameConfig::SetLastConfigAbsolutePath(const char *path)
{
    if (path && path[0] != '\0')
        m_LastConfigAbsolutePath = path;
    else
        m_LastConfigAbsolutePath.Clear();
}

bool CGameConfig::IsSameConfigPath(const char *path) const
{
    if (!path || m_LastConfigAbsolutePath.IsEmpty())
        return false;

    return SamePathIgnoreCase(m_LastConfigAbsolutePath.CStr(), path);
}
