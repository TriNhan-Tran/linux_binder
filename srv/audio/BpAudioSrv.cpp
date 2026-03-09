#include "BpAudioSrv.h"

#include "Parcel.h"
#include "TransactionCode.h"

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

int BpAudioSrv::registerCallback(IAudioSrvCallback* callback) {
    BBinder* binderCallback = dynamic_cast<BBinder*>(callback);
    if (callback != nullptr && binderCallback == nullptr) {
        LOGE("BpAudioSrv::registerCallback: callback is not binder-backed");
        return -1;
    }

    Parcel data;
    data.writeStrongBinder(binderCallback);

    Parcel reply;
    if (m_remote.transact(AUDIO_SRV_REGISTER_CALLBACK, data, &reply) != 0) {
        return -1;
    }

    return reply.readInt32();
}

} // namespace ipc
