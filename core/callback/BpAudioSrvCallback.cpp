#include "BpAudioSrvCallback.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

BpAudioSrvCallback::BpAudioSrvCallback(uint32_t handle)
    : m_remote(handle) {
}

void BpAudioSrvCallback::onAudioEvent(int32_t eventType, const std::string& message) {
    Parcel data;
    data.writeInt32(eventType);
    data.writeString(message);
    m_remote.send(AUDIO_SRV_CB_ON_EVENT, data);
}

} // namespace ipc
