#pragma once

#include "binder/srv/audio/BnAudioSrv.h"
#include "binder/callback/registry/SrvCallbackRegistry.h"

#include <atomic>


/**
 * @brief Real audio service implementation.
 */
class AudioSrv : public BnAudioSrv {
public:
    int playAudio() override;
    int stopAudio() override;
    int registerCallback(uint32_t callbackHandle) override;

private:
    std::atomic<bool> m_playing{false};
    SrvCallbackRegistry m_callbackRegistry{"[AudioSrv]", SrvCallbackType::Audio};
};

