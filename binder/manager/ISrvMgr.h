#pragma once

#include "binder/core/BinderClient.h"

#include <cstdint>
#include <string>


/**
 * @brief Public client contract for talking to the service registry.
 */
class ISrvMgr {
public:
    ISrvMgr() = default;
    virtual ~ISrvMgr() = default;

    ISrvMgr(const ISrvMgr&) = delete;
    ISrvMgr& operator=(const ISrvMgr&) = delete;
    ISrvMgr(ISrvMgr&&) = delete;
    ISrvMgr& operator=(ISrvMgr&&) = delete;

    virtual int addSrv(const std::string& name, BBinder* server) = 0;
    virtual uint32_t getSrv(const std::string& name) = 0;
};

