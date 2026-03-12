#include "binder/srv/led/BpLedSrv.h"

#include "binder/core/Parcel.h"
#include "binder/core/TransactionCode.h"


BpLedSrv::BpLedSrv(uint32_t handle)
    : m_remote(handle) {
    LOG_DEBUG("BpLedSrv::BpLedSrv created for handle=%u", handle);
}

int BpLedSrv::requestLedOn() {
    LOG_INFO("BpLedSrv::requestLedOn sending IPC request");
    Parcel data;
    Parcel reply;
    if (m_remote.transact(LED_SRV_REQUEST_ON, data, &reply) != 0) {
        LOG_ERROR("BpLedSrv::requestLedOn IPC transact failed");
        return -1;
    }

    const int status = reply.readInt32();
    LOG_INFO("BpLedSrv::requestLedOn received IPC response status=%d", status);
    return status;
}

int BpLedSrv::registerCallback(ILedSrvCallback* callback) {
    LOG_INFO("BpLedSrv::registerCallback sending IPC request callback=%p",
             static_cast<void*>(callback));

    BBinder* binderCallback = dynamic_cast<BBinder*>(callback);
    if (callback != nullptr && binderCallback == nullptr) {
        LOG_ERROR("BpLedSrv::registerCallback failed: callback is not binder-backed");
        return -1;
    }

    Parcel data;
    data.writeStrongBinder(binderCallback);

    Parcel reply;
    if (m_remote.transact(LED_SRV_REGISTER_CALLBACK, data, &reply) != 0) {
        LOG_ERROR("BpLedSrv::registerCallback IPC transact failed");
        return -1;
    }

    const int status = reply.readInt32();
    LOG_INFO("BpLedSrv::registerCallback received IPC response status=%d", status);
    return status;
}

