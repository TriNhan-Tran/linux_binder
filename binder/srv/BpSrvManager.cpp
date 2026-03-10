#include "BpSrvManager.h"

namespace ipc {

BpSrvManager::BpSrvManager() = default;

int BpSrvManager::addSrv(const std::string& name, BBinder* server) {
    LOG_INFO("BpSrvManager::addSrv forwarding register request for '%s'", name.c_str());
    const int status = m_serviceManager.addSrv(name, server);
    if (status != 0) {
        LOG_ERROR("BpSrvManager::addSrv failed for '%s' status=%d", name.c_str(), status);
    }
    return status;
}

uint32_t BpSrvManager::getSrv(const std::string& name) {
    LOG_INFO("BpSrvManager::getSrv forwarding lookup request for '%s'", name.c_str());
    const uint32_t handle = m_serviceManager.getSrv(name);
    if (handle == 0) {
        LOG_WARN("BpSrvManager::getSrv failed for '%s'", name.c_str());
    } else {
        LOG_INFO("BpSrvManager::getSrv obtained handle=%u for '%s'", handle, name.c_str());
    }
    return handle;
}

} // namespace ipc
