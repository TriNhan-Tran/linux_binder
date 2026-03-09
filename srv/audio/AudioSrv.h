#pragma once

#include "BnAudioSrv.h"
#include "SrvCallbackRegistry.h"

#include <atomic>

namespace demo {

/**
 * @brief Real audio service implementation.
 */
class AudioSrv : public ipc::BnAudioSrv {
public:
    int playAudio() override;
    int stopAudio() override;
    int registerCallback(uint32_t callbackHandle) override;

private:
    std::atomic<bool> m_playing{false};
    ipc::SrvCallbackRegistry m_callbackRegistry{"[AudioSrv]", ipc::SrvCallbackType::Audio};
};

} // namespace demo
