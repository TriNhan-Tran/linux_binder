#pragma once

#include "BnLedSrv.h"
#include "SrvCallbackRegistry.h"

namespace demo {

/**
 * @brief Real LED service implementation.
 */
class LedSrv : public ipc::BnLedSrv {
public:
    int requestLedOn() override;
    int registerCallback(uint32_t callbackHandle) override;

private:
    ipc::SrvCallbackRegistry m_callbackRegistry{"[LedSrv]", ipc::SrvCallbackType::Led};
};

} // namespace demo
