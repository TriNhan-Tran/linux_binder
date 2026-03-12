#pragma once

#include "binder/callback/interfaces/ISrvCallback.h"

#include "binder/core/BinderClient.h"


/**
 * @brief Bp proxy: client-side binder proxy for callback events.
 *
 * This class marshals `onEvent` into one-way binder transactions.
 */
class BpSrvCallback : public ISrvCallback {
public:
    explicit BpSrvCallback(uint32_t handle);

    void onEvent(int32_t eventType, const std::string& message) override;

private:
    BpBinder m_remote;
};

