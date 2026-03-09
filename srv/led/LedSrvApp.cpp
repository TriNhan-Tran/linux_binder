#include "LedSrvApp.h"

#include "BinderClient.h"
#include "BinderUtils.h"
#include "LedSrv.h"
#include "ServiceManager.h"
#include "TransactionCode.h"

namespace demo {

int LedSrvApp::run() {
    LOG_INFO("=== LedSrv starting ===");

    ipc::ProcessState::self().open(ipc::BINDER_DRIVER_PATH);
    if (!ipc::ProcessState::self().isOpen()) {
        LOG_ERROR("LedSrv: failed to open binder driver");
        return 1;
    }

    LedSrv ledSrv;

    ipc::ServiceManager srvManager;
    const int status = srvManager.addSrv(ipc::LED_SRV_NAME, &ledSrv);
    if (status != 0) {
        LOG_ERROR("LedSrv: failed to register with SrvManager");
        return 1;
    }
    LOG_INFO("LedSrv: registered with SrvManager");

    LOG_INFO("LedSrv: entering binder loop...");
    ipc::IPCThreadState::joinThreadPool(nullptr);
    return 0;
}

} // namespace demo
