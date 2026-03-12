#include "app/App.h"
#include "app/adapters/AudioSrvAdapter.h"

#include "binder/srv/audio/BpAudioSrv.h"

AudioSrvAdapter& AudioSrvAdapter::getInstance() {
    static AudioSrvAdapter instance;
    LOG_DEBUG("AudioSrvAdapter::getInstance singleton=%p", static_cast<void*>(&instance));
    return instance;
}

void AudioSrvAdapter::attachApp(App* app) {
    LOG_INFO("AudioSrvAdapter::attachApp app=%p", static_cast<void*>(app));
    m_app = app;
}

void AudioSrvAdapter::initialize(uint32_t handle) {
    if (handle == 0) {
        LOG_ERROR("AudioSrvAdapter::initialize failed: handle=0");
        m_audioSrv.reset();
        return;
    }

    if (m_audioSrv) {
        LOG_WARN("AudioSrvAdapter::initialize replacing existing audio proxy");
    }

    LOG_INFO("AudioSrvAdapter::initialize creating BpAudioSrv proxy for handle=%u", handle);
    m_audioSrv = std::make_unique<BpAudioSrv>(handle);
    LOG_INFO("AudioSrvAdapter::initialize completed");
}

int AudioSrvAdapter::registerCallback() {
    if (!m_audioSrv) {
        LOG_ERROR("AudioSrvAdapter::registerCallback failed: proxy not initialized");
        return -1;
    }

    LOG_INFO("AudioSrvAdapter::registerCallback sending IPC request");
    const int status = m_audioSrv->registerCallback(this);
    if (status != 0) {
        LOG_ERROR("AudioSrvAdapter::registerCallback failed status=%d", status);
    } else {
        LOG_INFO("AudioSrvAdapter::registerCallback succeeded");
    }
    return status;
}

int AudioSrvAdapter::playAudio() {
    if (!m_audioSrv) {
        LOG_ERROR("AudioSrvAdapter::playAudio failed: proxy not initialized");
        return -1;
    }

    LOG_INFO("AudioSrvAdapter::playAudio sending IPC request");
    const int status = m_audioSrv->playAudio();
    if (status != 0) {
        LOG_WARN("AudioSrvAdapter::playAudio returned status=%d", status);
    } else {
        LOG_INFO("AudioSrvAdapter::playAudio completed status=%d", status);
    }
    return status;
}

int AudioSrvAdapter::stopAudio() {
    if (!m_audioSrv) {
        LOG_ERROR("AudioSrvAdapter::stopAudio failed: proxy not initialized");
        return -1;
    }

    LOG_INFO("AudioSrvAdapter::stopAudio sending IPC request");
    const int status = m_audioSrv->stopAudio();
    if (status != 0) {
        LOG_WARN("AudioSrvAdapter::stopAudio returned status=%d", status);
    } else {
        LOG_INFO("AudioSrvAdapter::stopAudio completed status=%d", status);
    }
    return status;
}

void AudioSrvAdapter::shutdown() {
    LOG_INFO("AudioSrvAdapter::shutdown releasing proxy and app binding");
    m_audioSrv.reset();
    m_app = nullptr;
}

void AudioSrvAdapter::onAudioEvent(int32_t eventType, const std::string& message) {
    LOG_INFO("AudioSrvAdapter::onAudioEvent received eventType=%d message='%s'",
             eventType,
             message.c_str());
    if (m_app != nullptr) {
        Message appMessage;
        appMessage.what = App::MSG_AUDIO_EVENT;
        appMessage.obj = App::AudioEvent{eventType, message};
        m_app->sendMessage(appMessage);
    } else {
        LOG_WARN("AudioSrvAdapter::onAudioEvent dropped: app is null");
    }
}
