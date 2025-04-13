#include "NuklearSetupDialog.h"

#include <algorithm>
#include <sstream>

#include "CKAll.h"

#include "NuklearWindow.h"
#include "GameConfig.h"

// Define string constants
const char *NuklearSetupDialog::KEY_TITLE = "setup.title";
const char *NuklearSetupDialog::KEY_DRIVER_LABEL = "setup.driver_label";
const char *NuklearSetupDialog::KEY_MODE_LABEL = "setup.mode_label";
const char *NuklearSetupDialog::KEY_FULLSCREEN = "setup.fullscreen";
const char *NuklearSetupDialog::KEY_BORDERLESS = "setup.borderless";
const char *NuklearSetupDialog::KEY_OK = "setup.ok";
const char *NuklearSetupDialog::KEY_CANCEL = "setup.cancel";
const char *NuklearSetupDialog::KEY_NO_DRIVERS = "setup.no_drivers";
const char *NuklearSetupDialog::KEY_SELECT_DRIVER = "setup.select_driver";
const char *NuklearSetupDialog::KEY_NO_MODES = "setup.no_modes";
const char *NuklearSetupDialog::KEY_SELECT_REQUIRED = "setup.select_required";
const char *NuklearSetupDialog::KEY_DRIVER_REQUIRED = "setup.driver_required";
const char *NuklearSetupDialog::KEY_MODE_REQUIRED = "setup.mode_required";

std::string NuklearSetupDialog::ScreenModeInfo::ToString() const {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%4d x %-4d %2dbpp @ %dHz",
             width, height, bpp, refreshRate);
    return std::string(buffer);
}

NuklearSetupDialog::NuklearSetupDialog() :
    m_Window(std::make_unique<NuklearWindow>()),
    m_RenderManager(nullptr),
    m_GameConfig(nullptr),
    m_Result(IDCANCEL),
    m_SelectedDriverIndex(-1),
    m_SelectedScreenModeIndex(-1),
    m_IsFullScreen(false),
    m_IsBorderless(false),
    m_InitialDriverIndex(-1),
    m_InitialScreenModeIndex(-1),
    m_InitialFullScreen(false),
    m_InitialBorderless(false),
    m_CurrentLanguage("en") {
    InitializeDefaultStrings();
}

NuklearSetupDialog::~NuklearSetupDialog() = default;

void NuklearSetupDialog::InitializeDefaultStrings() {
    // English localization (default)
    std::unordered_map<std::string, std::string> enStrings = {
        {KEY_TITLE, "Ballance Setup"},
        {KEY_DRIVER_LABEL, "Graphics Driver:"},
        {KEY_MODE_LABEL, "Screen Mode:"},
        {KEY_FULLSCREEN, "Full Screen"},
        {KEY_BORDERLESS, "Borderless Window"},
        {KEY_OK, "OK"},
        {KEY_CANCEL, "Cancel"},
        {KEY_NO_DRIVERS, "(No drivers available)"},
        {KEY_SELECT_DRIVER, "(Select Graphics Driver)"},
        {KEY_NO_MODES, "(No supported screen modes found)"},
        {KEY_SELECT_REQUIRED, "Selection Required"},
        {KEY_DRIVER_REQUIRED, "Please select a graphics driver."},
        {KEY_MODE_REQUIRED, "Please select a screen mode."}
    };

    // German localization
    std::unordered_map<std::string, std::string> deStrings = {
        {KEY_TITLE, "Ballance Einrichtung"},
        {KEY_DRIVER_LABEL, "Grafiktreiber:"},
        {KEY_MODE_LABEL, "Anzeigemodus:"},
        {KEY_FULLSCREEN, "Vollbild"},
        {KEY_BORDERLESS, "Randloses Fenster"},
        {KEY_OK, "OK"},
        {KEY_CANCEL, "Abbrechen"},
        {KEY_NO_DRIVERS, "(Keine Treiber verfügbar)"},
        {KEY_SELECT_DRIVER, "(Grafiktreiber auswählen)"},
        {KEY_NO_MODES, "(Keine unterstützten Bildschirmmodi gefunden)"},
        {KEY_SELECT_REQUIRED, "Auswahl erforderlich"},
        {KEY_DRIVER_REQUIRED, "Bitte wählen Sie einen Grafiktreiber aus."},
        {KEY_MODE_REQUIRED, "Bitte wählen Sie einen Bildschirmmodus aus."}
    };

    // French localization
    std::unordered_map<std::string, std::string> frStrings = {
        {KEY_TITLE, "Configuration de Ballance"},
        {KEY_DRIVER_LABEL, "Pilote graphique:"},
        {KEY_MODE_LABEL, "Mode d'affichage:"},
        {KEY_FULLSCREEN, "Plein écran"},
        {KEY_BORDERLESS, "Fenêtre sans bordure"},
        {KEY_OK, "OK"},
        {KEY_CANCEL, "Annuler"},
        {KEY_NO_DRIVERS, "(Aucun pilote disponible)"},
        {KEY_SELECT_DRIVER, "(Sélectionnez un pilote graphique)"},
        {KEY_NO_MODES, "(Aucun mode d'affichage pris en charge)"},
        {KEY_SELECT_REQUIRED, "Sélection requise"},
        {KEY_DRIVER_REQUIRED, "Veuillez sélectionner un pilote graphique."},
        {KEY_MODE_REQUIRED, "Veuillez sélectionner un mode d'affichage."}
    };

    // Add default strings to the localization system
    m_Strings["en"] = enStrings;
    m_Strings["de"] = deStrings;
    m_Strings["fr"] = frStrings;
}

std::string NuklearSetupDialog::GetLocalizedString(const char *key) const {
    // Try current language
    auto langIt = m_Strings.find(m_CurrentLanguage);
    if (langIt != m_Strings.end()) {
        auto strIt = langIt->second.find(key);
        if (strIt != langIt->second.end()) {
            return strIt->second;
        }
    }

    // Try default language if different from current
    if (m_CurrentLanguage != "en") {
        langIt = m_Strings.find("en");
        if (langIt != m_Strings.end()) {
            auto strIt = langIt->second.find(key);
            if (strIt != langIt->second.end()) {
                return strIt->second;
            }
        }
    }

    // Return key as fallback
    return key;
}

void NuklearSetupDialog::SetLocalizedString(const char *key, const std::string &value, const std::string &lang) {
    // Use provided language code or current language
    std::string targetLanguage = lang.empty() ? m_CurrentLanguage : lang;

    // Ensure language map exists
    if (m_Strings.find(targetLanguage) == m_Strings.end()) {
        m_Strings[targetLanguage] = std::unordered_map<std::string, std::string>();
    }

    // Set the string
    m_Strings[targetLanguage][key] = value;
}

bool NuklearSetupDialog::Show(
    HINSTANCE hInstance,
    CKRenderManager *renderManager,
    BpGameConfig *config,
    SetupDialogCallback callback,
    int width,
    int height,
    const DialogOptions &options) {
    m_RenderManager = renderManager;
    m_GameConfig = config;
    m_Callback = std::move(callback);
    m_Options = options;
    m_CurrentLanguage = options.languageCode;
    m_Result = IDCANCEL;

    try {
        LoadConfiguration();
        if (!InitDriverList()) {
            throw std::runtime_error("Failed to initialize graphics driver list.");
        }
        UpdateScreenModeList();
        StoreInitialSettings();

        // Calculate position (centered if requested)
        int posX = CW_USEDEFAULT;
        int posY = CW_USEDEFAULT;

        if (options.centerOnScreen) {
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            posX = (screenWidth - width) / 2;
            posY = (screenHeight - height) / 2;
        }

        // Create Nuklear window configuration
        NuklearWindow::Config nuklearConfig;
        nuklearConfig.title = GetLocalizedString(KEY_TITLE);
        nuklearConfig.x = posX;
        nuklearConfig.y = posY;
        nuklearConfig.width = width;
        nuklearConfig.height = height;
        nuklearConfig.instance = hInstance;
        nuklearConfig.center = options.centerOnScreen;
        nuklearConfig.fontName = "Arial";
        nuklearConfig.fontSize = 16;

        // Create window with Nuklear
        if (!m_Window->Create(nuklearConfig)) {
            throw std::runtime_error("Failed to create setup dialog window.");
        }

        // Set callbacks
        m_Window->SetDrawCallback([this](struct nk_context *ctx) -> bool {
            return this->DrawDialogContent(ctx);
        });

        m_Window->SetCloseCallback([this]() -> bool {
            m_Result = IDCANCEL;
            return true;
        });

        m_Window->Show();

        while (m_Window->Update()) {
            MsgWaitForMultipleObjects(0, nullptr, FALSE, 1, QS_ALLINPUT);
        }

        m_Window->Destroy();
    } catch (const std::exception &e) {
        MessageBoxA(nullptr, e.what(), "Setup Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // Process result
    bool okPressed = (m_Result == IDOK);
    if (okPressed) {
        ApplyConfiguration();
        if (m_Callback) m_Callback(m_SelectedDriverIndex, m_SelectedScreenModeIndex);
    } else {
        RevertConfiguration();
    }

    return okPressed;
}

void NuklearSetupDialog::SetCustomDrawCallback(CustomDrawCallback callback) {
    m_CustomDrawCallback = std::move(callback);
}

void NuklearSetupDialog::LoadConfiguration() {
    if (!m_GameConfig) {
        m_SelectedDriverIndex = 0;
        m_SelectedScreenModeIndex = -1;
        m_IsFullScreen = false;
        m_IsBorderless = false;
        return;
    }

    m_SelectedDriverIndex = m_GameConfig->Get(BP_CONFIG_DRIVER).GetInt32();
    m_SelectedScreenModeIndex = m_GameConfig->Get(BP_CONFIG_SCREEN_MODE).GetInt32();
    m_IsFullScreen = m_GameConfig->Get(BP_CONFIG_FULLSCREEN).GetBool();
    m_IsBorderless = m_GameConfig->Get(BP_CONFIG_BORDERLESS).GetBool();
}

bool NuklearSetupDialog::InitDriverList() {
    if (!m_RenderManager) return false;
    m_Drivers.clear();
    const int driverCount = m_RenderManager->GetRenderDriverCount();
    if (driverCount <= 0) return false;

    for (int i = 0; i < driverCount; ++i) {
        VxDriverDesc *desc = m_RenderManager->GetRenderDriverDescription(i);
        if (desc) {
            DriverInfo info;
            info.index = i;
#if CKVERSION == 0x13022002
            const char *driverName = desc->DriverName ? desc->DriverName : "Unnamed Driver";
            const char *driverDesc = desc->DriverDesc ? desc->DriverDesc : "";
            info.name = driverName;
            info.description = driverDesc;
#else
            info.name = desc->DriverName.CStr() ? desc->DriverName.CStr() : "Unnamed Driver";
            info.description = desc->DriverDesc.CStr() ? desc->DriverDesc.CStr() : "";
#endif
            m_Drivers.push_back(std::move(info));
        }
    }

    // Validate selected index
    if (m_SelectedDriverIndex < 0 || m_SelectedDriverIndex >= static_cast<int>(m_Drivers.size())) {
        m_SelectedDriverIndex = m_Drivers.empty() ? -1 : 0;
    }
    return !m_Drivers.empty();
}

void NuklearSetupDialog::UpdateScreenModeList() {
    m_ScreenModes.clear();
    if (!m_RenderManager || m_SelectedDriverIndex < 0 ||
        m_SelectedDriverIndex >= static_cast<int>(m_Drivers.size()))
        return;

    VxDriverDesc *desc = m_RenderManager->GetRenderDriverDescription(m_SelectedDriverIndex);
    if (!desc) return;

#if CKVERSION == 0x13022002
    // CK1 version
    VxDisplayMode *modes = desc->DisplayModes;
    const int modeCount = desc->DisplayModeCount;
    if (!modes || modeCount <= 0) return;
#else
    // CK2 version
    XArray<VxDisplayMode>& modes = desc->DisplayModes;
    const int modeCount = modes.Size();
    if (modeCount <= 0) return;
#endif

    // Filter and collect modes
    for (int i = 0; i < modeCount; ++i) {
        if (modes[i].Bpp >= 16 && modes[i].Width >= 640 && modes[i].Height >= 480) {
            ScreenModeInfo info;
            info.index = i;
            info.width = modes[i].Width;
            info.height = modes[i].Height;
            info.bpp = modes[i].Bpp;
            info.refreshRate = modes[i].RefreshRate;
            m_ScreenModes.push_back(info);
        }
    }

    // Sort by resolution, depth, refresh rate
    std::sort(m_ScreenModes.begin(), m_ScreenModes.end(),
              [](const ScreenModeInfo &a, const ScreenModeInfo &b) {
                  if (a.width != b.width) return a.width < b.width;
                  if (a.height != b.height) return a.height < b.height;
                  if (a.bpp != b.bpp) return a.bpp < b.bpp;
                  return a.refreshRate < b.refreshRate;
              });


    const int width = m_GameConfig->Get(BP_CONFIG_WIDTH).GetInt32();
    const int height = m_GameConfig->Get(BP_CONFIG_HEIGHT).GetInt32();
    const int bpp = m_GameConfig->Get(BP_CONFIG_BPP).GetInt32();

    int refreshRate = 0;
    for (const auto &mode : m_ScreenModes) {
        if (mode.width == width &&
            mode.height == height &&
            mode.bpp == bpp) {
            refreshRate = mode.refreshRate;
            break;
        }
    }

    for (const auto &mode : m_ScreenModes) {
        if (mode.width == width &&
            mode.height == height &&
            mode.bpp == bpp &&
            mode.refreshRate == refreshRate) {
            m_SelectedScreenModeIndex = mode.index;
            break;
        }
    }

    if (m_SelectedScreenModeIndex == -1) {
        m_SelectedScreenModeIndex = m_ScreenModes.empty() ? -1 : m_ScreenModes[0].index;
    }
}

void NuklearSetupDialog::StoreInitialSettings() {
    m_InitialDriverIndex = m_SelectedDriverIndex;
    m_InitialScreenModeIndex = m_SelectedScreenModeIndex;
    m_InitialFullScreen = m_IsFullScreen;
    m_InitialBorderless = m_IsBorderless;
}

void NuklearSetupDialog::ApplyConfiguration() {
    if (!m_GameConfig) return;

    // Save basic settings
    m_GameConfig->Get(BP_CONFIG_DRIVER) = m_SelectedDriverIndex;
    m_GameConfig->Get(BP_CONFIG_SCREEN_MODE) = m_SelectedScreenModeIndex;
    m_GameConfig->Get(BP_CONFIG_FULLSCREEN) = m_IsFullScreen;
    m_GameConfig->Get(BP_CONFIG_BORDERLESS) = m_IsBorderless;

    // Find and save resolution if mode is selected
    if (m_SelectedScreenModeIndex != -1) {
        const auto it = std::find_if(m_ScreenModes.begin(), m_ScreenModes.end(),
                               [this](const ScreenModeInfo &mode) { return mode.index == m_SelectedScreenModeIndex; });
        if (it != m_ScreenModes.end()) {
            m_GameConfig->Get(BP_CONFIG_WIDTH) = it->width;
            m_GameConfig->Get(BP_CONFIG_HEIGHT) = it->height;
            m_GameConfig->Get(BP_CONFIG_BPP) = it->bpp;
        } else {
            // Reset resolution if mode not found
            m_GameConfig->Get(BP_CONFIG_SCREEN_MODE) = -1;
            m_GameConfig->Get(BP_CONFIG_WIDTH) = 0;
            m_GameConfig->Get(BP_CONFIG_HEIGHT) = 0;
            m_GameConfig->Get(BP_CONFIG_BPP) = 0;
        }
    } else {
        // Clear resolution if no mode selected
        m_GameConfig->Get(BP_CONFIG_WIDTH) = 0;
        m_GameConfig->Get(BP_CONFIG_HEIGHT) = 0;
        m_GameConfig->Get(BP_CONFIG_BPP) = 0;
    }
}

void NuklearSetupDialog::RevertConfiguration() {
    m_SelectedDriverIndex = m_InitialDriverIndex;
    m_SelectedScreenModeIndex = m_InitialScreenModeIndex;
    m_IsFullScreen = m_InitialFullScreen;
    m_IsBorderless = m_InitialBorderless;
}

bool NuklearSetupDialog::DrawDialogContent(struct nk_context *ctx) {
    if (!ctx) return false;

    // Get window size
    int width, height;
    m_Window->GetClientSize(width, height);

    // Calculate proportional sizes
    const auto w = static_cast<float>(width);
    const auto h = static_cast<float>(height);

    // Driver selection section
    nk_layout_row_dynamic(ctx, h * 0.04f, 1);
    nk_label(ctx, GetLocalizedString(KEY_DRIVER_LABEL).c_str(), NK_TEXT_LEFT);

    // Calculate list heights
    float baseListHeight = h * 0.25f;

    // Driver list
    nk_layout_row_dynamic(ctx, baseListHeight, 1);
    if (nk_group_begin(ctx, "DriverGroup", NK_WINDOW_BORDER)) {
        float itemHeight = h * 0.04f;

        // Show drivers or placeholder
        if (m_Drivers.empty()) {
            nk_layout_row_dynamic(ctx, itemHeight, 1);
            nk_label(ctx, GetLocalizedString(KEY_NO_DRIVERS).c_str(), NK_TEXT_CENTERED);
        } else {
            for (const auto &driver : m_Drivers) {
                nk_layout_row_dynamic(ctx, itemHeight, 1);
                bool isSelected = driver.index == m_SelectedDriverIndex;
                if (nk_option_label(ctx, driver.name.c_str(), isSelected)) {
                    if (driver.index != m_SelectedDriverIndex) {
                        m_SelectedDriverIndex = driver.index;
                        m_SelectedScreenModeIndex = -1; // Reset screen mode selection
                        UpdateScreenModeList();
                    }
                }
            }
        }
        nk_group_end(ctx);
    }

    // Screen mode section
    nk_layout_row_dynamic(ctx, h * 0.04f, 1);
    nk_label(ctx, GetLocalizedString(KEY_MODE_LABEL).c_str(), NK_TEXT_LEFT);

    // Screen mode list
    nk_layout_row_dynamic(ctx, baseListHeight * 1.2f, 1);
    if (nk_group_begin(ctx, "ModeGroup", NK_WINDOW_BORDER)) {
        float itemHeight = h * 0.04f;

        if (m_SelectedDriverIndex == -1 || m_Drivers.empty()) {
            nk_layout_row_dynamic(ctx, itemHeight, 1);
            nk_label(ctx, GetLocalizedString(KEY_SELECT_DRIVER).c_str(), NK_TEXT_CENTERED);
        } else if (m_ScreenModes.empty()) {
            nk_layout_row_dynamic(ctx, itemHeight, 1);
            nk_label(ctx, GetLocalizedString(KEY_NO_MODES).c_str(), NK_TEXT_CENTERED);
        } else {
            for (const auto &mode : m_ScreenModes) {
                nk_layout_row_dynamic(ctx, itemHeight, 1);
                bool isSelected = mode.index == m_SelectedScreenModeIndex;
                if (nk_option_label(ctx, mode.ToString().c_str(), isSelected)) {
                    m_SelectedScreenModeIndex = mode.index;
                }
            }
        }
        nk_group_end(ctx);
    }

    // Checkboxes
    if (m_Options.showFullScreenOption) {
        nk_layout_row_dynamic(ctx, h * 0.05f, 1);
        nk_checkbox_label(ctx, GetLocalizedString(KEY_FULLSCREEN).c_str(), &m_IsFullScreen);
    }

    if (m_Options.showBorderlessOption) {
        nk_layout_row_dynamic(ctx, h * 0.05f, 1);
        nk_checkbox_label(ctx, GetLocalizedString(KEY_BORDERLESS).c_str(), &m_IsBorderless);
    }

    // Custom content
    if (m_CustomDrawCallback) {
        nk_layout_row_dynamic(ctx, h * 0.02f, 1);
        nk_spacing(ctx, 1);

        nk_layout_row_dynamic(ctx, h * 0.1f, 1);
        if (!m_CustomDrawCallback(ctx, h * 0.1f)) {
            return false;
        }
    }

    // Buttons
    float buttonHeight = h * 0.06f;
    float buttonWidth = w * 0.25f;

    nk_layout_row_begin(ctx, NK_DYNAMIC, buttonHeight, 4);

    // Center buttons with spacing
    nk_layout_row_push(ctx, 0.5f - buttonWidth / w);
    nk_spacing(ctx, 1);

    // OK button
    nk_layout_row_push(ctx, buttonWidth / w);
    if (nk_button_label(ctx, GetLocalizedString(KEY_OK).c_str())) {
        if (ValidateSelections()) {
            m_Result = IDOK;
            return false; // Close window
        }
    }

    // Spacing between buttons
    nk_layout_row_push(ctx, 0.1f);
    nk_spacing(ctx, 1);

    // Cancel button
    nk_layout_row_push(ctx, buttonWidth / w);
    if (nk_button_label(ctx, GetLocalizedString(KEY_CANCEL).c_str())) {
        m_Result = IDCANCEL;
        return false; // Close window
    }

    nk_layout_row_end(ctx);

    return true;
}

bool NuklearSetupDialog::ValidateSelections() {
    // Driver selection is required
    if (m_SelectedDriverIndex < 0) {
        MessageBoxA(m_Window->GetHandle(),
                    GetLocalizedString(KEY_DRIVER_REQUIRED).c_str(),
                    GetLocalizedString(KEY_SELECT_REQUIRED).c_str(),
                    MB_OK | MB_ICONWARNING);
        return false;
    }

    // Screen mode selection is required if modes are available
    if (m_SelectedScreenModeIndex < 0 && !m_ScreenModes.empty()) {
        MessageBoxA(m_Window->GetHandle(),
                    GetLocalizedString(KEY_MODE_REQUIRED).c_str(),
                    GetLocalizedString(KEY_SELECT_REQUIRED).c_str(),
                    MB_OK | MB_ICONWARNING);
        return false;
    }

    return true;
}
