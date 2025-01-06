/**
 * @file RefCount.h
 * @brief The class implements reference counting
 */
#ifndef BP_REFCOUNT_H
#define BP_REFCOUNT_H

#include <atomic>

/**
 * @class RefCount
 * @brief The class implements reference counting
 */
class RefCount {
public:
    RefCount() : m_RefCount(1) {} /**< Default Constructor */
    explicit RefCount(int value) : m_RefCount(value) {} /**< Constructor */
    ~RefCount() = default; /**< Destructor */

    RefCount &operator++() { AddRef(); return *this; } /**< Operator++ override */
    RefCount &operator--() { Release(); return *this; } /**< Operator-- override */

    /**
     * @brief Increment reference count.
     * @return Current reference count.
     */
    int AddRef() { return m_RefCount.fetch_add(1, std::memory_order_relaxed); }

    /**
     * @brief Decrement reference count.
     * @return Current reference count.
     */
    int Release() { return m_RefCount.fetch_sub(1, std::memory_order_release); }

    /**
     * @brief Get reference count.
     * @return Current reference count.
     */
    int GetCount() { return m_RefCount; }

private:
    std::atomic<int> m_RefCount; /**< Reference count. */
};

#endif // BP_REFCOUNT_H
