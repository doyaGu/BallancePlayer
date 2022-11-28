#ifndef PLAYER_WINCONTEXT_H
#define PLAYER_WINCONTEXT_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

typedef LRESULT(CALLBACK *LPFNWNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef enum WINDOW_STYLE_FLAGS
{
    WINDOW_STYLE_USECURRENTSETTINGS = 0x00000000, // Do not change any setting
    WINDOW_STYLE_FULLSCREEN         = 0x00000001, // Full Screen
    WINDOW_STYLE_BORDERLESS         = 0x00000002, // Borderless
    WINDOW_STYLE_RESIZABLE          = 0x00000004, // Resizable
} WINDOW_STYLE_FLAGS;

class CWinContext
{
public:
    CWinContext();
    ~CWinContext() {}

    bool Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc);

    void UpdateWindows();

    void DestroyWindows();

    void ShowMainWindow(bool show = true);
    void ShowRenderWindow(bool show = true);

    void FocusMainWindow();
    void FocusRenderWindow();

    bool PostMessageToMainWindow(UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool PostMessageToRenderWindow(UINT uMsg, WPARAM wParam, LPARAM lParam);

    HINSTANCE GetAppInstance() const
    {
        return m_hInstance;
    }

    HACCEL GetAccelTable() const
    {
        return m_hAccelTable;
    }

    HWND GetMainWindow() const
    {
        return m_MainWindow;
    }

    HWND GetRenderWindow() const
    {
        return m_RenderWindow;
    }

    void GetPosition(int &x, int &y);
    void SetPosition(int x, int y);

    void GetMainSize(int &width, int &height);
    void GetRenderSize(int &width, int &height);

    void SetMainSize(int width, int height);
    void SetRenderSize(int width, int height);

    void SetMainStyle(int add, int remove);

    void SetWindowText(LPCSTR str = NULL);

    int TranslateAccelerators(MSG *msg);

    bool IsIconic();

private:
    CWinContext(const CWinContext &);
    CWinContext &operator=(const CWinContext &);

    void LoadWindowNames();
    bool RegisterWindowClasses(LPFNWNDPROC lpfnWndProc);
    bool CreateWindows();

    HWND m_MainWindow;
    HWND m_RenderWindow;
    HINSTANCE m_hInstance;
    HACCEL m_hAccelTable;
    WNDCLASSEX m_MainWndClass;
    WNDCLASS m_RenderWndClass;
    TCHAR m_MainWndName[128];
    TCHAR m_RenderWndName[128];
    TCHAR m_MainWndClassName[128];
    TCHAR m_RenderWndClassName[128];
    DWORD m_MainWndStyle;
    DWORD m_RenderWndStyle;
    int m_Width;
    int m_Height;
    int m_StyleFlags;
};

#endif /* PLAYER_WINCONTEXT_H */