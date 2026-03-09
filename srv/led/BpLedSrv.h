#pragma once

#include "ILedSrv.h"

#include "BinderClient.h"

namespace ipc {

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

} // namespace ipc
