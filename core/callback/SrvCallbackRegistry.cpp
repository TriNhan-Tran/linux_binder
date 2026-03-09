#include "SrvCallbackRegistry.h"

#include "BpAudioSrvCallback.h"
#include "BinderClient.h"
#include "BpLedSrvCallback.h"

namespace ipc {

SrvCallbackRegistry::SrvCallbackRegistry(const char* srvTag, SrvCallbackType callbackType)
    : m_srvTag(srvTag != nullptr ? srvTag : "[Srv]"),
      m_callbackType(callbackType) {
}

SrvCallbackRegistry::~SrvCallbackRegistry() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (uint32_t handle : m_callbacks) {
        if (handle != 0 && IPCThreadState::releaseHandle(handle) != 0) {
            LOGE("%s failed to release callback handle=%u", m_srvTag, handle);
        }
    }
}

int SrvCallbackRegistry::registerCallbackHandle(uint32_t callbackHandle) {
    if (callbackHandle == 0) {
        LOGE("%s registerCallback: invalid handle=0", m_srvTag);
        return -1;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    for (uint32_t handle : m_callbacks) {
        if (handle == callbackHandle) {
            if (IPCThreadState::releaseHandle(callbackHandle) != 0) {
                LOGE("%s duplicate callback release failed: handle=%u",
                     m_srvTag,
                     callbackHandle);
                return -1;
            }
            LOGI("%s callback already registered: handle=%u", m_srvTag, callbackHandle);
            return 0;
        }
    }

    LOGI("%s registerCallback: handle=%u", m_srvTag, callbackHandle);
    m_callbacks.push_back(callbackHandle);
    return 0;
}

void SrvCallbackRegistry::notifyCallbacks(int32_t eventType, const std::string& message) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (uint32_t handle : m_callbacks) {
        LOGI("%s notifying callback handle=%u event=%d", m_srvTag, handle, eventType);
        if (m_callbackType == SrvCallbackType::Led) {
            BpLedSrvCallback proxy(handle);
            proxy.onLedEvent(eventType, message);
        } else {
            BpAudioSrvCallback proxy(handle);
            proxy.onAudioEvent(eventType, message);
        }
    }
}

} // namespace ipc
