#include "AudioSrv.h"

#include "TransactionCode.h"

namespace demo {

int AudioSrv::playAudio() {
    LOGI("[AudioSrv] playAudio() called");
    if (m_playing.load()) {
        LOGI("[AudioSrv] already playing");
        return 1;
    }
    m_playing.store(true);
    LOGI("[AudioSrv] >>> AUDIO PLAYING <<<");
    m_callbackRegistry.notifyCallbacks(ipc::EVENT_AUDIO_PLAYING, "Audio playback started");
    return 0;
}

int AudioSrv::stopAudio() {
    LOGI("[AudioSrv] stopAudio() called");
    if (!m_playing.load()) {
        LOGI("[AudioSrv] not playing");
        return 1;
    }
    m_playing.store(false);
    LOGI("[AudioSrv] >>> AUDIO STOPPED <<<");
    m_callbackRegistry.notifyCallbacks(ipc::EVENT_AUDIO_STOPPED, "Audio playback stopped");
    return 0;
}

int AudioSrv::registerCallback(uint32_t callbackHandle) {
    return m_callbackRegistry.registerCallbackHandle(callbackHandle);
}

} // namespace demo
