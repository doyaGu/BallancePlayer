#include "WinContext.h"

#include <string.h>

#include "resource.h"

CWinContext::CWinContext()
    : m_MainWindow(NULL),
      m_RenderWindow(NULL),
      m_hInstance(NULL),
      m_hAccelTable(NULL),
      m_MainWndStyle(0),
      m_RenderWndStyle(0),
      m_Width(0),
      m_Height(0),
      m_StyleFlags(0)
{
    memset(&m_MainWndClass, 0, sizeof(WNDCLASSEX));
    memset(&m_RenderWndClass, 0, sizeof(WNDCLASS));
    memset(m_MainWndName, 0, sizeof(m_MainWndName));
    memset(m_RenderWndName, 0, sizeof(m_RenderWndName));
    memset(m_MainWndClassName, 0, sizeof(m_MainWndClassName));
    memset(m_RenderWndClassName, 0, sizeof(m_RenderWndClassName));
}

CWinContext::~CWinContext()
{
    DestroyWindows();
}

bool CWinContext::Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc)
{
    m_hInstance = hInstance;

    LoadWindowNames();
    if (!RegisterWindowClasses(lpfnWndProc))
        return false;

    if (!CreateWindows())
        return false;

    return true;
}

void CWinContext::UpdateWindows()
{
    ::UpdateWindow(m_MainWindow);
    ::UpdateWindow(m_RenderWindow);
}

void CWinContext::DestroyWindows()
{
    if (m_RenderWindow)
        ::DestroyWindow(m_RenderWindow);

    if (m_MainWindow)
        ::DestroyWindow(m_MainWindow);
}

void CWinContext::ShowMainWindow(bool show)
{
    int cmd = show ? SW_SHOW : SW_SHOWMINIMIZED;
    ::ShowWindow(m_MainWindow, cmd);
}

void CWinContext::ShowRenderWindow(bool show)
{
    int cmd = show ? SW_SHOW : SW_SHOWMINIMIZED;
    ::ShowWindow(m_RenderWindow, cmd);
}

void CWinContext::FocusMainWindow()
{
    ::SetFocus(m_MainWindow);
}

void CWinContext::FocusRenderWindow()
{
    ::SetFocus(m_RenderWindow);
}

bool CWinContext::PostMessageToMainWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessage(m_MainWindow, uMsg, wParam, lParam) == TRUE;
}

bool CWinContext::PostMessageToRenderWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessage(m_RenderWindow, uMsg, wParam, lParam) == TRUE;
}

void CWinContext::GetPosition(int &x, int &y)
{
    if (m_MainWindow)
    {
        RECT rc;
        ::GetWindowRect(m_MainWindow, &rc);
        x = rc.left;
        y = rc.top;
    }
}

void CWinContext::SetPosition(int x, int y)
{
    if (m_MainWindow && m_RenderWindow)
    {
        RECT rc;
        ::GetWindowRect(m_MainWindow, &rc);
        if (-(rc.right - rc.left) < x && x < ::GetSystemMetrics(SM_CXSCREEN) &&
            -(rc.bottom - rc.top) < y && y < ::GetSystemMetrics(SM_CYSCREEN))
        {
            ::SetWindowPos(m_MainWindow, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

            ::GetClientRect(m_MainWindow, &rc);
            ::SetWindowPos(m_RenderWindow, HWND_TOPMOST, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }
}

void CWinContext::GetMainSize(int &width, int &height)
{
    if (m_MainWindow)
    {
        RECT rc;
        ::GetClientRect(m_MainWindow, &rc);
        width = rc.right - rc.left;
        height = rc.bottom - rc.top;
    }
}

void CWinContext::GetRenderSize(int &width, int &height)
{
    if (m_RenderWindow)
    {
        RECT rc;
        ::GetClientRect(m_RenderWindow, &rc);
        width = rc.right - rc.left;
        height = rc.bottom - rc.top;
    }
}

void CWinContext::SetMainSize(int width, int height)
{
    m_Width = width;
    m_Height = height;

    if (m_MainWindow)
    {
        RECT rect = {0, 0, width, height};
        ::AdjustWindowRect(&rect, m_MainWndStyle, FALSE);
        ::SetWindowPos(m_MainWindow, HWND_TOPMOST, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
    }
}

void CWinContext::SetRenderSize(int width, int height)
{
    if (m_RenderWindow)
        ::SetWindowPos(m_RenderWindow, HWND_TOPMOST, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void CWinContext::SetMainStyle(int add, int remove)
{
    m_StyleFlags |= add;
    m_StyleFlags &= ~remove;

    if (m_MainWindow)
    {
        DWORD style = WS_POPUP;
        if (!(m_StyleFlags & WINDOW_STYLE_FULLSCREEN))
        {
            if (!(m_StyleFlags & WINDOW_STYLE_BORDERLESS))
                style = WS_OVERLAPPED | WS_CAPTION;
            if (m_StyleFlags & WINDOW_STYLE_RESIZABLE)
                style |= WS_SIZEBOX;
        }

        if (style != m_MainWndStyle)
        {
            ::SetWindowLong(m_MainWindow, GWL_STYLE, style);
            ::SetWindowPos(m_MainWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            m_MainWndStyle = style;
        }
    }
}

void CWinContext::SetWindowText(LPCSTR str)
{
    if (!str)
        str = m_MainWndName;
    ::SetWindowText(m_MainWindow, str);
}

int CWinContext::TranslateAccelerators(MSG *msg)
{
    return ::TranslateAccelerator(m_MainWindow, m_hAccelTable, msg);
}

bool CWinContext::IsIconic()
{
    return ::IsIconic(m_MainWindow) == TRUE;
}

void CWinContext::LoadWindowNames()
{
    ::LoadString(m_hInstance, IDS_MAIN_WND_NAME, m_MainWndName, 100);
    ::LoadString(m_hInstance, IDS_RENDER_WND_NAME, m_RenderWndName, 100);
    ::LoadString(m_hInstance, IDS_MAIN_WND_CLASS_NAME, m_MainWndClassName, 100);
    ::LoadString(m_hInstance, IDS_RENDER_WND_CLASS_NAME, m_RenderWndClassName, 100);
}

bool CWinContext::RegisterWindowClasses(LPFNWNDPROC lpfnWndProc)
{
    m_MainWndClass.lpfnWndProc = lpfnWndProc;
    m_MainWndClass.cbSize = sizeof(WNDCLASSEX);
    m_MainWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    m_MainWndClass.cbClsExtra = 0;
    m_MainWndClass.cbWndExtra = 0;
    m_MainWndClass.hInstance = m_hInstance;
    m_MainWndClass.hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_PLAYER));
    m_MainWndClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    m_MainWndClass.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    m_MainWndClass.lpszMenuName = NULL;
    m_MainWndClass.lpszClassName = m_MainWndClassName;
    m_MainWndClass.hIconSm = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_PLAYER));

    m_RenderWndClass.lpfnWndProc = lpfnWndProc;
    m_RenderWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    m_RenderWndClass.hInstance = m_hInstance;
    m_RenderWndClass.lpszClassName = m_RenderWndClassName;

    if (!::RegisterClass(&m_RenderWndClass))
        return false;

    if (!::RegisterClassEx(&m_MainWndClass))
        return false;

    return true;
}

bool CWinContext::CreateWindows()
{
    m_MainWndStyle = (m_StyleFlags & (WINDOW_STYLE_FULLSCREEN | WINDOW_STYLE_BORDERLESS)) ? WS_POPUP : WS_OVERLAPPED | WS_CAPTION;
    if (!(m_StyleFlags & WINDOW_STYLE_FULLSCREEN))
    {
        if (m_StyleFlags & WINDOW_STYLE_RESIZABLE)
            m_MainWndStyle |= WS_SIZEBOX;
    }

    RECT rect = {0, 0, m_Width, m_Height};
    ::AdjustWindowRect(&rect, m_MainWndStyle, FALSE);

    int mainWidth = rect.right - rect.left;
    int mainHeight = rect.bottom - rect.top;

    int x = (m_StyleFlags & WINDOW_STYLE_FULLSCREEN) ? CW_USEDEFAULT : (::GetSystemMetrics(SM_CXSCREEN) - mainWidth) / 2;
    int y = (m_StyleFlags & WINDOW_STYLE_FULLSCREEN) ? CW_USEDEFAULT : (::GetSystemMetrics(SM_CYSCREEN) - mainHeight) / 2;

    m_MainWindow = ::CreateWindowEx(
        WS_EX_LEFT,
        m_MainWndClassName,
        m_MainWndName,
        m_MainWndStyle,
        x,
        y,
        mainWidth,
        mainHeight,
        NULL,
        NULL,
        m_hInstance,
        NULL);
    if (!m_MainWindow)
        return false;

    m_RenderWndStyle = WS_CHILD | WS_VISIBLE;
    m_RenderWindow = ::CreateWindowEx(
        WS_EX_TOPMOST,
        m_RenderWndClassName,
        m_RenderWndName,
        m_RenderWndStyle,
        0,
        0,
        m_Width,
        m_Height,
        m_MainWindow,
        NULL,
        m_hInstance,
        NULL);
    if (!m_RenderWindow)
        return false;

    m_hAccelTable = ::LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCEL));

    return true;
}