#include "BpAudioSrvCallback.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

BpAudioSrvCallback::BpAudioSrvCallback(uint32_t handle)
    : m_remote(handle) {
    LOG_DEBUG("BpAudioSrvCallback::BpAudioSrvCallback created for handle=%u", handle);
}

void BpAudioSrvCallback::onAudioEvent(int32_t eventType, const std::string& message) {
    LOG_DEBUG("BpAudioSrvCallback::onAudioEvent sending callback eventType=%d message='%s'",
              eventType,
              message.c_str());
    Parcel data;
    data.writeInt32(eventType);
    data.writeString(message);
    const int status = m_remote.send(AUDIO_SRV_CB_ON_EVENT, data);
    if (status != 0) {
        LOG_ERROR("BpAudioSrvCallback::onAudioEvent send failed status=%d", status);
    } else {
        LOG_DEBUG("BpAudioSrvCallback::onAudioEvent send completed");
    }
}

} // namespace ipc
