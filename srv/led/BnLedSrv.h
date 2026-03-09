#pragma once

#include "BinderClient.h"

#include <cstdint>

namespace ipc {

/**
 * @brief Bn stub: server-side binder dispatcher for LED transactions.
 *
 * Real LED behavior is implemented in `demo::LedSrv`.
 */
class BnLedSrv : public BBinder {
public:
    virtual int requestLedOn() = 0;
    virtual int registerCallback(uint32_t callbackHandle) = 0;

    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

} // namespace ipc
