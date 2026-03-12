#pragma once

#include <cstdint>
#include <string>

/**
 * @brief Audio service callback contract.
 */
class IAudioSrvCallback {
public:
    virtual ~IAudioSrvCallback() = default;
    virtual void onAudioEvent(int32_t eventType, const std::string& message) = 0;
};
