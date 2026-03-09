#include "BnLedSrvCallback.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

int BnLedSrvCallback::onTransact(uint32_t code, const Parcel& data, Parcel* /*reply*/) {
    if (code != LED_SRV_CB_ON_EVENT) {
        LOGE("BnLedSrvCallback: unknown code %u", code);
        return -1;
    }

    Parcel in(data.data(), data.dataSize());
    const int32_t eventType = in.readInt32();
    const std::string message = in.readString();
    onLedEvent(eventType, message);
    return 0;
}

} // namespace ipc
