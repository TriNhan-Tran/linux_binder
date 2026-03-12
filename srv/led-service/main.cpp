#include "srv/led-service/LedSrv.h"

#include "binder/core/BinderClient.h"
#include "binder/core/BinderUtils.h"
#include "binder/core/BinderSrvMgr.h"
#include "binder/core/TransactionCode.h"

int main() {
    LOG_INFO("=== LedSrv starting ===");

    ProcessState::self().open(BINDER_DRIVER_PATH);
    if (!ProcessState::self().isOpen()) {
        LOG_ERROR("LedSrv: failed to open binder driver");
        return 1;
    }

    LedSrv ledSrv;

    BinderSrvMgr binderSrvMgr;
    const int status = binderSrvMgr.addSrv(LED_SRV_NAME, &ledSrv);
    if (status != 0) {
        LOG_ERROR("LedSrv: failed to register with SrvMgr");
        return 1;
    }
    LOG_INFO("LedSrv: registered with SrvMgr");

    LOG_INFO("LedSrv: entering binder loop...");
    IPCThreadState::joinThreadPool(nullptr);
    return 0;
}
