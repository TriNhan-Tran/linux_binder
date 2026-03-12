#pragma once

#include "binder/callback/interfaces/ILedSrvCallback.h"

#include "binder/core/BinderClient.h"


/**
 * @brief Bp proxy for LED callback notifications.
 */
class BpLedSrvCallback : public ILedSrvCallback {
public:
    explicit BpLedSrvCallback(uint32_t handle);

    void onLedEvent(int32_t eventType, const std::string& message) override;

private:
    BpBinder m_remote;
};

