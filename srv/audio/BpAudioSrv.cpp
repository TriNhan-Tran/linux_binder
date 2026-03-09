#include "BpAudioSrv.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

BpAudioSrv::BpAudioSrv(uint32_t handle)
    : m_remote(handle) {
    LOG_DEBUG("BpAudioSrv::BpAudioSrv created for handle=%u", handle);
}

int BpAudioSrv::playAudio() {
    LOG_INFO("BpAudioSrv::playAudio sending IPC request");
    Parcel data;
    Parcel reply;
    if (m_remote.transact(AUDIO_SRV_PLAY, data, &reply) != 0) {
        LOG_ERROR("BpAudioSrv::playAudio IPC transact failed");
        return -1;
    }

    const int status = reply.readInt32();
    LOG_INFO("BpAudioSrv::playAudio received IPC response status=%d", status);
    return status;
}

int BpAudioSrv::stopAudio() {
    LOG_INFO("BpAudioSrv::stopAudio sending IPC request");
    Parcel data;
    Parcel reply;
    if (m_remote.transact(AUDIO_SRV_STOP, data, &reply) != 0) {
        LOG_ERROR("BpAudioSrv::stopAudio IPC transact failed");
        return -1;
    }

    const int status = reply.readInt32();
    LOG_INFO("BpAudioSrv::stopAudio received IPC response status=%d", status);
    return status;
}

int BpAudioSrv::registerCallback(IAudioSrvCallback* callback) {
    LOG_INFO("BpAudioSrv::registerCallback sending IPC request callback=%p",
             static_cast<void*>(callback));

    BBinder* binderCallback = dynamic_cast<BBinder*>(callback);
    if (callback != nullptr && binderCallback == nullptr) {
        LOG_ERROR("BpAudioSrv::registerCallback failed: callback is not binder-backed");
        return -1;
    }

    Parcel data;
    data.writeStrongBinder(binderCallback);

    Parcel reply;
    if (m_remote.transact(AUDIO_SRV_REGISTER_CALLBACK, data, &reply) != 0) {
        LOG_ERROR("BpAudioSrv::registerCallback IPC transact failed");
        return -1;
    }

    const int status = reply.readInt32();
    LOG_INFO("BpAudioSrv::registerCallback received IPC response status=%d", status);
    return status;
}

} // namespace ipc
