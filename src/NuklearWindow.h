#ifndef BP_NUKLEAR_WINDOW_H
#define BP_NUKLEAR_WINDOW_H

#include <functional>
#include <memory>
#include <string>

#include "Window.h"

// Nuklear definitions
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_BOOL
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"
#include "nuklear_gdi.h"

// Define a unique class name for Nuklear windows
#ifdef UNICODE
#define NUKLEAR_WINDOW_CLASS_NAME L"NuklearWindowClassW"
#else
#define NUKLEAR_WINDOW_CLASS_NAME "NuklearWindowClassA"
#endif

/**
 * Window class with Nuklear GUI capabilities.
 * A C++ wrapper around Nuklear immediate-mode GUI
 */
class NuklearWindow : public Window {
public:
    // Callback types
    using DrawCallback = std::function<bool(struct nk_context *)>;
    using CloseCallback = std::function<bool()>;

    // Configuration for window creation
    struct Config {
        std::string title = "Nuklear Window";
        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;
        int width = 800;
        int height = 600;
        HWND parent = nullptr;
        HINSTANCE instance = nullptr;
        bool center = true;
        std::string fontName = "Arial";
        int fontSize = 14;
    };

    // Image class for GDI resources
    class Image {
    public:
        Image() = default;
        ~Image();

        // Disable copy
        Image(const Image &) = delete;
        Image &operator=(const Image &) = delete;

        // Enable move
        Image(Image &&other) noexcept;
        Image &operator=(Image &&other) noexcept;

        // Get nuklear image
        const struct nk_image &GetNkImage() const { return m_NkImage; }

        // Image dimensions
        int GetWidth() const { return m_NkImage.w; }
        int GetHeight() const { return m_NkImage.h; }

    private:
        friend class NuklearWindow;
        struct nk_image m_NkImage = {};
        bool m_Initialized = false;
    };

    // Font class for GDI fonts
    class Font {
    public:
        Font() = default;
        ~Font();

        // Disable copy
        Font(const Font &) = delete;
        Font &operator=(const Font &) = delete;

        // Enable move
        Font(Font &&other) noexcept;
        Font &operator=(Font &&other) noexcept;

        // Font metrics
        int GetHeight() const;
        float GetWidth(const char *text, int len) const;

    private:
        friend class NuklearWindow;
        GdiFont *m_GdiFont = nullptr;
    };

    NuklearWindow();
    ~NuklearWindow() override;

    // Disable copy
    NuklearWindow(const NuklearWindow &) = delete;
    NuklearWindow &operator=(const NuklearWindow &) = delete;

    // Enable move
    NuklearWindow(NuklearWindow &&) noexcept;
    NuklearWindow &operator=(NuklearWindow &&) noexcept;

    // Window creation and update
    bool Create(const Config &config = Config());
    bool Update() override;

    // Behavior configuration
    void SetAllowSizing(bool allow) { m_AllowSizing = allow; }
    void SetAllowMaximize(bool allow) { m_AllowMaximize = allow; }
    void SetAllowMove(bool allow) { m_AllowMove = allow; }
    void SetTitleBar(bool enable) { m_HasTitleBar = enable; }

    // Callbacks
    void SetCloseCallback(CloseCallback callback) { m_CloseCallback = std::move(callback); }
    void SetDrawCallback(DrawCallback callback) { m_DrawCallback = std::move(callback); }

    // Appearance
    void SetBackgroundColor(int r, int g, int b);

    // Font management
    std::shared_ptr<Font> CreateFont(const std::string &fontName, int fontSize);
    bool SetFont(const std::shared_ptr<Font> &font);
    bool SetFont(const std::string &fontName, int fontSize);

    // Image management
    std::shared_ptr<Image> CreateImage(const char *frameBuffer, int width, int height);

    // Accessors
    struct nk_context *GetNuklearContext() const { return m_NkCtx; }
    bool IsOpen() const { return m_IsOpen; }

    // Class registration
    static bool RegisterNuklearClass(HINSTANCE instance = nullptr);

protected:
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) override;

private:
    // Helper methods
    bool InitializeNuklear(const char *fontName, int fontSize);
    void CleanupNuklear();
    void HandleDragging(int x, int y);
    void ToggleMaximize(bool maximize);

    static LRESULT CALLBACK NuklearWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static bool s_ClassRegistered;

    // Window configuration
    bool m_AllowSizing;
    bool m_AllowMaximize;
    bool m_AllowMove;
    bool m_HasTitleBar;

    // Callbacks
    CloseCallback m_CloseCallback;
    DrawCallback m_DrawCallback;

    // Nuklear resources
    GdiFont *m_Font;
    nk_gdi_ctx m_NkGdiCtx;
    struct nk_context *m_NkCtx;
    HDC m_WindowDC;
    struct nk_color m_BackgroundColor;

    // Window state
    bool m_IsOpen;
    bool m_IsDragging;
    bool m_IsMaximized;
    bool m_WasOverride;
    POINT m_DragOffset;
};

#endif // BP_NUKLEAR_WINDOW_H
