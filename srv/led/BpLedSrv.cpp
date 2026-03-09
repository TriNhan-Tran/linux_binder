#include "BpLedSrv.h"

#include "Parcel.h"
#include "TransactionCode.h"

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

int BpLedSrv::registerCallback(ILedSrvCallback* callback) {
    BBinder* binderCallback = dynamic_cast<BBinder*>(callback);
    if (callback != nullptr && binderCallback == nullptr) {
        LOGE("BpLedSrv::registerCallback: callback is not binder-backed");
        return -1;
    }

    Parcel data;
    data.writeStrongBinder(binderCallback);

    Parcel reply;
    if (m_remote.transact(LED_SRV_REGISTER_CALLBACK, data, &reply) != 0) {
        return -1;
    }

    return reply.readInt32();
}

} // namespace ipc
