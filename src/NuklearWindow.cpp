#include "NuklearWindow.h"

#include <cassert>
#include <utility>

// Define implementation details only once in the project
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_BOOL
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"
#include "nuklear_gdi.h"

bool NuklearWindow::s_ClassRegistered = false;

//
// NuklearWindow::Image implementation
//

NuklearWindow::Image::~Image() {
    if (m_Initialized) {
        nk_delete_image(&m_NkImage);
        m_Initialized = false;
    }
}

NuklearWindow::Image::Image(Image &&other) noexcept
    : m_NkImage(other.m_NkImage),
      m_Initialized(other.m_Initialized) {
    other.m_Initialized = false;
}

NuklearWindow::Image &NuklearWindow::Image::operator=(Image &&other) noexcept {
    if (this != &other) {
        // Clean up current resources if needed
        if (m_Initialized) {
            nk_delete_image(&m_NkImage);
        }

        // Move resources
        m_NkImage = other.m_NkImage;
        m_Initialized = other.m_Initialized;

        // Clear source object
        other.m_Initialized = false;
    }
    return *this;
}

//
// NuklearWindow::Font implementation
//

NuklearWindow::Font::~Font() {
    if (m_GdiFont) {
        nk_gdifont_del(m_GdiFont);
        m_GdiFont = nullptr;
    }
}

NuklearWindow::Font::Font(Font &&other) noexcept
    : m_GdiFont(other.m_GdiFont) {
    other.m_GdiFont = nullptr;
}

NuklearWindow::Font &NuklearWindow::Font::operator=(Font &&other) noexcept {
    if (this != &other) {
        // Clean up current resources
        if (m_GdiFont) {
            nk_gdifont_del(m_GdiFont);
        }

        // Move resources
        m_GdiFont = other.m_GdiFont;

        // Clear source object
        other.m_GdiFont = nullptr;
    }
    return *this;
}

int NuklearWindow::Font::GetHeight() const {
    if (!m_GdiFont) return 0;
    return m_GdiFont->height;
}

float NuklearWindow::Font::GetWidth(const char *text, int len) const {
    if (!m_GdiFont) return 0.0f;
    return nk_gdifont_get_text_width(nk_handle_ptr(m_GdiFont),
                                     static_cast<float>(m_GdiFont->height),
                                     text, len);
}

//
// NuklearWindow implementation
//

NuklearWindow::NuklearWindow()
    : m_AllowSizing(true),
      m_AllowMaximize(true),
      m_AllowMove(true),
      m_HasTitleBar(true),
      m_Font(nullptr),
      m_NkGdiCtx(nullptr),
      m_NkCtx(nullptr),
      m_WindowDC(nullptr),
      m_BackgroundColor(nk_rgb(50, 50, 50)),
      m_IsOpen(false),
      m_IsDragging(false),
      m_IsMaximized(false),
      m_WasOverride(false),
      m_DragOffset({0, 0}) {}

NuklearWindow::~NuklearWindow() {
    CleanupNuklear();
}

NuklearWindow::NuklearWindow(NuklearWindow &&other) noexcept
    : Window(std::move(other)),
      m_AllowSizing(other.m_AllowSizing),
      m_AllowMaximize(other.m_AllowMaximize),
      m_AllowMove(other.m_AllowMove),
      m_HasTitleBar(other.m_HasTitleBar),
      m_CloseCallback(std::move(other.m_CloseCallback)),
      m_DrawCallback(std::move(other.m_DrawCallback)),
      m_Font(other.m_Font),
      m_NkGdiCtx(other.m_NkGdiCtx),
      m_NkCtx(other.m_NkCtx),
      m_WindowDC(other.m_WindowDC),
      m_BackgroundColor(other.m_BackgroundColor),
      m_IsOpen(other.m_IsOpen),
      m_IsDragging(other.m_IsDragging),
      m_IsMaximized(other.m_IsMaximized),
      m_WasOverride(other.m_WasOverride),
      m_DragOffset(other.m_DragOffset) {
    // Clear source object's resources to prevent double-free
    other.m_Font = nullptr;
    other.m_NkGdiCtx = nullptr;
    other.m_NkCtx = nullptr;
    other.m_WindowDC = nullptr;
    other.m_IsOpen = false;
}

NuklearWindow &NuklearWindow::operator=(NuklearWindow &&other) noexcept {
    if (this != &other) {
        // Clean up current resources
        CleanupNuklear();

        // Call base class move assignment
        Window::operator=(std::move(other));

        // Move resources
        m_AllowSizing = other.m_AllowSizing;
        m_AllowMaximize = other.m_AllowMaximize;
        m_AllowMove = other.m_AllowMove;
        m_HasTitleBar = other.m_HasTitleBar;
        m_CloseCallback = std::move(other.m_CloseCallback);
        m_DrawCallback = std::move(other.m_DrawCallback);
        m_Font = other.m_Font;
        m_NkGdiCtx = other.m_NkGdiCtx;
        m_NkCtx = other.m_NkCtx;
        m_WindowDC = other.m_WindowDC;
        m_BackgroundColor = other.m_BackgroundColor;
        m_IsOpen = other.m_IsOpen;
        m_IsDragging = other.m_IsDragging;
        m_IsMaximized = other.m_IsMaximized;
        m_WasOverride = other.m_WasOverride;
        m_DragOffset = other.m_DragOffset;

        // Clear source object's resources
        other.m_Font = nullptr;
        other.m_NkGdiCtx = nullptr;
        other.m_NkCtx = nullptr;
        other.m_WindowDC = nullptr;
        other.m_IsOpen = false;
    }
    return *this;
}

bool NuklearWindow::RegisterNuklearClass(HINSTANCE instance) {
    if (s_ClassRegistered) {
        return true;
    }

    if (!instance) {
        instance = GetModuleHandle(nullptr);
    }

    s_ClassRegistered = RegisterWindowClass(
        instance,
        tstring(NUKLEAR_WINDOW_CLASS_NAME),
        NuklearWindowProc,
        CS_OWNDC | CS_DBLCLKS, // CS_OWNDC is important for GDI performance
        LoadIcon(nullptr, IDI_APPLICATION),
        LoadCursor(nullptr, IDC_ARROW),
        nullptr // No background brush; Nuklear draws everything
    );

    return s_ClassRegistered;
}

bool NuklearWindow::Create(const Config &config) {
    if (!s_ClassRegistered && !RegisterNuklearClass(config.instance)) {
        return false;
    }

    WindowCreateParams params;
    params.className = NUKLEAR_WINDOW_CLASS_NAME;
#ifdef UNICODE
    params.title = AnsiToWide(config.title);
#else
    params.title = config.title;
#endif
    params.x = config.x;
    params.y = config.y;
    params.width = config.width;
    params.height = config.height;
    params.parent = config.parent;
    params.instance = config.instance;
    params.styles.style = WS_POPUP;
    params.styles.exStyle = WS_EX_WINDOWEDGE;
    params.center = config.center;

    if (!Window::Create(params)) {
        return false;
    }

    // Initialize Nuklear context with specified font
    if (!InitializeNuklear(config.fontName.c_str(), config.fontSize)) {
        Destroy();
        return false;
    }

    m_IsOpen = true;
    return true;
}

bool NuklearWindow::InitializeNuklear(const char *fontName, int fontSize) {
    // Clean up any existing resources
    CleanupNuklear();

    // Get window size
    int width, height;
    if (!GetClientSize(width, height)) {
        return false;
    }

    // Get device context using Window's method
    m_WindowDC = GetDC();
    if (!m_WindowDC) {
        return false;
    }

    // Create font
    m_Font = nk_gdifont_create(fontName, fontSize);
    if (!m_Font) {
        ReleaseDC(m_WindowDC);
        m_WindowDC = nullptr;
        return false;
    }

    // Initialize Nuklear GDI context
    m_NkCtx = nk_gdi_init(&m_NkGdiCtx, m_Font, m_WindowDC, width, height);
    if (!m_NkCtx) {
        nk_gdifont_del(m_Font);
        m_Font = nullptr;
        ReleaseDC(m_WindowDC);
        m_WindowDC = nullptr;
        return false;
    }

    return true;
}

void NuklearWindow::CleanupNuklear() {
    // Free resources in reverse order of acquisition
    if (m_NkGdiCtx) {
        nk_gdi_shutdown(m_NkGdiCtx);
        m_NkGdiCtx = nullptr;
        m_NkCtx = nullptr; // Context is freed by nk_gdi_shutdown
    }

    if (m_Font) {
        nk_gdifont_del(m_Font);
        m_Font = nullptr;
    }

    if (m_WindowDC && m_hWnd) {
        ReleaseDC(m_WindowDC);
        m_WindowDC = nullptr;
    }
}

bool NuklearWindow::Update() {
    if (!m_IsOpen || !m_NkCtx) {
        return false;
    }

    // Process input events
    MSG msg;
    nk_input_begin(m_NkCtx);
    while (PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    nk_input_end(m_NkCtx);

    auto title = GetTitleA();

    // Configure window flags
    nk_flags windowFlags = NK_WINDOW_BORDER;
    if (m_HasTitleBar) {
        windowFlags |= NK_WINDOW_CLOSABLE | NK_WINDOW_TITLE;
    }
    if (!m_IsMaximized && m_AllowSizing) {
        windowFlags |= NK_WINDOW_SCALABLE;
    }

    int width, height;
    if (!GetClientSize(width, height)) {
        return false;
    }

    // Resize Nuklear context if needed
    if (m_WasOverride) {
        nk_window_set_bounds(m_NkCtx, title.c_str(), nk_rect(0, 0, width, height));
    }

    // Begin Nuklear window
    if (nk_begin(m_NkCtx, title.c_str(), nk_rect(0, 0, static_cast<float>(width), static_cast<float>(height)),
                 windowFlags)) {
        // Execute drawing callback
        if (m_DrawCallback && !m_DrawCallback(m_NkCtx)) {
            m_IsOpen = false;
        }

        // Handle resizing from Nuklear
        struct nk_rect bounds = nk_window_get_bounds(m_NkCtx);
        if (bounds.w != width || bounds.h != height) {
            SetPosition(nullptr, 0, 0, bounds.w, bounds.h, SWP_NOMOVE | SWP_NOOWNERZORDER);
        }
    } else {
        // Window was closed via Nuklear - execute close callback
        if (!m_CloseCallback || m_CloseCallback()) {
            m_IsOpen = false;
        }
    }
    nk_end(m_NkCtx);

    m_WasOverride = false;

    // Render the Nuklear UI
    nk_gdi_render(m_NkGdiCtx, m_BackgroundColor);

    return m_IsOpen;
}

void NuklearWindow::SetBackgroundColor(int r, int g, int b) {
    m_BackgroundColor = nk_rgb(static_cast<nk_byte>(r),
                               static_cast<nk_byte>(g),
                               static_cast<nk_byte>(b));
}

std::shared_ptr<NuklearWindow::Font> NuklearWindow::CreateFont(
    const std::string &fontName, int fontSize) {
    auto font = std::make_shared<Font>();
    font->m_GdiFont = nk_gdifont_create(fontName.c_str(), fontSize);
    return font->m_GdiFont ? font : nullptr;
}

bool NuklearWindow::SetFont(const std::shared_ptr<Font> &font) {
    if (!m_NkGdiCtx || !m_NkCtx || !font || !font->m_GdiFont) {
        return false;
    }

    // Clean up old font if owned by this window
    if (m_Font) {
        nk_gdifont_del(m_Font);
        m_Font = nullptr;
    }

    // Set new font in the Nuklear context
    nk_gdi_set_font(m_NkGdiCtx, font->m_GdiFont);

    // Store font reference - this doesn't take ownership
    m_Font = font->m_GdiFont;

    return true;
}

bool NuklearWindow::SetFont(const std::string &fontName, int fontSize) {
    auto font = CreateFont(fontName, fontSize);
    if (!font) {
        return false;
    }

    return SetFont(font);
}

std::shared_ptr<NuklearWindow::Image> NuklearWindow::CreateImage(
    const char *frameBuffer, int width, int height) {
    if (!frameBuffer || width <= 0 || height <= 0) {
        return nullptr;
    }

    auto image = std::make_shared<Image>();
    nk_create_image(&image->m_NkImage, frameBuffer, width, height);
    image->m_Initialized = true;

    return image;
}

void NuklearWindow::HandleDragging(int x, int y) {
    if (!m_IsDragging || m_IsMaximized) {
        return;
    }

    POINT cursorPos;
    GetCursorPos(&cursorPos);

    // Adjust position by drag offset
    cursorPos.x -= m_DragOffset.x;
    cursorPos.y -= m_DragOffset.y;

    Restore();
    SetPosition(nullptr, cursorPos.x, cursorPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void NuklearWindow::ToggleMaximize(bool maximize) {
    if (maximize && !m_IsMaximized) {
        // Find the monitor and its work area
        HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfo(monitor, &monitorInfo)) {
            // Set window to fill monitor work area
            SetPosition(nullptr,
                        monitorInfo.rcWork.left, monitorInfo.rcWork.top,
                        monitorInfo.rcWork.right - monitorInfo.rcWork.left,
                        monitorInfo.rcWork.bottom - monitorInfo.rcWork.top,
                        SWP_NOZORDER);
            m_WasOverride = true;
            m_IsMaximized = true;
        }
    } else if (!maximize && m_IsMaximized) {
        m_IsMaximized = false;
    }
}

LRESULT NuklearWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    // Handle specific window messages
    switch (message) {
    case WM_CLOSE:
        // Process close request
        if (!m_CloseCallback || m_CloseCallback()) {
            m_IsOpen = false;
        }
        return 0;

    case WM_SIZE:
        // Handle maximize/restore
        if (wParam == SIZE_MAXIMIZED) {
            ToggleMaximize(true);
        } else if (wParam == SIZE_RESTORED) {
            m_IsMaximized = false;
        }
        break;

    case WM_LBUTTONDOWN:
        // Check if click is in titlebar area (top 30 pixels)
        if (HIWORD(lParam) <= 30 && m_AllowMove && m_HasTitleBar) {
            m_IsDragging = true;
            m_DragOffset.x = LOWORD(lParam);
            m_DragOffset.y = HIWORD(lParam);
        }
        break;

    case WM_LBUTTONUP:
        // End dragging
        if (m_IsDragging) {
            m_IsDragging = false;
        }
        break;

    case WM_MOUSEMOVE:
        // Handle window dragging
        HandleDragging(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_LBUTTONDBLCLK:
        // Handle double-click on titlebar
        if (HIWORD(lParam) <= 30 && m_AllowMaximize && m_HasTitleBar) {
            if (m_IsMaximized) {
                Restore();
            } else {
                Maximize();
            }
            m_WasOverride = true;
        }
        break;

    default:
        break;
    }

    // Pass the message to Nuklear
    if (m_NkGdiCtx && nk_gdi_handle_event(m_NkGdiCtx, m_hWnd, message, wParam, lParam)) {
        return 0;
    }

    // Pass to base Window class handler for default processing
    return Window::HandleMessage(message, wParam, lParam);
}

LRESULT CALLBACK NuklearWindow::NuklearWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    NuklearWindow *window = nullptr;
    if (msg == WM_NCCREATE) {
        auto *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
        window = static_cast<NuklearWindow *>(pCreate->lpCreateParams);
        if (window) {
            window->m_hWnd = hWnd;
        }
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<NuklearWindow *>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (window) {
        return window->HandleMessage(msg, wParam, lParam);
    }

    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
