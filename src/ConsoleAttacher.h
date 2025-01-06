#ifndef BP_CONSOLEATTACHER_H
#define BP_CONSOLEATTACHER_H

#include <cstdio>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

class ConsoleAttacher {
public:
    ConsoleAttacher() {
        if (::AttachConsole(ATTACH_PARENT_PROCESS)) {
            freopen("CONOUT$", "w", stdout);
            m_Attached = true;
        } else {
            m_Attached = false;
        }
    }

    ConsoleAttacher(const ConsoleAttacher &) = delete;
    ConsoleAttacher(ConsoleAttacher &&) = delete;

    ~ConsoleAttacher() { Release(); }

    ConsoleAttacher &operator=(const ConsoleAttacher &) = delete;
    ConsoleAttacher &operator=(ConsoleAttacher &&) = delete;

    void Release() {
        if (m_Attached) {
            ::FreeConsole();
            m_Attached = false;
        }
    }

private:
    bool m_Attached;
};

#endif // BP_CONSOLEATTACHER_H
