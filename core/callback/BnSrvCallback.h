#pragma once

#include "ISrvCallback.h"

#include "BinderClient.h"

namespace ipc {

/**
 * @brief Bn stub: server-side binder dispatcher for callback transactions.
 *
 * Real callback behavior is implemented by app-side callback classes,
 * for example `demo::App::Callback`.
 */
class BnSrvCallback : public BBinder, public ISrvCallback {
public:
    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

} // namespace ipc
