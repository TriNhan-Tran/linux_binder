#pragma once

#include "IAudioSrvCallback.h"

#include "BinderClient.h"

namespace ipc {

/**
 * @brief Bp proxy for audio callback notifications.
 */
class BpAudioSrvCallback : public IAudioSrvCallback {
public:
    explicit BpAudioSrvCallback(uint32_t handle);

    void onAudioEvent(int32_t eventType, const std::string& message) override;

private:
    BpBinder m_remote;
};

} // namespace ipc
