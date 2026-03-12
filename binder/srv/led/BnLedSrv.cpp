#include "binder/srv/led/BnLedSrv.h"

#include "binder/core/Parcel.h"
#include "binder/core/TransactionCode.h"


int BnLedSrv::onTransact(uint32_t code, const Parcel& data, Parcel* reply) {
    Parcel in(data.data(), data.dataSize());

    switch (code) {
    case LED_SRV_REQUEST_ON: {
        const int status = requestLedOn();
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    case LED_SRV_REGISTER_CALLBACK: {
        uint32_t callbackHandle = in.readBinderHandle();
        if (callbackHandle != 0 && IPCThreadState::acquireHandle(callbackHandle) != 0) {
            LOG_ERROR("BnLedSrv: failed to acquire callback handle %u", callbackHandle);
            callbackHandle = 0;
        }

        const int status = registerCallback(callbackHandle);
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    default:
        LOG_ERROR("BnLedSrv: unknown code %u", code);
        return -1;
    }
}

