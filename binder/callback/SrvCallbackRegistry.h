#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace ipc {

enum class SrvCallbackType {
    Led,
    Audio,
};

/**
 * @brief Shared callback-handle registry used by service implementations.
 */
class SrvCallbackRegistry {
public:
    SrvCallbackRegistry(const char* srvTag, SrvCallbackType callbackType);

    ~SrvCallbackRegistry();

    int registerCallbackHandle(uint32_t callbackHandle);
    void notifyCallbacks(int32_t eventType, const std::string& message) const;

private:
    const char* m_srvTag;
    SrvCallbackType m_callbackType;
    mutable std::mutex m_mutex;
    std::vector<uint32_t> m_callbacks;
};

} // namespace ipc
