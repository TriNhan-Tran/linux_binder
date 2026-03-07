#include "SrvManagerIpc.hpp"

#include "Parcel.hpp"
#include "TxnCodes.hpp"

namespace ipc {

BpSrvManager::BpSrvManager()
    : m_remote(0) {
}

int BpSrvManager::addSrv(const std::string& name, BBinder* server) {
    Parcel data;
    data.writeString(name);
    data.writeStrongBinder(server);

    Parcel reply;
    if (m_remote.transact(SRV_MGR_ADD_SRV, data, &reply) != 0) {
        LOGE("BpSrvManager::addSrv: transact failed for '%s'", name.c_str());
        return -1;
    }

    return reply.readInt32();
}

uint32_t BpSrvManager::getSrv(const std::string& name) {
    Parcel data;
    data.writeString(name);

    Parcel reply;
    if (m_remote.transact(SRV_MGR_GET_SRV, data, &reply) != 0) {
        LOGE("BpSrvManager::getSrv: transact failed for '%s'", name.c_str());
        return 0;
    }

    const int32_t status = reply.readInt32();
    if (status != 0) {
        return 0;
    }

    return reply.readBinderHandle();
}

int BnSrvManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply) {
    Parcel in(data.data(), data.dataSize());

    switch (code) {
    case SRV_MGR_ADD_SRV: {
        const std::string name = in.readString();
        uint32_t handle = in.readBinderHandle();
        if (handle != 0 && IPCThreadState::acquireHandle(handle) != 0) {
            LOGE("BnSrvManager: failed to acquire handle %u for '%s'",
                 handle,
                 name.c_str());
            handle = 0;
        }

        const int status = onAddSrv(name, handle);
        if (status != 0 && handle != 0 && IPCThreadState::releaseHandle(handle) != 0) {
            LOGE("BnSrvManager: failed to release handle %u for '%s'",
                 handle,
                 name.c_str());
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
