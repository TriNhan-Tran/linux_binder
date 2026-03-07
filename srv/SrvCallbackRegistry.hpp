#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace ipc {

/**
 * @brief Shared callback-handle registry used by server implementations.
 *
 * This helper encapsulates callback handle ownership:
 * registration takes ownership of acquired handles, and destruction releases
 * all retained handles through `IPCThreadState::releaseHandle`.
 */
class SrvCallbackRegistry {
public:
    /**
     * @brief Construct callback registry.
     *
     * @param srvTag Prefix used in server log messages.
     */
    explicit SrvCallbackRegistry(const char* srvTag);

    ~SrvCallbackRegistry();

    /**
     * @brief Register callback handle if not already present.
     *
     * If the handle is already registered, the extra acquired reference is
     * released immediately.
     *
     * @param callbackHandle Kernel callback handle.
     * @return 0 on success, non-zero on failure.
     */
    int registerCallbackHandle(uint32_t callbackHandle);

    /**
     * @brief Notify all registered callback handles.
     *
     * @param eventType Event code.
     * @param message Event message payload.
     */
    void notifyCallbacks(int32_t eventType, const std::string& message) const;

private:
    const char* m_srvTag;
    mutable std::mutex m_mutex;
    std::vector<uint32_t> m_callbacks;
};

} // namespace ipc
