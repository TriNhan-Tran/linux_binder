#include "BnAudioSrvCallback.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

int BnAudioSrvCallback::onTransact(uint32_t code, const Parcel& data, Parcel* /*reply*/) {
    if (code != AUDIO_SRV_CB_ON_EVENT) {
        LOG_ERROR("BnAudioSrvCallback: unknown code %u", code);
        return -1;
    }

    Parcel in(data.data(), data.dataSize());
    const int32_t eventType = in.readInt32();
    const std::string message = in.readString();
    LOG_INFO("BnAudioSrvCallback::onTransact received eventType=%d message='%s'",
             eventType,
             message.c_str());
    onAudioEvent(eventType, message);
    return 0;
}

} // namespace ipc
