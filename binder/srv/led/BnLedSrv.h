#pragma once

#include "binder/core/BinderClient.h"

#include <cstdint>


/**
 * @brief Bn stub: server-side binder dispatcher for LED transactions.
 *
 * Real LED behavior is implemented in `LedSrv`.
 */
class BnLedSrv : public BBinder {
public:
    ~BnLedSrv() override = default;

    virtual int requestLedOn() = 0;
    virtual int registerCallback(uint32_t callbackHandle) = 0;

    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

