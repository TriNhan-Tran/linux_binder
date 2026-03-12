#pragma once

#include "binder/core/BinderClient.h"

#include <cstdint>
#include <string>


/**
 * @brief Bn stub: server-side binder dispatcher for service-manager calls.
 *
 * Real registry behavior is implemented in `SrvMgr`.
 */
class BnSrvMgr : public BBinder {
public:
    ~BnSrvMgr() override = default;

    virtual int onAddSrv(const std::string& name, uint32_t handle) = 0;
    virtual uint32_t onGetSrv(const std::string& name) = 0;

    int onTransact(uint32_t code, const Parcel& data, Parcel* reply) override;
};

