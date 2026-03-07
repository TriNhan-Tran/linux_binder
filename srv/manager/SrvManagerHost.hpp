#pragma once

#include "SrvManagerIpc.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace demo {

/**
 * @brief In-process implementation of the srv manager binder stub.
 */
class SrvManagerHost : public ipc::BnSrvManager {
public:
    ~SrvManagerHost() override;

    int onAddSrv(const std::string& name, uint32_t handle) override;
    uint32_t onGetSrv(const std::string& name) override;

private:
    std::unordered_map<std::string, uint32_t> m_registry;
};

} // namespace demo
