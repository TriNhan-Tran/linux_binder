#include "AudioSrvIpc.hpp"

#include "Parcel.hpp"
#include "TxnCodes.hpp"

namespace ipc {

BpAudioSrv::BpAudioSrv(uint32_t handle)
    : m_remote(handle) {
}

int BpAudioSrv::playAudio() {
    Parcel data;
    Parcel reply;
    if (m_remote.transact(AUDIO_SRV_PLAY, data, &reply) != 0) {
        return -1;
    }

    return reply.readInt32();
}

int BpAudioSrv::stopAudio() {
    Parcel data;
    Parcel reply;
    if (m_remote.transact(AUDIO_SRV_STOP, data, &reply) != 0) {
        return -1;
    }

    return reply.readInt32();
}

int BpAudioSrv::registerCallback(BBinder* callback) {
    Parcel data;
    data.writeStrongBinder(callback);

    Parcel reply;
    if (m_remote.transact(AUDIO_SRV_REGISTER_CALLBACK, data, &reply) != 0) {
        return -1;
    }

    return reply.readInt32();
}

int BnAudioSrv::onTransact(uint32_t code, const Parcel& data, Parcel* reply) {
    Parcel in(data.data(), data.dataSize());

    switch (code) {
    case AUDIO_SRV_PLAY: {
        const int status = onPlayAudio();
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    case AUDIO_SRV_STOP: {
        const int status = onStopAudio();
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    case AUDIO_SRV_REGISTER_CALLBACK: {
        uint32_t callbackHandle = in.readBinderHandle();
        if (callbackHandle != 0 && IPCThreadState::acquireHandle(callbackHandle) != 0) {
            LOGE("BnAudioSrv: failed to acquire callback handle %u", callbackHandle);
            callbackHandle = 0;
        }

        const int status = onRegisterCallback(callbackHandle);
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    default:
        LOGE("BnAudioSrv: unknown code %u", code);
        return -1;
    }
}

} // namespace ipc
