#include "binder/core/BinderSrvMgr.h"

#include "binder/core/Parcel.h"
#include "binder/core/TransactionCode.h"


BinderSrvMgr::BinderSrvMgr()
    : m_remote(0) {
    LOG_DEBUG("BinderSrvMgr::BinderSrvMgr created with context-manager handle=0");
}

int BinderSrvMgr::addSrv(const std::string& name, BBinder* server) {
    if (name.empty()) {
        LOG_WARN("BinderSrvMgr::addSrv called with empty service name");
    }
    if (server == nullptr) {
        LOG_ERROR("BinderSrvMgr::addSrv failed for '%s': server binder is null", name.c_str());
        return -1;
    }

    LOG_INFO("BinderSrvMgr::addSrv sending register request for '%s' binder=%p",
             name.c_str(),
             static_cast<void*>(server));

    Parcel data;
    data.writeString(name);
    data.writeStrongBinder(server);

    Parcel reply;
    if (m_remote.transact(SRV_MGR_ADD_SRV, data, &reply) != 0) {
        LOG_ERROR("BinderSrvMgr::addSrv transact failed for '%s'", name.c_str());
        return -1;
    }

    const int status = reply.readInt32();
    LOG_INFO("BinderSrvMgr::addSrv response for '%s': status=%d", name.c_str(), status);
    return status;
}

uint32_t BinderSrvMgr::getSrv(const std::string& name) {
    if (name.empty()) {
        LOG_WARN("BinderSrvMgr::getSrv called with empty service name");
    }

    LOG_INFO("BinderSrvMgr::getSrv sending lookup request for '%s'", name.c_str());

    Parcel data;
    data.writeString(name);

    Parcel reply;
    if (m_remote.transact(SRV_MGR_GET_SRV, data, &reply) != 0) {
        LOG_ERROR("BinderSrvMgr::getSrv transact failed for '%s'", name.c_str());
        return 0;
    }

    const int32_t status = reply.readInt32();
    if (status != 0) {
        LOG_WARN("BinderSrvMgr::getSrv '%s' not available (status=%d)", name.c_str(), status);
        return 0;
    }

    const uint32_t handle = reply.readBinderHandle();
    if (handle == 0) {
        LOG_ERROR("BinderSrvMgr::getSrv '%s' returned invalid handle=0", name.c_str());
        return 0;
    }

    LOG_INFO("BinderSrvMgr::getSrv obtained handle=%u for '%s'", handle, name.c_str());
    return handle;
}

