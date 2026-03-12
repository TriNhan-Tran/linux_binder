#pragma once

#include "binder/core/BinderClient.h"

#include <cstdint>


/**
 * @brief Bn stub: server-side binder dispatcher for audio transactions.
 *
 * Real audio behavior is implemented in `AudioSrv`.
 */
class BnAudioSrv : public BBinder {
public:
    ~BnAudioSrv() override = default;

    virtual int playAudio() = 0;
    virtual int stopAudio() = 0;
    virtual int registerCallback(uint32_t callbackHandle) = 0;

    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

