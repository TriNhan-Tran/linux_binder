#pragma once

#include "binder/manager/BnSrvMgr.h"

#include <cstdint>
#include <string>
#include <unordered_map>


/**
 * @brief Real service-manager implementation (registry + handle ownership).
 */
class SrvMgr : public BnSrvMgr {
public:
    SrvMgr() = default;
    ~SrvMgr() override;

    SrvMgr(const SrvMgr&) = delete;
    SrvMgr& operator=(const SrvMgr&) = delete;
    SrvMgr(SrvMgr&&) = delete;
    SrvMgr& operator=(SrvMgr&&) = delete;

    /** @brief Register a service handle under the given name. */
    int onAddSrv(const std::string& name, uint32_t handle) override;

    /** @brief Return the handle registered for the given name, or 0. */
    uint32_t onGetSrv(const std::string& name) override;

private:
    std::unordered_map<std::string, uint32_t> m_registry;
};

