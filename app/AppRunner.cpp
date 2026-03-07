#include "AppRunner.hpp"

#include "AppCallback.hpp"
#include "AudioSrvIpc.hpp"
#include "BinderCore.hpp"
#include "LedSrvIpc.hpp"
#include "SrvManagerIpc.hpp"
#include "TxnCodes.hpp"

#include <chrono>
#include <thread>

namespace demo {

void AppRunner::releaseHandleIfValid(uint32_t handle, const char* label) {
    if (handle == 0) {
        return;
    }

    if (ipc::IPCThreadState::releaseHandle(handle) != 0) {
        LOGE("[App] Failed to release %s handle=%u", label, handle);
    } else {
        LOGI("[App] Released %s handle=%u", label, handle);
    }
}

void AppRunner::startCallbackListener() {
    std::thread([]() {
        LOGI("[App] Callback listener thread started");
        ipc::IPCThreadState::joinThreadPool(nullptr);
    }).detach();
}

uint32_t AppRunner::lookupSrvWithRetry(
    ipc::BpSrvManager& srvManager,
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

int AppRunner::run() {
    LOGI("=== App starting ===");

    ipc::ProcessState::self().open("/dev/binderfs/binder");
    if (!ipc::ProcessState::self().isOpen()) {
        LOGE("App: failed to open binder driver");
        return 1;
    }

    startCallbackListener();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    AppCallback appCallback;
    ipc::BpSrvManager srvManager;

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

    ipc::BpLedSrv ledProxy(ledHandle);
    ipc::BpAudioSrv audioProxy(audioHandle);

    LOGI("[App] Registering callback with LedSrv...");
    ledProxy.registerCallback(&appCallback);

    LOGI("[App] Registering callback with AudioSrv...");
    audioProxy.registerCallback(&appCallback);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    LOGI("[App] ========================================");
    LOGI("[App] Calling LedSrv::requestLedOn()");
    LOGI("[App] ========================================");
    int ret = ledProxy.requestLedOn();
    LOGI("[App] requestLedOn() returned: %d", ret);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    LOGI("[App] ========================================");
    LOGI("[App] Calling AudioSrv::playAudio()");
    LOGI("[App] ========================================");
    ret = audioProxy.playAudio();
    LOGI("[App] playAudio() returned: %d", ret);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    LOGI("[App] ========================================");
    LOGI("[App] Calling AudioSrv::stopAudio()");
    LOGI("[App] ========================================");
    ret = audioProxy.stopAudio();
    LOGI("[App] stopAudio() returned: %d", ret);

    LOGI("[App] Waiting for LED auto-off notification...");
    std::this_thread::sleep_for(std::chrono::seconds(5));

    LOGI("[App] ========================================");
    LOGI("[App] All tests completed. Exiting.");
    LOGI("[App] ========================================");

    releaseHandleIfValid(audioHandle, ipc::AUDIO_SRV_NAME);
    releaseHandleIfValid(ledHandle, ipc::LED_SRV_NAME);
    return 0;
}

} // namespace demo
