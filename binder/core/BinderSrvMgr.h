#pragma once

#include "binder/core/BinderClient.h"

#include <cstdint>
#include <string>


/**
 * @brief Shared helper for talking to the context-manager service registry.
 */
class BinderSrvMgr {
public:
    /** @brief Create helper bound to context-manager handle 0. */
    BinderSrvMgr();

    /**
     * @brief Register local binder server under a name.
     *
     * @param name Stable server name key.
     * @param server Local server binder object.
     * @return 0 on success, non-zero on failure.
     */
    int addSrv(const std::string& name, BBinder* server);

    /**
     * @brief Look up a remote server handle by name.
     *
     * @param name Server name key.
     * @return Remote binder handle, or 0 if not found.
     */
    uint32_t getSrv(const std::string& name);

private:
    BpBinder m_remote;
};

