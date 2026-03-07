#include "LedSrvApp.hpp"

#include "BinderCore.hpp"
#include "LedSrvHost.hpp"
#include "SrvManagerIpc.hpp"
#include "TxnCodes.hpp"

namespace demo {

int LedSrvApp::run() {
    LOGI("=== LedSrv starting ===");

    ipc::ProcessState::self().open("/dev/binderfs/binder");
    if (!ipc::ProcessState::self().isOpen()) {
        LOGE("LedSrv: failed to open binder driver");
        return 1;
    }

    LedSrvHost host;

    ipc::BpSrvManager srvManager;
    const int status = srvManager.addSrv(ipc::LED_SRV_NAME, &host);
    if (status != 0) {
        LOGE("LedSrv: failed to register with SrvManager");
        return 1;
    }
    LOGI("LedSrv: registered with SrvManager");

    LOGI("LedSrv: entering binder loop...");
    ipc::IPCThreadState::joinThreadPool(nullptr);
    return 0;
}

} // namespace demo
