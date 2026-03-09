#include "App.h"

#include "adapters/AudioSrvAdapter.h"
#include "adapters/LedSrvAdapter.h"

#include "BinderClient.h"
#include "BinderUtils.h"
#include "TransactionCode.h"

#include <chrono>
#include <thread>

namespace demo {

App::App() = default;

App::~App() = default;

void App::handleLedEvent(int32_t eventType, const std::string& message) {
    const char* eventName = "LED_UNKNOWN";
    switch (eventType) {
    case ipc::EVENT_LED_ON: eventName = "LED_ON"; break;
    case ipc::EVENT_LED_OFF: eventName = "LED_OFF"; break;
    default:
        LOG_WARN("[App] handleLedEvent unexpected eventType=%d message='%s'",
                 eventType,
                 message.c_str());
        break;
    }
    LOG_INFO("[App] CALLBACK >>> %s (%d): \"%s\"", eventName, eventType, message.c_str());
}

void App::handleAudioEvent(int32_t eventType, const std::string& message) {
    const char* eventName = "UNKNOWN";
    switch (eventType) {
    case ipc::EVENT_AUDIO_PLAYING: eventName = "AUDIO_PLAYING"; break;
    case ipc::EVENT_AUDIO_STOPPED: eventName = "AUDIO_STOPPED"; break;
    default:
        eventName = "AUDIO_UNKNOWN";
        LOG_WARN("[App] handleAudioEvent unexpected eventType=%d message='%s'",
                 eventType,
                 message.c_str());
        break;
    }
    LOG_INFO("[App] CALLBACK >>> %s (%d): \"%s\"", eventName, eventType, message.c_str());
}

void App::releaseHandleIfValid(uint32_t handle, const char* label) {
    if (handle == 0) {
        return;
    }

    if (ipc::IPCThreadState::releaseHandle(handle) != 0) {
        LOG_ERROR("[App] Failed to release %s handle=%u", label, handle);
    } else {
        LOG_INFO("[App] Released %s handle=%u", label, handle);
    }
}

void App::startCallbackListener() {
    std::thread([]() {
        LOG_INFO("[App] Callback listener thread started");
        ipc::IPCThreadState::joinThreadPool(nullptr);
    }).detach();
}

uint32_t App::lookupSrvWithRetry(
    ipc::ServiceManager& srvManager,
    const char* srvName,
    int maxRetries) {
    LOG_INFO("[App] Looking up %s...", srvName);
    for (int retry = 0; retry < maxRetries; ++retry) {
        LOG_INFO("[App][Stress] lookup iteration %d/%d for %s", retry + 1, maxRetries, srvName);
        const uint32_t handle = srvManager.getSrv(srvName);
        if (handle != 0) {
            LOG_INFO("[App] Found %s -> handle %u (DIRECT)", srvName, handle);
            return handle;
        }

        LOG_INFO("[App] %s not ready, retrying...", srvName);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOG_ERROR("[App] Failed to find %s", srvName);
    return 0;
}

int App::run() {
    LOG_INFO("=== App starting ===");
    LOG_INFO("[App][Stress] Test sequence started");

    ipc::ProcessState::self().open(ipc::BINDER_DRIVER_PATH);
    if (!ipc::ProcessState::self().isOpen()) {
        LOG_ERROR("App: failed to open binder driver");
        return 1;
    }

    startCallbackListener();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ipc::ServiceManager srvManager;

    const uint32_t ledHandle = lookupSrvWithRetry(
        srvManager,
        ipc::LED_SRV_NAME,
        LOOKUP_RETRY_COUNT);
    if (ledHandle == 0) {
        return 1;
    }

    const uint32_t audioHandle = lookupSrvWithRetry(
        srvManager,
        ipc::AUDIO_SRV_NAME,
        LOOKUP_RETRY_COUNT);
    if (audioHandle == 0) {
        releaseHandleIfValid(ledHandle, ipc::LED_SRV_NAME);
        return 1;
    }

    auto& ledAdapter = LedSrvAdapter::getInstance();
    auto& audioAdapter = AudioSrvAdapter::getInstance();

    ledAdapter.attachApp(this);
    audioAdapter.attachApp(this);

    ledAdapter.initialize(ledHandle);
    audioAdapter.initialize(audioHandle);

    LOG_INFO("[App] Registering callback with LedSrv...");
    if (ledAdapter.registerCallback() != 0) {
        LOG_ERROR("[App] Failed to register LED callback");
        audioAdapter.shutdown();
        ledAdapter.shutdown();
        releaseHandleIfValid(audioHandle, ipc::AUDIO_SRV_NAME);
        releaseHandleIfValid(ledHandle, ipc::LED_SRV_NAME);
        return 1;
    }

    LOG_INFO("[App] Registering callback with AudioSrv...");
    if (audioAdapter.registerCallback() != 0) {
        LOG_ERROR("[App] Failed to register audio callback");
        audioAdapter.shutdown();
        ledAdapter.shutdown();
        releaseHandleIfValid(audioHandle, ipc::AUDIO_SRV_NAME);
        releaseHandleIfValid(ledHandle, ipc::LED_SRV_NAME);
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    LOG_INFO("[App] ========================================");
    LOG_INFO("[App] Calling LedSrv::requestLedOn()");
    LOG_INFO("[App] ========================================");
    int ret = ledAdapter.requestLedOn();
    LOG_INFO("[App] requestLedOn() returned: %d", ret);
    if (ret != 0) {
        LOG_WARN("[App] requestLedOn() returned non-zero status=%d", ret);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    LOG_INFO("[App] ========================================");
    LOG_INFO("[App] Calling AudioSrv::playAudio()");
    LOG_INFO("[App] ========================================");
    ret = audioAdapter.playAudio();
    LOG_INFO("[App] playAudio() returned: %d", ret);
    if (ret != 0) {
        LOG_WARN("[App] playAudio() returned non-zero status=%d", ret);
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    LOG_INFO("[App] ========================================");
    LOG_INFO("[App] Calling AudioSrv::stopAudio()");
    LOG_INFO("[App] ========================================");
    ret = audioAdapter.stopAudio();
    LOG_INFO("[App] stopAudio() returned: %d", ret);
    if (ret != 0) {
        LOG_WARN("[App] stopAudio() returned non-zero status=%d", ret);
    }

    LOG_INFO("[App] Waiting for LED auto-off notification...");
    std::this_thread::sleep_for(std::chrono::seconds(5));

    LOG_INFO("[App] ========================================");
    LOG_INFO("[App] All tests completed. Exiting.");
    LOG_INFO("[App] ========================================");
    LOG_INFO("[App][Stress] Test sequence completed");

    audioAdapter.shutdown();
    ledAdapter.shutdown();

    releaseHandleIfValid(audioHandle, ipc::AUDIO_SRV_NAME);
    releaseHandleIfValid(ledHandle, ipc::LED_SRV_NAME);
    return 0;
}

} // namespace demo
