#include "SrvCallbackIpc.hpp"

#include "Parcel.hpp"
#include "TxnCodes.hpp"

namespace ipc {

int BnSrvCallback::onTransact(uint32_t code, const Parcel& data, Parcel* /*reply*/) {
    if (code != SRV_CB_ON_EVENT) {
        LOGE("BnSrvCallback: unknown code %u", code);
        return -1;
    }

    Parcel in(data.data(), data.dataSize());
    const int32_t eventType = in.readInt32();
    const std::string message = in.readString();
    onEvent(eventType, message);
    return 0;
}

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
