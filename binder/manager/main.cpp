#include "binder/manager/SrvMgr.h"

#include "binder/core/BinderClient.h"
#include "binder/core/BinderUtils.h"

int main() {
    LOG_INFO("=== SrvMgr starting ===");

    ProcessState::self().open(BINDER_DRIVER_PATH);
    if (!ProcessState::self().isOpen()) {
        LOG_ERROR("SrvMgr: failed to open binder driver");
        return 1;
    }

    if (IPCThreadState::becomeContextManager() != 0) {
        LOG_ERROR("SrvMgr: failed to become context manager");
        return 1;
    }
    LOG_INFO("SrvMgr: became context manager (handle 0)");

    SrvMgr localSrvMgr;
    LOG_INFO("SrvMgr: entering binder loop...");
    IPCThreadState::joinThreadPool(&localSrvMgr);
    return 0;
}
