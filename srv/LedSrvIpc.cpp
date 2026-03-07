#include "LedSrvIpc.hpp"

#include "Parcel.hpp"
#include "TxnCodes.hpp"

namespace ipc {

BpLedSrv::BpLedSrv(uint32_t handle)
    : m_remote(handle) {
}

int BpLedSrv::requestLedOn() {
    Parcel data;
    Parcel reply;
    if (m_remote.transact(LED_SRV_REQUEST_ON, data, &reply) != 0) {
        return -1;
    }

    return reply.readInt32();
}

int BpLedSrv::registerCallback(BBinder* callback) {
    Parcel data;
    data.writeStrongBinder(callback);

    Parcel reply;
    if (m_remote.transact(LED_SRV_REGISTER_CALLBACK, data, &reply) != 0) {
        return -1;
    }

    return reply.readInt32();
}

int BnLedSrv::onTransact(uint32_t code, const Parcel& data, Parcel* reply) {
    Parcel in(data.data(), data.dataSize());

    switch (code) {
    case LED_SRV_REQUEST_ON: {
        const int status = onRequestLedOn();
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    case LED_SRV_REGISTER_CALLBACK: {
        uint32_t callbackHandle = in.readBinderHandle();
        if (callbackHandle != 0 && IPCThreadState::acquireHandle(callbackHandle) != 0) {
            LOGE("BnLedSrv: failed to acquire callback handle %u", callbackHandle);
            callbackHandle = 0;
        }

        const int status = onRegisterCallback(callbackHandle);
        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    default:
        LOGE("BnLedSrv: unknown code %u", code);
        return -1;
    }
}

} // namespace ipc
