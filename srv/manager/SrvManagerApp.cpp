#include "SrvManagerApp.hpp"

#include "BinderCore.hpp"
#include "SrvManagerHost.hpp"

namespace demo {

int SrvManagerApp::run() {
    LOGI("=== SrvManager starting ===");

    ipc::ProcessState::self().open("/dev/binderfs/binder");
    if (!ipc::ProcessState::self().isOpen()) {
        LOGE("SrvManager: failed to open binder driver");
        return 1;
    }

    if (ipc::IPCThreadState::becomeContextManager() != 0) {
        LOGE("SrvManager: failed to become context manager");
        return 1;
    }
    LOGI("SrvManager: became context manager (handle 0)");

    SrvManagerHost host;
    LOGI("SrvManager: entering binder loop...");
    ipc::IPCThreadState::joinThreadPool(&host);
    return 0;
}

} // namespace demo
