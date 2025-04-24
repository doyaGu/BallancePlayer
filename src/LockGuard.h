#ifndef PLAYER_LOCKGUARD_H
#define PLAYER_LOCKGUARD_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

class LockGuard
{
public:
    explicit LockGuard(HANDLE mutex) : m_Mutex(mutex) {}

    ~LockGuard()
    {
        Release();
    }

    void Release()
    {
        if (m_Mutex)
        {
            ::CloseHandle(m_Mutex);
            m_Mutex = NULL;
        }
    }

private:
    LockGuard(const LockGuard &);
    LockGuard &operator=(const LockGuard &);

    HANDLE m_Mutex;
};

#endif // PLAYER_LOCKGUARD_H