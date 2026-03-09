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
    }
    LOGI("[App] CALLBACK >>> %s (%d): \"%s\"", eventName, eventType, message.c_str());
}

void App::handleAudioEvent(int32_t eventType, const std::string& message) {
    const char* eventName = "UNKNOWN";
    switch (eventType) {
    case ipc::EVENT_AUDIO_PLAYING: eventName = "AUDIO_PLAYING"; break;
    case ipc::EVENT_AUDIO_STOPPED: eventName = "AUDIO_STOPPED"; break;
    default: eventName = "AUDIO_UNKNOWN"; break;
    }
    LOGI("[App] CALLBACK >>> %s (%d): \"%s\"", eventName, eventType, message.c_str());
}

void App::releaseHandleIfValid(uint32_t handle, const char* label) {
    if (handle == 0) {
        return;
    }

    if (ipc::IPCThreadState::releaseHandle(handle) != 0) {
        LOGE("[App] Failed to release %s handle=%u", label, handle);
    } else {
        LOGI("[App] Released %s handle=%u", label, handle);
    }
}

void App::startCallbackListener() {
    std::thread([]() {
        LOGI("[App] Callback listener thread started");
        ipc::IPCThreadState::joinThreadPool(nullptr);
    }).detach();
}

uint32_t App::lookupSrvWithRetry(
    ipc::ServiceManager& srvManager,
    const char* srvName,
    int maxRetries) {
    LOGI("[App] Looking up %s...", srvName);
    for (int retry = 0; retry < maxRetries; ++retry) {
        const uint32_t handle = srvManager.getSrv(srvName);
        if (handle != 0) {
            LOGI("[App] Found %s -> handle %u (DIRECT)", srvName, handle);
            return handle;
        }

        LOGI("[App] %s not ready, retrying...", srvName);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOGE("[App] Failed to find %s", srvName);
    return 0;
}

int App::run() {
    LOGI("=== App starting ===");

    ipc::ProcessState::self().open(ipc::BINDER_DRIVER_PATH);
    if (!ipc::ProcessState::self().isOpen()) {
        LOGE("App: failed to open binder driver");
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

    LOGI("[App] Registering callback with LedSrv...");
    if (ledAdapter.registerCallback() != 0) {
        LOGE("[App] Failed to register LED callback");
        audioAdapter.shutdown();
        ledAdapter.shutdown();
        releaseHandleIfValid(audioHandle, ipc::AUDIO_SRV_NAME);
        releaseHandleIfValid(ledHandle, ipc::LED_SRV_NAME);
        return 1;
    }

    LOGI("[App] Registering callback with AudioSrv...");
    if (audioAdapter.registerCallback() != 0) {
        LOGE("[App] Failed to register audio callback");
        audioAdapter.shutdown();
        ledAdapter.shutdown();
        releaseHandleIfValid(audioHandle, ipc::AUDIO_SRV_NAME);
        releaseHandleIfValid(ledHandle, ipc::LED_SRV_NAME);
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    LOGI("[App] ========================================");
    LOGI("[App] Calling LedSrv::requestLedOn()");
    LOGI("[App] ========================================");
    int ret = ledAdapter.requestLedOn();
    LOGI("[App] requestLedOn() returned: %d", ret);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    LOGI("[App] ========================================");
    LOGI("[App] Calling AudioSrv::playAudio()");
    LOGI("[App] ========================================");
    ret = audioAdapter.playAudio();
    LOGI("[App] playAudio() returned: %d", ret);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    LOGI("[App] ========================================");
    LOGI("[App] Calling AudioSrv::stopAudio()");
    LOGI("[App] ========================================");
    ret = audioAdapter.stopAudio();
    LOGI("[App] stopAudio() returned: %d", ret);

    LOGI("[App] Waiting for LED auto-off notification...");
    std::this_thread::sleep_for(std::chrono::seconds(5));

    LOGI("[App] ========================================");
    LOGI("[App] All tests completed. Exiting.");
    LOGI("[App] ========================================");

    audioAdapter.shutdown();
    ledAdapter.shutdown();

    releaseHandleIfValid(audioHandle, ipc::AUDIO_SRV_NAME);
    releaseHandleIfValid(ledHandle, ipc::LED_SRV_NAME);
    return 0;
}

} // namespace demo
