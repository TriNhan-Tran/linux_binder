#include "BpLedSrvCallback.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

BpLedSrvCallback::BpLedSrvCallback(uint32_t handle)
    : m_remote(handle) {
}

void BpLedSrvCallback::onLedEvent(int32_t eventType, const std::string& message) {
    Parcel data;
    data.writeInt32(eventType);
    data.writeString(message);
    m_remote.send(LED_SRV_CB_ON_EVENT, data);
}

} // namespace ipc
