#pragma once

#include "binder/srv/led/BnLedSrv.h"
#include "binder/callback/registry/SrvCallbackRegistry.h"


/**
 * @brief Real LED service implementation.
 */
class LedSrv : public BnLedSrv {
public:
    int requestLedOn() override;
    int registerCallback(uint32_t callbackHandle) override;

private:
    SrvCallbackRegistry m_callbackRegistry{"[LedSrv]", SrvCallbackType::Led};
};

