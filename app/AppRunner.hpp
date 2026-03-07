#pragma once

#include "SrvManagerIpc.hpp"

#include <cstdint>

namespace demo {

/**
 * @brief Coordinates the app client and callback test flow.
 */
class AppRunner {
public:
    int run();

private:
    static constexpr int LOOKUP_RETRY_COUNT = 10;

    static void startCallbackListener();
    static uint32_t lookupSrvWithRetry(ipc::BpSrvManager& srvManager,
                                       const char* srvName,
                                       int maxRetries);
    static void releaseHandleIfValid(uint32_t handle, const char* label);
};

} // namespace demo
