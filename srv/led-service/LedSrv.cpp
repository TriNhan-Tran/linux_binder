#include "srv/led-service/LedSrv.h"

#include "binder/core/TransactionCode.h"

#include <chrono>
#include <thread>


int LedSrv::requestLedOn() {
    LOG_INFO("LedSrv::requestLedOn() called by client");
    LOG_INFO("[LedSrv] >>> LED ON <<<");

    m_callbackRegistry.notifyCallbacks(EVENT_LED_ON, "LED is ON");

    std::thread([this]() {
        LOG_INFO("[LedSrv] LED will auto-off in 5 seconds...");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        LOG_INFO("[LedSrv] >>> LED OFF <<<");
        m_callbackRegistry.notifyCallbacks(EVENT_LED_OFF, "LED is OFF (auto-timeout)");
    }).detach();

    LOG_INFO("LedSrv::requestLedOn() completed status=0");
    return 0;
}

int LedSrv::registerCallback(uint32_t callbackHandle) {
    LOG_INFO("LedSrv::registerCallback() called by client handle=%u", callbackHandle);
    const int status = m_callbackRegistry.registerCallbackHandle(callbackHandle);
    if (status != 0) {
        LOG_ERROR("LedSrv::registerCallback() failed status=%d handle=%u", status, callbackHandle);
    } else {
        LOG_INFO("LedSrv::registerCallback() completed status=%d handle=%u", status, callbackHandle);
    }
    return status;
}

