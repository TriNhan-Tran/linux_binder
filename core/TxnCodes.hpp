#pragma once

#include <cstdint>

namespace ipc {

/** @brief SrvManager registered name for LED server. */
inline constexpr const char* LED_SRV_NAME = "led.service";

/** @brief SrvManager registered name for audio server. */
inline constexpr const char* AUDIO_SRV_NAME = "audio.service";

/**
 * @brief Transaction codes for SrvManager interface.
 */
enum SrvManagerCode : uint32_t {
    SRV_MGR_GET_SRV = 1,
    SRV_MGR_ADD_SRV = 2,
};

/**
 * @brief Transaction codes for LED server interface.
 */
enum LedSrvCode : uint32_t {
    LED_SRV_REQUEST_ON = 1,
    LED_SRV_REGISTER_CALLBACK = 2,
};

/**
 * @brief Transaction codes for audio server interface.
 */
enum AudioSrvCode : uint32_t {
    AUDIO_SRV_PLAY = 1,
    AUDIO_SRV_STOP = 2,
    AUDIO_SRV_REGISTER_CALLBACK = 3,
};

/**
 * @brief Transaction codes for callback interface.
 */
enum SrvCallbackCode : uint32_t {
    SRV_CB_ON_EVENT = 1,
};

/**
 * @brief Event codes sent from servers to application callback.
 */
enum NotifyEvent : int32_t {
    EVENT_LED_ON = 100,
    EVENT_LED_OFF = 101,
    EVENT_AUDIO_PLAYING = 200,
    EVENT_AUDIO_STOPPED = 201,
};

} // namespace ipc
