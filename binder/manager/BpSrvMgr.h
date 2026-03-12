#pragma once

#include "binder/manager/ISrvMgr.h"

#include "binder/core/BinderSrvMgr.h"


/**
 * @brief Bp proxy: client-side binder proxy for service-manager calls.
 *
 * This proxy delegates binder marshalling to the shared `BinderSrvMgr`
 * helper while exposing service-scoped Bp naming.
 */
class BpSrvMgr : public ISrvMgr {
public:
    BpSrvMgr();

    int addSrv(const std::string& name, BBinder* server) override;
    uint32_t getSrv(const std::string& name) override;

private:
    BinderSrvMgr m_binderSrvMgr;
};

