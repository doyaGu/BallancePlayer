#include "ConfigTool.h"

#include <commctrl.h>
#include <string>
#include <string.h>
#include <tchar.h>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "ConfigToolResource.h"
#include "Utils.h"

#ifndef TTF_TRANSPARENT
#define TTF_TRANSPARENT 0x0100
#endif

// String Resource Management

typedef enum
{
    LANG_UI_ENGLISH = 0,
    LANG_UI_CHINESE,
    LANG_UI_COUNT
} UILanguage;

enum
{
    STRING_RESOURCE_BUFFER_COUNT = 8,
    STRING_RESOURCE_BUFFER_LENGTH = 4096
};

class StringResource
{
public:
    static bool Initialize(HINSTANCE hInstance);
    static LPCTSTR GetString(UINT resourceID);
    static UILanguage GetLanguage();
    static void SetLanguage(UILanguage lang);
    static UILanguage DetectSystemLanguage();

private:
    static HINSTANCE m_hInstance;
    static UILanguage m_CurrentLanguage;
    static TCHAR m_Buffers[STRING_RESOURCE_BUFFER_COUNT][STRING_RESOURCE_BUFFER_LENGTH];
    static int m_BufferIndex;

    static UINT MapToLanguageID(UINT resourceID);
};

static HFONT g_hFonts[LANG_UI_COUNT] = {NULL};
static const int CHINESE_UI_FONT_HEIGHT = -14;

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
typedef BOOL PLAYER_DIALOG_RESULT;
typedef UINT TOOLINFO_ID;
#else
typedef INT_PTR PLAYER_DIALOG_RESULT;
typedef UINT_PTR TOOLINFO_ID;
#endif

typedef struct
{
    int ctrlID;
    UINT strID;
} ControlTextMapping;

const ControlTextMapping g_TextMappings[] = {
    {IDOK, IDS_BTN_OK}, {IDCANCEL, IDS_BTN_CANCEL}, {IDC_BUTTON_DEFAULTS, IDS_BTN_DEFAULTS},
    {IDC_GROUP_STARTUP, IDS_GROUP_STARTUP}, {IDC_GROUP_GRAPHICS, IDS_GROUP_GRAPHICS},
    {IDC_GROUP_WINDOW, IDS_GROUP_WINDOW}, {IDC_GROUP_GAME, IDS_GROUP_GAME}, {IDC_GROUP_INTERFACE, IDS_GROUP_INTERFACE},
    {IDC_CHECK_VERBOSE, IDS_VERBOSE}, {IDC_CHECK_MANUALSETUP, IDS_MANUAL_SETUP}, {IDC_CHECK_FULLSCREEN, IDS_FULLSCREEN},
    {IDC_CHECK_CHILDWINRENDER, IDS_CHILD_WINDOW_RENDER},
    {IDC_CHECK_BORDERLESS, IDS_BORDERLESS}, {IDC_CHECK_CLIPCURSOR, IDS_CLIP_CURSOR},
    {IDC_CHECK_ALWAYSHANDLEINPUT, IDS_ALWAYS_HANDLE_INPUT}, {IDC_CHECK_SKIPOPENING, IDS_SKIP_OPENING},
    {IDC_CHECK_APPLYHOTFIX, IDS_APPLY_HOTFIX}, {IDC_CHECK_UNLOCKFRAMERATE, IDS_UNLOCK_FRAMERATE},
    {IDC_CHECK_UNLOCKWIDESCREEN, IDS_UNLOCK_WIDESCREEN}, {IDC_CHECK_UNLOCKHIGHRES, IDS_UNLOCK_HIGHRES},
    {IDC_CHECK_DEBUG, IDS_DEBUG}, {IDC_CHECK_ROOKIE, IDS_ROOKIE},
    {IDC_LABEL_LOGMODE, IDS_LOG_MODE}, {IDC_LABEL_DRIVER, IDS_DRIVER_ID}, {IDC_LABEL_BPP, IDS_BPP},
    {IDC_LABEL_WIDTH, IDS_WIDTH}, {IDC_LABEL_HEIGHT, IDS_HEIGHT}, {IDC_LABEL_POSX, IDS_POSITION_X},
    {IDC_LABEL_POSY, IDS_POSITION_Y}, {IDC_LABEL_GAME_LANGUAGE, IDS_LANGUAGE},
    {IDC_LABEL_UI_LANGUAGE, IDS_UI_LANGUAGE},
    {0, 0} // Terminator
};

static const ControlTextMapping g_ToolTipMappings[] = {
    {IDC_COMBO_LOGMODE, IDS_TIP_LOGMODE}, {IDC_CHECK_VERBOSE, IDS_TIP_VERBOSE},
    {IDC_CHECK_MANUALSETUP, IDS_TIP_MANUAL_SETUP}, {IDC_CONFIG_EDIT_DRIVER, IDS_TIP_DRIVER},
    {IDC_COMBO_BPP, IDS_TIP_BPP}, {IDC_EDIT_WIDTH, IDS_TIP_SIZE}, {IDC_EDIT_HEIGHT, IDS_TIP_SIZE},
    {IDC_CHECK_FULLSCREEN, IDS_TIP_FULLSCREEN}, {IDC_CHECK_CHILDWINRENDER, IDS_TIP_CHILD_WINDOW_RENDER},
    {IDC_CHECK_BORDERLESS, IDS_TIP_BORDERLESS}, {IDC_CHECK_CLIPCURSOR, IDS_TIP_CLIP_CURSOR},
    {IDC_CHECK_ALWAYSHANDLEINPUT, IDS_TIP_ALWAYS_HANDLE_INPUT}, {IDC_EDIT_POSX, IDS_TIP_POSITION},
    {IDC_EDIT_POSY, IDS_TIP_POSITION}, {IDC_COMBO_LANG, IDS_TIP_GAME_LANGUAGE},
    {IDC_CHECK_SKIPOPENING, IDS_TIP_SKIP_OPENING}, {IDC_CHECK_APPLYHOTFIX, IDS_TIP_APPLY_HOTFIX},
    {IDC_CHECK_UNLOCKFRAMERATE, IDS_TIP_UNLOCK_FRAMERATE}, {IDC_CHECK_UNLOCKWIDESCREEN, IDS_TIP_UNLOCK_WIDESCREEN},
    {IDC_CHECK_UNLOCKHIGHRES, IDS_TIP_UNLOCK_HIGHRES}, {IDC_CHECK_DEBUG, IDS_TIP_DEBUG},
    {IDC_CHECK_ROOKIE, IDS_TIP_ROOKIE}, {IDC_COMBO_LANGUAGE, IDS_TIP_UI_LANGUAGE},
    {IDC_BUTTON_DEFAULTS, IDS_TIP_DEFAULTS},
    {0, 0} // Terminator
};

enum
{
    TOOLTIP_COUNT = (sizeof(g_ToolTipMappings) / sizeof(g_ToolTipMappings[0])) - 1
};

typedef struct
{
    CGameConfig *config;
    HWND tooltip;
    TCHAR toolTipTexts[TOOLTIP_COUNT][256];
} ConfigDialogState;

// StringResource Implementation

HINSTANCE StringResource::m_hInstance = NULL;
UILanguage StringResource::m_CurrentLanguage = LANG_UI_ENGLISH;
TCHAR StringResource::m_Buffers[STRING_RESOURCE_BUFFER_COUNT][STRING_RESOURCE_BUFFER_LENGTH] = {{0}};
int StringResource::m_BufferIndex = 0;

bool StringResource::Initialize(HINSTANCE hInstance)
{
    m_hInstance = hInstance;
    return (m_hInstance != NULL);
}

LPCTSTR StringResource::GetString(UINT resourceID)
{
    TCHAR *buffer = m_Buffers[m_BufferIndex];
    m_BufferIndex = (m_BufferIndex + 1) % STRING_RESOURCE_BUFFER_COUNT;

    if (!m_hInstance)
    {
        _tcscpy(buffer, TEXT("ERR: Uninitialized"));
        return buffer;
    }

    UINT mappedID = MapToLanguageID(resourceID);

#ifdef UNICODE
    int len = ::LoadStringW(m_hInstance, mappedID, buffer, STRING_RESOURCE_BUFFER_LENGTH - 1);

    if (len <= 0)
    {
        if (m_CurrentLanguage != LANG_UI_ENGLISH && resourceID != mappedID)
        {
            mappedID = resourceID;
            len = ::LoadStringW(m_hInstance, mappedID, buffer, STRING_RESOURCE_BUFFER_LENGTH - 1);
        }
    }

    if (len <= 0)
    {
        _stprintf(buffer, TEXT("ERR: ID %u"), resourceID);
        len = (int)_tcslen(buffer);
    }
#else
    int len = ::LoadStringA(m_hInstance, mappedID, buffer, STRING_RESOURCE_BUFFER_LENGTH - 1);

    if (len <= 0)
    {
        if (m_CurrentLanguage != LANG_UI_ENGLISH && resourceID != mappedID)
        {
            mappedID = resourceID;
            len = ::LoadStringA(m_hInstance, mappedID, buffer, STRING_RESOURCE_BUFFER_LENGTH - 1);
        }
    }

    if (len <= 0)
    {
        sprintf(buffer, "ERR: ID %u", resourceID);
        len = (int)strlen(buffer);
    }
#endif

    buffer[len] = TEXT('\0');
    return buffer;
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

// Font Management

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
        CHINESE_UI_FONT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("SimSun"));

    if (!g_hFonts[LANG_UI_CHINESE])
    {
        g_hFonts[LANG_UI_CHINESE] = ::CreateFont(CHINESE_UI_FONT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                                 GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("NSimSun"));
    }
    if (!g_hFonts[LANG_UI_CHINESE])
    {
        g_hFonts[LANG_UI_CHINESE] = ::CreateFont(CHINESE_UI_FONT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                                 GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Microsoft YaHei"));
    }
    if (!g_hFonts[LANG_UI_CHINESE])
    {
        g_hFonts[LANG_UI_CHINESE] = ::CreateFont(CHINESE_UI_FONT_HEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
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

static bool BuildToolInfo(HWND hDlg, ConfigDialogState *state, int index, TOOLINFO *toolInfo)
{
    if (!state)
        return false;

    HWND hwndCtrl = ::GetDlgItem(hDlg, g_ToolTipMappings[index].ctrlID);
    if (!hwndCtrl)
        return false;

    ZeroMemory(toolInfo, sizeof(*toolInfo));
    toolInfo->cbSize = sizeof(*toolInfo);
    toolInfo->uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
    toolInfo->hwnd = hDlg;
    toolInfo->uId = (TOOLINFO_ID)hwndCtrl;
    toolInfo->lpszText = state->toolTipTexts[index];
    return true;
}

static void CopyToolTipText(ConfigDialogState *state, int index)
{
    _tcsncpy(state->toolTipTexts[index], StringResource::GetString(g_ToolTipMappings[index].strID),
             sizeof(state->toolTipTexts[index]) / sizeof(state->toolTipTexts[index][0]) - 1);
    state->toolTipTexts[index][sizeof(state->toolTipTexts[index]) / sizeof(state->toolTipTexts[index][0]) - 1] = TEXT('\0');
}

static void UpdateToolTips(HWND hDlg, ConfigDialogState *state)
{
    if (!state || !state->tooltip)
        return;

    for (int i = 0; i < TOOLTIP_COUNT; i++)
    {
        CopyToolTipText(state, i);

        TOOLINFO toolInfo;
        if (BuildToolInfo(hDlg, state, i, &toolInfo))
        {
            ::SendMessage(state->tooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
        }
    }
}

static void InitializeToolTips(HWND hDlg, ConfigDialogState *state)
{
    if (!state || state->tooltip)
        return;

    INITCOMMONCONTROLSEX commonControls;
    commonControls.dwSize = sizeof(commonControls);
    commonControls.dwICC = ICC_WIN95_CLASSES;
    if (!::InitCommonControlsEx(&commonControls))
        ::InitCommonControls();

    state->tooltip = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
                                      WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
                                      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                      hDlg, NULL, NULL, NULL);
    if (!state->tooltip)
        return;

    ::SetWindowPos(state->tooltip, HWND_TOPMOST, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    ::SendMessage(state->tooltip, TTM_SETMAXTIPWIDTH, 0, 260);

    for (int i = 0; i < TOOLTIP_COUNT; i++)
    {
        CopyToolTipText(state, i);

        TOOLINFO toolInfo;
        if (BuildToolInfo(hDlg, state, i, &toolInfo))
        {
            ::SendMessage(state->tooltip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
        }
    }
}

static void CleanupToolTips(ConfigDialogState *state)
{
    if (state && state->tooltip)
    {
        ::DestroyWindow(state->tooltip);
        state->tooltip = NULL;
    }
}

static void ResetComboBoxItems(HWND hDlg, int ctrlID)
{
    ::SendDlgItemMessage(hDlg, ctrlID, CB_RESETCONTENT, 0, 0);
}

static void AddComboBoxString(HWND hDlg, int ctrlID, LPCTSTR text)
{
    ::SendDlgItemMessage(hDlg, ctrlID, CB_ADDSTRING, 0, (LPARAM)text);
}

static void SetComboBoxSelection(HWND hDlg, int ctrlID, int selection, int count, int fallback)
{
    if (selection < 0 || selection >= count)
        selection = fallback;
    ::SendDlgItemMessage(hDlg, ctrlID, CB_SETCURSEL, selection, 0);
}

static void UpdateResourceComboBox(HWND hDlg, int ctrlID, const UINT *items, int count, int fallback)
{
    int selection = (int)::SendDlgItemMessage(hDlg, ctrlID, CB_GETCURSEL, 0, 0);

    ResetComboBoxItems(hDlg, ctrlID);
    for (int i = 0; i < count; i++)
        AddComboBoxString(hDlg, ctrlID, StringResource::GetString(items[i]));

    SetComboBoxSelection(hDlg, ctrlID, selection, count, fallback);
}

static void UpdateTextComboBox(HWND hDlg, int ctrlID, const LPCTSTR *items, int count, int fallback)
{
    int selection = (int)::SendDlgItemMessage(hDlg, ctrlID, CB_GETCURSEL, 0, 0);

    ResetComboBoxItems(hDlg, ctrlID);
    for (int i = 0; i < count; i++)
        AddComboBoxString(hDlg, ctrlID, items[i]);

    SetComboBoxSelection(hDlg, ctrlID, selection, count, fallback);
}

// Dialog Update Logic

static void UpdateDialogLanguage(HWND hDlg, ConfigDialogState *state)
{
    int i;

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
    for (i = 0; g_TextMappings[i].ctrlID != 0; i++)
    {
        ::SetDlgItemText(hDlg, g_TextMappings[i].ctrlID, StringResource::GetString(g_TextMappings[i].strID));
    }

    static const UINT logModeItems[] = {IDS_LOG_APPEND, IDS_LOG_OVERWRITE};
    static const LPCTSTR bppItems[] = {TEXT("16"), TEXT("32")};
    static const UINT gameLanguageItems[] = {
        IDS_LANG_GERMAN, IDS_LANG_ENGLISH, IDS_LANG_SPANISH, IDS_LANG_ITALIAN, IDS_LANG_FRENCH};
    static const UINT uiLanguageItems[] = {IDS_UI_ENGLISH, IDS_UI_CHINESE};

    UpdateResourceComboBox(hDlg, IDC_COMBO_LOGMODE, logModeItems, sizeof(logModeItems) / sizeof(logModeItems[0]), 1);
    UpdateTextComboBox(hDlg, IDC_COMBO_BPP, bppItems, sizeof(bppItems) / sizeof(bppItems[0]), 1);
    UpdateResourceComboBox(hDlg, IDC_COMBO_LANG, gameLanguageItems, sizeof(gameLanguageItems) / sizeof(gameLanguageItems[0]), 1);

    ResetComboBoxItems(hDlg, IDC_COMBO_LANGUAGE);
    for (i = 0; i < (int)(sizeof(uiLanguageItems) / sizeof(uiLanguageItems[0])); i++)
        AddComboBoxString(hDlg, IDC_COMBO_LANGUAGE, StringResource::GetString(uiLanguageItems[i]));
    SetComboBoxSelection(hDlg, IDC_COMBO_LANGUAGE, currentLang, LANG_UI_COUNT, LANG_UI_ENGLISH);

    UpdateToolTips(hDlg, state);

    // Force redraw
    ::InvalidateRect(hDlg, NULL, TRUE);
    ::UpdateWindow(hDlg);
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

static void SetConfigIntControl(HWND hDlg, int ctrlID, int value)
{
    if (ctrlID == IDC_COMBO_LOGMODE)
    {
        ::SendDlgItemMessage(hDlg, ctrlID, CB_SETCURSEL, (value == eLogAppend) ? 0 : 1, 0);
        return;
    }
    if (ctrlID == IDC_COMBO_BPP)
    {
        ::SendDlgItemMessage(hDlg, ctrlID, CB_SETCURSEL, (value == 16) ? 0 : 1, 0);
        return;
    }
    if (ctrlID == IDC_COMBO_LANG)
    {
        int langSel = value;
        if (langSel < 0 || langSel > 4)
            langSel = 1;
        ::SendDlgItemMessage(hDlg, ctrlID, CB_SETCURSEL, langSel, 0);
        return;
    }
    if (ctrlID == IDC_EDIT_POSX || ctrlID == IDC_EDIT_POSY)
    {
        if (value != CW_USEDEFAULT)
            ::SetDlgItemInt(hDlg, ctrlID, value, TRUE);
        else
            ::SetDlgItemText(hDlg, ctrlID, TEXT(""));
        return;
    }

    ::SetDlgItemInt(hDlg, ctrlID, value, FALSE);
}

static int GetConfigIntControl(HWND hDlg, int ctrlID, int fallback)
{
    if (ctrlID == IDC_COMBO_LOGMODE)
    {
        int sel = (int)::SendDlgItemMessage(hDlg, ctrlID, CB_GETCURSEL, 0, 0);
        if (sel == CB_ERR)
            return fallback;
        return (sel == 0) ? eLogAppend : eLogOverwrite;
    }
    if (ctrlID == IDC_COMBO_BPP)
    {
        int sel = (int)::SendDlgItemMessage(hDlg, ctrlID, CB_GETCURSEL, 0, 0);
        if (sel == CB_ERR)
            return fallback;
        return (sel == 0) ? 16 : 32;
    }
    if (ctrlID == IDC_COMBO_LANG)
    {
        int sel = (int)::SendDlgItemMessage(hDlg, ctrlID, CB_GETCURSEL, 0, 0);
        if (sel >= 0 && sel <= 4)
            return sel;
        return fallback;
    }
    if (ctrlID == IDC_EDIT_POSX || ctrlID == IDC_EDIT_POSY)
    {
        return GetDlgItemIntSafe(hDlg, ctrlID, CW_USEDEFAULT);
    }

    return GetDlgItemIntSafe(hDlg, ctrlID, fallback);
}

static void SetConfigBoolControl(HWND hDlg, int ctrlID, bool value)
{
    ::SendDlgItemMessage(hDlg, ctrlID, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
}

static bool GetConfigBoolControl(HWND hDlg, int ctrlID)
{
    return ::SendDlgItemMessage(hDlg, ctrlID, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

static bool ConfigPathToTChar(const char *iniPath, TCHAR *buffer, size_t size)
{
    if (!iniPath || iniPath[0] == '\0' || !buffer || size == 0)
        return false;

#ifdef UNICODE
    if (utils::CharToWchar(iniPath, buffer, size) == 0)
    {
        buffer[0] = TEXT('\0');
        return false;
    }
    buffer[size - 1] = TEXT('\0');
#else
    size_t len = strlen(iniPath);
    if (len + 1 > size)
    {
        buffer[0] = TEXT('\0');
        return false;
    }
    memcpy(buffer, iniPath, len + 1);
#endif
    return true;
}

static void LoadConfigToDialog(HWND hDlg, const CGameConfig &config)
{
#define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
    SetConfigBoolControl(hDlg, IDC_CONFIG_##member, config.member);
#define X_INT(sec,key,member,def,cliLong,cliShort) \
    SetConfigIntControl(hDlg, IDC_CONFIG_##member, config.member);
    GAMECONFIG_FIELDS
#undef X_BOOL
#undef X_INT

    ::SendDlgItemMessage(hDlg, IDC_COMBO_LANGUAGE, CB_SETCURSEL, StringResource::GetLanguage(), 0);
}

static void SaveDialogToConfig(HWND hDlg, CGameConfig &config)
{
#define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
    config.member = GetConfigBoolControl(hDlg, IDC_CONFIG_##member);
#define X_INT(sec,key,member,def,cliLong,cliShort) \
    config.member = GetConfigIntControl(hDlg, IDC_CONFIG_##member, config.member);
    GAMECONFIG_FIELDS
#undef X_BOOL
#undef X_INT

    // UI Language is saved separately in SaveUILanguageToIni.
}

static void LoadUILanguageFromIni(const char *iniPath)
{
    TCHAR tIniPath[MAX_PATH];
    if (!ConfigPathToTChar(iniPath, tIniPath, MAX_PATH))
    {
        StringResource::SetLanguage(StringResource::DetectSystemLanguage());
        return;
    }

    int langId = (int)::GetPrivateProfileInt(TEXT("Interface"), TEXT("UILanguage"), -1, tIniPath);

    if (langId >= 0 && langId < LANG_UI_COUNT)
    {
        StringResource::SetLanguage((UILanguage)langId);
    }
    else
    {
        StringResource::SetLanguage(StringResource::DetectSystemLanguage()); // Default to system
    }
}

static bool SaveUILanguageToIni(const char *iniPath)
{
    TCHAR tIniPath[MAX_PATH];
    if (!ConfigPathToTChar(iniPath, tIniPath, MAX_PATH))
        return false;

    TCHAR buffer[16];
    _stprintf(buffer, TEXT("%d"), StringResource::GetLanguage());

    return ::WritePrivateProfileString(TEXT("Interface"), TEXT("UILanguage"), buffer, tIniPath) != 0;
}

static int ShowResourceMessage(HWND owner, UINT textID, UINT titleID, UINT type)
{
    LPCTSTR text = StringResource::GetString(textID);
    LPCTSTR title = StringResource::GetString(titleID);
    return ::MessageBox(owner, text, title, type);
}

static bool SaveDialogConfig(HWND hDlg, CGameConfig &config)
{
    if (!config.SaveToIni())
    {
        ShowResourceMessage(hDlg, IDS_ERR_CANNOT_SAVE, IDS_ERR_CONFIG_TITLE, MB_OK | MB_ICONERROR);
        return false;
    }

    const char *configPath = config.GetPath(eConfigPath);
    if (!SaveUILanguageToIni(configPath))
    {
        ShowResourceMessage(hDlg, IDS_ERR_CANNOT_SAVE, IDS_ERR_CONFIG_TITLE, MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

// Dialog Procedure

// Compatibility definitions for Get/SetWindowLongPtr for VC++ 6.0
#ifndef GWLP_USERDATA
#define GWLP_USERDATA (-21)
typedef LONG LONG_PTR;
#define GetWindowLongPtr GetWindowLong
#define SetWindowLongPtr SetWindowLong
#endif

static ConfigDialogState *GetDialogState(HWND hDlg)
{
    return (ConfigDialogState *)::GetWindowLongPtr(hDlg, GWLP_USERDATA);
}

static void SetDialogState(HWND hDlg, ConfigDialogState *state)
{
    ::SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)state);
}

PLAYER_DIALOG_RESULT CALLBACK ConfigDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    ConfigDialogState *state = NULL;
    CGameConfig *pConfig = NULL;
    if (message != WM_INITDIALOG)
    {
        state = GetDialogState(hDlg);
        if (state)
            pConfig = state->config;
    }

    switch (message)
    {
    case WM_INITDIALOG:
    {
        InitializeFonts();
        state = (ConfigDialogState *)lParam;
        SetDialogState(hDlg, state);
        pConfig = state ? state->config : NULL;
        InitializeToolTips(hDlg, state);

        if (pConfig)
        {
            UpdateDialogLanguage(hDlg, state);  // Set initial language strings/fonts
            LoadConfigToDialog(hDlg, *pConfig); // Load settings into controls
        }
        else
        {
            // Error: No config object provided
            ShowResourceMessage(hDlg, IDS_ERR_NO_CONFIG, IDS_ERR_CONFIG_TITLE, MB_OK | MB_ICONERROR);
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
                if (!SaveDialogConfig(hDlg, *pConfig))
                    return TRUE; // Keep dialog open so the user can retry or cancel
                ::EndDialog(hDlg, IDOK);
            }
            else
            {
                // Should not happen
                ShowResourceMessage(hDlg, IDS_ERR_CANNOT_SAVE, IDS_ERR_CONFIG_TITLE, MB_OK | MB_ICONERROR);
                ::EndDialog(hDlg, IDCANCEL);
            }
            return TRUE; // Handled

        case IDCANCEL:
            ::EndDialog(hDlg, IDCANCEL);
            return TRUE; // Handled

        case IDC_BUTTON_DEFAULTS:
            if (pConfig && ShowResourceMessage(hDlg, IDS_RESET_CONFIRM, IDS_RESET_TITLE,
                                               MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
            {
                CGameConfig defaultConfig;
                LoadConfigToDialog(hDlg, defaultConfig); // Load defaults into UI
            }
            return TRUE; // Handled

        case IDC_COMBO_LANGUAGE:
            if (notifyCode == CBN_SELCHANGE)
            {
                // Check if selection changed
                int langSel = (int)::SendDlgItemMessage(hDlg, IDC_COMBO_LANGUAGE, CB_GETCURSEL, 0, 0);
                if (langSel != CB_ERR && langSel >= 0 && langSel < LANG_UI_COUNT)
                {
                    if ((UILanguage)langSel != StringResource::GetLanguage())
                    {
                        StringResource::SetLanguage((UILanguage)langSel);
                        InitializeFonts();                 // Font might need recreation
                        UpdateDialogLanguage(hDlg, state); // Update all UI text/fonts
                    }
                }
            }
            return TRUE; // Handled

        default: break;
        } // End switch (command)
    }
    break; // End of WM_COMMAND

    case WM_CLOSE:
        ::EndDialog(hDlg, IDCANCEL); // Treat close as Cancel
        return TRUE;                 // Handled

    case WM_DESTROY:
        CleanupToolTips(state);
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

    if (!config.EnsureConfigPath())
    {
        ShowResourceMessage(NULL, IDS_WARN_CONFIG_PATH, IDS_WARN_CONFIG_TITLE, MB_OK | MB_ICONWARNING);
        return false;
    }

    const char *configPath = config.GetPath(eConfigPath);

    if (loadIni)
    {
        // Load current settings from INI (or defaults if INI missing/invalid)
        config.LoadFromIni(configPath);

        // Load UI language preference from INI (must be after main load, before dialog creation)
        LoadUILanguageFromIni(configPath);
    }

    ConfigDialogState dialogState;
    ZeroMemory(&dialogState, sizeof(dialogState));
    dialogState.config = &config;

    // Show the modal dialog
    INT_PTR result = ::DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CONFIG), NULL, ConfigDlgProc, (LPARAM)&dialogState);
    if (result == IDOK)
        return true; // Success
    else if (result == -1)
    {
        // Dialog creation failed
        DWORD dwError = ::GetLastError();
        TCHAR errMsg[256];
        _stprintf(errMsg, StringResource::GetString(IDS_ERR_DIALOG_CREATE), dwError);
        ::MessageBox(NULL, errMsg, StringResource::GetString(IDS_ERR_CONFIG_TITLE), MB_OK | MB_ICONERROR);
        return false; // Failure
    }
    else
    {
        // Cancel or other non-OK result
        return false; // Cancelled or failed
    }
}

static std::string GetCommandLineConfigPath(LPTSTR lpCmdLine)
{
    CmdlineParser parser(lpCmdLine);
    CmdlineArg arg;
    std::string path;
    while (!parser.Done())
    {
        if (parser.Next(arg, "--config", '\0', 1))
        {
            arg.GetValue(0, path);
            break;
        }
        parser.Skip();
    }
    return path;
}

#ifdef CONFIGTOOL_STANDALONE
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    (void)hPrevInstance;
    (void)nCmdShow;

    CGameConfig config;
    std::string configPath = GetCommandLineConfigPath(lpCmdLine);
    if (!configPath.empty())
        config.SetPath(eConfigPath, configPath.c_str());

    return ShowConfigTool(hInstance, config, true) ? 0 : 1;
}
#endif // CONFIGTOOL_STANDALONE
