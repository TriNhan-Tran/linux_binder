#include "AppCallback.hpp"

#include "TxnCodes.hpp"

namespace demo {

void AppCallback::onEvent(int32_t eventType, const std::string& message) {
    const char* eventName = "UNKNOWN";
    switch (eventType) {
    case ipc::EVENT_LED_ON: eventName = "LED_ON"; break;
    case ipc::EVENT_LED_OFF: eventName = "LED_OFF"; break;
    case ipc::EVENT_AUDIO_PLAYING: eventName = "AUDIO_PLAYING"; break;
    case ipc::EVENT_AUDIO_STOPPED: eventName = "AUDIO_STOPPED"; break;
    }
    LOGI("[App] CALLBACK >>> %s (%d): \"%s\"", eventName, eventType, message.c_str());
}

} // namespace demo
