#include "ServiceManager.h"

#include "Parcel.h"
#include "TransactionCode.h"

namespace ipc {

ServiceManager::ServiceManager()
    : m_remote(0) {
    LOG_DEBUG("ServiceManager::ServiceManager created with context-manager handle=0");
}

int ServiceManager::addSrv(const std::string& name, BBinder* server) {
    if (name.empty()) {
        LOG_WARN("ServiceManager::addSrv called with empty service name");
    }
    if (server == nullptr) {
        LOG_ERROR("ServiceManager::addSrv failed for '%s': server binder is null", name.c_str());
        return -1;
    }

    LOG_INFO("ServiceManager::addSrv sending register request for '%s' binder=%p",
             name.c_str(),
             static_cast<void*>(server));

    Parcel data;
    data.writeString(name);
    data.writeStrongBinder(server);

    Parcel reply;
    if (m_remote.transact(SRV_MGR_ADD_SRV, data, &reply) != 0) {
        LOG_ERROR("ServiceManager::addSrv transact failed for '%s'", name.c_str());
        return -1;
    }

    const int status = reply.readInt32();
    LOG_INFO("ServiceManager::addSrv response for '%s': status=%d", name.c_str(), status);
    return status;
}

uint32_t ServiceManager::getSrv(const std::string& name) {
    if (name.empty()) {
        LOG_WARN("ServiceManager::getSrv called with empty service name");
    }

    LOG_INFO("ServiceManager::getSrv sending lookup request for '%s'", name.c_str());

    Parcel data;
    data.writeString(name);

    Parcel reply;
    if (m_remote.transact(SRV_MGR_GET_SRV, data, &reply) != 0) {
        LOG_ERROR("ServiceManager::getSrv transact failed for '%s'", name.c_str());
        return 0;
    }

    const int32_t status = reply.readInt32();
    if (status != 0) {
        LOG_WARN("ServiceManager::getSrv '%s' not available (status=%d)", name.c_str(), status);
        return 0;
    }

    const uint32_t handle = reply.readBinderHandle();
    if (handle == 0) {
        LOG_ERROR("ServiceManager::getSrv '%s' returned invalid handle=0", name.c_str());
        return 0;
    }

    LOG_INFO("ServiceManager::getSrv obtained handle=%u for '%s'", handle, name.c_str());
    return handle;
}

} // namespace ipc
