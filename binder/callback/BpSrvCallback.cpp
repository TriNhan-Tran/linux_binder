#include "BpSrvCallback.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

BpSrvCallback::BpSrvCallback(uint32_t handle)
    : m_remote(handle) {
}

void BpSrvCallback::onEvent(int32_t eventType, const std::string& message) {
    Parcel data;
    data.writeInt32(eventType);
    data.writeString(message);
    m_remote.send(SRV_CB_ON_EVENT, data);
}

} // namespace ipc
