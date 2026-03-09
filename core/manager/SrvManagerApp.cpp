#include "SrvManagerApp.h"

#include "BinderClient.h"
#include "BinderUtils.h"
#include "SrvManager.h"

namespace demo {

int SrvManagerApp::run() {
    LOGI("=== SrvManager starting ===");

    ipc::ProcessState::self().open(ipc::BINDER_DRIVER_PATH);
    if (!ipc::ProcessState::self().isOpen()) {
        LOGE("SrvManager: failed to open binder driver");
        return 1;
    }

    if (ipc::IPCThreadState::becomeContextManager() != 0) {
        LOGE("SrvManager: failed to become context manager");
        return 1;
    }
    LOGI("SrvManager: became context manager (handle 0)");

    SrvManager srvManager;
    LOGI("SrvManager: entering binder loop...");
    ipc::IPCThreadState::joinThreadPool(&srvManager);
    return 0;
}

} // namespace demo
