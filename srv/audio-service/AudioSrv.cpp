#include "srv/audio-service/AudioSrv.h"

#include "binder/core/TransactionCode.h"


int AudioSrv::playAudio() {
    LOG_INFO("AudioSrv::playAudio() called by client");
    if (m_playing.load()) {
        LOG_WARN("AudioSrv::playAudio() ignored: already playing");
        return 1;
    }
    m_playing.store(true);
    LOG_INFO("[AudioSrv] >>> AUDIO PLAYING <<<");
    m_callbackRegistry.notifyCallbacks(EVENT_AUDIO_PLAYING, "Audio playback started");
    LOG_INFO("AudioSrv::playAudio() completed status=0");
    return 0;
}

int AudioSrv::stopAudio() {
    LOG_INFO("AudioSrv::stopAudio() called by client");
    if (!m_playing.load()) {
        LOG_WARN("AudioSrv::stopAudio() ignored: audio is not playing");
        return 1;
    }
    m_playing.store(false);
    LOG_INFO("[AudioSrv] >>> AUDIO STOPPED <<<");
    m_callbackRegistry.notifyCallbacks(EVENT_AUDIO_STOPPED, "Audio playback stopped");
    LOG_INFO("AudioSrv::stopAudio() completed status=0");
    return 0;
}

int AudioSrv::registerCallback(uint32_t callbackHandle) {
    LOG_INFO("AudioSrv::registerCallback() called by client handle=%u", callbackHandle);
    const int status = m_callbackRegistry.registerCallbackHandle(callbackHandle);
    if (status != 0) {
        LOG_ERROR("AudioSrv::registerCallback() failed status=%d handle=%u", status, callbackHandle);
    } else {
        LOG_INFO("AudioSrv::registerCallback() completed status=%d handle=%u", status, callbackHandle);
    }
    return status;
}

