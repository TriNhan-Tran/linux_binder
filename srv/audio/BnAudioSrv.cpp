#include "BnAudioSrv.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

int BnAudioSrv::onTransact(uint32_t code, const Parcel& data, Parcel* reply) {
    Parcel in(data.data(), data.dataSize());

    switch (code) {
    case AUDIO_SRV_PLAY: {
        const int status = playAudio();
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    case AUDIO_SRV_STOP: {
        const int status = stopAudio();
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    case AUDIO_SRV_REGISTER_CALLBACK: {
        uint32_t callbackHandle = in.readBinderHandle();
        if (callbackHandle != 0 && IPCThreadState::acquireHandle(callbackHandle) != 0) {
            LOG_ERROR("BnAudioSrv: failed to acquire callback handle %u", callbackHandle);
            callbackHandle = 0;
        }

        const int status = registerCallback(callbackHandle);
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    default:
        LOG_ERROR("BnAudioSrv: unknown code %u", code);
        return -1;
    }
}

} // namespace ipc
