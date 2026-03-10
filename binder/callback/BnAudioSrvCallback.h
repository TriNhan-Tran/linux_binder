#pragma once

#include "IAudioSrvCallback.h"

#include "BinderClient.h"

namespace ipc {

/**
 * @brief Bn stub for audio callback transactions.
 */
class BnAudioSrvCallback : public BBinder, public IAudioSrvCallback {
public:
    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

} // namespace ipc
