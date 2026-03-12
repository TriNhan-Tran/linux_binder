#include "binder/callback/proxy/BpSrvCallback.h"

#include "binder/core/Parcel.h"
#include "binder/core/TransactionCode.h"


BpSrvCallback::BpSrvCallback(uint32_t handle)
    : m_remote(handle) {
}

void BpSrvCallback::onEvent(int32_t eventType, const std::string& message) {
    Parcel data;
    data.writeInt32(eventType);
    data.writeString(message);
    m_remote.send(SRV_CB_ON_EVENT, data);
}

