#pragma once

#include "AudioSrvIpc.hpp"
#include "SrvCallbackRegistry.hpp"

#include <atomic>

namespace demo {

/**
 * @brief Audio server implementation bound to binder stub.
 */
class AudioSrvHost : public ipc::BnAudioSrv {
public:
    int onPlayAudio() override;
    int onStopAudio() override;
    int onRegisterCallback(uint32_t callbackHandle) override;

private:
    std::atomic<bool> m_playing{false};
    ipc::SrvCallbackRegistry m_callbackRegistry{"[AudioSrv]"};
};

} // namespace demo
