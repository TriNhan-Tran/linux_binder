#include "AudioSrvApp.hpp"

#include "AudioSrvHost.hpp"
#include "BinderCore.hpp"
#include "SrvManagerIpc.hpp"
#include "TxnCodes.hpp"

namespace demo {

int AudioSrvApp::run() {
    LOGI("=== AudioSrv starting ===");

    ipc::ProcessState::self().open("/dev/binderfs/binder");
    if (!ipc::ProcessState::self().isOpen()) {
        LOGE("AudioSrv: failed to open binder driver");
        return 1;
    }

    AudioSrvHost host;

    ipc::BpSrvManager srvManager;
    const int status = srvManager.addSrv(ipc::AUDIO_SRV_NAME, &host);
    if (status != 0) {
        LOGE("AudioSrv: failed to register with SrvManager");
        return 1;
    }
    LOGI("AudioSrv: registered with SrvManager");

    LOGI("AudioSrv: entering binder loop...");
    ipc::IPCThreadState::joinThreadPool(nullptr);
    return 0;
}

} // namespace demo
