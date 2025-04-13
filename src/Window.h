#ifndef BP_WINDOW_H
#define BP_WINDOW_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <windowsx.h>
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <type_traits>

// Helper macros for character set neutrality
#ifdef UNICODE
#define TEXT_STRING(str) L##str
using tstring = std::wstring;
#else
#define TEXT_STRING(str) str
using tstring = std::string;
#endif

/** @brief Represents window states */
enum class WindowState {
    Normal, /**< Normal window size */
    Minimized, /**< Minimized/Iconified window */
    Maximized, /**< Maximized window */
    Fullscreen /**< Fullscreen window */
};

/** @brief Window styles structure */
struct WindowStyles {
    DWORD style = WS_OVERLAPPEDWINDOW; /**< Window styles */
    DWORD exStyle = 0; /**< Extended window styles */
};

/**
 * @brief Base template for window creation parameters
 */
template <typename StringT>
struct WindowCreateParamsT {
    StringT className; /**< Window class name */
    StringT title; /**< Window title */
    int x = CW_USEDEFAULT; /**< X position */
    int y = CW_USEDEFAULT; /**< Y position */
    int width = CW_USEDEFAULT; /**< Width */
    int height = CW_USEDEFAULT; /**< Height */
    HWND parent = nullptr; /**< Parent window */
    HMENU menu = nullptr; /**< Menu or child ID */
    HINSTANCE instance = nullptr; /**< Instance handle */
    LPVOID param = nullptr; /**< Creation parameter */
    WindowStyles styles; /**< Window styles */
    bool center = false; /**< Center window on screen */
};

using WindowCreateParamsA = WindowCreateParamsT<std::string>;
using WindowCreateParamsW = WindowCreateParamsT<std::wstring>;

#ifdef UNICODE
using WindowCreateParams = WindowCreateParamsW;
#else
using WindowCreateParams = WindowCreateParamsA;
#endif

/**
 * @brief Base window class providing Win32 window functionality
 *
 * Provides an object-oriented abstraction over the Win32 API with
 * automatic support for both ANSI and Unicode versions.
 * Uses RAII principles for resource management and event-based
 * abstractions for message handling.
 */
class Window {
public:
    // Forward declarations
    class Builder;

    using MessageHandler = std::function<bool(UINT, WPARAM, LPARAM, LRESULT &)>;

    // ----- Constructors & Destructor -----

    /**
     * @brief Default constructor
     */
    Window();

    /**
     * @brief Constructor from an existing window handle
     * @param handle The window handle to wrap
     */
    explicit Window(HWND handle);

    /**
     * @brief Destructor - automatically destroys the window
     */
    virtual ~Window();

    // Prevent copying
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    // Allow moving
    Window(Window &&other) noexcept;
    Window &operator=(Window &&other) noexcept;

    // ----- Factory methods -----

    /**
     * @brief Creates a standard window with default settings
     * @param title The window title
     * @param width Width of the window (default: CW_USEDEFAULT)
     * @param height Height of the window (default: CW_USEDEFAULT)
     * @return A new Window instance
     */
    template <typename StringT>
    static std::unique_ptr<Window> CreateStandard(
        const StringT &title,
        int width = CW_USEDEFAULT,
        int height = CW_USEDEFAULT);

    /**
     * @brief Creates a dialog-style window
     * @param title The window title
     * @param width Width of the window
     * @param height Height of the window
     * @param parent Optional parent window
     * @return A new Window instance
     */
    template <typename StringT>
    static std::unique_ptr<Window> CreateDialogBox(
        const StringT &title,
        int width,
        int height,
        HWND parent = nullptr);

    /**
     * @brief Creates a child window
     * @param parent Parent window handle
     * @param id Control ID
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @return A new Window instance
     */
    static std::unique_ptr<Window> CreateChild(
        HWND parent,
        int id,
        int x,
        int y,
        int width,
        int height);

    // ----- Window management -----

    /**
     * @brief Creates a window with ANSI strings
     * @param params Window creation parameters
     * @return True if window was created successfully
     */
    virtual bool CreateA(const WindowCreateParamsA &params);

    /**
     * @brief Creates a window with Unicode strings
     * @param params Window creation parameters
     * @return True if window was created successfully
     */
    virtual bool CreateW(const WindowCreateParamsW &params);

    /**
     * @brief Generic window creation function
     * @param params Window creation parameters
     * @return True if window was created successfully
     */
    template <typename StringT>
    bool Create(const WindowCreateParamsT<StringT> &params) {
        return CreateImpl(params, typename CharTraits<StringT>::tag{});
    }

    /**
     * @brief Destroys the window
     * @return True if window was destroyed successfully
     */
    virtual bool Destroy();

    // ----- Window state management -----

    /**
     * @brief Shows the window
     * @param cmdShow Show command (default: SW_SHOW)
     * @return True if successful
     */
    bool Show(int cmdShow = SW_SHOW);

    /**
     * @brief Hides the window
     * @return True if successful
     */
    bool Hide();

    /**
     * @brief Minimizes the window
     * @return True if successful
     */
    bool Minimize();

    /**
     * @brief Maximizes the window
     * @return True if successful
     */
    bool Maximize();

    /**
     * @brief Restores the window from minimized/maximized state
     * @return True if successful
     */
    bool Restore();

    /**
     * @brief Gets the current window state
     * @return The window state
     */
    WindowState GetState() const;

    /**
     * @brief Brings the window to the front of the Z order
     * @return True if successful
     */
    bool BringToFront();

    /**
     * @brief Makes this window the foreground window
     * @return True if successful
     */
    bool SetForeground();

    /**
     * @brief Sets the keyboard focus to this window
     * @return Previous focus window handle
     */
    HWND SetFocus();

    // ----- Window position and size -----

    /**
     * @brief Moves the window to a new position
     * @param x New X position
     * @param y New Y position
     * @param preserveZOrder Whether to preserve Z order
     * @return True if successful
     */
    bool Move(int x, int y, bool preserveZOrder = true);

    /**
     * @brief Resizes the window
     * @param width New width
     * @param height New height
     * @return True if successful
     */
    bool Resize(int width, int height);

    /**
     * @brief Sets the window position and size
     * @param hWndInsertAfter Window to insert after (nullptr for no specific order)
     * @param x New X position
     * @param y New Y position
     * @param cx New width
     * @param cy New height
     * @param uFlags Flags for positioning (SWP_*)
     * @return True if successful
     */
    bool SetPosition(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags);

    /**
     * @brief Sets both position and size
     * @param x New X position
     * @param y New Y position
     * @param width New width
     * @param height New height
     * @param repaint Whether to repaint the window
     * @return True if successful
     */
    bool SetPositionAndSize(int x, int y, int width, int height, bool repaint = true);

    /**
     * @brief Gets the window position
     * @param x Out parameter for X position
     * @param y Out parameter for Y position
     * @return True if successful
     */
    bool GetPosition(int &x, int &y) const;

    /**
     * @brief Gets the window size
     * @param width Out parameter for width
     * @param height Out parameter for height
     * @return True if successful
     */
    bool GetSize(int &width, int &height) const;

    /**
     * @brief Gets the client area size
     * @param width Out parameter for width
     * @param height Out parameter for height
     * @return True if successful
     */
    bool GetClientSize(int &width, int &height) const;

    /**
     * @brief Gets the client rectangle in screen coordinates
     * @param rect Rectangle to fill
     * @return True if successful
     */
    bool GetClientRect(RECT &rect);

    // ----- Window appearance -----

    /**
     * @brief Sets the window title (ANSI version)
     * @param title New window title
     * @return True if successful
     */
    bool SetTitleA(const std::string &title);

    /**
     * @brief Sets the window title (Unicode version)
     * @param title New window title
     * @return True if successful
     */
    bool SetTitleW(const std::wstring &title);

    /**
     * @brief Generic title setter that works with any string type
     * @param title New window title
     * @return True if successful
     */
    template <typename StringT>
    bool SetTitle(const StringT &title) {
        return SetTitleImpl(title, typename CharTraits<StringT>::tag{});
    }

    /**
     * @brief Gets the window title (ANSI version)
     * @return The window title
     */
    std::string GetTitleA() const;

    /**
     * @brief Gets the window title (Unicode version)
     * @return The window title
     */
    std::wstring GetTitleW() const;

    /**
     * @brief Generic title getter that returns the appropriate string type
     * @return The window title
     */
    template <typename StringT>
    StringT GetTitle() const {
        return GetTitleImpl<StringT>(typename CharTraits<StringT>::tag{});
    }

    /**
     * @brief Gets the current window title in the build's default string type
     * @return The window title
     */
    tstring GetTitle() const {
#ifdef UNICODE
        return GetTitleW();
#else
        return GetTitleA();
#endif
    }

    /**
     * @brief Sets the window icon
     * @param icon Icon handle
     * @param isSmallIcon Whether to set the small icon
     * @return True if successful
     */
    bool SetIcon(HICON icon, bool isSmallIcon = false);

    // ----- Window properties -----

    /**
     * @brief Sets the window style
     * @param style New style
     * @return Previous style
     */
    DWORD SetStyle(DWORD style);

    /**
     * @brief Gets the window style
     * @return The window style
     */
    DWORD GetStyle() const;

    /**
     * @brief Sets the extended window style
     * @param exStyle New extended style
     * @return Previous extended style
     */
    DWORD SetExStyle(DWORD exStyle);

    /**
     * @brief Gets the extended window style
     * @return The extended window style
     */
    DWORD GetExStyle() const;

    // ----- Coordinate conversion -----

    /**
     * @brief Converts client coordinates to screen coordinates
     * @param point Point to convert
     * @return True if successful
     */
    bool ClientToScreen(POINT &point) const;

    /**
     * @brief Converts screen coordinates to client coordinates
     * @param point Point to convert
     * @return True if successful
     */
    bool ScreenToClient(POINT &point) const;

    /**
     * @brief Converts client rectangle to screen coordinates
     * @param rect Rectangle to convert
     */
    void ClientToScreen(RECT &rect) const;

    /**
     * @brief Converts screen rectangle to client coordinates
     * @param rect Rectangle to convert
     */
    void ScreenToClient(RECT &rect) const;

    // ----- Drawing and update -----

    /**
     * @brief Updates the window
     * @return True if successful
     */
    virtual bool Update();

    /**
     * @brief Invalidates a rectangle in the window
     * @param rect Rectangle to invalidate (nullptr for entire client area)
     * @param erase Whether to erase the background
     * @return True if successful
     */
    bool InvalidateRect(const RECT *rect = nullptr, bool erase = true);

    /**
     * @brief Enables or disables redrawing
     * @param redraw Whether to enable redrawing
     */
    void SetRedraw(bool redraw = true);

    /**
     * @brief Gets the device context for the window
     * @return Device context handle
     */
    HDC GetDC() const;

    /**
     * @brief Releases the device context
     * @param hdc Device context handle
     * @return True if successful
     */
    bool ReleaseDC(HDC hdc) const;

    // ----- Message handling -----

    /**
     * @brief Sends a message to the window
     * @param message Message ID
     * @param wParam WPARAM
     * @param lParam LPARAM
     * @return Message result
     */
    LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const;

    /**
     * @brief Posts a message to the window
     * @param message Message ID
     * @param wParam WPARAM
     * @param lParam LPARAM
     * @return True if successful
     */
    bool PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const;

    void RegisterMessageHandler(UINT message, MessageHandler handler);
    void UnregisterMessageHandler(UINT message);

    // ----- DPI awareness -----

    /**
     * @brief Enables DPI awareness for the application
     * @param enable Whether to enable DPI awareness
     */
    void EnableDpiAwareness(bool enable = true);

    /**
     * @brief Gets the DPI scaling factor
     * @return The DPI scaling factor
     */
    float GetDpiScale() const;

    // ----- Window state queries -----

    /**
     * @brief Checks if the window handle is valid
     * @return True if valid
     */
    bool IsWindow() const;

    /**
     * @brief Checks if the window is a child window
     * @return True if child window
     */
    bool IsChildWindow() const;

    /**
     * @brief Checks if the window is visible
     * @return True if visible
     */
    bool IsVisible() const;

    /**
     * @brief Checks if the window is enabled
     * @return True if enabled
     */
    bool IsEnabled() const;

    /**
     * @brief Checks if the window is minimized
     * @return True if minimized
     */
    bool Minimized() const;

    /**
     * @brief Checks if the window is maximized
     * @return True if maximized
     */
    bool Maximized() const;

    /**
     * @brief Checks if the window has keyboard focus
     * @return True if focused
     */
    bool IsFocused() const;

    // ----- Handle management -----

    /**
     * @brief Gets the window handle
     * @return The window handle
     */
    HWND GetHandle() const { return m_hWnd; }

    /**
     * @brief Attaches to an existing window handle
     * @param newWindow Window handle to attach to
     */
    void Attach(HWND newWindow);

    /**
     * @brief Detaches from the current window handle
     * @return The detached window handle
     */
    HWND Detach();

    // ----- Window positioning -----

    /**
     * @brief Centers the window on the screen
     */
    void CenterOnScreen();

    /**
     * @brief Centers the window on its parent
     */
    void CenterOnParent();

    /**
     * @brief Centers the window on a specific monitor
     * @param hMonitor Monitor handle (nullptr for nearest monitor)
     */
    void CenterOnMonitor(HMONITOR hMonitor = nullptr);

    // ----- Window class registration -----

    /**
     * @brief Window procedure function type
     */
    using WindowProcFunc = LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM);

    /**
     * @brief Registers a window class (ANSI version)
     * @param hInstance Instance handle
     * @param className Class name
     * @param windowProc Window procedure
     * @param style Class style
     * @param icon Icon
     * @param cursor Cursor
     * @param background Background brush
     * @return True if successful
     */
    static bool RegisterWindowClassA(
        HINSTANCE hInstance,
        const std::string &className,
        WindowProcFunc windowProc,
        UINT style = CS_HREDRAW | CS_VREDRAW,
        HICON icon = nullptr,
        HCURSOR cursor = nullptr,
        HBRUSH background = nullptr);

    /**
     * @brief Registers a window class (Unicode version)
     * @param hInstance Instance handle
     * @param className Class name
     * @param windowProc Window procedure
     * @param style Class style
     * @param icon Icon
     * @param cursor Cursor
     * @param background Background brush
     * @return True if successful
     */
    static bool RegisterWindowClassW(
        HINSTANCE hInstance,
        const std::wstring &className,
        WindowProcFunc windowProc,
        UINT style = CS_HREDRAW | CS_VREDRAW,
        HICON icon = nullptr,
        HCURSOR cursor = nullptr,
        HBRUSH background = nullptr);

    /**
     * @brief Generic window class registration
     * @param hInstance Instance handle
     * @param className Class name
     * @param windowProc Window procedure
     * @param style Class style
     * @param icon Icon
     * @param cursor Cursor
     * @param background Background brush
     * @return True if successful
     */
    template <typename StringT>
    static bool RegisterWindowClass(
        HINSTANCE hInstance,
        const StringT &className,
        WindowProcFunc windowProc,
        UINT style = CS_HREDRAW | CS_VREDRAW,
        HICON icon = nullptr,
        HCURSOR cursor = nullptr,
        HBRUSH background = nullptr) {
        return RegisterWindowClassImpl(
            hInstance,
            className,
            windowProc,
            style,
            icon,
            cursor,
            background,
            typename CharTraits<StringT>::tag{});
    }

    /**
     * @brief Unregisters a window class (ANSI version)
     * @param hInstance Instance handle
     * @param className Class name
     * @return True if successful
     */
    static bool UnregisterWindowClassA(HINSTANCE hInstance, const std::string &className);

    /**
     * @brief Unregisters a window class (Unicode version)
     * @param hInstance Instance handle
     * @param className Class name
     * @return True if successful
     */
    static bool UnregisterWindowClassW(HINSTANCE hInstance, const std::wstring &className);

    /**
     * @brief Generic window class unregistration
     * @param hInstance Instance handle
     * @param className Class name
     * @return True if successful
     */
    template <typename StringT>
    static bool UnregisterWindowClass(HINSTANCE hInstance, const StringT &className) {
        return UnregisterWindowClassImpl(
            hInstance,
            className,
            typename CharTraits<StringT>::tag{});
    }

    // ----- Window procedure -----

    /**
     * @brief Handles window messages
     * @param message Message ID
     * @param wParam WPARAM
     * @param lParam LPARAM
     * @return Message result
     */
    virtual LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Builder class for fluent window creation
     */
    class Builder {
    public:
        /**
         * @brief Constructor with ANSI string
         * @param className Class name
         */
        explicit Builder(const std::string &className) {
            SetClassName(className);
        }

        /**
         * @brief Constructor with wide string
         * @param className Class name
         */
        explicit Builder(const std::wstring &className) {
            SetClassName(className);
        }

        /**
         * @brief Sets the window class name (ANSI version)
         * @param className Class name
         * @return Reference to this builder
         */
        Builder &SetClassName(const std::string &className) {
            m_paramsA.className = className;
            m_useAnsi = true;
            return *this;
        }

        /**
         * @brief Sets the window class name (Unicode version)
         * @param className Class name
         * @return Reference to this builder
         */
        Builder &SetClassName(const std::wstring &className) {
            m_paramsW.className = className;
            m_useAnsi = false;
            return *this;
        }

        /**
         * @brief Sets the window title (ANSI version)
         * @param title Window title
         * @return Reference to this builder
         */
        Builder &SetTitle(const std::string &title) {
            m_paramsA.title = title;
            return *this;
        }

        /**
         * @brief Sets the window title (Unicode version)
         * @param title Window title
         * @return Reference to this builder
         */
        Builder &SetTitle(const std::wstring &title) {
            m_paramsW.title = title;
            return *this;
        }

        /**
         * @brief Sets the window position
         * @param x X position
         * @param y Y position
         * @return Reference to this builder
         */
        Builder &SetPosition(int x, int y) {
            m_paramsA.x = m_paramsW.x = x;
            m_paramsA.y = m_paramsW.y = y;
            return *this;
        }

        /**
         * @brief Sets the window size
         * @param width Width
         * @param height Height
         * @return Reference to this builder
         */
        Builder &SetSize(int width, int height) {
            m_paramsA.width = m_paramsW.width = width;
            m_paramsA.height = m_paramsW.height = height;
            return *this;
        }

        /**
         * @brief Sets the parent window
         * @param parent Parent window handle
         * @return Reference to this builder
         */
        Builder &SetParent(HWND parent) {
            m_paramsA.parent = m_paramsW.parent = parent;
            return *this;
        }

        /**
         * @brief Sets the menu
         * @param menu Menu handle
         * @return Reference to this builder
         */
        Builder &SetMenu(HMENU menu) {
            m_paramsA.menu = m_paramsW.menu = menu;
            return *this;
        }

        /**
         * @brief Sets the instance handle
         * @param instance Instance handle
         * @return Reference to this builder
         */
        Builder &SetInstance(HINSTANCE instance) {
            m_paramsA.instance = m_paramsW.instance = instance;
            return *this;
        }

        /**
         * @brief Sets the creation parameter
         * @param param Creation parameter
         * @return Reference to this builder
         */
        Builder &SetParam(LPVOID param) {
            m_paramsA.param = m_paramsW.param = param;
            return *this;
        }

        /**
         * @brief Sets the window styles
         * @param style Window style
         * @param exStyle Extended window style
         * @return Reference to this builder
         */
        Builder &SetStyles(DWORD style, DWORD exStyle = 0) {
            m_paramsA.styles.style = m_paramsW.styles.style = style;
            m_paramsA.styles.exStyle = m_paramsW.styles.exStyle = exStyle;
            return *this;
        }

        /**
         * @brief Sets whether to center the window
         * @param center Whether to center
         * @return Reference to this builder
         */
        Builder &SetCenter(bool center) {
            m_paramsA.center = m_paramsW.center = center;
            return *this;
        }

        /**
         * @brief Creates the window
         * @return Newly created Window instance
         */
        std::unique_ptr<Window> Build() {
            auto window = std::make_unique<Window>();
            if (m_useAnsi) {
                window->CreateA(m_paramsA);
            } else {
                window->CreateW(m_paramsW);
            }
            return window;
        }

    private:
        WindowCreateParamsA m_paramsA;
        WindowCreateParamsW m_paramsW;
        bool m_useAnsi = false;
    };

protected:
    // ----- Core window data -----
    HWND m_hWnd;
    HINSTANCE m_hInstance;
    float m_DPIScale;
    std::unordered_map<UINT, MessageHandler> m_MessageHandlers;

    // ----- Helper methods -----

    /**
     * @brief Updates the DPI scale
     */
    void UpdateDpiScale();

    /**
     * @brief Calculates a centered position for the window
     * @param windowWidth Window width
     * @param windowHeight Window height
     * @param outX Out parameter for X position
     * @param outY Out parameter for Y position
     * @param hMonitor Monitor to center on
     * @return True if successful
     */
    bool CalculateCenteredPosition(
        int windowWidth,
        int windowHeight,
        int &outX,
        int &outY,
        HMONITOR hMonitor = nullptr) const;

    /**
     * @brief Gets the work area of a monitor
     * @param hMonitor Monitor handle
     * @return Work area rectangle
     */
    static RECT GetMonitorWorkArea(HMONITOR hMonitor);

private:
    // ----- Template implementation details -----

    /**
     * @brief Type traits for character types
     */
    template <typename T>
    struct CharTraits {};

    struct AnsiTag {};

    struct UnicodeTag {};

    template <>
    struct CharTraits<std::string> {
        using tag = AnsiTag;
    };

    template <>
    struct CharTraits<std::wstring> {
        using tag = UnicodeTag;
    };

    // Implementation details for Create
    bool CreateImpl(const WindowCreateParamsA &params, AnsiTag) {
        return CreateA(params);
    }

    bool CreateImpl(const WindowCreateParamsW &params, UnicodeTag) {
        return CreateW(params);
    }

    // Implementation details for SetTitle
    bool SetTitleImpl(const std::string &title, AnsiTag) {
        return SetTitleA(title);
    }

    bool SetTitleImpl(const std::wstring &title, UnicodeTag) {
        return SetTitleW(title);
    }

    // Implementation details for GetTitle
    template <typename T>
    T GetTitleImpl(AnsiTag) const {
        if (std::is_same<T, std::string>::value) {
            return GetTitleA();
        } else {
            auto ansiTitle = GetTitleA();
            return AnsiToWide(ansiTitle);
        }
    }

    template <typename T>
    T GetTitleImpl(UnicodeTag) const {
        if (std::is_same<T, std::wstring>::value) {
            return GetTitleW();
        } else {
            auto wideTitle = GetTitleW();
            return WideToAnsi(wideTitle);
        }
    }

    // Implementation details for RegisterWindowClass
    static bool RegisterWindowClassImpl(
        HINSTANCE hInstance,
        const std::string &className,
        WindowProcFunc windowProc,
        UINT style,
        HICON icon,
        HCURSOR cursor,
        HBRUSH background,
        AnsiTag) {
        return RegisterWindowClassA(hInstance, className, windowProc, style, icon, cursor, background);
    }

    static bool RegisterWindowClassImpl(
        HINSTANCE hInstance,
        const std::wstring &className,
        WindowProcFunc windowProc,
        UINT style,
        HICON icon,
        HCURSOR cursor,
        HBRUSH background,
        UnicodeTag) {
        return RegisterWindowClassW(hInstance, className, windowProc, style, icon, cursor, background);
    }

    // Implementation details for UnregisterWindowClass
    static bool UnregisterWindowClassImpl(
        HINSTANCE hInstance,
        const std::string &className,
        AnsiTag) {
        return UnregisterWindowClassA(hInstance, className);
    }

    static bool UnregisterWindowClassImpl(
        HINSTANCE hInstance,
        const std::wstring &className,
        UnicodeTag) {
        return UnregisterWindowClassW(hInstance, className);
    }

    static std::wstring AnsiToWide(const std::string &str);
    static std::string WideToAnsi(const std::wstring &str);
};

// ----- Factory method implementations -----

template <typename StringT>
std::unique_ptr<Window> Window::CreateStandard(
    const StringT &title,
    int width,
    int height) {
    return Builder(TEXT_STRING("StandardWindow"))
           .SetTitle(title)
           .SetSize(width, height)
           .SetStyles(WS_OVERLAPPEDWINDOW)
           .SetCenter(true)
           .Build();
}

template <typename StringT>
std::unique_ptr<Window> Window::CreateDialogBox(
    const StringT &title,
    int width,
    int height,
    HWND parent) {
    DWORD style = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
    if (parent) {
        style |= WS_POPUP;
    }

    return Builder(TEXT_STRING("DialogWindow"))
           .SetTitle(title)
           .SetSize(width, height)
           .SetParent(parent)
           .SetStyles(style)
           .SetCenter(true)
           .Build();
}

#endif // BP_WINDOW_H
