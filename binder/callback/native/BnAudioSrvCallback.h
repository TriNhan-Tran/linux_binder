#pragma once

#include "binder/callback/interfaces/IAudioSrvCallback.h"

#include "binder/core/BinderClient.h"

/**
 * @brief Bn stub for audio callback transactions.
 */
class BnAudioSrvCallback : public BBinder, public IAudioSrvCallback {
public:
    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

