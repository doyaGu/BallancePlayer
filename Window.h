#ifndef PLAYER_WINDOW_H
#define PLAYER_WINDOW_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

class CWindow
{
public:
    CWindow() : m_Window(NULL) {}

    explicit CWindow(HWND newWindow) : m_Window(newWindow) {}

    CWindow& operator=(HWND newWindow)
    {
        m_Window = newWindow;
        return *this;
    }

    HWND GetHandle() const { return m_Window; }

    void Attach(HWND newWindow) { m_Window = newWindow; }

    HWND Detach()
    {
        HWND window = m_Window;
        m_Window = NULL;
        return window;
    }

    bool Create(LPCTSTR className,
                LPCTSTR windowName, DWORD style,
                int x, int y, int width, int height,
                HWND parentWindow, HMENU idOrHMenu,
                HINSTANCE instance, LPVOID createParam)
    {
        m_Window = ::CreateWindow(className, windowName,
                                 style, x, y, width, height, parentWindow,
                                 idOrHMenu, instance, createParam);
        return m_Window != NULL;
    }

    bool CreateEx(DWORD exStyle, LPCTSTR className,
                  LPCTSTR windowName, DWORD style,
                  int x, int y, int width, int height,
                  HWND parentWindow, HMENU idOrHMenu,
                  HINSTANCE instance, LPVOID createParam)
    {
        m_Window = ::CreateWindowEx(exStyle, className, windowName,
                                    style, x, y, width, height, parentWindow,
                                    idOrHMenu, instance, createParam);
        return m_Window != NULL;
    }

    bool Destroy()
    {
        if (m_Window == NULL)
            return true;
        bool result = ::DestroyWindow(m_Window) == TRUE;
        if (result)
            m_Window = NULL;
        return result;
    }

    bool IsWindow() {  return ::IsWindow(m_Window) == TRUE; }

    bool Foreground() { return ::SetForegroundWindow(m_Window) == TRUE; }
    HWND GetParent() const { return ::GetParent(m_Window); }
    bool GetWindowRect(LPRECT rect) const { return ::GetWindowRect(m_Window,rect) == TRUE; }
    bool IsIconic() const { return ::IsIconic(m_Window) == TRUE; }
    bool IsZoomed() const { return ::IsZoomed(m_Window) == TRUE; }
    bool ClientToScreen(LPPOINT point) const { return ::ClientToScreen(m_Window, point) == TRUE; }
    bool ScreenToClient(LPPOINT point) const { return ::ScreenToClient(m_Window, point) == TRUE; }

    void ScreenToClient(RECT *rect)
    {
        POINT p1, p2;
        p1.x = rect->left;
        p1.y = rect->top;
        p2.x = rect->right;
        p2.y = rect->bottom;
        ScreenToClient(&p1);
        ScreenToClient(&p2);

        rect->left = p1.x;
        rect->top = p1.y;
        rect->right = p2.x;
        rect->bottom = p2.y;
    }
    bool GetClientRect(LPRECT rect) { return ::GetClientRect(m_Window, rect) == TRUE; }

    bool Show(int cmdShow = SW_SHOW) { return ::ShowWindow(m_Window, cmdShow) == TRUE; }

    bool Move(int x, int y, int width, int height, bool repaint = true)
    {
        return ::MoveWindow(m_Window, x, y, width, height, (BOOL)repaint) == TRUE;
    }

    bool Update() { return ::UpdateWindow(m_Window) == TRUE; }

    bool InvalidateRect(LPCRECT rect, bool backgroundErase = true)
    {
        return ::InvalidateRect(m_Window, rect, (BOOL)backgroundErase) == TRUE;
    }

    void SetRedraw(bool redraw = true) { SendMsg(WM_SETREDRAW, (WPARAM)(BOOL)redraw, 0); }

    LONG SetStyle(LONG style) { return SetLong(GWL_STYLE, style); }
    LONG GetStyle() const { return GetLong(GWL_STYLE); }

    bool SetPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT flags) {
        return ::SetWindowPos(m_Window, hWndInsertAfter, x, y, cx, cy, flags) == TRUE;
    }

    LONG SetLong(int index, LONG newLongPtr) { return ::SetWindowLong(m_Window, index, newLongPtr); }
    LONG GetLong(int index) const { return ::GetWindowLong(m_Window, index); }

    HWND SetFocus() { return ::SetFocus(m_Window); }

    LRESULT SendMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
    {
        return ::SendMessage(m_Window, message, wParam, lParam);
    }

    bool PostMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
    {
        return ::PostMessage(m_Window, message, wParam, lParam) == TRUE;
    }

    bool SetText(LPCTSTR s) { return ::SetWindowText(m_Window, s) == TRUE; }

    int GetTextLength() const { return ::GetWindowTextLength(m_Window); }

    int GetText(LPTSTR string, int maxCount) const { return ::GetWindowText(m_Window, string, maxCount); }

protected:
    HWND m_Window;
};

#endif // PLAYER_WINDOW_H
