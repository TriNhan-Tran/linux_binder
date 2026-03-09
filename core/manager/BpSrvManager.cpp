#include "BpSrvManager.h"

namespace ipc {

BpSrvManager::BpSrvManager() = default;

int BpSrvManager::addSrv(const std::string& name, BBinder* server) {
    return m_serviceManager.addSrv(name, server);
}

uint32_t BpSrvManager::getSrv(const std::string& name) {
    return m_serviceManager.getSrv(name);
}

} // namespace ipc
