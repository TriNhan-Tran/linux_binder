#pragma once

#include "base/Handler.h"
#include "ServiceManager.h"

#include <cstdint>
#include <string>
#include <thread>

namespace demo {

class AudioSrvAdapter;
class LedSrvAdapter;

/**
 * @brief Coordinates app-side client and callback test flow.
 */
class App : public Handler {
public:
    static constexpr int32_t MSG_LED_EVENT = 1;
    static constexpr int32_t MSG_AUDIO_EVENT = 2;

    struct LedEvent {
        int32_t eventType = 0;
        std::string message;
    };

    struct AudioEvent {
        int32_t eventType = 0;
        std::string message;
    };

    App();
    ~App();

protected:
    void onInit() override;
    void onStart() override;
    void onRun() override;
    void onStop() override;

    void handleMessage(const Message& message) override;

private:
    static constexpr int LOOKUP_RETRY_COUNT = 10;

    void startBinderThreadPool();
    static uint32_t lookupSrvWithRetry(ipc::ServiceManager& srvManager,
                                       const char* srvName,
                                       int maxRetries);
    static void releaseHandleIfValid(uint32_t handle, const char* label);

    void handleLedEvent(int32_t eventType, const std::string& message);
    void handleAudioEvent(int32_t eventType, const std::string& message);

    uint32_t m_ledHandle = 0;
    uint32_t m_audioHandle = 0;
    bool m_binderThreadPoolStarted = false;
    std::thread m_binderThread;
};

} // namespace demo
