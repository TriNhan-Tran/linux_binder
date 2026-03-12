#pragma once

#include "binder/callback/native/BnAudioSrvCallback.h"
#include "binder/srv/audio/IAudioSrv.h"

#include <memory>

class App;

/**
 * @brief Adapter that handles only audio service callbacks.
 */
class AudioSrvAdapter : public BnAudioSrvCallback {
public:
    AudioSrvAdapter() = default;
    ~AudioSrvAdapter() override = default;
    AudioSrvAdapter(const AudioSrvAdapter&) = delete;
    AudioSrvAdapter& operator=(const AudioSrvAdapter&) = delete;
    AudioSrvAdapter(AudioSrvAdapter&&) = delete;
    AudioSrvAdapter& operator=(AudioSrvAdapter&&) = delete;

    /** @brief Get the singleton instance of the adapter. */
    static AudioSrvAdapter& getInstance();

    /** @brief Provide the App reference for posting callback messages. */
    void attachApp(App* app);

    /** @brief Connect to the audio service at the given binder handle. */
    void initialize(uint32_t handle);

    /** @brief Register this adapter's callback with the audio service. */
    int registerCallback();

    /** @brief Request the audio service to start playback. */
    int playAudio();

    /** @brief Request the audio service to stop playback. */
    int stopAudio();

    /** @brief Release the audio service proxy. */
    void shutdown();

    void onAudioEvent(int32_t eventType, const std::string& message) override;

private:
    App* m_app = nullptr;
    std::unique_ptr<IAudioSrv> m_audioSrv;
};

