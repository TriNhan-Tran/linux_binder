#include "binder/manager/BnSrvMgr.h"

#include "binder/core/Parcel.h"
#include "binder/core/TransactionCode.h"


int BnSrvMgr::onTransact(uint32_t code, const Parcel& data, Parcel* reply) {
    Parcel in(data.data(), data.dataSize());

    switch (code) {
    case SRV_MGR_ADD_SRV: {
        const std::string name = in.readString();
        uint32_t handle = in.readBinderHandle();
        LOG_INFO("BnSrvMgr::onTransact ADD_SRV name='%s' incomingHandle=%u",
                 name.c_str(),
                 handle);
        if (handle != 0 && IPCThreadState::acquireHandle(handle) != 0) {
            LOG_ERROR("BnSrvMgr: failed to acquire handle %u for '%s'", handle, name.c_str());
            handle = 0;
        }

        const int status = onAddSrv(name, handle);
        LOG_INFO("BnSrvMgr::onTransact ADD_SRV name='%s' result=%d finalHandle=%u",
                 name.c_str(),
                 status,
                 handle);
        if (status != 0 && handle != 0 && IPCThreadState::releaseHandle(handle) != 0) {
            LOG_ERROR("BnSrvMgr: failed to release handle %u for '%s'", handle, name.c_str());
        }

        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    case SRV_MGR_GET_SRV: {
        const std::string name = in.readString();
        LOG_INFO("BnSrvMgr::onTransact GET_SRV name='%s'", name.c_str());
        const uint32_t handle = onGetSrv(name);
        LOG_INFO("BnSrvMgr::onTransact GET_SRV name='%s' returned handle=%u",
                 name.c_str(),
                 handle);
        if (reply != nullptr) {
            if (handle != 0) {
                reply->writeInt32(0);
                reply->writeBinderHandle(handle);
            } else {
                reply->writeInt32(-1);
            }
        }
        return 0;
    }
    default:
        LOG_ERROR("BnSrvMgr: unknown code %u", code);
        return -1;
    }
}

