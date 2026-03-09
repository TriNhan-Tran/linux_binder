#include "LedSrv.h"

#include "TransactionCode.h"

#include <chrono>
#include <thread>

namespace demo {

int LedSrv::requestLedOn() {
    LOGI("[LedSrv] requestLedOn() called");
    LOGI("[LedSrv] >>> LED ON <<<");

    m_callbackRegistry.notifyCallbacks(ipc::EVENT_LED_ON, "LED is ON");

    std::thread([this]() {
        LOGI("[LedSrv] LED will auto-off in 5 seconds...");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        LOGI("[LedSrv] >>> LED OFF <<<");
        m_callbackRegistry.notifyCallbacks(ipc::EVENT_LED_OFF, "LED is OFF (auto-timeout)");
    }).detach();

    return 0;
}

int LedSrv::registerCallback(uint32_t callbackHandle) {
    return m_callbackRegistry.registerCallbackHandle(callbackHandle);
}

} // namespace demo
