#include "Splash.h"

#include <stdio.h>
#include <stdlib.h>

#include "resource.h"

#define PALVERSION 0x300

static CSplash gSplash;
static HPALETTE hPalette = NULL;

LRESULT CALLBACK SplashWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hDc;
    PAINTSTRUCT ps;

    static HINSTANCE hInstance;
    static DWORD dwNewWidth, dwNewHeight;

    switch (uMsg)
    {
    case WM_CREATE:
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        hPalette = gSplash.GetPalette();
        ::SetCursor(::LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
        return 0;

    case WM_DESTROY:
        ::DeleteObject(hPalette);
        return 0;

    case WM_SIZE:
        dwNewWidth = LOWORD(lParam);
        dwNewHeight = HIWORD(lParam);
        return 0;

    case WM_PAINT:
        hDc = ::BeginPaint(hWnd, &ps);
        if (hPalette)
        {
            ::SelectPalette(hDc, hPalette, FALSE);
            ::RealizePalette(hDc);
        }
        ::SetDIBitsToDevice(hDc,
                            0,
                            0,
                            gSplash.GetWidth(),
                            gSplash.GetHeight(),
                            0,
                            0,
                            0,
                            gSplash.GetHeight(),
                            gSplash.GetBitmapData(),
                            gSplash.GetBitmapInfo(),
                            DIB_RGB_COLORS);
        ::EndPaint(hWnd, &ps);
        return 0;

    case WM_QUERYNEWPALETTE:
        if (hPalette)
        {
            hDc = ::GetDC(hWnd);
            ::SelectPalette(hDc, hPalette, FALSE);
            ::RealizePalette(hDc);
            ::InvalidateRect(hWnd, NULL, TRUE);
            ::ReleaseDC(hWnd, hDc);
            return 1;
        }
        break;

    case WM_PALETTECHANGED:
        if (hPalette && (HWND)wParam != hWnd)
        {
            hDc = ::GetDC(hWnd);
            ::SelectPalette(hDc, hPalette, FALSE);
            ::RealizePalette(hDc);
            ::UpdateColors(hDc);
            ::ReleaseDC(hWnd, hDc);
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

CSplash::CSplash() : m_Data(NULL), m_hWnd(NULL), m_hInstance(NULL) {}

CSplash::CSplash(HINSTANCE hInstance) : m_Data(NULL), m_hWnd(NULL), m_hInstance(hInstance) {}

CSplash::~CSplash()
{
    if (m_Data)
        delete[] m_Data;
}

bool CSplash::Show()
{
    m_Data = NULL;

    WNDCLASSA wndclass;
    memset(&wndclass, 0, sizeof(WNDCLASSA));
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = SplashWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = m_hInstance;
    wndclass.hIcon = 0;
    wndclass.hCursor = 0;
    wndclass.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName = 0;
    wndclass.lpszClassName = "SPLASH";
    wndclass.hIcon = ::LoadIconA(m_hInstance, (LPCSTR)IDI_PLAYER);

    if (!::RegisterClassA(&wndclass))
        return false;

    char buffer[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    ::GetModuleFileNameA(NULL, buffer, 1024);
    _splitpath(buffer, drive, dir, filename, NULL);
    _snprintf(buffer, MAX_PATH, "%s%ssplash.bmp", drive, dir);
    if (!gSplash.LoadBMP(buffer))
        return false;

    int width = gSplash.GetWidth();
    int height = gSplash.GetHeight();

    m_hWnd = ::CreateWindowExA(
        WS_EX_LEFT,
        "SPLASH",
        "SPLASH",
        WS_POPUP | WS_BORDER,
        ::GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
        ::GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2,
        width,
        height,
        NULL,
        NULL,
        m_hInstance,
        NULL);
    ::UpdateWindow(m_hWnd);
    ::ShowWindow(m_hWnd, SW_SHOW);

    MSG msg;
    int counter = 0;
    while (::GetMessageA(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_QUIT)
        {
            break;
        }

        ::TranslateMessage(&msg);
        ::DispatchMessageA(&msg);
        ::InvalidateRect(m_hWnd, NULL, FALSE);

        if (counter++ == 500)
        {
            ::Sleep(100);
            ::DestroyWindow(m_hWnd);
            break;
        }
    }

    ::DeleteObject(hPalette);
    return true;
}

bool CSplash::LoadBMP(LPCSTR lpFileName)
{
    if (m_Data)
        delete[] m_Data;

    HANDLE hFile = ::CreateFileA(lpFileName,
                                 GENERIC_READ,
                                 1,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_FLAG_SEQUENTIAL_SCAN,
                                 NULL);
    if (hFile == (HANDLE)-1)
        return false;

    DWORD dwBytesRead, dwBytesToRead;
    char buffer[16];
    if (!::ReadFile(hFile, buffer, 14, &dwBytesRead, NULL) || dwBytesRead != 14 ||
        (buffer[0] != 'B' || buffer[1] != 'M'))
    {
        ::CloseHandle(hFile);
        return false;
    }

    dwBytesToRead = *(DWORD *)&buffer[2] - 14;
    m_Data = new BYTE[dwBytesToRead];
    if (!::ReadFile(hFile, m_Data, dwBytesToRead, &dwBytesRead, NULL) || dwBytesRead != dwBytesToRead)
    {
        ::CloseHandle(hFile);
        delete[] m_Data;
        return false;
    }

    ::CloseHandle(hFile);
    return true;
}

DWORD CSplash::GetWidth() const
{
    DWORD dwHeaderSize = *(DWORD *)&m_Data[0];
    return (dwHeaderSize == 12) ? *(WORD *)&m_Data[4] : *(DWORD *)&m_Data[4];
}

DWORD CSplash::GetHeight() const
{
    DWORD dwHeaderSize = *(DWORD *)&m_Data[0];
    return (dwHeaderSize == 12) ? *(WORD *)&m_Data[6] : *(DWORD *)&m_Data[8];
}

DWORD CSplash::GetBPP() const
{
    DWORD dwHeaderSize = *(DWORD *)&m_Data[0];
    return (dwHeaderSize == 12) ? *(WORD *)&m_Data[10] : *(DWORD *)&m_Data[14];
}

DWORD CSplash::GetDIBHeaderSize() const
{
    DWORD dwHeaderSize = *(DWORD *)&m_Data[0];
    if (dwHeaderSize == 40 && *(DWORD *)&m_Data[16] == 3)
        return 52;
    return dwHeaderSize;
}

DWORD CSplash::GetPaletteNumColors() const
{
    DWORD dwHeaderSize = *(DWORD *)&m_Data[0];
    return (dwHeaderSize == 12) ? 0 : *(DWORD *)&m_Data[32];
}

DWORD CSplash::GetPaletteNumEntries() const
{
    DWORD dwPaletteNumColors = GetPaletteNumColors();
    if (dwPaletteNumColors || GetBPP() >= 16)
        return dwPaletteNumColors;
    else
        return 1 << GetBPP();
}

DWORD CSplash::GetPaletteSize() const
{
    DWORD dwHeaderSize = *(DWORD *)&m_Data[0];
    return (dwHeaderSize == 12) ? 3 * GetPaletteNumEntries() : 4 * GetPaletteNumEntries();
}

BYTE *CSplash::GetPaletteData() const
{
    if (!GetPaletteNumEntries())
        return NULL;

    return &m_Data[GetDIBHeaderSize()];
}

BYTE *CSplash::GetPaletteEntry(DWORD index) const
{
    if (!GetPaletteNumEntries())
        return NULL;

    DWORD dwHeaderSize = *(DWORD *)&m_Data[0];
    return (dwHeaderSize == 12) ? &GetPaletteData()[3 * index] : &GetPaletteData()[4 * index];
}

BITMAPINFO *CSplash::GetBitmapInfo() const
{
    static BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = GetWidth();
    bmi.bmiHeader.biHeight = GetHeight();
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = (WORD)GetBPP();
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;
    return &bmi;
}

BYTE *CSplash::GetBitmapData() const
{
    return &m_Data[GetDIBHeaderSize() + GetPaletteSize()];
}

HPALETTE CSplash::GetPalette() const
{
    WORD palNumEntries = (WORD)GetPaletteNumEntries();
    if (palNumEntries == 0)
        return NULL;

    LPLOGPALETTE lpLogPalette = (LPLOGPALETTE)malloc(sizeof(LOGPALETTE) + palNumEntries * sizeof(PALETTEENTRY));
    if (!lpLogPalette)
        return NULL;
    lpLogPalette->palVersion = PALVERSION;
    lpLogPalette->palNumEntries = palNumEntries;

    BYTE *lpEntry = NULL;
    LPPALETTEENTRY lpPalEntry = &lpLogPalette->palPalEntry[0];
    for (int i = 0; i < palNumEntries; i++)
    {
        lpEntry = GetPaletteEntry(i);
        lpPalEntry->peRed = lpEntry[0];
        lpPalEntry->peGreen = lpEntry[1];
        lpPalEntry->peBlue = lpEntry[2];
        lpPalEntry->peFlags = PC_NONE;
        ++lpPalEntry;
    }

    HPALETTE hPal = ::CreatePalette(lpLogPalette);
    free(lpLogPalette);
    return hPal;
}