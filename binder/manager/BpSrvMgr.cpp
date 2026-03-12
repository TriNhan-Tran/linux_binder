#include "binder/manager/BpSrvMgr.h"

int BpSrvMgr::addSrv(const std::string& name, BBinder* server) {
    LOG_INFO("BpSrvMgr::addSrv forwarding register request for '%s'", name.c_str());
    const int status = m_binderSrvMgr.addSrv(name, server);
    if (status != 0) {
        LOG_ERROR("BpSrvMgr::addSrv failed for '%s' status=%d", name.c_str(), status);
    }
    return status;
}

uint32_t BpSrvMgr::getSrv(const std::string& name) {
    LOG_INFO("BpSrvMgr::getSrv forwarding lookup request for '%s'", name.c_str());
    const uint32_t handle = m_binderSrvMgr.getSrv(name);
    if (handle == 0) {
        LOG_WARN("BpSrvMgr::getSrv failed for '%s'", name.c_str());
    } else {
        LOG_INFO("BpSrvMgr::getSrv obtained handle=%u for '%s'", handle, name.c_str());
    }
    return handle;
}

