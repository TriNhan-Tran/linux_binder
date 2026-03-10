#include "App.h"

#include "adapters/AudioSrvAdapter.h"
#include "adapters/LedSrvAdapter.h"

#include "BinderClient.h"
#include "BinderUtils.h"
#include "TransactionCode.h"

#include <any>
#include <chrono>
#include <stdexcept>
#include <thread>

namespace demo {

App::App() = default;

App::~App() = default;

void App::onInit() {
    LOG_INFO("=== App starting ===");
    LOG_INFO("[App][Stress] Test sequence started");

    ipc::ProcessState::self().open(ipc::BINDER_DRIVER_PATH);
    if (!ipc::ProcessState::self().isOpen()) {
        throw std::runtime_error("App: failed to open binder driver");
    }

    ipc::ServiceManager srvManager;

    m_ledHandle = lookupSrvWithRetry(
        srvManager,
        ipc::LED_SRV_NAME,
        LOOKUP_RETRY_COUNT);
    if (m_ledHandle == 0) {
        throw std::runtime_error("App: failed to find LED service");
    }

    m_audioHandle = lookupSrvWithRetry(
        srvManager,
        ipc::AUDIO_SRV_NAME,
        LOOKUP_RETRY_COUNT);
    if (m_audioHandle == 0) {
        releaseHandleIfValid(m_ledHandle, ipc::LED_SRV_NAME);
        m_ledHandle = 0;
        throw std::runtime_error("App: failed to find audio service");
    }

    auto& ledAdapter = LedSrvAdapter::getInstance();
    auto& audioAdapter = AudioSrvAdapter::getInstance();

    ledAdapter.attachApp(this);
    audioAdapter.attachApp(this);

    ledAdapter.initialize(m_ledHandle);
    audioAdapter.initialize(m_audioHandle);
}

void App::onStart() {
    if (!m_binderThreadPoolStarted) {
        startBinderThreadPool();
        m_binderThreadPoolStarted = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto& ledAdapter = LedSrvAdapter::getInstance();
    auto& audioAdapter = AudioSrvAdapter::getInstance();

    LOG_INFO("[App] Registering callback with LedSrv...");
    if (ledAdapter.registerCallback() != 0) {
        throw std::runtime_error("[App] Failed to register LED callback");
    }

    LOG_INFO("[App] Registering callback with AudioSrv...");
    if (audioAdapter.registerCallback() != 0) {
        throw std::runtime_error("[App] Failed to register audio callback");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void App::onRun() {
    auto& ledAdapter = LedSrvAdapter::getInstance();
    auto& audioAdapter = AudioSrvAdapter::getInstance();

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
}

void App::onStop() {
    releaseHandleIfValid(m_audioHandle, ipc::AUDIO_SRV_NAME);
    releaseHandleIfValid(m_ledHandle, ipc::LED_SRV_NAME);

    m_audioHandle = 0;
    m_ledHandle = 0;

    auto& audioAdapter = AudioSrvAdapter::getInstance();
    auto& ledAdapter = LedSrvAdapter::getInstance();
    audioAdapter.shutdown();
    ledAdapter.shutdown();

    ipc::ProcessState::self().close();

    if (m_binderThread.joinable()) {
        m_binderThread.join();
    }
    m_binderThreadPoolStarted = false;
}

void App::handleMessage(const Message& message) {
    LOG_INFO("[App][Queue] message processed what=%d", message.what);

    switch (message.what) {
    case MSG_LED_EVENT: {
        const auto* payload = std::any_cast<LedEvent>(&message.obj);
        if (payload == nullptr) {
            LOG_ERROR("[App] MSG_LED_EVENT payload type mismatch");
            return;
        }
        handleLedEvent(payload->eventType, payload->message);
        return;
    }
    case MSG_AUDIO_EVENT: {
        const auto* payload = std::any_cast<AudioEvent>(&message.obj);
        if (payload == nullptr) {
            LOG_ERROR("[App] MSG_AUDIO_EVENT payload type mismatch");
            return;
        }
        handleAudioEvent(payload->eventType, payload->message);
        return;
    }
    default:
        LOG_WARN("[App] Unknown message id what=%d", message.what);
        return;
    }
}

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

void App::startBinderThreadPool() {
    m_binderThread = std::thread([]() {
        LOG_INFO("[App] Binder thread pool started");
        ipc::IPCThreadState::joinThreadPool(nullptr);
    });
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

} // namespace demo
