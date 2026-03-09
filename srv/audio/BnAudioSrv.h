#pragma once

#include "BinderClient.h"

#include <cstdint>

namespace ipc {

/**
 * @brief Bn stub: server-side binder dispatcher for audio transactions.
 *
 * Real audio behavior is implemented in `demo::AudioSrv`.
 */
class BnAudioSrv : public BBinder {
public:
    virtual int playAudio() = 0;
    virtual int stopAudio() = 0;
    virtual int registerCallback(uint32_t callbackHandle) = 0;

    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

} // namespace ipc
