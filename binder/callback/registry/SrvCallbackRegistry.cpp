#include "binder/callback/registry/SrvCallbackRegistry.h"

#include "binder/callback/proxy/BpAudioSrvCallback.h"
#include "binder/core/BinderClient.h"
#include "binder/callback/proxy/BpLedSrvCallback.h"


SrvCallbackRegistry::SrvCallbackRegistry(const char* srvTag, SrvCallbackType callbackType)
    : m_srvTag(srvTag != nullptr ? srvTag : "[Srv]"),
      m_callbackType(callbackType) {
    LOG_INFO("%s SrvCallbackRegistry initialized type=%s",
             m_srvTag,
             m_callbackType == SrvCallbackType::Led ? "Led" : "Audio");
}

SrvCallbackRegistry::~SrvCallbackRegistry() {
    std::lock_guard<std::mutex> lock(m_mutex);
    LOG_INFO("%s SrvCallbackRegistry releasing %zu callback handles", m_srvTag, m_callbacks.size());
    for (uint32_t handle : m_callbacks) {
        if (handle != 0 && IPCThreadState::releaseHandle(handle) != 0) {
            LOG_ERROR("%s failed to release callback handle=%u", m_srvTag, handle);
        }
    }
}

int SrvCallbackRegistry::registerCallbackHandle(uint32_t callbackHandle) {
    if (callbackHandle == 0) {
        LOG_ERROR("%s registerCallback: invalid handle=0", m_srvTag);
        return -1;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    for (uint32_t handle : m_callbacks) {
        if (handle == callbackHandle) {
            if (IPCThreadState::releaseHandle(callbackHandle) != 0) {
                LOG_ERROR("%s duplicate callback release failed: handle=%u", m_srvTag, callbackHandle);
                return -1;
            }
            LOG_WARN("%s callback already registered: handle=%u", m_srvTag, callbackHandle);
            return 0;
        }
    }

    LOG_INFO("%s registerCallback: handle=%u", m_srvTag, callbackHandle);
    m_callbacks.push_back(callbackHandle);
    LOG_INFO("%s registerCallback: total callbacks=%zu", m_srvTag, m_callbacks.size());
    return 0;
}

void SrvCallbackRegistry::notifyCallbacks(int32_t eventType, const std::string& message) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_callbacks.empty()) {
        LOG_WARN("%s notifyCallbacks skipped: no registered callbacks event=%d", m_srvTag, eventType);
        return;
    }

    LOG_INFO("%s notifyCallbacks dispatch event=%d listeners=%zu",
             m_srvTag,
             eventType,
             m_callbacks.size());
    for (uint32_t handle : m_callbacks) {
        LOG_INFO("%s notifying callback handle=%u event=%d", m_srvTag, handle, eventType);
        if (m_callbackType == SrvCallbackType::Led) {
            BpLedSrvCallback proxy(handle);
            proxy.onLedEvent(eventType, message);
        } else {
            BpAudioSrvCallback proxy(handle);
            proxy.onAudioEvent(eventType, message);
        }
    }
}

