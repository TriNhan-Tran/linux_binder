#include "BpLedSrvCallback.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

BpLedSrvCallback::BpLedSrvCallback(uint32_t handle)
    : m_remote(handle) {
    LOG_DEBUG("BpLedSrvCallback::BpLedSrvCallback created for handle=%u", handle);
}

void BpLedSrvCallback::onLedEvent(int32_t eventType, const std::string& message) {
    LOG_DEBUG("BpLedSrvCallback::onLedEvent sending callback eventType=%d message='%s'",
              eventType,
              message.c_str());
    Parcel data;
    data.writeInt32(eventType);
    data.writeString(message);
    const int status = m_remote.send(LED_SRV_CB_ON_EVENT, data);
    if (status != 0) {
        LOG_ERROR("BpLedSrvCallback::onLedEvent send failed status=%d", status);
    } else {
        LOG_DEBUG("BpLedSrvCallback::onLedEvent send completed");
    }
}

} // namespace ipc
