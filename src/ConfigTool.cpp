#include "ConfigTool.h"

#include <tchar.h>

#include "ConfigToolResource.h"
#include "GameConfig.h"
#include "Utils.h"

// --- String Resource Management ---

typedef enum
{
    LANG_UI_ENGLISH = 0,
    LANG_UI_CHINESE,
    LANG_UI_COUNT
} UILanguage;

class StringResource
{
public:
    static bool Initialize(HINSTANCE hInstance);
    static const char *GetString(UINT resourceID);
    static UILanguage GetLanguage();
    static void SetLanguage(UILanguage lang);
    static UILanguage DetectSystemLanguage();

private:
    static HINSTANCE m_hInstance;
    static UILanguage m_CurrentLanguage;
    static char m_Buffer[4096];
    static TCHAR m_TBuffer[4096];

    static UINT MapToLanguageID(UINT resourceID);
};

static HFONT g_hFonts[LANG_UI_COUNT] = {NULL};

typedef struct
{
    int ctrlID;
    UINT strID;
} ControlTextMapping;

const ControlTextMapping g_TextMappings[] = {
    {IDOK, IDS_BTN_OK}, {IDCANCEL, IDS_BTN_CANCEL}, {IDC_BUTTON_DEFAULTS, IDS_BTN_DEFAULTS},
    {IDC_GROUP_STARTUP, IDS_GROUP_STARTUP}, {IDC_GROUP_GRAPHICS, IDS_GROUP_GRAPHICS},
    {IDC_GROUP_WINDOW, IDS_GROUP_WINDOW}, {IDC_GROUP_GAME, IDS_GROUP_GAME}, {IDC_GROUP_INTERFACE, IDS_GROUP_INTERFACE},
    {IDC_CHECK_VERBOSE, IDS_VERBOSE}, {IDC_CHECK_MANUALSETUP, IDS_MANUAL_SETUP},
    {IDC_CHECK_LOADALLMGR, IDS_LOAD_ALL_MGR}, {IDC_CHECK_LOADALLBB, IDS_LOAD_ALL_BB},
    {IDC_CHECK_LOADALLPLUG, IDS_LOAD_ALL_PLUGINS}, {IDC_CHECK_FULLSCREEN, IDS_FULLSCREEN},
    {IDC_CHECK_DISPERSPCORRECT, IDS_DISABLE_PERSP_CORRECT}, {IDC_CHECK_FORCELINEARFOG, IDS_FORCE_LINEAR_FOG},
    {IDC_CHECK_FORCESOFTWARE, IDS_FORCE_SOFTWARE}, {IDC_CHECK_DISABLEFILTER, IDS_DISABLE_FILTER},
    {IDC_CHECK_ENSUREVS, IDS_ENSURE_VS}, {IDC_CHECK_USEINDEXBUFFERS, IDS_USE_INDEX_BUFFERS},
    {IDC_CHECK_DISABLEDITHER, IDS_DISABLE_DITHER}, {IDC_CHECK_DISABLEMIPMAP, IDS_DISABLE_MIPMAP},
    {IDC_CHECK_DISABLESPECULAR, IDS_DISABLE_SPECULAR}, {IDC_CHECK_ENABLESCREENDUMP, IDS_ENABLE_SCREEN_DUMP},
    {IDC_CHECK_ENABLEDEBUGMODE_GFX, IDS_ENABLE_DEBUG_GFX}, {IDC_CHECK_TEXCACHEMGMT, IDS_TEXTURE_CACHE_MGMT},
    {IDC_CHECK_SORTTRANSPARENT, IDS_SORT_TRANSPARENT}, {IDC_CHECK_CHILDWINRENDER, IDS_CHILD_WINDOW_RENDER},
    {IDC_CHECK_BORDERLESS, IDS_BORDERLESS}, {IDC_CHECK_CLIPCURSOR, IDS_CLIP_CURSOR},
    {IDC_CHECK_ALWAYSHANDLEINPUT, IDS_ALWAYS_HANDLE_INPUT}, {IDC_CHECK_SKIPOPENING, IDS_SKIP_OPENING},
    {IDC_CHECK_APPLYHOTFIX, IDS_APPLY_HOTFIX}, {IDC_CHECK_UNLOCKFRAMERATE, IDS_UNLOCK_FRAMERATE},
    {IDC_CHECK_UNLOCKWIDESCREEN, IDS_UNLOCK_WIDESCREEN}, {IDC_CHECK_UNLOCKHIGHRES, IDS_UNLOCK_HIGHRES},
    {IDC_CHECK_DEBUG, IDS_DEBUG}, {IDC_CHECK_ROOKIE, IDS_ROOKIE},
    {0, 0} // Terminator
};

typedef struct
{
    UINT strID;
} LabelTextMapping;

static const LabelTextMapping g_LabelMappings[] = {
    {IDS_LOG_MODE}, {IDS_DRIVER_ID}, {IDS_BPP}, {IDS_WIDTH}, {IDS_HEIGHT}, {IDS_ANTIALIAS},
    {IDS_VERTEX_CACHE}, {IDS_TEXTURE_VIDEO_FORMAT}, {IDS_SPRITE_VIDEO_FORMAT}, {IDS_POSITION_X},
    {IDS_POSITION_Y}, {IDS_LANGUAGE}, {IDS_UI_LANGUAGE},
    {0} // Terminator
};

typedef struct
{
    int left;
    int top;
} LabelPosition;

static const LabelPosition g_LabelPositions[] = {
    {14, 20}, {235, 20}, {330, 20}, {235, 36}, {330, 36}, {235, 158}, {235, 227},
    {235, 270}, {235, 286}, {14, 172}, {115, 172}, {14, 216}, {14, 350},
    {0, 0} // Terminator
};

// --- StringResource Implementation ---

HINSTANCE StringResource::m_hInstance = NULL;
UILanguage StringResource::m_CurrentLanguage = LANG_UI_ENGLISH;
char StringResource::m_Buffer[4096] = {0};
TCHAR StringResource::m_TBuffer[4096] = {0};

bool StringResource::Initialize(HINSTANCE hInstance)
{
    m_hInstance = hInstance;
    return (m_hInstance != NULL);
}

const char *StringResource::GetString(UINT resourceID)
{
    if (!m_hInstance)
    {
#ifdef UNICODE
        _tcscpy(m_TBuffer, TEXT("ERR: Uninitialized"));
        utils::WcharToChar(m_TBuffer, m_Buffer, sizeof(m_Buffer));
#else
        strcpy(m_Buffer, "ERR: Uninitialized");
#endif
        return m_Buffer;
    }

    UINT mappedID = MapToLanguageID(resourceID);

#ifdef UNICODE
    int len = ::LoadStringW(m_hInstance, mappedID, m_TBuffer, sizeof(m_TBuffer) / sizeof(TCHAR) - 1);

    if (len <= 0)
    {
        if (m_CurrentLanguage != LANG_UI_ENGLISH && resourceID != mappedID)
        {
            mappedID = resourceID;
            len = ::LoadStringW(m_hInstance, mappedID, m_TBuffer, sizeof(m_TBuffer) / sizeof(TCHAR) - 1);
        }
    }

    if (len <= 0)
    {
        _stprintf(m_TBuffer, TEXT("ERR: ID %u"), resourceID);
        len = _tcslen(m_TBuffer);
    }

    m_TBuffer[len] = TEXT('\0');
    utils::WcharToChar(m_TBuffer, m_Buffer, sizeof(m_Buffer));
#else
    int len = ::LoadStringA(m_hInstance, mappedID, m_Buffer, sizeof(m_Buffer) - 1);

    if (len <= 0)
    {
        if (m_CurrentLanguage != LANG_UI_ENGLISH && resourceID != mappedID)
        {
            mappedID = resourceID;
            len = ::LoadStringA(m_hInstance, mappedID, m_Buffer, sizeof(m_Buffer) - 1);
        }
    }

    if (len <= 0)
    {
        sprintf(m_Buffer, "ERR: ID %u", resourceID);
        len = strlen(m_Buffer);
    }

    m_Buffer[len] = '\0';
#endif

    return m_Buffer;
}

UILanguage StringResource::GetLanguage()
{
    return m_CurrentLanguage;
}

void StringResource::SetLanguage(UILanguage lang)
{
    if (lang >= 0 && lang < LANG_UI_COUNT)
    {
        m_CurrentLanguage = lang;
    }
}

UILanguage StringResource::DetectSystemLanguage()
{
    LANGID langId = ::GetSystemDefaultLangID();
    WORD primaryLangId = PRIMARYLANGID(langId);

    if (primaryLangId == LANG_CHINESE)
        return LANG_UI_CHINESE;

    return LANG_UI_ENGLISH; // Default
}

UINT StringResource::MapToLanguageID(UINT resourceID)
{
    if (m_CurrentLanguage == LANG_UI_ENGLISH)
    {
        return resourceID; // English uses base IDs
    }

    if (m_CurrentLanguage == LANG_UI_CHINESE)
    {
        if (resourceID >= 1000 && resourceID < 2000)
        {
            return resourceID + 1000;
        }
    }

    return resourceID; // Fallback
}

// --- Font Management ---

static void CleanupFonts()
{
    if (g_hFonts[LANG_UI_CHINESE] != NULL &&
        g_hFonts[LANG_UI_CHINESE] != ::GetStockObject(DEFAULT_GUI_FONT) &&
        g_hFonts[LANG_UI_CHINESE] != ::GetStockObject(SYSTEM_FONT))
    {
        ::DeleteObject(g_hFonts[LANG_UI_CHINESE]);
    }
    g_hFonts[LANG_UI_CHINESE] = NULL;
}

static void InitializeFonts()
{
    CleanupFonts();

    // Create Chinese Font (SimSun preferred for compatibility, GB2312 charset)
    g_hFonts[LANG_UI_CHINESE] = ::CreateFont(
        -12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("SimSun"));

    if (!g_hFonts[LANG_UI_CHINESE])
    {
        g_hFonts[LANG_UI_CHINESE] = ::CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                                 GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("NSimSun"));
    }
    if (!g_hFonts[LANG_UI_CHINESE])
    {
        g_hFonts[LANG_UI_CHINESE] = ::CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                                 GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Microsoft YaHei"));
    }
    if (!g_hFonts[LANG_UI_CHINESE])
    {
        g_hFonts[LANG_UI_CHINESE] = ::CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                                 GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("MS Sans Serif"));
    }
    // Last resort: Use system default (may not render CJK well)
    if (!g_hFonts[LANG_UI_CHINESE])
    {
        g_hFonts[LANG_UI_CHINESE] = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
    }
}

// Callback to apply font to child windows
static BOOL CALLBACK SetFontToChildProc(HWND hwnd, LPARAM lParam)
{
    SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
    return TRUE;
}

// --- Dialog Update Logic ---

static void UpdateDialogLanguage(HWND hDlg)
{
    // Set the appropriate font for the current language
    HFONT hFont = NULL;
    UILanguage currentLang = StringResource::GetLanguage();

    if (currentLang == LANG_UI_CHINESE && g_hFonts[LANG_UI_CHINESE] != NULL)
    {
        hFont = g_hFonts[LANG_UI_CHINESE];
    }
    else
    {
        hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
    }

    // Apply the selected font
    if (hFont)
    {
        SendMessage(hDlg, WM_SETFONT, (WPARAM)hFont, FALSE);
        ::EnumChildWindows(hDlg, SetFontToChildProc, (LPARAM)hFont);
    }

    // Set window title
    ::SetWindowText(hDlg, StringResource::GetString(IDS_DIALOG_TITLE));

    // Set text for controls with explicit mappings (Buttons, Checkboxes, Group Boxes)
    for (int i = 0; g_TextMappings[i].ctrlID != 0; i++)
    {
        ::SetDlgItemText(hDlg, g_TextMappings[i].ctrlID, StringResource::GetString(g_TextMappings[i].strID));
    }

    // Update ComboBox items and retain selection

    // Log Mode combo
    int logModeSel = ::SendDlgItemMessage(hDlg, IDC_COMBO_LOGMODE, CB_GETCURSEL, 0, 0);
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LOGMODE, CB_RESETCONTENT, 0, 0);
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LOGMODE, CB_ADDSTRING, 0, (LPARAM)StringResource::GetString(IDS_LOG_APPEND));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LOGMODE, CB_ADDSTRING, 0, (LPARAM)StringResource::GetString(IDS_LOG_OVERWRITE));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LOGMODE, CB_SETCURSEL, (logModeSel == 0 || logModeSel == 1) ? logModeSel : 1, 0);

    // BPP combo
    int bppSel = ::SendDlgItemMessage(hDlg, IDC_COMBO_BPP, CB_GETCURSEL, 0, 0);
    ::SendDlgItemMessage(hDlg, IDC_COMBO_BPP, CB_RESETCONTENT, 0, 0);
    ::SendDlgItemMessage(hDlg, IDC_COMBO_BPP, CB_ADDSTRING, 0, (LPARAM)TEXT("16"));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_BPP, CB_ADDSTRING, 0, (LPARAM)TEXT("32"));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_BPP, CB_SETCURSEL, (bppSel == 0 || bppSel == 1) ? bppSel : 1, 0);

    // Game Language combo
    int langGameSel = ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_GETCURSEL, 0, 0);
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_RESETCONTENT, 0, 0);
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_ADDSTRING, 0, (LPARAM)StringResource::GetString(IDS_LANG_GERMAN));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_ADDSTRING, 0, (LPARAM)StringResource::GetString(IDS_LANG_ENGLISH));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_ADDSTRING, 0, (LPARAM)StringResource::GetString(IDS_LANG_SPANISH));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_ADDSTRING, 0, (LPARAM)StringResource::GetString(IDS_LANG_ITALIAN));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_ADDSTRING, 0, (LPARAM)StringResource::GetString(IDS_LANG_FRENCH));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_SETCURSEL, (langGameSel >= 0 && langGameSel <= 4) ? langGameSel : 1, 0);

    // Interface Language combo
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANGUAGE, CB_RESETCONTENT, 0, 0);
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)StringResource::GetString(IDS_UI_ENGLISH));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)StringResource::GetString(IDS_UI_CHINESE));
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANGUAGE, CB_SETCURSEL, currentLang, 0);

    // Update static text labels (LTEXT) using position matching
    HWND hwndChild = ::GetWindow(hDlg, GW_CHILD);
    while (hwndChild)
    {
        TCHAR className[32];
        if (::GetClassName(hwndChild, className, sizeof(className) / sizeof(TCHAR)) && _tcscmp(className, TEXT("Static")) == 0)
        {
            DWORD ctrlId = ::GetDlgCtrlID(hwndChild);
            bool isHandledByMapping = false;
            for (int i = 0; g_TextMappings[i].ctrlID != 0; i++)
            {
                if (g_TextMappings[i].ctrlID == (int)ctrlId)
                {
                    isHandledByMapping = true;
                    break;
                }
            }

            // If it's an LTEXT label (unmapped Static control, not IDC_STATIC=-1), match by position
            if (!isHandledByMapping && ctrlId == (DWORD)-1)
            {
                // Get the control's actual position in PIXELS relative to the dialog client area
                RECT controlPixelRect;
                ::GetWindowRect(hwndChild, &controlPixelRect);
                ::MapWindowPoints(NULL, hDlg, (POINT *)&controlPixelRect, 2);

                // Now, iterate through our known DLU positions and convert each to pixels for comparison
                for (int i = 0; g_LabelMappings[i].strID != 0; i++)
                {
                    // Create a RECT using the stored DLU coordinates from g_LabelPositions
                    RECT labelDluRect;
                    labelDluRect.left = g_LabelPositions[i].left;
                    labelDluRect.top = g_LabelPositions[i].top;
                    labelDluRect.right = g_LabelPositions[i].left + 1; // Minimal width DLU
                    labelDluRect.bottom = g_LabelPositions[i].top + 1; // Minimal height DLU

                    // Convert this DLU rect to PIXEL coordinates relative to the dialog
                    RECT labelPixelRect = labelDluRect; // Copy structure
                    if (::MapDialogRect(hDlg, &labelPixelRect))
                    {
                        // Conversion successful. Now compare the TOP-LEFT PIXEL coordinates
                        const int POSITION_TOLERANCE = 10;
                        if (abs(controlPixelRect.left - labelPixelRect.left) <= POSITION_TOLERANCE &&
                            abs(controlPixelRect.top - labelPixelRect.top) <= POSITION_TOLERANCE)
                        {
                            // PIXEL positions match! Update the text
                            ::SetWindowText(hwndChild, StringResource::GetString(g_LabelMappings[i].strID));
                            break;
                        }
                    }
                }
            }
        }
        hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
    }

    // Force redraw
    ::InvalidateRect(hDlg, NULL, TRUE);
    ::UpdateWindow(hDlg);
}

// --- Configuration File Handling ---

static BOOL SetConfigPath(CGameConfig &config)
{
    TCHAR modulePath[MAX_PATH];
    char configPath[MAX_PATH];

    // Try <ExeDirectory>\Player.ini
    DWORD dwResult = ::GetModuleFileName(NULL, modulePath, MAX_PATH);
    if (dwResult > 0 && dwResult < MAX_PATH)
    {
#ifdef UNICODE
        char ansiModulePath[MAX_PATH];
        utils::WcharToChar(modulePath, ansiModulePath, MAX_PATH);
        ansiModulePath[dwResult] = '\0';
        char *lastSlash = strrchr(ansiModulePath, '\\');
#else
        modulePath[dwResult] = '\0';
        char *lastSlash = strrchr(modulePath, '\\');
#endif

        if (lastSlash)
        {
            *(lastSlash + 1) = '\0';
#ifdef UNICODE
            sprintf(configPath, "%sPlayer.ini", ansiModulePath);
#else
            sprintf(configPath, "%sPlayer.ini", modulePath);
#endif
            config.SetPath(eConfigPath, configPath);
            return TRUE;
        }
    }

    // Fallback: <CurrentWorkingDirectory>\Player.ini
    TCHAR currentDir[MAX_PATH];
    if (::GetCurrentDirectory(MAX_PATH, currentDir))
    {
#ifdef UNICODE
        char ansiCurrentDir[MAX_PATH];
        utils::WcharToChar(currentDir, ansiCurrentDir, MAX_PATH);
        size_t len = strlen(ansiCurrentDir);
#else
        size_t len = _tcslen(currentDir);
#endif

        if (len > 0)
        {
#ifdef UNICODE
            if (ansiCurrentDir[len - 1] != '\\')
            {
                if (len + 11 < MAX_PATH)
                {
                    strcat(ansiCurrentDir, "\\");
                }
                else
                {
                    config.SetPath(eConfigPath, "Player.ini");
                    return FALSE;
                }
            }
            else if (len + 10 >= MAX_PATH)
            {
                config.SetPath(eConfigPath, "Player.ini");
                return FALSE;
            }
            sprintf(configPath, "%sPlayer.ini", ansiCurrentDir);
#else
            if (currentDir[len - 1] != '\\')
            {
                if (len + 11 < MAX_PATH)
                {
                    strcat(currentDir, "\\");
                }
                else
                {
                    config.SetPath(eConfigPath, "Player.ini");
                    return FALSE;
                }
            }
            else if (len + 10 >= MAX_PATH)
            {
                config.SetPath(eConfigPath, "Player.ini");
                return FALSE;
            }
            sprintf(configPath, "%sPlayer.ini", currentDir);
#endif

            config.SetPath(eConfigPath, configPath);
            return TRUE;
        }
    }

    // Last resort: relative "Player.ini"
    config.SetPath(eConfigPath, "Player.ini");
    return FALSE;
}

static int GetDlgItemIntSafe(HWND hDlg, int nIDDlgItem, int defaultVal)
{
    TCHAR buffer[64];
    if (::GetDlgItemText(hDlg, nIDDlgItem, buffer, sizeof(buffer) / sizeof(TCHAR)))
    {
        TCHAR *p = buffer;
        while (*p && _istspace(*p))
            p++;
        if (*p == TEXT('\0'))
            return defaultVal; // Empty or whitespace

        TCHAR *endPtr;
        long val = _tcstol(p, &endPtr, 10);

        // Check conversion success and valid range for int
        if (endPtr != p && (*endPtr == TEXT('\0') || _istspace(*endPtr)))
        {
            if (val >= -2147483647L - 1 && val <= 2147483647L)
            {
                return (int)val;
            }
        }
    }
    return defaultVal;
}

static void CreateDefaultConfig(CGameConfig &config)
{
    config.logMode = eLogOverwrite;
    config.verbose = false;
    config.manualSetup = false;
    config.loadAllManagers = true;
    config.loadAllBuildingBlocks = true;
    config.loadAllPlugins = true;
    config.driver = 0;
    config.bpp = PLAYER_DEFAULT_BPP;
    config.width = PLAYER_DEFAULT_WIDTH;
    config.height = PLAYER_DEFAULT_HEIGHT;
    config.fullscreen = false;
    config.disablePerspectiveCorrection = false;
    config.forceLinearFog = false;
    config.forceSoftware = false;
    config.disableFilter = false;
    config.ensureVertexShader = false;
    config.useIndexBuffers = true;
    config.disableDithering = false;
    config.antialias = 0;
    config.disableMipmap = false;
    config.disableSpecular = false;
    config.enableScreenDump = false;
    config.enableDebugMode = false;
    config.vertexCache = 16;
    config.textureCacheManagement = true;
    config.sortTransparentObjects = true;
    config.textureVideoFormat = UNKNOWN_PF;
    config.spriteVideoFormat = UNKNOWN_PF;
    config.childWindowRendering = false;
    config.borderless = false;
    config.clipCursor = false;
    config.alwaysHandleInput = false;
    config.posX = 2147483647;
    config.posY = 2147483647;
    config.langId = 1;
    config.skipOpening = false;
    config.applyHotfix = true;
    config.unlockFramerate = false;
    config.unlockWidescreen = false;
    config.unlockHighResolution = false;
    config.debug = false;
    config.rookie = false;
}

static void LoadConfigToDialog(HWND hDlg, const CGameConfig &config)
{
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LOGMODE, CB_SETCURSEL, (config.logMode == eLogAppend) ? 0 : 1, 0);
    ::SetDlgItemInt(hDlg, IDC_EDIT_DRIVER, config.driver, FALSE);
    ::SendDlgItemMessage(hDlg, IDC_COMBO_BPP, CB_SETCURSEL, (config.bpp == 16) ? 0 : 1, 0);
    ::SetDlgItemInt(hDlg, IDC_EDIT_WIDTH, config.width, FALSE);
    ::SetDlgItemInt(hDlg, IDC_EDIT_HEIGHT, config.height, FALSE);
    ::SetDlgItemInt(hDlg, IDC_EDIT_ANTIALIAS, config.antialias, FALSE);
    ::SetDlgItemInt(hDlg, IDC_EDIT_VERTEXCACHE, config.vertexCache, FALSE);

    typedef struct
    {
        int ctrlID;
        bool value;
    } BooleanMapping;
    const BooleanMapping boolMap[] = {
        {IDC_CHECK_VERBOSE, config.verbose}, {IDC_CHECK_MANUALSETUP, config.manualSetup},
        {IDC_CHECK_LOADALLMGR, config.loadAllManagers}, {IDC_CHECK_LOADALLBB, config.loadAllBuildingBlocks},
        {IDC_CHECK_LOADALLPLUG, config.loadAllPlugins}, {IDC_CHECK_FULLSCREEN, config.fullscreen},
        {IDC_CHECK_DISPERSPCORRECT, config.disablePerspectiveCorrection},
        {IDC_CHECK_FORCELINEARFOG, config.forceLinearFog},
        {IDC_CHECK_FORCESOFTWARE, config.forceSoftware}, {IDC_CHECK_DISABLEFILTER, config.disableFilter},
        {IDC_CHECK_ENSUREVS, config.ensureVertexShader}, {IDC_CHECK_USEINDEXBUFFERS, config.useIndexBuffers},
        {IDC_CHECK_DISABLEDITHER, config.disableDithering}, {IDC_CHECK_DISABLEMIPMAP, config.disableMipmap},
        {IDC_CHECK_DISABLESPECULAR, config.disableSpecular}, {IDC_CHECK_ENABLESCREENDUMP, config.enableScreenDump},
        {IDC_CHECK_ENABLEDEBUGMODE_GFX, config.enableDebugMode},
        {IDC_CHECK_TEXCACHEMGMT, config.textureCacheManagement},
        {IDC_CHECK_SORTTRANSPARENT, config.sortTransparentObjects},
        {IDC_CHECK_CHILDWINRENDER, config.childWindowRendering},
        {IDC_CHECK_BORDERLESS, config.borderless}, {IDC_CHECK_CLIPCURSOR, config.clipCursor},
        {IDC_CHECK_ALWAYSHANDLEINPUT, config.alwaysHandleInput}, {IDC_CHECK_SKIPOPENING, config.skipOpening},
        {IDC_CHECK_APPLYHOTFIX, config.applyHotfix}, {IDC_CHECK_UNLOCKFRAMERATE, config.unlockFramerate},
        {IDC_CHECK_UNLOCKWIDESCREEN, config.unlockWidescreen}, {IDC_CHECK_UNLOCKHIGHRES, config.unlockHighResolution},
        {IDC_CHECK_DEBUG, config.debug}, {IDC_CHECK_ROOKIE, config.rookie},
        {0, false} // Terminator
    };
    for (int i = 0; boolMap[i].ctrlID != 0; i++)
    {
        ::SendDlgItemMessage(hDlg, boolMap[i].ctrlID, BM_SETCHECK, boolMap[i].value ? BST_CHECKED : BST_UNCHECKED, 0);
    }

    // Get pixel format string from utility function
    const char *texFormatStr = utils::PixelFormat2String(config.textureVideoFormat);
    const char *sprFormatStr = utils::PixelFormat2String(config.spriteVideoFormat);

#ifdef UNICODE
    WCHAR texFormatWStr[32];
    WCHAR sprFormatWStr[32];
    utils::CharToWchar(texFormatStr, texFormatWStr, 32);
    utils::CharToWchar(sprFormatStr, sprFormatWStr, 32);
    ::SetDlgItemTextW(hDlg, IDC_EDIT_TEXVIDFORMAT, texFormatWStr);
    ::SetDlgItemTextW(hDlg, IDC_EDIT_SPRVIDFORMAT, sprFormatWStr);
#else
    ::SetDlgItemTextA(hDlg, IDC_EDIT_TEXVIDFORMAT, texFormatStr);
    ::SetDlgItemTextA(hDlg, IDC_EDIT_SPRVIDFORMAT, sprFormatStr);
#endif

    // Handle Window Position (display empty for CW_USEDEFAULT)
    if (config.posX != CW_USEDEFAULT)
        ::SetDlgItemInt(hDlg, IDC_EDIT_POSX, config.posX, TRUE);
    else
        ::SetDlgItemText(hDlg, IDC_EDIT_POSX, TEXT(""));
    if (config.posY != CW_USEDEFAULT)
        ::SetDlgItemInt(hDlg, IDC_EDIT_POSY, config.posY, TRUE);
    else
        ::SetDlgItemText(hDlg, IDC_EDIT_POSY, TEXT(""));

    // Set language combobox selections
    int langSel = config.langId;
    if (langSel < 0 || langSel > 4)
        langSel = 1; // Default to English
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_SETCURSEL, langSel, 0);
    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANGUAGE, CB_SETCURSEL, StringResource::GetLanguage(), 0);
}

static void SaveDialogToConfig(HWND hDlg, CGameConfig &config)
{
    int logModeSel = ::SendDlgItemMessage(hDlg, IDC_COMBO_LOGMODE, CB_GETCURSEL, 0, 0);
    config.logMode = (logModeSel == 0) ? eLogAppend : eLogOverwrite;
    config.driver = GetDlgItemIntSafe(hDlg, IDC_EDIT_DRIVER, config.driver);
    int bppSel = ::SendDlgItemMessage(hDlg, IDC_COMBO_BPP, CB_GETCURSEL, 0, 0);
    config.bpp = (bppSel == 0) ? 16 : 32;
    config.width = GetDlgItemIntSafe(hDlg, IDC_EDIT_WIDTH, config.width);
    config.height = GetDlgItemIntSafe(hDlg, IDC_EDIT_HEIGHT, config.height);
    config.antialias = GetDlgItemIntSafe(hDlg, IDC_EDIT_ANTIALIAS, config.antialias);
    config.vertexCache = GetDlgItemIntSafe(hDlg, IDC_EDIT_VERTEXCACHE, config.vertexCache);
    config.posX = GetDlgItemIntSafe(hDlg, IDC_EDIT_POSX, CW_USEDEFAULT);
    config.posY = GetDlgItemIntSafe(hDlg, IDC_EDIT_POSY, CW_USEDEFAULT);
    int langSel = ::SendDlgItemMessage(hDlg, IDC_COMBO_LANG, CB_GETCURSEL, 0, 0);
    if (langSel >= 0 && langSel <= 4)
        config.langId = langSel;

    // Get checkbox states using a map
    typedef struct
    {
        int ctrlID;
        bool *value;
    } BooleanPtrMapping;
    const BooleanPtrMapping boolMap[] = {
        {IDC_CHECK_VERBOSE, &config.verbose}, {IDC_CHECK_MANUALSETUP, &config.manualSetup},
        {IDC_CHECK_LOADALLMGR, &config.loadAllManagers}, {IDC_CHECK_LOADALLBB, &config.loadAllBuildingBlocks},
        {IDC_CHECK_LOADALLPLUG, &config.loadAllPlugins}, {IDC_CHECK_FULLSCREEN, &config.fullscreen},
        {IDC_CHECK_DISPERSPCORRECT, &config.disablePerspectiveCorrection},
        {IDC_CHECK_FORCELINEARFOG, &config.forceLinearFog},
        {IDC_CHECK_FORCESOFTWARE, &config.forceSoftware}, {IDC_CHECK_DISABLEFILTER, &config.disableFilter},
        {IDC_CHECK_ENSUREVS, &config.ensureVertexShader}, {IDC_CHECK_USEINDEXBUFFERS, &config.useIndexBuffers},
        {IDC_CHECK_DISABLEDITHER, &config.disableDithering}, {IDC_CHECK_DISABLEMIPMAP, &config.disableMipmap},
        {IDC_CHECK_DISABLESPECULAR, &config.disableSpecular}, {IDC_CHECK_ENABLESCREENDUMP, &config.enableScreenDump},
        {IDC_CHECK_ENABLEDEBUGMODE_GFX, &config.enableDebugMode},
        {IDC_CHECK_TEXCACHEMGMT, &config.textureCacheManagement},
        {IDC_CHECK_SORTTRANSPARENT, &config.sortTransparentObjects},
        {IDC_CHECK_CHILDWINRENDER, &config.childWindowRendering},
        {IDC_CHECK_BORDERLESS, &config.borderless}, {IDC_CHECK_CLIPCURSOR, &config.clipCursor},
        {IDC_CHECK_ALWAYSHANDLEINPUT, &config.alwaysHandleInput}, {IDC_CHECK_SKIPOPENING, &config.skipOpening},
        {IDC_CHECK_APPLYHOTFIX, &config.applyHotfix}, {IDC_CHECK_UNLOCKFRAMERATE, &config.unlockFramerate},
        {IDC_CHECK_UNLOCKWIDESCREEN, &config.unlockWidescreen}, {IDC_CHECK_UNLOCKHIGHRES, &config.unlockHighResolution},
        {IDC_CHECK_DEBUG, &config.debug}, {IDC_CHECK_ROOKIE, &config.rookie},
        {0, NULL} // Terminator
    };
    for (int i = 0; boolMap[i].ctrlID != 0; i++)
    {
        *(boolMap[i].value) = (::SendDlgItemMessage(hDlg, boolMap[i].ctrlID, BM_GETCHECK, 0, 0) == BST_CHECKED);
    }

    // Handle Pixel Formats
    TCHAR buffer[MAX_PATH];

#ifdef UNICODE
    char ansiBuffer[MAX_PATH];
#endif

    ::GetDlgItemText(hDlg, IDC_EDIT_TEXVIDFORMAT, buffer, sizeof(buffer) / sizeof(TCHAR));
#ifdef UNICODE
    WcharToChar(buffer, ansiBuffer, MAX_PATH);
    config.textureVideoFormat = utils::String2PixelFormat(ansiBuffer, strlen(ansiBuffer));
#else
    config.textureVideoFormat = utils::String2PixelFormat(buffer, strlen(buffer));
#endif

    ::GetDlgItemText(hDlg, IDC_EDIT_SPRVIDFORMAT, buffer, sizeof(buffer) / sizeof(TCHAR));
#ifdef UNICODE
    WcharToChar(buffer, ansiBuffer, MAX_PATH);
    config.spriteVideoFormat = utils::String2PixelFormat(ansiBuffer, strlen(ansiBuffer));
#else
    config.spriteVideoFormat = utils::String2PixelFormat(buffer, strlen(buffer));
#endif

    // UI Language is saved separately in SaveUILanguageToIni
}

static void LoadUILanguageFromIni(const char *iniPath)
{
    if (!iniPath || *iniPath == '\0')
    {
        StringResource::SetLanguage(StringResource::DetectSystemLanguage());
        return;
    }

#ifdef UNICODE
    WCHAR wIniPath[MAX_PATH];
    utils::CharToWchar(iniPath, wIniPath, MAX_PATH);
    int langId = (int)::GetPrivateProfileIntW(TEXT("Interface"), TEXT("UILanguage"), -1, wIniPath);
#else
    int langId = (int)::GetPrivateProfileIntA("Interface", "UILanguage", -1, iniPath);
#endif

    if (langId >= 0 && langId < LANG_UI_COUNT)
    {
        StringResource::SetLanguage((UILanguage)langId);
    }
    else
    {
        StringResource::SetLanguage(StringResource::DetectSystemLanguage()); // Default to system
    }
}

static void SaveUILanguageToIni(const char *iniPath)
{
    if (!iniPath || *iniPath == '\0')
        return;

    TCHAR buffer[16];
    _stprintf(buffer, TEXT("%d"), StringResource::GetLanguage());

#ifdef UNICODE
    WCHAR wIniPath[MAX_PATH];
    utils::CharToWchar(iniPath, wIniPath, MAX_PATH);
    ::WritePrivateProfileStringW(TEXT("Interface"), TEXT("UILanguage"), buffer, wIniPath);
#else
    ::WritePrivateProfileStringA("Interface", "UILanguage", buffer, iniPath);
#endif
}

// --- Dialog Procedure ---

// Compatibility definitions for Get/SetWindowLongPtr for VC++ 6.0
#ifndef GWLP_USERDATA
#define GWLP_USERDATA (-21)
typedef LONG LONG_PTR;
#define GetWindowLongPtrA GetWindowLongA
#define SetWindowLongPtrA SetWindowLongA
#define GetWindowLongPtrW GetWindowLongW
#define SetWindowLongPtrW SetWindowLongW
#endif

BOOL CALLBACK ConfigDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    CGameConfig *pConfig = NULL;
    if (message != WM_INITDIALOG)
    {
        // Retrieve stored pointer after init
#ifdef UNICODE
        pConfig = (CGameConfig *)::GetWindowLongPtrW(hDlg, GWLP_USERDATA);
#else
        pConfig = (CGameConfig *)::GetWindowLongPtrA(hDlg, GWLP_USERDATA);
#endif
    }

    switch (message)
    {
    case WM_INITDIALOG:
    {
        InitializeFonts();
        // Store the CGameConfig pointer passed via lParam
#ifdef UNICODE
        ::SetWindowLongPtrW(hDlg, GWLP_USERDATA, lParam);
#else
        ::SetWindowLongPtrA(hDlg, GWLP_USERDATA, lParam);
#endif
        pConfig = (CGameConfig *)lParam;

        if (pConfig)
        {
            UpdateDialogLanguage(hDlg);         // Set initial language strings/fonts
            LoadConfigToDialog(hDlg, *pConfig); // Load settings into controls
        }
        else
        {
            // Critical error: No config object provided
            ::MessageBox(hDlg, StringResource::GetString(IDS_ERR_NO_CONFIG), StringResource::GetString(IDS_ERR_CONFIG_TITLE), MB_OK | MB_ICONERROR);
            ::EndDialog(hDlg, IDCANCEL);
        }
        return TRUE; // Handled
    }

    case WM_COMMAND:
    {
        WORD command = LOWORD(wParam);
        WORD notifyCode = HIWORD(wParam);

        switch (command)
        {
        case IDOK:
            if (pConfig)
            {
                SaveDialogToConfig(hDlg, *pConfig); // Save UI state to config object
                if (pConfig->HasPath(eConfigPath))
                {
                    SaveUILanguageToIni(pConfig->GetPath(eConfigPath)); // Save UI language selection
                }
                CleanupFonts();
                ::EndDialog(hDlg, IDOK);
            }
            else
            {
                // Should not happen
                ::MessageBox(hDlg, StringResource::GetString(IDS_ERR_CANNOT_SAVE),
                             StringResource::GetString(IDS_ERR_CONFIG_TITLE), MB_OK | MB_ICONERROR);
                ::EndDialog(hDlg, IDCANCEL);
            }
            return TRUE; // Handled

        case IDCANCEL:
            CleanupFonts();
            ::EndDialog(hDlg, IDCANCEL);
            return TRUE; // Handled

        case IDC_BUTTON_DEFAULTS:
            if (pConfig && ::MessageBox(hDlg, StringResource::GetString(IDS_RESET_CONFIRM),
                                        StringResource::GetString(IDS_RESET_TITLE),
                                        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
            {
                CGameConfig defaultConfig;
                CreateDefaultConfig(defaultConfig);
                LoadConfigToDialog(hDlg, defaultConfig); // Load defaults into UI
            }
            return TRUE; // Handled

        case IDC_COMBO_LANGUAGE:
            if (notifyCode == CBN_SELCHANGE)
            {
                // Check if selection changed
                int langSel = ::SendDlgItemMessage(hDlg, IDC_COMBO_LANGUAGE, CB_GETCURSEL, 0, 0);
                if (langSel != CB_ERR && langSel >= 0 && langSel < LANG_UI_COUNT)
                {
                    if ((UILanguage)langSel != StringResource::GetLanguage())
                    {
                        StringResource::SetLanguage((UILanguage)langSel);
                        InitializeFonts();          // Font might need recreation
                        UpdateDialogLanguage(hDlg); // Update all UI text/fonts
                    }
                }
            }
            return TRUE; // Handled

        default: break;
        } // End switch (command)
    }
    break; // End of WM_COMMAND

    case WM_CLOSE:
        CleanupFonts();
        ::EndDialog(hDlg, IDCANCEL); // Treat close as Cancel
        return TRUE;                 // Handled

    case WM_DESTROY:
        CleanupFonts(); // Final cleanup
        return TRUE;    // Handled

    default: break;
    } // End switch (message)

    return FALSE; // Message is not handled
}

bool ShowConfigTool(HINSTANCE hInstance, CGameConfig &config, bool loadIni)
{
    if (!StringResource::Initialize(hInstance))
    {
        ::MessageBox(NULL, TEXT("Critical Error: Failed to initialize string resources."), TEXT("Configuration Tool Error"), MB_OK | MB_ICONERROR);
        return false;
    }

    // Ensure config path is set, determine if necessary
    const char *configPath = config.GetPath(eConfigPath);
    if (!configPath || !*configPath)
    {
        if (!SetConfigPath(config))
        {
            ::MessageBox(NULL, StringResource::GetString(IDS_WARN_CONFIG_PATH), StringResource::GetString(IDS_WARN_CONFIG_TITLE), MB_OK | MB_ICONWARNING);
        }
        configPath = config.GetPath(eConfigPath); // Get potentially updated path
    }

    if (loadIni)
    {
        // Load current settings from INI (or defaults if INI missing/invalid)
        config.LoadFromIni(configPath);

        // Load UI language preference from INI (must be after main load, before dialog creation)
        LoadUILanguageFromIni(configPath);
    }

    // Show the modal dialog
    INT_PTR result = ::DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CONFIG), NULL, ConfigDlgProc, (LPARAM)&config);
    if (result == IDOK)
    {
        config.SaveToIni(configPath); // Save config object state to INI
        return true; // Success
    }
    else if (result == -1)
    {
        // Dialog creation failed
        DWORD dwError = ::GetLastError();
        TCHAR errMsg[256];
        _stprintf(errMsg, TEXT("%s"), StringResource::GetString(IDS_ERR_DIALOG_CREATE));
        ::MessageBox(NULL, errMsg, StringResource::GetString(IDS_ERR_CONFIG_TITLE), MB_OK | MB_ICONERROR);
        return false; // Failure
    }
    else
    {
        // Cancel or other non-OK result
        return false; // Cancelled or failed
    }
}

#ifdef CONFIGTOOL_STANDALONE
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow; // Suppress unused warnings

    CGameConfig config;
    bool result = ShowConfigTool(hInstance, config); // Handles load, UI, save
    return result ? 0 : 1; // 0 on success (OK), 1 on cancel/fail
}
#endif // CONFIGTOOL_STANDALONE