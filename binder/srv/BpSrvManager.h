#pragma once

#include "ISrvManager.h"

#include "ServiceManager.h"

namespace ipc {

/**
 * @brief Bp proxy: client-side binder proxy for service-manager calls.
 *
 * This proxy delegates binder marshalling to the shared `ipc::ServiceManager`
 * helper while exposing service-scoped Bp naming.
 */
class BpSrvManager : public ISrvManager {
public:
    BpSrvManager();

    int addSrv(const std::string& name, BBinder* server) override;
    uint32_t getSrv(const std::string& name) override;

private:
    ServiceManager m_serviceManager;
};

} // namespace ipc
