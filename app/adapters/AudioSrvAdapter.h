#pragma once

#include "BnAudioSrvCallback.h"
#include "IAudioSrv.h"

#include <memory>

namespace demo {

class App;

/**
 * @brief Adapter that handles only audio service callbacks.
 */
class AudioSrvAdapter : public ipc::BnAudioSrvCallback {
public:
    static AudioSrvAdapter& getInstance();

    AudioSrvAdapter(const AudioSrvAdapter&) = delete;
    AudioSrvAdapter& operator=(const AudioSrvAdapter&) = delete;

    void attachApp(App* app);
    void initialize(uint32_t handle);
    int registerCallback();
    int playAudio();
    int stopAudio();
    void shutdown();

    void onAudioEvent(int32_t eventType, const std::string& message) override;

private:
    AudioSrvAdapter() = default;

    App* m_app = nullptr;
    std::unique_ptr<ipc::IAudioSrv> m_audioSrv;
};

} // namespace demo
