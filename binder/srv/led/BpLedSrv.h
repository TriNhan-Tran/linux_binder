#pragma once

#include "binder/srv/led/ILedSrv.h"

#include "binder/core/BinderClient.h"


/**
 * @brief Bp proxy: client-side binder proxy for the LED service.
 */
class BpLedSrv : public ILedSrv {
public:
    explicit BpLedSrv(uint32_t handle);

    int requestLedOn() override;
    int registerCallback(ILedSrvCallback* callback) override;

private:
    BpBinder m_remote;
};

