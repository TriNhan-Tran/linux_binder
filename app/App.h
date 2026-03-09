#pragma once

#include "ServiceManager.h"

#include <cstdint>
#include <string>

namespace demo {

class AudioSrvAdapter;
class LedSrvAdapter;

/**
 * @brief Coordinates app-side client and callback test flow.
 */
class App {
public:
    App();
    ~App();

    int run();

    void handleLedEvent(int32_t eventType, const std::string& message);
    void handleAudioEvent(int32_t eventType, const std::string& message);

private:
    static constexpr int LOOKUP_RETRY_COUNT = 10;

    static void startCallbackListener();
    static uint32_t lookupSrvWithRetry(ipc::ServiceManager& srvManager,
                                       const char* srvName,
                                       int maxRetries);
    static void releaseHandleIfValid(uint32_t handle, const char* label);
};

} // namespace demo
