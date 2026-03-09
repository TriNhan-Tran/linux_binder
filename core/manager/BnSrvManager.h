#pragma once

#include "BinderClient.h"

#include <cstdint>
#include <string>

namespace ipc {

/**
 * @brief Bn stub: server-side binder dispatcher for service-manager calls.
 *
 * Real registry behavior is implemented in `demo::SrvManager`.
 */
class BnSrvManager : public BBinder {
public:
    virtual int onAddSrv(const std::string& name, uint32_t handle) = 0;
    virtual uint32_t onGetSrv(const std::string& name) = 0;

    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

} // namespace ipc
