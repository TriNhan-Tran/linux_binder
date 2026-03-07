#pragma once

#include "LedSrvIpc.hpp"
#include "SrvCallbackRegistry.hpp"

namespace demo {

/**
 * @brief LED server implementation bound to binder stub.
 */
class LedSrvHost : public ipc::BnLedSrv {
public:
    int onRequestLedOn() override;
    int onRegisterCallback(uint32_t callbackHandle) override;

private:
    ipc::SrvCallbackRegistry m_callbackRegistry{"[LedSrv]"};
};

} // namespace demo
