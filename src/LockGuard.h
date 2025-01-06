#ifndef BP_LOCKGUARD_H
#define BP_LOCKGUARD_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

class LockGuard {
public:
    explicit LockGuard(HANDLE mutex) : m_Mutex(mutex) {}

    LockGuard(const LockGuard &) = delete;
    LockGuard(LockGuard &&) = delete;

    ~LockGuard() { Release(); }

    LockGuard &operator=(const LockGuard &) = delete;
    LockGuard &operator=(LockGuard &&) = delete;

    void Release() {
        ::CloseHandle(m_Mutex);
        m_Mutex = nullptr;
    }

private:
    HANDLE m_Mutex;
};

#endif // BP_LOCKGUARD_H
