#include "BnSrvCallback.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

int BnSrvCallback::onTransact(uint32_t code, const Parcel& data, Parcel* /*reply*/) {
    if (code != SRV_CB_ON_EVENT) {
        LOG_ERROR("BnSrvCallback: unknown code %u", code);
        return -1;
    }

    Parcel in(data.data(), data.dataSize());
    const int32_t eventType = in.readInt32();
    const std::string message = in.readString();
    onEvent(eventType, message);
    return 0;
}

} // namespace ipc
