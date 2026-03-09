#include "AudioSrvApp.h"

#include "AudioSrv.h"
#include "BinderClient.h"
#include "BinderUtils.h"
#include "ServiceManager.h"
#include "TransactionCode.h"

namespace demo {

int AudioSrvApp::run() {
    LOGI("=== AudioSrv starting ===");

    ipc::ProcessState::self().open(ipc::BINDER_DRIVER_PATH);
    if (!ipc::ProcessState::self().isOpen()) {
        LOGE("AudioSrv: failed to open binder driver");
        return 1;
    }

    AudioSrv audioSrv;

    ipc::ServiceManager srvManager;
    const int status = srvManager.addSrv(ipc::AUDIO_SRV_NAME, &audioSrv);
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
