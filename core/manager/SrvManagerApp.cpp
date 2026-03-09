#include "SrvManagerApp.h"

#include "BinderClient.h"
#include "BinderUtils.h"
#include "SrvManager.h"

namespace demo {

int SrvManagerApp::run() {
    LOG_INFO("=== SrvManager starting ===");

    ipc::ProcessState::self().open(ipc::BINDER_DRIVER_PATH);
    if (!ipc::ProcessState::self().isOpen()) {
        LOG_ERROR("SrvManager: failed to open binder driver");
        return 1;
    }

    if (ipc::IPCThreadState::becomeContextManager() != 0) {
        LOG_ERROR("SrvManager: failed to become context manager");
        return 1;
    }
    LOG_INFO("SrvManager: became context manager (handle 0)");

    SrvManager srvManager;
    LOG_INFO("SrvManager: entering binder loop...");
    ipc::IPCThreadState::joinThreadPool(&srvManager);
    return 0;
}

} // namespace demo
