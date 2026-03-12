#include "srv/audio-service/AudioSrv.h"

#include "binder/core/BinderClient.h"
#include "binder/core/TransactionCode.h"
#include "binder/core/BinderSrvMgr.h"
#include "binder/core/TransactionCode.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    LOG_INFO("=== AudioSrv starting ===");

    ProcessState::self().open(BINDER_DRIVER_PATH);
    if (!ProcessState::self().isOpen()) {
        LOG_ERROR("AudioSrv: failed to open binder driver");
        return 1;
    }

    AudioSrv audioSrv;

    BinderSrvMgr binderSrvMgr;
    const int status = binderSrvMgr.addSrv(AUDIO_SRV_NAME, &audioSrv);
    if (status != 0) {
        LOG_ERROR("AudioSrv: failed to register with SrvMgr");
        return 1;
    }
    LOG_INFO("AudioSrv: registered with SrvMgr");

    LOG_INFO("AudioSrv: entering binder loop...");
    IPCThreadState::joinThreadPool(nullptr);
    return 0;
}