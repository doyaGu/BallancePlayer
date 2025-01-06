#ifndef BP_SPLASH_H
#define BP_SPLASH_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

class Splash {
public:
    Splash();
    explicit Splash(HINSTANCE hInstance);

    Splash(const Splash &) = delete;
    Splash(Splash &&) = delete;

    ~Splash();

    Splash &operator=(const Splash &) = delete;
    Splash &operator=(Splash &&) = delete;

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
    BYTE *m_Data;
    HWND m_hWnd;
    HINSTANCE m_hInstance;
};

#endif /* BP_SPLASH_H */
