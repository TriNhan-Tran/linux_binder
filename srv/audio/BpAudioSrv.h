#pragma once

#include "IAudioSrv.h"

#include "BinderClient.h"

namespace ipc {

/**
 * @brief Bp proxy: client-side binder proxy for the audio service.
 */
class BpAudioSrv : public IAudioSrv {
public:
    explicit BpAudioSrv(uint32_t handle);

    int playAudio() override;
    int stopAudio() override;
    int registerCallback(IAudioSrvCallback* callback) override;

private:
    BpBinder m_remote;
};

} // namespace ipc
