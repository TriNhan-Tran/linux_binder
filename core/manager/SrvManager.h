#pragma once

#include "BnSrvManager.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace demo {

/**
 * @brief Real service-manager implementation (registry + handle ownership).
 */
class SrvManager : public ipc::BnSrvManager {
public:
    ~SrvManager() override;

    int onAddSrv(const std::string& name, uint32_t handle) override;
    uint32_t onGetSrv(const std::string& name) override;

private:
    std::unordered_map<std::string, uint32_t> m_registry;
};

} // namespace demo
