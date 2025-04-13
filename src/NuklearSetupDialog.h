#ifndef BP_NUKLEAR_SETUP_DIALOG_H
#define BP_NUKLEAR_SETUP_DIALOG_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

// Windows Headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

// Forward declarations
class CKRenderManager;
class NuklearWindow;
struct nk_context;
struct BpGameConfig;

/**
 * @brief Dialog for configuring graphics settings using Nuklear UI
 */
class NuklearSetupDialog {
public:
    // Driver information
    struct DriverInfo {
        int index = -1;
        std::string name;
        std::string description;
    };

    // Screen mode information
    struct ScreenModeInfo {
        int index = -1;
        int width = 0;
        int height = 0;
        int bpp = 0;
        int refreshRate = 0;

        std::string ToString() const;
    };

    // Dialog options
    struct DialogOptions {
        bool showFullScreenOption = true;
        bool showBorderlessOption = true;
        bool centerOnScreen = true;
        std::string languageCode = "en";
    };

    // Callback when setup is completed
    using SetupDialogCallback = std::function<void(int driverIndex, int screenModeIndex)>;

    // Custom draw callback for additional content
    using CustomDrawCallback = std::function<bool(struct nk_context *, float)>;

    // Constructor & Destructor
    NuklearSetupDialog();
    ~NuklearSetupDialog();

    // Prevent copying and moving
    NuklearSetupDialog(const NuklearSetupDialog &) = delete;
    NuklearSetupDialog &operator=(const NuklearSetupDialog &) = delete;
    NuklearSetupDialog(NuklearSetupDialog &&) = delete;
    NuklearSetupDialog &operator=(NuklearSetupDialog &&) = delete;

    /**
     * @brief Show the setup dialog
     * @return true if OK was pressed, false if cancelled
     */
    bool Show(HINSTANCE hInstance,
              CKRenderManager *renderManager,
              BpGameConfig *config,
              SetupDialogCallback callback = nullptr,
              int width = 600,
              int height = 500,
              const DialogOptions &options = DialogOptions());

    /**
     * @brief Set custom draw callback for additional content
     */
    void SetCustomDrawCallback(CustomDrawCallback callback);

    // String constants
    static const char *KEY_TITLE;
    static const char *KEY_DRIVER_LABEL;
    static const char *KEY_MODE_LABEL;
    static const char *KEY_FULLSCREEN;
    static const char *KEY_BORDERLESS;
    static const char *KEY_OK;
    static const char *KEY_CANCEL;
    static const char *KEY_NO_DRIVERS;
    static const char *KEY_SELECT_DRIVER;
    static const char *KEY_NO_MODES;
    static const char *KEY_SELECT_REQUIRED;
    static const char *KEY_DRIVER_REQUIRED;
    static const char *KEY_MODE_REQUIRED;

private:
    // Initialization and string handling
    void InitializeDefaultStrings();
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_Strings;
    std::string m_CurrentLanguage;
    std::string GetLocalizedString(const char *key) const;
    void SetLocalizedString(const char *key, const std::string &value, const std::string &lang = "");

    // Configuration methods
    void LoadConfiguration();
    bool InitDriverList();
    void UpdateScreenModeList();
    void StoreInitialSettings();
    void ApplyConfiguration();
    void RevertConfiguration();

    // UI Methods
    bool DrawDialogContent(struct nk_context *ctx);
    bool ValidateSelections();

    // Dialog state
    std::unique_ptr<NuklearWindow> m_Window;
    CKRenderManager *m_RenderManager;
    BpGameConfig *m_GameConfig;
    SetupDialogCallback m_Callback;
    CustomDrawCallback m_CustomDrawCallback;
    DialogOptions m_Options;
    int m_Result;

    // Driver and screen mode data
    std::vector<DriverInfo> m_Drivers;
    std::vector<ScreenModeInfo> m_ScreenModes;

    // Current selections
    int m_SelectedDriverIndex;
    int m_SelectedScreenModeIndex;
    bool m_IsFullScreen;
    bool m_IsBorderless;

    // Initial values for reverting on cancel
    int m_InitialDriverIndex;
    int m_InitialScreenModeIndex;
    bool m_InitialFullScreen;
    bool m_InitialBorderless;
};

#endif // BP_NUKLEAR_SETUP_DIALOG_H
