#include "BnSrvManager.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

int BnSrvManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply) {
    Parcel in(data.data(), data.dataSize());

    switch (code) {
    case SRV_MGR_ADD_SRV: {
        const std::string name = in.readString();
        uint32_t handle = in.readBinderHandle();
        if (handle != 0 && IPCThreadState::acquireHandle(handle) != 0) {
            LOGE("BnSrvManager: failed to acquire handle %u for '%s'", handle, name.c_str());
            handle = 0;
        }

        const int status = onAddSrv(name, handle);
        if (status != 0 && handle != 0 && IPCThreadState::releaseHandle(handle) != 0) {
            LOGE("BnSrvManager: failed to release handle %u for '%s'", handle, name.c_str());
        }

        if (reply != nullptr) {
            reply->writeInt32(status);
        }
        return 0;
    }
    case SRV_MGR_GET_SRV: {
        const std::string name = in.readString();
        const uint32_t handle = onGetSrv(name);
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
        LOGE("BnSrvManager: unknown code %u", code);
        return -1;
    }
}

} // namespace ipc
