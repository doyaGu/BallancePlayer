#ifndef PLAYER_SPLASH_H
#define PLAYER_SPLASH_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

class CSplash
{
public:
    CSplash();
    explicit CSplash(HINSTANCE hInstance);
    ~CSplash();

    bool Show();
    bool LoadBMP(LPCSTR lpFileName);
    DWORD GetWidth() const;
    DWORD GetHeight() const;
    DWORD GetBPP() const;
    DWORD GetDIBHeaderSize() const;
    DWORD GetPaletteNumColors() const;
    DWORD GetPaletteNumEntries() const;
    DWORD GetPaletteSize() const;
    BYTE *GetPaletteData() const;
    BYTE *GetPaletteEntry(DWORD index) const;
    BITMAPINFO *GetBitmapInfo() const;
    BYTE *GetBitmapData() const;
    HPALETTE GetPalette() const;

private:
    CSplash(const CSplash &);
    CSplash &operator=(const CSplash &);

    BYTE *m_Data;
    HWND m_hWnd;
    HINSTANCE m_hInstance;
};

#endif /* PLAYER_SPLASH_H */