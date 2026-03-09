#include "AudioSrvAdapter.h"

#include "App.h"

#include "BpAudioSrv.h"

namespace demo {

AudioSrvAdapter& AudioSrvAdapter::getInstance() {
    static AudioSrvAdapter instance;
    return instance;
}

void AudioSrvAdapter::attachApp(App* app) {
    m_app = app;
}

void AudioSrvAdapter::initialize(uint32_t handle) {
    if (handle == 0) {
        m_audioSrv.reset();
        return;
    }

    m_audioSrv = std::make_unique<ipc::BpAudioSrv>(handle);
}

int AudioSrvAdapter::registerCallback() {
    if (!m_audioSrv) {
        return -1;
    }

    return m_audioSrv->registerCallback(this);
}

int AudioSrvAdapter::playAudio() {
    if (!m_audioSrv) {
        return -1;
    }

    return m_audioSrv->playAudio();
}

int AudioSrvAdapter::stopAudio() {
    if (!m_audioSrv) {
        return -1;
    }

    return m_audioSrv->stopAudio();
}

void AudioSrvAdapter::shutdown() {
    m_audioSrv.reset();
    m_app = nullptr;
}

void AudioSrvAdapter::onAudioEvent(int32_t eventType, const std::string& message) {
    if (m_app != nullptr) {
        m_app->handleAudioEvent(eventType, message);
    }
}

} // namespace demo
