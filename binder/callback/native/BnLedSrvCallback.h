#pragma once

#include "binder/callback/interfaces/ILedSrvCallback.h"

#include "binder/core/BinderClient.h"


/**
 * @brief Bn stub for LED callback transactions.
 */
class BnLedSrvCallback : public BBinder, public ILedSrvCallback {
public:
    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};
