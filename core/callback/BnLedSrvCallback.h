#pragma once

#include "ILedSrvCallback.h"

#include "BinderClient.h"

namespace ipc {

/**
 * @brief Bn stub for LED callback transactions.
 */
class BnLedSrvCallback : public BBinder, public ILedSrvCallback {
public:
    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

} // namespace ipc
