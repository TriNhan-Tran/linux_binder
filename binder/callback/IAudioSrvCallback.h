#pragma once

#include <cstdint>
#include <string>

namespace ipc {

/**
 * @brief Audio service callback contract.
 */
class IAudioSrvCallback {
public:
    virtual ~IAudioSrvCallback() = default;
    virtual void onAudioEvent(int32_t eventType, const std::string& message) = 0;
};

} // namespace ipc
