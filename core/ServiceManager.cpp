#include "ServiceManager.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

ServiceManager::ServiceManager()
    : m_remote(0) {
}

int ServiceManager::addSrv(const std::string& name, BBinder* server) {
    Parcel data;
    data.writeString(name);
    data.writeStrongBinder(server);

    Parcel reply;
    if (m_remote.transact(SRV_MGR_ADD_SRV, data, &reply) != 0) {
        LOGE("ServiceManager::addSrv: transact failed for '%s'", name.c_str());
        return -1;
    }

    return reply.readInt32();
}

uint32_t ServiceManager::getSrv(const std::string& name) {
    Parcel data;
    data.writeString(name);

    Parcel reply;
    if (m_remote.transact(SRV_MGR_GET_SRV, data, &reply) != 0) {
        LOGE("ServiceManager::getSrv: transact failed for '%s'", name.c_str());
        return 0;
    }

    const int32_t status = reply.readInt32();
    if (status != 0) {
        return 0;
    }

    return reply.readBinderHandle();
}

} // namespace ipc
