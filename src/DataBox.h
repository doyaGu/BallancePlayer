/**
 * @file DataBox.h
 * @brief The class implements data storage box.
 */
#ifndef BP_DATABOX_H
#define BP_DATABOX_H

#include <mutex>
#include <vector>

/**
 * @brief A container for storing user data.
 */
class DataBox {
public:
    /**
     * @brief Retrieves the data associated with the specified type.
     * @param type The type of the data to retrieve.
     * @return A pointer to the data of the specified type, or nullptr if not found.
     */
    void *GetData(size_t type) const {
        std::lock_guard<std::mutex> guard(m_RWLock);

        const auto n = m_UserData.size();
        for (size_t i = 0; i < n; i += 2) {
            if (m_UserData[i] == type)
                return reinterpret_cast<void *>(m_UserData[i + 1]);
        }
        return nullptr;
    }

    /**
     * @brief Sets the data of the specified type.
     * @param data A pointer to the data to set.
     * @param type The type of the data to set.
     * @return A pointer to the previous data of the specified type, or nullptr if not found.
     */
    void *SetData(void *data, size_t type) {
        std::lock_guard<std::mutex> guard(m_RWLock);

        const auto n = m_UserData.size();
        for (size_t i = 0; i < n; i += 2) {
            if (m_UserData[i] == type) {
                void *oldData = reinterpret_cast<void *>(m_UserData[i + 1]);
                m_UserData[i + 1] = reinterpret_cast<size_t>(data);
                return oldData;
            }
        }

        m_UserData.push_back(type);
        m_UserData.push_back(reinterpret_cast<size_t>(data));
        return nullptr;
    }

private:
    mutable std::mutex m_RWLock; /**< A mutex for thread-safe access to the user data. */
    std::vector<size_t> m_UserData; /**< The storage for user data. */
};

#endif // BP_DATABOX_H
