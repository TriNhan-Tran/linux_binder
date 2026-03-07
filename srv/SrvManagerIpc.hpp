#pragma once

#include "BinderCore.hpp"

#include <cstdint>
#include <string>

namespace ipc {

/**
 * @brief Client proxy for binder srv manager (context manager).
 */
class BpSrvManager {
public:
    /** @brief Create proxy that talks to handle 0 context manager. */
    BpSrvManager();

    /**
     * @brief Register local binder server under a name.
     *
     * @param name Stable server name key.
     * @param server Local server binder object.
     * @return 0 on success, non-zero on failure.
     */
    int addSrv(const std::string& name, BBinder* server);

    /**
     * @brief Look up remote server handle by name.
     *
     * @param name Server name key.
     * @return Remote binder handle, or 0 if not found.
     */
    uint32_t getSrv(const std::string& name);

private:
    BpBinder m_remote;
};

/**
 * @brief Server-side srv manager binder interface.
 */
class BnSrvManager : public BBinder {
public:
    /**
     * @brief Store server handle under a server name.
     *
     * @param name Server name key.
     * @param handle Remote binder handle translated by kernel.
     * @return 0 on success, non-zero on failure.
     */
    virtual int onAddSrv(const std::string& name, uint32_t handle) = 0;

    /**
     * @brief Query a server handle by name.
     *
     * @param name Server name key.
     * @return Remote binder handle, or 0 when not found.
     */
    virtual uint32_t onGetSrv(const std::string& name) = 0;

    /** @brief Dispatch binder transaction for srv manager codes. */
    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

} // namespace ipc
