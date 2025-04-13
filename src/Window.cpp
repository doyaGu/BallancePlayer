#include "Window.h"

#include <algorithm>
#include <cassert>

Window::Window() :
    m_hWnd(nullptr),
    m_hInstance(nullptr),
    m_DPIScale(1.0f) {}

Window::Window(HWND handle) :
    m_hWnd(handle),
    m_hInstance(handle ? (HINSTANCE) GetWindowLongPtr(handle, GWLP_HINSTANCE) : nullptr),
    m_DPIScale(1.0f) {
    if (m_hWnd) {
        UpdateDpiScale();
    }
}

Window::~Window() {
    Window::Destroy();
}

Window::Window(Window &&other) noexcept :
    m_hWnd(other.m_hWnd),
    m_hInstance(other.m_hInstance),
    m_DPIScale(other.m_DPIScale),
    m_MessageHandlers(std::move(other.m_MessageHandlers)) {
    other.m_hWnd = nullptr;
    other.m_hInstance = nullptr;
}

Window &Window::operator=(Window &&other) noexcept {
    if (this != &other) {
        Destroy();

        m_hWnd = other.m_hWnd;
        m_hInstance = other.m_hInstance;
        m_DPIScale = other.m_DPIScale;
        m_MessageHandlers = std::move(other.m_MessageHandlers);

        other.m_hWnd = nullptr;
        other.m_hInstance = nullptr;
    }
    return *this;
}

std::unique_ptr<Window> Window::CreateChild(
    HWND parent,
    int id,
    int x,
    int y,
    int width,
    int height) {
    auto window = std::make_unique<Window>();

    WindowCreateParams params;
    params.className = TEXT_STRING("STATIC"); // Use standard Windows static control class
    params.title = TEXT_STRING("");
    params.x = x;
    params.y = y;
    params.width = width;
    params.height = height;
    params.parent = parent;
    params.menu = (HMENU) id; // Child window ID
    params.styles.style = WS_CHILD | WS_VISIBLE;

    window->Create(params);
    return window;
}

bool Window::CreateA(const WindowCreateParamsA &params) {
    // Don't create if already created
    if (m_hWnd) {
        return false;
    }

    // Use default instance if none provided
    m_hInstance = params.instance ? params.instance : GetModuleHandle(nullptr);

    // Create the window
    m_hWnd = ::CreateWindowExA(
        params.styles.exStyle,
        params.className.c_str(),
        params.title.c_str(),
        params.styles.style,
        params.x, params.y,
        params.width, params.height,
        params.parent,
        params.menu,
        m_hInstance,
        params.param ? params.param : this
    );

    if (!m_hWnd) {
        // Get error information for debugging
        DWORD error = GetLastError();
        (void) error; // Avoid unused warning in release build
        assert(!"Failed to create window");
        return false;
    }

    // Update DPI scale
    UpdateDpiScale();

    // Center window if requested
    if (params.center) {
        CenterOnScreen();
    }

    return true;
}

bool Window::CreateW(const WindowCreateParamsW &params) {
    // Don't create if already created
    if (m_hWnd) {
        return false;
    }

    // Use default instance if none provided
    m_hInstance = params.instance ? params.instance : GetModuleHandle(nullptr);

    // Create the window
    m_hWnd = ::CreateWindowExW(
        params.styles.exStyle,
        params.className.c_str(),
        params.title.c_str(),
        params.styles.style,
        params.x, params.y,
        params.width, params.height,
        params.parent,
        params.menu,
        m_hInstance,
        params.param ? params.param : this
    );

    if (!m_hWnd) {
        // Get error information for debugging
        DWORD error = GetLastError();
        (void) error; // Avoid unused warning in release build
        assert(!"Failed to create window");
        return false;
    }

    // Update DPI scale
    UpdateDpiScale();

    // Center window if requested
    if (params.center) {
        CenterOnScreen();
    }

    return true;
}

bool Window::Destroy() {
    if (!m_hWnd) {
        return true;
    }

    bool result = ::DestroyWindow(m_hWnd) != 0;
    if (result) {
        m_hWnd = nullptr;
    }

    return result;
}

bool Window::Show(int cmdShow) {
    if (!m_hWnd) {
        return false;
    }

    return ::ShowWindow(m_hWnd, cmdShow) != 0;
}

bool Window::Hide() {
    return Show(SW_HIDE);
}

bool Window::Minimize() {
    return Show(SW_MINIMIZE);
}

bool Window::Maximize() {
    return Show(SW_MAXIMIZE);
}

bool Window::Restore() {
    return Show(SW_RESTORE);
}

WindowState Window::GetState() const {
    if (!m_hWnd) {
        return WindowState::Normal;
    }

    if (Minimized()) {
        return WindowState::Minimized;
    }

    if (Maximized()) {
        return WindowState::Maximized;
    }

    // Check if fullscreen by comparing window rect to monitor work area
    RECT windowRect;
    if (::GetWindowRect(m_hWnd, &windowRect)) {
        HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo = {sizeof(MONITORINFO)};

        if (::GetMonitorInfo(hMonitor, &monitorInfo)) {
            // If window covers entire monitor screen (not just work area), it's likely fullscreen
            if (::EqualRect(&windowRect, &monitorInfo.rcMonitor)) {
                return WindowState::Fullscreen;
            }
        }
    }

    return WindowState::Normal;
}

bool Window::BringToFront() {
    if (!m_hWnd) {
        return false;
    }

    return ::BringWindowToTop(m_hWnd) != 0;
}

bool Window::SetForeground() {
    if (!m_hWnd) {
        return false;
    }

    return ::SetForegroundWindow(m_hWnd) != 0;
}

HWND Window::SetFocus() {
    if (!m_hWnd) {
        return nullptr;
    }

    return ::SetFocus(m_hWnd);
}

bool Window::Move(int x, int y, bool preserveZOrder) {
    if (!m_hWnd) {
        return false;
    }

    RECT rect;
    if (!::GetWindowRect(m_hWnd, &rect)) {
        return false;
    }

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    UINT flags = SWP_NOSIZE | SWP_NOACTIVATE;
    if (preserveZOrder) {
        flags |= SWP_NOZORDER;
    }

    return SetPosition(nullptr, x, y, width, height, flags);
}

bool Window::Resize(int width, int height) {
    if (!m_hWnd) {
        return false;
    }

    RECT rect;
    if (!::GetWindowRect(m_hWnd, &rect)) {
        return false;
    }

    return SetPosition(nullptr, rect.left, rect.top, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}

bool Window::SetPosition(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags) {
    if (!m_hWnd) {
        return false;
    }

    return ::SetWindowPos(m_hWnd, hWndInsertAfter, x, y, cx, cy, uFlags) == TRUE;
}

bool Window::SetPositionAndSize(int x, int y, int width, int height, bool repaint) {
    if (!m_hWnd) {
        return false;
    }

    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
    if (!repaint) {
        flags |= SWP_NOREDRAW;
    }

    return SetPosition(nullptr, x, y, width, height, flags);
}

bool Window::GetPosition(int &x, int &y) const {
    if (!m_hWnd) {
        x = y = 0;
        return false;
    }

    RECT rect;
    if (!::GetWindowRect(m_hWnd, &rect)) {
        x = y = 0;
        return false;
    }

    x = rect.left;
    y = rect.top;
    return true;
}

bool Window::GetSize(int &width, int &height) const {
    if (!m_hWnd) {
        width = height = 0;
        return false;
    }

    RECT rect;
    if (!::GetWindowRect(m_hWnd, &rect)) {
        width = height = 0;
        return false;
    }

    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    return true;
}

bool Window::GetClientSize(int &width, int &height) const {
    if (!m_hWnd) {
        width = height = 0;
        return false;
    }

    RECT rect;
    if (!::GetClientRect(m_hWnd, &rect)) {
        width = height = 0;
        return false;
    }

    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    return true;
}

bool Window::GetClientRect(RECT &rect) {
    if (!m_hWnd) {
        return false;
    }

    return ::GetClientRect(m_hWnd, &rect) == TRUE;
}

bool Window::SetTitleA(const std::string &title) {
    if (!m_hWnd) {
        return false;
    }

    return ::SetWindowTextA(m_hWnd, title.c_str()) != 0;
}

bool Window::SetTitleW(const std::wstring &title) {
    if (!m_hWnd) {
        return false;
    }

    return ::SetWindowTextW(m_hWnd, title.c_str()) != 0;
}

std::string Window::GetTitleA() const {
    if (!m_hWnd) {
        return "";
    }

    int length = ::GetWindowTextLengthA(m_hWnd);
    if (length == 0) {
        return "";
    }

    std::string title;
    title.resize(length);
    ::GetWindowTextA(m_hWnd, &title[0], length + 1);

    return title;
}

std::wstring Window::GetTitleW() const {
    if (!m_hWnd) {
        return L"";
    }

    int length = ::GetWindowTextLengthW(m_hWnd);
    if (length == 0) {
        return L"";
    }

    std::wstring title(length + 1, L'\0');
    ::GetWindowTextW(m_hWnd, &title[0], length + 1);
    title.resize(length); // Remove null terminator from string

    return title;
}

bool Window::SetIcon(HICON icon, bool isSmallIcon) {
    if (!m_hWnd) {
        return false;
    }

    WPARAM iconType = isSmallIcon ? ICON_SMALL : ICON_BIG;
    return SendMessage(WM_SETICON, iconType, (LPARAM) icon) != 0;
}

DWORD Window::SetStyle(DWORD style) {
    if (!m_hWnd) {
        return 0;
    }

    auto oldStyle = static_cast<DWORD>(::GetWindowLongW(m_hWnd, GWL_STYLE));

    if (::SetWindowLongW(m_hWnd, GWL_STYLE, static_cast<LONG>(style)) == 0) {
        // Failed to set style
        return 0;
    }

    // Force the window to redraw and recalculate its frame
    SetPosition(nullptr, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    return oldStyle;
}

DWORD Window::GetStyle() const {
    if (!m_hWnd) {
        return 0;
    }

    return static_cast<DWORD>(::GetWindowLongW(m_hWnd, GWL_STYLE));
}

DWORD Window::SetExStyle(DWORD exStyle) {
    if (!m_hWnd) {
        return 0;
    }

    auto oldExStyle = static_cast<DWORD>(::GetWindowLongW(m_hWnd, GWL_EXSTYLE));

    if (::SetWindowLongW(m_hWnd, GWL_EXSTYLE, static_cast<LONG>(exStyle)) == 0) {
        // Failed to set extended style
        return 0;
    }

    // Force the window to redraw and recalculate its frame
    SetPosition(nullptr, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    return oldExStyle;
}

DWORD Window::GetExStyle() const {
    if (!m_hWnd) {
        return 0;
    }

    return static_cast<DWORD>(::GetWindowLongW(m_hWnd, GWL_EXSTYLE));
}

bool Window::ClientToScreen(POINT &point) const {
    if (!m_hWnd) {
        return false;
    }

    return ::ClientToScreen(m_hWnd, &point) != 0;
}

bool Window::ScreenToClient(POINT &point) const {
    if (!m_hWnd) {
        return false;
    }

    return ::ScreenToClient(m_hWnd, &point) != 0;
}

void Window::ClientToScreen(RECT &rect) const {
    if (!m_hWnd) {
        return;
    }

    POINT topLeft = {rect.left, rect.top};
    POINT bottomRight = {rect.right, rect.bottom};

    ClientToScreen(topLeft);
    ClientToScreen(bottomRight);

    rect.left = topLeft.x;
    rect.top = topLeft.y;
    rect.right = bottomRight.x;
    rect.bottom = bottomRight.y;
}

void Window::ScreenToClient(RECT &rect) const {
    if (!m_hWnd) {
        return;
    }

    POINT topLeft = {rect.left, rect.top};
    POINT bottomRight = {rect.right, rect.bottom};

    ScreenToClient(topLeft);
    ScreenToClient(bottomRight);

    rect.left = topLeft.x;
    rect.top = topLeft.y;
    rect.right = bottomRight.x;
    rect.bottom = bottomRight.y;
}

bool Window::Update() {
    if (!m_hWnd) {
        return false;
    }

    return ::UpdateWindow(m_hWnd) != 0;
}

bool Window::InvalidateRect(const RECT *rect, bool erase) {
    if (!m_hWnd) {
        return false;
    }

    return ::InvalidateRect(m_hWnd, rect, erase ? TRUE : FALSE) != 0;
}

void Window::SetRedraw(bool redraw) {
    if (!m_hWnd) {
        return;
    }

    (void) SendMessage(WM_SETREDRAW, redraw ? TRUE : FALSE, 0);
}

HDC Window::GetDC() const {
    if (!m_hWnd) {
        return nullptr;
    }

    return ::GetDC(m_hWnd);
}

bool Window::ReleaseDC(HDC hdc) const {
    if (!m_hWnd) {
        return false;
    }

    return ::ReleaseDC(m_hWnd, hdc) != 0;
}

LRESULT Window::SendMessage(UINT message, WPARAM wParam, LPARAM lParam) const {
    if (!m_hWnd) {
        return 0;
    }

    return ::SendMessage(m_hWnd, message, wParam, lParam);
}

bool Window::PostMessage(UINT message, WPARAM wParam, LPARAM lParam) const {
    if (!m_hWnd) {
        return false;
    }

    return ::PostMessage(m_hWnd, message, wParam, lParam) != 0;
}

void Window::RegisterMessageHandler(UINT message, MessageHandler handler) {
    m_MessageHandlers[message] = std::move(handler);
}

void Window::UnregisterMessageHandler(UINT message) {
    m_MessageHandlers.erase(message);
}

void Window::EnableDpiAwareness(bool enable) {
    if (enable) {
        // Try to enable Per-Monitor DPI awareness
        HINSTANCE hUser32 = LoadLibraryA("user32.dll");
        if (hUser32) {
            using SetProcessDPIAwarePtr = BOOL (WINAPI*)();
            using SetProcessDpiAwarenessPtr = HRESULT (WINAPI*)(int);
            using SetProcessDpiAwarenessContextPtr = BOOL (WINAPI*)(DPI_AWARENESS_CONTEXT);

            // Try Windows 10 (1703) API first - most advanced
            auto setProcessDpiAwarenessContextPtr =
                reinterpret_cast<SetProcessDpiAwarenessContextPtr>(
                    GetProcAddress(hUser32, "SetProcessDpiAwarenessContext"));

            if (setProcessDpiAwarenessContextPtr) {
                // Try for Per-Monitor V2 first (Windows 10 1703+)
                auto perMonitorV2 = reinterpret_cast<DPI_AWARENESS_CONTEXT>(-4);

                if (!setProcessDpiAwarenessContextPtr(perMonitorV2)) {
                    // Fall back to regular Per-Monitor (Windows 8.1+)
                    auto perMonitor = reinterpret_cast<DPI_AWARENESS_CONTEXT>(-3);
                    setProcessDpiAwarenessContextPtr(perMonitor);
                }
            } else {
                // Try Windows 8.1+ API
                auto setProcessDpiAwarenessPtr = reinterpret_cast<SetProcessDpiAwarenessPtr>(
                    GetProcAddress(hUser32, "SetProcessDpiAwareness"));

                if (setProcessDpiAwarenessPtr) {
                    setProcessDpiAwarenessPtr(2); // PROCESS_PER_MONITOR_DPI_AWARE
                } else {
                    // Fall back to Windows Vista API
                    auto setProcessDPIAwarePtr = reinterpret_cast<SetProcessDPIAwarePtr>(
                        GetProcAddress(hUser32, "SetProcessDPIAware"));

                    if (setProcessDPIAwarePtr) {
                        setProcessDPIAwarePtr();
                    }
                }
            }

            FreeLibrary(hUser32);
        }
    }

    // Update DPI scale
    UpdateDpiScale();
}

float Window::GetDpiScale() const {
    return m_DPIScale;
}

bool Window::IsWindow() const {
    return m_hWnd != nullptr && ::IsWindow(m_hWnd) != 0;
}

bool Window::IsChildWindow() const {
    if (!m_hWnd) {
        return false;
    }

    return (GetStyle() & WS_CHILD) != 0;
}

bool Window::IsVisible() const {
    if (!m_hWnd) {
        return false;
    }

    return ::IsWindowVisible(m_hWnd) != 0;
}

bool Window::IsEnabled() const {
    if (!m_hWnd) {
        return false;
    }

    return ::IsWindowEnabled(m_hWnd) != 0;
}

bool Window::Minimized() const {
    if (!m_hWnd) {
        return false;
    }

    return ::IsIconic(m_hWnd) != 0;
}

bool Window::Maximized() const {
    if (!m_hWnd) {
        return false;
    }

    return ::IsZoomed(m_hWnd) != 0;
}

bool Window::IsFocused() const {
    if (!m_hWnd) {
        return false;
    }

    return ::GetFocus() == m_hWnd;
}

void Window::Attach(HWND newWindow) {
    if (m_hWnd == newWindow) {
        return;
    }

    if (m_hWnd) {
        Destroy();
    }

    m_hWnd = newWindow;

    if (m_hWnd) {
        m_hInstance = (HINSTANCE) ::GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE);
        UpdateDpiScale();
    } else {
        m_hInstance = nullptr;
    }
}

HWND Window::Detach() {
    HWND oldWindow = m_hWnd;
    m_hWnd = nullptr;
    m_hInstance = nullptr;
    return oldWindow;
}

void Window::CenterOnScreen() {
    if (!m_hWnd) {
        return;
    }

    int width, height;
    if (!GetSize(width, height)) {
        return;
    }

    int x, y;
    if (CalculateCenteredPosition(width, height, x, y)) {
        Move(x, y);
    }
}

void Window::CenterOnParent() {
    if (!m_hWnd) {
        return;
    }

    HWND parent = ::GetParent(m_hWnd);
    if (!parent) {
        CenterOnScreen();
        return;
    }

    RECT parentRect;
    if (!::GetWindowRect(parent, &parentRect)) {
        CenterOnScreen();
        return;
    }

    int parentWidth = parentRect.right - parentRect.left;
    int parentHeight = parentRect.bottom - parentRect.top;

    int width, height;
    if (!GetSize(width, height)) {
        return;
    }

    int x = parentRect.left + (parentWidth - width) / 2;
    int y = parentRect.top + (parentHeight - height) / 2;

    Move(x, y);
}

void Window::CenterOnMonitor(HMONITOR hMonitor) {
    if (!m_hWnd) {
        return;
    }

    if (!hMonitor) {
        hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
    }

    int width, height;
    if (!GetSize(width, height)) {
        return;
    }

    int x, y;
    if (CalculateCenteredPosition(width, height, x, y, hMonitor)) {
        Move(x, y);
    }
}

bool Window::RegisterWindowClassA(
    HINSTANCE hInstance,
    const std::string &className,
    WindowProcFunc windowProc,
    UINT style,
    HICON icon,
    HCURSOR cursor,
    HBRUSH background) {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);

    // Check if already registered
    if (::GetClassInfoExA(hInstance, className.c_str(), &wc)) {
        return true;
    }

    // Setup window class
    wc.style = style;
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.hIcon = icon ? icon : ::LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = cursor ? cursor : ::LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = background;
    wc.lpszClassName = className.c_str();

    return ::RegisterClassExA(&wc) != 0;
}

bool Window::RegisterWindowClassW(
    HINSTANCE hInstance,
    const std::wstring &className,
    WindowProcFunc windowProc,
    UINT style,
    HICON icon,
    HCURSOR cursor,
    HBRUSH background) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);

    // Check if already registered
    if (::GetClassInfoExW(hInstance, className.c_str(), &wc)) {
        return true;
    }

    // Setup window class
    wc.style = style;
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.hIcon = icon ? icon : ::LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = cursor ? cursor : ::LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = background;
    wc.lpszClassName = className.c_str();

    return ::RegisterClassExW(&wc) != 0;
}

bool Window::UnregisterWindowClassA(HINSTANCE hInstance, const std::string &className) {
    return ::UnregisterClassA(className.c_str(), hInstance) != 0;
}

bool Window::UnregisterWindowClassW(HINSTANCE hInstance, const std::wstring &className) {
    return ::UnregisterClassW(className.c_str(), hInstance) != 0;
}

LRESULT Window::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    // Check for registered message handlers
    auto it = m_MessageHandlers.find(message);
    if (it != m_MessageHandlers.end()) {
        LRESULT result = 0;
        if (it->second(message, wParam, lParam, result)) {
            return result;
        }
    }

    // Default processing
    return ::DefWindowProc(m_hWnd, message, wParam, lParam);
}

void Window::UpdateDpiScale() {
    if (!m_hWnd) {
        m_DPIScale = 1.0f;
        return;
    }

    // Try to get DPI using modern APIs first
    UINT dpiX = 96;

    // Use GetModuleHandleA only once since it's an expensive operation
    static HMODULE hUser32 = GetModuleHandleA("user32.dll");

    if (hUser32) {
        // Cache function pointers to avoid repeated lookups
        static auto getDpiForWindowPtr =
            reinterpret_cast<UINT (WINAPI*)(HWND)>(
                GetProcAddress(hUser32, "GetDpiForWindow"));

        if (getDpiForWindowPtr) {
            dpiX = getDpiForWindowPtr(m_hWnd);
        } else {
            // Fall back to older method
            HDC hdc = GetDC();
            if (hdc) {
                dpiX = ::GetDeviceCaps(hdc, LOGPIXELSX);
                ReleaseDC(hdc);
            }
        }
    }

    // Calculate scale factor (96 is the default DPI)
    m_DPIScale = static_cast<float>(dpiX) / 96.0f;
}

bool Window::CalculateCenteredPosition(int windowWidth, int windowHeight, int &outX, int &outY,
                                       HMONITOR hMonitor) const {
    outX = 0;
    outY = 0;

    if (!hMonitor) {
        if (m_hWnd) {
            hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
        }

        if (!hMonitor) {
            hMonitor = ::MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
        }
    }

    if (!hMonitor) {
        return false;
    }

    // Get monitor work area
    MONITORINFO monitorInfo = {sizeof(MONITORINFO)};
    if (!::GetMonitorInfo(hMonitor, &monitorInfo)) {
        return false;
    }

    const RECT &workArea = monitorInfo.rcWork;

    // Calculate centered position
    int monitorWidth = workArea.right - workArea.left;
    int monitorHeight = workArea.bottom - workArea.top;

    // Check if window fits in monitor
    if (windowWidth > monitorWidth || windowHeight > monitorHeight) {
        // Window doesn't fit, place at monitor origin
        outX = workArea.left;
        outY = workArea.top;
    } else {
        outX = workArea.left + (monitorWidth - windowWidth) / 2;
        outY = workArea.top + (monitorHeight - windowHeight) / 2;
    }

    return true;
}

RECT Window::GetMonitorWorkArea(HMONITOR hMonitor) {
    if (hMonitor) {
        MONITORINFO monitorInfo = {sizeof(MONITORINFO)};
        if (::GetMonitorInfo(hMonitor, &monitorInfo)) {
            return monitorInfo.rcWork;
        }
    }

    // Fallback to primary monitor work area
    RECT workArea = {0, 0, 0, 0};
    ::SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    return workArea;
}

std::wstring Window::AnsiToWide(const std::string &str) {
    if (str.empty()) return {};

    int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
    if (size <= 0) return {};

    std::wstring result(size, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), &result[0], size);

    return result;
}

std::string Window::WideToAnsi(const std::wstring &str) {
    if (str.empty()) return {};

    int size = WideCharToMultiByte(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
    if (size <= 0) return {};

    std::string result(size, 0);
    WideCharToMultiByte(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), &result[0], size, nullptr, nullptr);

    return result;
}
